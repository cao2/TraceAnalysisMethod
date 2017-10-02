#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include "lpn.h"
#include <set>
#include <algorithm>    // std::sort
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>/* needed to define getpid() */
#include <stdio.h>/* needed for printf() */
#include <pthread.h>
#include <sstream>
using namespace std;
uint32_t num_flow=22;
vector<string> flow_names;
bool po;
void init(vector<string> x, int y){
    flow_names=x;
    num_flow=y;
    po=false;
}
void getMemUsage(unsigned int pid, char* name)
{
    //printf("my process ID is %d\n", pid);
    
    char cmd[256];
    sprintf(cmd, "top -stats \"pid,command,cpu, mem\" -l 1 -pid %d | grep \"%d\" >> %s", pid, pid,name);
    system(cmd);
}

void max_mem(char *filename){
    
    uint32_t rst=0;
    ifstream trace_file(filename);
    int num;
    if (trace_file.is_open()) {
        std::string line;
        while (getline(trace_file, line)){
            stringstream ss(line);
            string temp;
            ss>>temp;
            ss>>temp;
            ss>>temp;
            ss>>temp;
            if (temp.substr(temp.length()-2).compare("K+")==0){
                num=stoi(temp.substr(0,temp.length()-2),nullptr);
                if (num>rst){
                    rst=num;
                }
            }
            else if (temp.substr(temp.length()-2).compare("M+")==0){
                num=stoi(temp.substr(0,temp.length()-2),nullptr)*1000;
                if (num>rst){
                    rst=num;
                }
            }
            else
                cout<<"erro processing "<<temp<<endl;

        }
        trace_file.close();
        
    }
    else {
        cout << "Unable to open file" << endl;
    }
    cout<<"memory usage : "<<rst<<" KB"<<endl;
    
}
struct order_inst
{
    bool start;
    uint32_t flow;
    uint32_t addr;
    uint32_t time;
    uint32_t idd;
    order_inst(){
        start=true;
        flow=0;
        addr=0;
        time=0;
        idd=0;
    }
    bool operator ==(const order_inst& other) const
    {
        return (start==other.start && flow==other.flow &&addr==other.addr&&time==other.time&&idd==other.idd);
    }
    void print() const{
        cout<<"flow :"<<flow_names.at(flow)<<",             addr: "<<addr<<"    time: "<<time<<"    id: "<<idd;
        if (start==true)
            cout<<"     start"<<endl;
        else
            cout<<"     end"<<endl;
    }
};
struct flow_instance_t
{
    lpn_t* flow_inst;
    config_t cfg;
    uint32_t addr;
    uint32_t time;
    uint32_t it;
    flow_instance_t() {
        flow_inst = nullptr;
        cfg = null_cfg;
        time=0;
        addr=0;
        it=0;
    }
    
    flow_instance_t(uint32_t x) {
        flow_inst = nullptr;
        cfg = null_cfg;
        addr = x;
        time=0;
    }
    
    flow_instance_t(const flow_instance_t& other) {
        flow_inst = other.flow_inst;
        cfg = other.cfg;
        addr= other.addr;
        time=other.time;
        it=other.it;
    }
    
    bool operator==(const flow_instance_t& other) const {
        
        return (flow_inst->get_flow_name() == other.flow_inst->get_flow_name() &&cfg == other.cfg  && addr == other.addr);
    }
    
    flow_instance_t& operator=(const flow_instance_t& other) {
        flow_inst = other.flow_inst;
        cfg = other.cfg;
        addr= other.addr;
        time=other.time;
        it=other.it;
        return *this;
    }
    
};

struct active_list{
    vector< vector<uint32_t> > active_inst;
    //rd0,rd1,wt0,wt1,wb0,wb1,rd0_gfx,rd0_audio,rd0_usb,rd0_uart,rd1_gfx,rd1_audio,rd1_usb,rd1_uart,wt0_gfx,wt0_audio,wt0_usb,wt0_uart,wt1_gfx,wt1_audio,wt1_usb,wt1_uart,pwron_gfx,pwron_audio,pwron_usb,pwron_uart,pwroff_gfx,pwroff_audio,pwroff_usb,pwroff_uart,gfx_up_rd,gfx_up_wt,usb_up_rd,usb_up_wt,uart_up_rd,uart_up_wt,audio_up_rd,audio_up_wt;
    active_list(){
        for (uint32_t i=0; i<num_flow+4;i++){
            vector<uint32_t> tmp;
            active_inst.push_back(tmp);
        }
    }
    void sortall(){
        for(uint32_t i=0; i<active_inst.size();i++)
            sort(active_inst.at(i).begin(),active_inst.at(i).end());
    }
};

struct scenario_t{
    vector<uint32_t> finished;
    
    vector<flow_instance_t> active_t;
    //active_list active_sort;
    vector<order_inst> order;
    //vector<uint32_t> order_addr;
    vector<message_t> last_valid;
    
    scenario_t(){
        for( uint32_t i=0;i<num_flow+4;i++)
            finished.push_back(0);
        message_t tmp;
        for(uint16_t i=0;i<75;i++)
            last_valid.push_back(tmp);
    }
    scenario_t(const scenario_t &other){
        finished=other.finished;
        active_t=other.active_t;
        order=other.order;
        last_valid=other.last_valid;
        
    }
};

//typedef vector<flow_instance_t> scenario_t;
uint32_t state(uint32_t cfg){
    for (uint32_t i = 0; i < 32; i++) {
        if ((cfg & 1) == 1 )
            return i;
        cfg = cfg >> 1;
    }
    return 33;
}


std::hash<std::string> str_hash;

