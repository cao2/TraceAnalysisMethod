#include <stdlib.h>/* needed to define exit() */
#include <unistd.h>/* needed to define getpid() */
#include <stdio.h>/* needed for printf() */
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
#include "msgs.h"
#include "read_spec.h"
#include <sys/time.h>
#include <sys/resource.h>
#define CHECK_MEM


using namespace std;
bool memcheck=false;
bool order_en=true;
//vector<message_t> uniq_msg;
set<message_t> uniq_msg;
vector<message_t> trace_tt;


int main(int argc, char *argv[]) {
    int tagoffset=1;
    uint16_t tst_trace=0;
    
    //obtain process id for memory tracking
    unsigned int pid = getpid();
    //keep track of the max # of scenario
    uint32_t max=0;
    //clear out the log file
    ofstream memfile;
    memfile.open("bin/log",ios::trunc);
    memfile.close();

    vector<lpn_t*> flow_spec;
    vector<uint32_t> noneed_monitors;
    
    vector<message_t> trace;
    string filename(argv[1]);
    ifstream trace_file(filename);
    cout<<"trace name is "<<filename<<endl;
    
    //cout<<filename.substr(filename.length()-3)<<endl;
    if (filename.substr(filename.length()-3)=="tst")
        tst_trace=1;
    else if (filename.substr(filename.length()-4)=="tstt")
        tst_trace=2;
//cout<<"tst_trace is "<<tst_trace<<endl;
    //read type of execution scenario from argv[2]
    int sc_type=atoi(argv[2]);
    if (sc_type==3)
        order_en=true;
    cout<<"type of execution scenario is "<<sc_type<<endl;
    
    //parse in the signal selection option from argv[3
    msgs* readmsg=new msgs();
    if (tst_trace>0)
        readmsg->init_transaction(stoi(argv[3]));
    else
        readmsg->init(stoi(argv[3]));
    cout<<"signal selection optin: "<<stoi(argv[3])<<endl;
    
    //read in specification from argv[4]
    read_spec x;
    x.parse(argv[4]);
    init(x.get_flow_names(),x.get_num_flows());
    flow_spec=x.get_flow_spec();
    cout<<"totoal flows "<<flow_spec.size()<<endl;
    
    
    
    
    if (trace_file.is_open()) {
        uint32_t max_msg_line=0;
        std::string line;
        stack<scenario_t> s_stack;
        s_stack.push(scenario_t());
        stack<scenario_t> new_s_stack;
        //vector<scenario_t> *tmp;
        uint32_t tim=0;
        vector<pair< vector<scenario_t>,uint32_t> >  bad_scenario_vec;
        uint32_t old_size=100;
        scenario_t new_scenario;
        uint32_t tri;
        
        vector<uint32_t> flow_spec_flag;
        scenario_t scenario;
        config_t new_cfg;
        
        string cfg_str;
        message_t new_msg;
        uint32_t total_data_drop=0;
        uint32_t data_drop=0;
        uint32_t drop_round=0;
        uint16_t tmp_drop=0;
        bool tstt_start=false;
        while (getline(trace_file, line)){
            
            if (tst_trace==1){
                readmsg->parse_transaction(line);
                trace=readmsg->getMsgs();
                tim++;
            }
            else if (tst_trace==2){
                //do something here
                if (line.at(32)=='1'){
                    //this is the first event
                    //vector<message_t>().swap(trace_tt);
                    trace=trace_tt;
                    tim++;
                    tstt_start=true;
                    tmp_drop=stoi(line.substr(line.length()-5),nullptr,2);
                    if (tmp_drop>0){
                        total_data_drop+=tmp_drop;
                        data_drop+=tmp_drop;
                        drop_round+=1;
                        
                    }
                    cout<<"total_data drop: "<<total_data_drop<<"   data_drop: "<<data_drop<<"   round: "<<drop_round<<endl;
                    //cout<<line<<endl;
                    if (line.at(0)=='1'){
                        //cout<<line<<endl;
                        new_msg=readmsg->parse_message(line);
                    }
                }
               else if (line.at(0)=='1'){
                    //cout<<line<<endl;
                    new_msg=readmsg->parse_message(line);
                  // if (new_msg.src==1 || new_msg.src==3 || new_msg.dest==1 || new_msg.dest==3)
                    trace_tt.push_back(new_msg);
                   //cout<<"add to trace"<<endl;
                    tstt_start=false;
                }
                else
                    tstt_start=false;
                
            }
            else if (tst_trace==0){
                readmsg->parse(line);
                trace=readmsg->getMsgs();
                tim++;
            }
            
            
            
            
            //cout<<"parsed one line " <<tim<<endl;
            //readmsg->set_idoffset(6);
            //readmsg->set_cmd_en(true,true,false,false);
            if (tstt_start==true || tst_trace!=2){
                bool match = true;
                bool flag =false;
                bool fin=false;
                old_size=s_stack.size();
                if(tim%100==0){
                    //dscen(s_stack);
                    if (memcheck)
                        getMemUsage(pid, "bin/log");
                    //max_mem("bin/log");
                    //break;
            }
            stack<scenario_t> tmp_stack;
            if (trace.size()>max_msg_line)
                max_msg_line=trace.size();
            for(tri=0;tri<trace.size();tri++){
                tmp_stack=s_stack;
                match=false;
                message_t msg(trace.at(tri));
                
                //last_valid[msg.channel]=msg;
                cout << tim
                <<"***  " << msg.toString() <<"  "<<s_stack.size() <<" max:" <<max<<endl << endl;
                
                
                bool newflag=false;
                vector<int> newflow;
                vector<config_t> newflowcfg;
                //find out if new msg can create a new flow_inst
                for (uint32_t i = 0; i < flow_spec.size(); i++) {
                    lpn_t* f = flow_spec.at(i);
                    //cout<<"flow name "<<f->get_flow_name()<<endl;
                    uint16_t ct=2;
                    config_t new_cfg = f->accept(msg);
                    if (new_cfg != null_cfg && (f->get_tag()%tagoffset==msg.tag%tagoffset||msg.tag==0 )){
                        cout<<" matched new flow "<<f->get_flow_name()<<endl;
                        //flow_spec_flag.push_back(new_cfg);
                        newflag=true;
                        newflow.push_back(i);
                        newflowcfg.push_back(new_cfg);
                        //break;
                    }
                }
                
                // Match the next message from trace against the current scenario.
                
                while (!s_stack.empty())
                {
                    scenario = s_stack.top();
                    if (newflag==false){
                        for (uint32_t i = 0; i < scenario.active_t.size(); i++) {
                            flow_instance_t f = scenario.active_t.at(i);
                            new_cfg = f.flow_inst->accept(msg, f.cfg);
                            if (new_cfg != null_cfg && f.time!=tim && (f.flow_inst->get_tag()==msg.tag || msg.tag==0) && (f.addr==msg.addr||f.addr==0))
                                //&&(msg.src==mem||msg.src==gfx||msg.src==usb||msg.src==uart||msg.src==audio))
                            {
                                //cout<<"matched "<<f.flow_inst->get_flow_name()<<" addr:"<<f.addr<<endl;
                                
                                uint32_t flow_index = f.flow_inst->get_index();
                                new_scenario = scenario;
                                new_scenario.last_valid.at(msg.channel)=msg;
                                cfg_str=cfg_str_c(new_cfg);
                                if(cfg_str==cfg_str_c(f.flow_inst->get_end_cfg()))
                                {
                                    fin=true;
                                    // cout<<"finished 17"<<f.flow_inst->get_flow_name()<<endl;
                                    new_scenario.finished.at(flow_index)=new_scenario.finished.at(flow_index)+1;
                                    //cout<<"1"<<endl;
                                    
                                    if (order_en){
                                        order_inst end;
                                        end.start=false;
                                        end.addr=f.addr;
                                        end.flow=flow_index;
                                        end.time=tim;
                                        end.idd=f.it;
                                        new_scenario.order.push_back(end);
                                    }
                                    new_scenario.active_t.erase(new_scenario.active_t.begin()+i);
                                }
                                else{
                                    new_scenario.active_t.at(i).cfg = new_cfg;
                                    new_scenario.active_t.at(i).time= tim;
                                }
                                match = true;
                                new_s_stack.push(new_scenario);
                            }
//                            else if (new_cfg!=null_cfg){
//                                
//                               cout<<"new_cfg: "<<new_cfg<<endl;
//                               cout<<"tag "<<msg.tag<<endl;
//                                                          cout<<"addr "<<msg.addr<<endl;
//                                
//                                cout<<"check "<<f.flow_inst->get_flow_name()<<" addr:"<<f.addr<<endl;
//                            }
                        }
                    }
                    // Create a new flow instance to match msg.
                   else{
                       
                        for(int i=0;i<newflow.size();i++){
                            new_scenario = scenario;
                            //cout<<"channel:   "<<msg.channel<<endl;
                            new_scenario.last_valid.at(msg.channel)=msg;
                            //cout<<"why"<<endl;
                            flow_instance_t new_f;
                            new_f.flow_inst = flow_spec.at(newflow.at(i));
                            new_f.cfg = newflowcfg.at(i);
                            new_f.addr = msg.addr;
                            new_f.time=tim;
                            new_f.it=tim;
                           // cout<<"new flow "<<endl;
                            new_scenario.active_t.push_back(new_f);
                        
                            
                            match = true;
                            if (order_en){
                                order_inst end;
                                end.start=true;
                                end.addr=msg.addr;
                                end.flow=newflow.at(i);
                                end.time=tim;
                                end.idd=tim;
                                new_scenario.order.push_back(end);
                            }
                        
                            new_s_stack.push(new_scenario);
                        }
                    }
                    
                    
                    s_stack.pop();
                    
                }
                if (match == false) {
                    flag=true;
                    cout << "Info: " << trace.at(tri).toString() << " not matched, backtrack." << endl;
                    //cout <<"tag "<<trace.at(tri).tag<<endl;
                    pair< vector<scenario_t>,uint32_t> tmp_bad;
                    s_stack=tmp_stack;
                    break;
                    //errorfile<<trace.at(tri).toString()<<"line #:"<<tri<<"\n";
                    
                }
                else
                    s_stack.swap(new_s_stack);
                    
                if (fin){
                    cout<<"Flow Finished "<<endl;

                    //dscen(s_stack);
                    //flag=true;
                   // break;
                }
                
                if (s_stack.size()>max)
                {
                    max= s_stack.size();
                }
                cout << "======================================" << endl;
            
            }
            if (flag==true){
                break;
            }
            if (tst_trace==2 ){
                vector<message_t>().swap(trace_tt);
                if (line.at(0)=='1'){
                    trace_tt.push_back(new_msg);
                    //cout<<"add to trace"<<endl;
                }
            }
            }
        }
        
        bool succ = false;
        cout<<"==============================================="<<endl;
        cout<<"==============================================="<<endl;
        cout<<"==============================================="<<endl;
        cout<<"==============================================="<<endl;
        if (s_stack.size() > 0) {
            
            dscen(s_stack);
            for(uint32_t ctt=0;ctt<s_stack.size();ctt++){
                scenario_t good_scen = s_stack.top();
                s_stack.pop();
                if(good_scen.active_t.size()==0){
                    cout << endl
                    << "***************"<<endl<<
                    "  Success -  the scenario that matches all messages is" << endl;
                    succ = true;
                    cout<<"maximum tracfic number is "<<max_msg_line<<endl;
                }
                cout<<"possiblity #"<<ctt<<endl;
                print_scenario(good_scen);
                readmsg->check_last_valid(good_scen.last_valid);
                break;
                
                
                cout << endl;
                
                
            }
            
            if (succ == false)
                cout<<"non of the scenarios have finished all its flow"<<endl;
            
        }
        else if (bad_scenario_vec.size()>0) {
            cout << endl
            << "***  Failed - generating the partial scenarios" << endl;
            pair<vector<scenario_t>,uint32_t> bad_scen= bad_scenario_vec.at(0);
            uint32_t msg_idx = bad_scen.second;
            message_t msg = trace.at(msg_idx);
            cout << "***  the following partial scenario failed to match message (" << msg_idx << ") " << msg.toString() << endl;
            
            for(uint32_t ctt=0;ctt<s_stack.size();ctt++){
                scenario_t tmp_print=bad_scen.first.at(ctt);
                print_scenario(tmp_print);
                cout << endl;}
        }
        if (memcheck){
            getMemUsage(pid, "bin/log");
            max_mem("bin/log");}
        cout<<"max # of flow execution scenario: "<<max<<endl;
        
    }
    else{
        cout<<"ERROR 1: Trace file "<<filename<<" cannot be opened!!!!!!!!"<<endl;
    }
    
    return 0;
    
}


