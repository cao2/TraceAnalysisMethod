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
//#include "spec.h"
#include <set>
#include <algorithm>    // std::sort
#include <math.h>
#include "msgs.h"
#include "read_spec.h"
//#include "vcd_msg.h"
#include <sys/time.h>
#include <sys/resource.h>
//#include "fuc.h"
#define CHECK_MEM

//uint32_t num_flow=36;

using namespace std;
bool memcheck=false;
//vector<message_t> uniq_msg;
set<message_t> uniq_msg;
void uniq(lpn_t* protocol){/**
                            ofstream uniq_msg;
                            uniq_msg.open("uniq_msg.txt", ios::trunc);
                            uniq_msg.close();**/
    for (message_t x: protocol->msg_vector){
        //        flag=false;
        //        for (message_t y: uniq_msg)
        //            if (y.src==x.src && y.dest==x.dest && y.cmd==x.cmd){
        //                flag=true;
        //                break;
        //            }
        //        if (flag==false)
        //            uniq_msg.push_back(x);
        uniq_msg.insert(x);
    }
}

int main(int argc, char *argv[]) {
   
    unsigned int pid = getpid();
    
    uint32_t max=0;
    struct rusage usage;
    struct timeval start, end;
    getrusage(RUSAGE_SELF, &usage);
    start = usage.ru_stime;
    // Build flow specification
    vector<lpn_t*> flow_spec;
    vector<uint32_t> noneed_monitors;
    
    read_spec x;
    x.parse(argv[4]);
    init(x.get_flow_names(),x.get_num_flows());
    flow_spec=x.get_flow_spec();
    //cout<<"totoal flows "<<flow_spec.size()<<endl;

    //sort(uniq_msg.begin(),uniq_msg.end());
    //uniq_msg.erase( unique( uniq_msg.begin(), uniq_msg.end() ), uniq_msg.end() );
    
    //get the unique mesgs, USEFULLLL!!!!!!!!!!!!!
    /**
     cout<<"unique messages:"<<endl;
     ofstream uni_msg;
     uni_msg.open("uniq_msgs.txt",ios::trunc);
     for(message_t x: uniq_msg){
     uni_msg<<x.toString()<<"\n";
     cout<<x.toString()<<endl;}
     uni_msg.close();
     **/
    /**
    flow_spec.push_back(cpu0_read);
    cpu0_read->set_index(0);
    flow_spec.push_back(cpu1_read);//write1
    cpu1_read->set_index(1);
    flow_spec.push_back(cpu0_write);
    cpu0_write->set_index(2);
    flow_spec.push_back(cpu1_write);
    cpu1_write->set_index(3);
    flow_spec.push_back(write_back0);
    write_back0->set_index(4);
    flow_spec.push_back(write_back1);
    write_back1->set_index(5);
    
    
    flow_spec.push_back(poweron_uart);
    poweron_uart->set_index(6);
    flow_spec.push_back(poweroff_uart);
    poweroff_uart->set_index(7);
    
    flow_spec.push_back(poweron_usb);
    poweron_usb->set_index(8);
    flow_spec.push_back(poweroff_usb);
    poweroff_usb->set_index(9);
    
    flow_spec.push_back(poweron_audio);
    poweron_audio->set_index(10);
    flow_spec.push_back(poweroff_audio);
    poweroff_audio->set_index(11);
    
    flow_spec.push_back(poweron_gfx);
    poweron_gfx->set_index(12);
    flow_spec.push_back(poweroff_gfx);
    poweroff_gfx->set_index(13);
    
    flow_spec.push_back(gfx_upstream_write);
    gfx_upstream_write->set_index(14);
    flow_spec.push_back(gfx_upstream_read);
    gfx_upstream_read->set_index(15);
    
    flow_spec.push_back(audio_upstream_write);
    audio_upstream_write->set_index(16);
    flow_spec.push_back(audio_upstream_read);
    audio_upstream_read->set_index(17);
    
    flow_spec.push_back(usb_upstream_read);
    usb_upstream_read->set_index(18);
    flow_spec.push_back(uart_upstream_read);
    uart_upstream_read->set_index(19);
    **/
    //    vector<uint32_t> flow_inst_cnt;
    //    for(uint32_t ii=0;ii<num_flow+4;ii++){
    //        flow_inst_cnt.push_back(0);}
    
    
    
    ofstream errorfile;
    errorfile.open ("erromsg.txt",ios::trunc);
    
    vector<message_t> trace;
    string filename(argv[1]);
    
    ofstream memfile;
    memfile.open(argv[2],ios::trunc);
    memfile.close();
    ifstream trace_file(filename);
    int traced_signal[75];
    for (int i=0;i<74;i++)
        traced_signal[i]=i;
    if (trace_file.is_open()) {
        
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
        msgs* readmsg=new msgs();
        readmsg->init(stoi(argv[3]));
        while (getline(trace_file, line)){
            tim++;
            stack<uint32_t> tri_stack;
            tri_stack.push(0);
            readmsg->parse(line);
            trace=readmsg->getMsgs();
            bool match = true;
            bool flag =false;
            
            old_size=s_stack.size();
            if(tim%5000==0){
                
                //dscen(s_stack);
                if (memcheck)
                    getMemUsage(pid, argv[2]);
                //break;
            }
            stack<scenario_t> tmp_stack;
            while (tri_stack.size() != 0) {
                tmp_stack=s_stack;
                match=false;
                tri= tri_stack.top();
                tri_stack.pop();
                
                // If index tri reaches the end of trace, store the current scenario.
                if (tri == trace.size()) {
                    //break if a scenario is found to match all messages.
                    break;
                }
                
                message_t msg(trace.at(tri));
                
                
                cout << tim
                <<"***  " << msg.toString() <<"  "<<s_stack.size() <<endl << endl;
                
                
                bool newflag=false;
                vector<int> newflow;
                vector<config_t> newflowcfg;
                //find out if new msg can create a new flow_inst
                for (uint32_t i = 0; i < flow_spec.size(); i++) {
                    lpn_t* f = flow_spec.at(i);
                    //cout<<"flow name "<<f->get_flow_name()<<endl;
                    config_t new_cfg = f->accept(msg);
                    //cout<<"tag :"<<msg.tag<<" : "<<f->get_tag()<<endl;
                    //cout<<"new_cfg="<<new_cfg<<endl;
                    if (new_cfg != null_cfg && (f->get_tag()==msg.tag||msg.tag==0 )){
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
                            if ((new_cfg != null_cfg && f.time!=tim &&(f.flow_inst->get_tag()==msg.tag || msg.tag==0) && (f.addr==msg.addr||((msg.dest==membus&&                                                                                           msg.addr==0)))) {
                               // cout<<"matched "<<f.flow_inst->get_flow_name()<<" addr:"<<f.addr<<endl;
                                uint32_t flow_index = f.flow_inst->get_index();
                                new_scenario = scenario;
                                cfg_str=cfg_str_c(new_cfg);
                                if(cfg_str==cfg_str_c(f.flow_inst->get_end_cfg()))
                                {
                                    // cout<<"finished 17"<<f.flow_inst->get_flow_name()<<endl;
                                    new_scenario.finished.at(flow_index)=new_scenario.finished.at(flow_index)+1;
                                    //cout<<"1"<<endl;
                                    order_inst end;
                                    end.start=false;
                                    end.addr=f.addr;
                                    end.flow=flow_index;
                                    end.time=tim;
                                    end.idd=f.it;
                                    new_scenario.order.push_back(end);
                                    new_scenario.active_t.erase(new_scenario.active_t.begin()+i);
                                }
//                                else if(cfg_str =="28"){
//                                    
//                                    new_scenario.finished.at(flow_index+num_flow)++;                                                       order_inst end;
//                                    end.start=false;
//                                    end.addr=f.addr;
//                                    end.flow=flow_index+6;
//                                    end.time=tim;
//                                    end.idd=f.it;
//                                    new_scenario.order.push_back(end);
//                                    
//                                    new_scenario.active_t.erase(new_scenario.active_t.begin()+i);
//                                }
                                else{
                                    new_scenario.active_t.at(i).cfg = new_cfg;
                                    new_scenario.active_t.at(i).time= tim;
                                }
                                match = true;
                                new_s_stack.push(new_scenario);
                                tri_stack.push(tri+1);
                            }
                        }
                    }
                    // Create a new flow instance to match msg.
                   else{
                        for(int i=0;i<newflow.size();i++){
                        new_scenario = scenario;
                        flow_instance_t new_f;
                        new_f.flow_inst = flow_spec.at(newflow.at(i));
                        //++flow_inst_cnt.at(newflow);
                        new_f.cfg = newflowcfg.at(i);
                        new_f.addr = msg.addr;
                        new_f.time=tim;
                        new_f.it=tim;
                        new_scenario.active_t.push_back(new_f);
                        
                        tri_stack.push(tri+1);
                        match = true;
                        //cout<<"new flow: "<< flow_spec.at(i)->get_flow_name() << "adds: "<< new_f.addr<<endl;
                        order_inst end;
                        end.start=true;
                        end.addr=msg.addr;
                        end.flow=newflow.at(i);
                        end.time=tim;
                        end.idd=tim;
                        new_scenario.order.push_back(end);
                        //cout<<"active : size"<<new_scenario.active_t.size()<<endl;
                        //cout<<"stack size: "<<new_s_stack->size()<<endl;
                        new_s_stack.push(new_scenario);
                        }
                    }
                    
                    
                    s_stack.pop();
                    
                }
                
                if (match == false) {
                    flag=true;
                    tri_stack.push(tri+1);
                    cout << "Info: " << trace.at(tri).toString() << " not matched, backtrack." << endl;
                    pair< vector<scenario_t>,uint32_t> tmp_bad;
                    s_stack=tmp_stack;
                    break;
                    //errorfile<<trace.at(tri).toString()<<"line #:"<<tri<<"\n";
                    
                }
                else
                    s_stack.swap(new_s_stack);
                    
                
                
                if (s_stack.size()>max)
                {
                    max= s_stack.size();
                }
                cout << "======================================" << endl;
            }
            if (flag==true){
                break;
            }
        }
        errorfile.close();
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
                }
                cout<<"possiblity #"<<ctt<<endl;
                print_scenario(good_scen);
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
            getMemUsage(pid, argv[2]);
            max_mem(argv[2]);}
        cout<<"max # of flow execution scenario: "<<max<<endl;
    }
    
    
    return 0;
    
}