active_list sort(vector<flow_instance_t> active_t){
    uint32_t inde;
    active_list sorted;
    
    for(uint32_t i=0;i<active_t.size();i++){
        //cout<<"active: ind:"<<inde<<endl;
        inde=active_t.at(i).flow_inst->get_index();
        sorted.active_inst.at(inde).push_back(active_t.at(i).cfg);
        
    }
    sorted.sortall();
    return sorted;
    
}

void print_scenario( const scenario_t& sce)
{
    vector<flow_instance_t> scen=sce.active_t;
    //cout<<"sce.finishe: size:"<<sce.finished.size()<<endl;
    //for(uint32_t mi:sce.finished)
    //    cout<<mi<<endl;
    //cout<<"......"<<endl;
    vector<uint32_t> flow_inst_cnt;
    for(uint32_t i=0;i<num_flow;i++){
        if (i<4)
            flow_inst_cnt.push_back(sce.finished.at(i)+sce.finished.at(i+num_flow));
        else
            flow_inst_cnt.push_back(sce.finished.at(i));
        
    }
    cout<<"==============================================="<<endl;
    cout << "finished flow instances:" << endl;
    
    for(uint32_t j=0;j<sce.finished.size();j++){
        if (sce.finished.at(j)){
            if (j< num_flow)
                if (sce.finished.at(j)>0){
                    cout<<"        *****"<<flow_names.at(j)<<": ";
                    cout<<sce.finished.at(j)<<endl;}
        }
    }
    cout<<"==============================================="<<endl;
    if(sce.active_t.size()!=0){
        cout<<"active flow specification states: "<<endl;
        
        active_list ac_l=sort(sce.active_t);
        
        for(uint32_t j=0;j<ac_l.active_inst.size();j++){
            if (ac_l.active_inst.at(j).size()>0){
                cout<<"        *****"<<flow_names.at(j)<<": ";
                for(uint32_t i=0;i<ac_l.active_inst.at(j).size();i++){
                    uint32_t cfg=ac_l.active_inst.at(j).at(i);
                    cout<<"<"<<state(cfg)<<">  ";
                    flow_inst_cnt.at(j)++;
                }
                cout<<endl;
            }
        }
       cout<<"==============================================="<<endl;
        cout << "total flow instances:" << endl;
        for (uint32_t i = 0; i < num_flow; i++) {
            if (flow_inst_cnt.at(i)>0)
            cout << "\t" << flow_names.at(i) << ": \t" << flow_inst_cnt.at(i) << endl;
        }
        cout<<"==============================================="<<endl;
        cout<<"==============================================="<<endl;
    }
    cout << endl;
    if (po){
    cout<<"==============================================="<<endl;
    cout<<"order"<<endl;
    for (uint32_t j=0;j<sce.order.size();j++)
        sce.order.at(j).print();
    }
}
//add more
bool equalscen(const scenario_t &x, const scenario_t &y){
    for (uint32_t i=0;i<x.finished.size();i++){
        if (x.finished.at(i)!=y.finished.at(i))
            return false;
    }
    
    if(x.active_t.size()!=y.active_t.size())
        return false;
    return true;
}
bool equalact(const active_list &fi, const active_list &se){
    
    for (uint32_t i=0;i<fi.active_inst.size();i++){
        if (fi.active_inst.at(i).size()==se.active_inst.at(i).size()){
            for (uint32_t j=0;j<fi.active_inst.at(i).size();j++)
                if (fi.active_inst.at(i).at(j)!=se.active_inst.at(i).at(j)) {
                    return false;
                }
        }
        else
            return false;
        
    }
    
   
    return true;
}
bool equalorder(const vector<order_inst> &o1, const vector<order_inst> &o2){
    if (o1.size()!=o2.size())
        return false;
    for (uint32_t i=0;i<o1.size();i++){
        if (!(o1.at(i)==o2.at(i)))
            return false;
            
    }
    return true;
}
bool equal_act(const vector<flow_instance_t> &o1, const vector<flow_instance_t> &o2){
    if (o1.size()!=o2.size())
        return false;
    for (uint32_t i=0;i<o1.size();i++)
        if (!(o1.at(i)==o2.at(i)))
            return false;
    return true;
}
void dscen(stack<scenario_t> &sk){
    if (sk.size()>2){
       
    vector<scenario_t> vec;
    cout<<"decen: orignal size: "<<sk.size()<<endl;
    while(!sk.empty()){
        scenario_t tmp=sk.top();
        //cout<<"__________secneaior____________________________"<<endl;
        //print_scenario(tmp);
        bool match=false;
        for (int i=0;i<vec.size();i++)
            if(equalscen(vec.at(i),tmp))
                if(equal_act(vec.at(i).active_t,tmp.active_t)){
                    if(equalorder(vec.at(i).order,tmp.order)){
                        match=true;
                        break;
                        }
                                    }
        
        sk.pop();
        if (!match)
            vec.push_back(tmp);
    }
    for(int i=0;i<vec.size();i++)
        sk.push(vec.at(i));

    
    cout<<"decen: new   size: "<<sk.size()<<endl;
    vector<scenario_t>().swap(vec);
    }
}

string cfg_str_c(const uint32_t& xcfg){
    uint32_t cfg=xcfg;
    string cfg_str;
    bool cfg_convert_begin = true;
    for (uint32_t i = 0; i < 32; i++) {
        if ((cfg & 1) == 1 ) {
            if (cfg_convert_begin) {
                cfg_str = to_string(i);
                cfg_convert_begin = false;
            }
            else
                cfg_str += " " + to_string(i);
        }
        cfg = cfg >> 1;
    }
    return cfg_str;
}

