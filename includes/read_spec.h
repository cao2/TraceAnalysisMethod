//
//  read_spec.cpp
//  
//
//  Created by Yuting Cao on 5/28/17.
//
//
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "lpn.h"
#include "fuc.h"
struct path_node{
    int sequence;
    int postcfg;
    double condition;
    
};

class read_spec{
private:
    vector<path_node> trees;
    vector<lpn_t*> flow_spec;
    vector<string> flow_names;
    int index;
    bool pt=true;
public:
    read_spec(){
        index=0;
    }
    int findindex(int sequence){
        for (int i=0;i<trees.size();i++){
            if (trees.at(i).sequence==sequence)
                return i;
        }
        return -1;
    }
    int get_num_flows(){
        return index;
    }
    vector<string> get_flow_names(){
        return flow_names;
    }
    vector<lpn_t*> get_flow_spec(){
        return flow_spec;
    }
    void print_all_msg(lpn_t* lpn,ofstream myfile){
        
        
        cout<<"******************lpn "<<lpn->get_flow_name()<<endl;
        for(int i=0;i<lpn->msg_vector.size();i++){
            cout<<lpn->msg_vector.at(i).toString()<<" pre "<<lpn->msg_vector.at(i).pre_cfg<<" post "<<lpn->msg_vector.at(i).post_cfg<<endl;
        }
        
    }
    void parse(string filename){
        ofstream myfile ("bin/lpn.txt");
        string line;
        ifstream lpn_file(filename);
        index=0;
        
        if (lpn_file.is_open()){
            while(getline(lpn_file,line)){
                if ( !line.empty()&&line.at(0)=='*'){
                    
                int max=0;
                //beginning of one lpn
                string flow_name=line;
                flow_names.push_back(flow_name);
                lpn_t* lpn = new lpn_t;
                lpn->set_flow_name(flow_name);
                lpn->set_index(index);
                index++;
                //cout<<"*********************************create new lpn, name is: "<<line<<endl;
                double precondi=1;
                path_node tmp_branch;
                int num_branch=0;
                 vector<path_node>().swap(trees);
                while(getline(lpn_file,line)&& !line.empty()){
                    ////cout<<"size of trees: "<<trees.size()<<endl;
                    message_t msg;
                    
                    msg.post_cfg=(1<<max++);
                    std::istringstream iss;
                    iss.str(line);
                    string srt,dest,cmd,sequence,condition;
                    iss>>srt;
                    iss>>dest;
                    iss>>cmd;
                    iss>>sequence;
                    iss>>condition;
                    ////cout<<"pre:now conditon: "<<precondi<<":"<<stod(condition)<<endl;
                    //cout<<line<<endl;
                    //cout<<"sequence "<<sequence<<endl;
                    msg.src=msg.get_src(srt);
                    msg.dest=msg.get_src(dest);
                    msg.cmd=msg.get_cmd(cmd);
                    bool flag=false;
                    if (trees.size()==0){
                        lpn->set_tag(msg.src);
                        ////cout<<"hdfsfdsere"<<endl;
                        msg.pre_cfg=(1<<0);
                        msg.post_cfg=(1<<max);
                        max++;
                        path_node cur;
                        cur.sequence=stol(sequence);
                        cur.postcfg=msg.post_cfg;
                        cur.condition=stod(condition);
                        trees.push_back(cur);
                        ////cout<<"push back to trees "<<cur.sequence<<" "<<cur.postcfg<<" "<<cur.condition<<endl;
                        precondi=1;
                    }
                    else{
                        if (stod(condition)<precondi){
                            //find the previous one and update its states
                            
                            int pre=findindex(stol(sequence)-1);
                            if (pre>-1){
                               // //cout<<line<<endl;
                                ////cout<<"find, index is "<<pre<<endl;
                                while (num_branch>0)
                                {
                                    num_branch--;
                                    path_node cur=tmp_branch;
                                    trees.push_back(cur);
                                    ////cout<<"******************num branch push back to trees "<<cur.sequence<<" "<<cur.postcfg<<" "<<cur.condition<<endl;
                                }
                                
                                tmp_branch=trees.at(pre);
                                trees.at(pre).sequence=stol(sequence);
                                trees.at(pre).condition=stod(condition);
                                tmp_branch.condition=stod(condition);
                                msg.pre_cfg=trees.at(pre).postcfg;
                                trees.at(pre).postcfg=msg.post_cfg;
                                num_branch=precondi/stod(condition)-1;
                                ////cout<<"update treees at "<<pre<<" sequence:"<<trees.at(pre).sequence<<" postcfg:"<<trees.at(pre).postcfg<<endl;
                            }
                            else
                                cout<<"ERROR 1!!!!"<<endl;
                            
                            }
                        else if (stod(condition)>precondi){
                            
                            while (num_branch>0)
                            {
                                num_branch--;
                                path_node cur=tmp_branch;
                                trees.push_back(cur);
                                ////cout<<"******************num branch push back to trees "<<cur.sequence<<" "<<cur.postcfg<<" "<<cur.condition<<endl;
                            }
                            flag=true;
                            message_t msg_1;
                            msg_1.src=msg.src;
                            msg_1.dest=msg.dest;
                            msg_1.cmd=msg.cmd;
                            msg_1.post_cfg=msg.post_cfg;
                            double sum_condi=0;
                            for (int i=trees.size()-1;i>=0;i--)
                            {
                                path_node tmp=trees.at(i);
                                if (tmp.condition<stod(condition)){
                                    sum_condi+=tmp.condition;
                                    msg_1.pre_cfg=trees.at(i).postcfg;
                                    
                                   
                                    lpn->insert_msg(msg_1);
                                    
                                    if (sum_condi<stod(condition)){
                                        trees.erase(trees.begin()+i);
                                    }

                                }
                                if (sum_condi>=stod(condition)){
                                    trees.at(i).sequence=stol(sequence);
                                    trees.at(i).postcfg=msg_1.post_cfg;
                                    trees.at(i).condition=stol(condition);
                                    break;
                                }
                                
                                                            }
                        }
                        else if (stod(condition)==precondi){
                            //find a pre sequence.
                            int pre=findindex(stol(sequence)-1);
                            if (pre>-1){
                                trees.at(pre).sequence=stol(sequence);
                                trees.at(pre).condition=stod(condition);
                                msg.pre_cfg=trees.at(pre).postcfg;
                                trees.at(pre).postcfg=msg.post_cfg;
                            }
                            else{
                                num_branch--;
                                msg.pre_cfg=tmp_branch.postcfg;
                                path_node cur=tmp_branch;
                                cur.sequence=stol(sequence);
                                cur.postcfg=msg.post_cfg;
                                cur.condition=stod(condition);
                                trees.push_back(cur);
                            }

                        }
                        
                    }
                    if (flag==false){
                        lpn->insert_msg(msg);}
                    precondi=stod(condition);
                }
                    lpn->set_init_cfg(1<<0);
                    lpn->set_end_cfg(1<<(max-1));
                    flow_spec.push_back(lpn);
                    if (pt){
                        myfile<<"******************lpn "<<lpn->get_flow_name()<<endl;
                        for(int i=0;i<lpn->msg_vector.size();i++){
                            myfile<<lpn->msg_vector.at(i).toString()<<" pre "<<cfg_str_c(lpn->msg_vector.at(i).pre_cfg)<<" post "<<cfg_str_c(lpn->msg_vector.at(i).post_cfg)<<endl;
                        }
                        myfile<<endl<<endl<<endl;
                    }
            }
                
            }
        }
        else{
            cout<<"ERROR lpn file "<<filename<<" can't be opened"<<endl;
        }
        myfile.close();
    }
};


