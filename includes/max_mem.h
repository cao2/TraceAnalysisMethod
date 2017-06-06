//
//  msgs.cpp
//  
//
//  Created by Yuting Cao on 4/7/16.
//
//

#include <vector>
#include <stdlib.h>
#include <iostream>
#include <fstream>

class max{


private:
    vector<message_t> trace;
public:
    msgs() {}
    
    uint32_t max_mem(char *filename){
        
        uint32_t rst=0;
        ifstream trace_file(filename);
        ofstream msg_file;
        
        if (trace_file.is_open()) {
            std::string line;
            while (getline(trace_file, line)){
                linenm++;
                uint32_t state=0;
                
                for (uint32_t i = 0; i < line.size(); i++)
                    if (line.at(i) == ',') {
                        pl[state] = i;
                        state++;
                    }
                //cout<<endl<<line<<endl;
                uint32_t j =0;
                string tmp_str = line.substr(0, 52);
                if (tmp_str.at(0)=='1'){
                    new_msg.src = cpu0;
                    new_msg.dest = cache0;
                    //new_msg //
                    if (tmp_str.substr(1,2)=="10")
                        new_msg.cmd = rd;
                    else if (tmp_str.substr(1,2)=="11")
                        new_msg.cmd =pwr;
                    else
                        new_msg.cmd = wt;
                    new_msg.addr = stol(tmp_str.substr(3,6));
                    //new_msg.addr=0;
                    msg_file<<new_msg.toString()<<"\n";
                    trace.push_back(new_msg);
                    //cout<<new_msg.toString()<<endl;

                    num++;
                }
                
                for (j=1; j<17; j++) {
                    tmp_str = line.substr(pl[j-1]+2,52);
                    //cout<<"start:"<<j<<":"<<tmp_str<<endl;
                    if (j==14 or j==15)
                        tmp_str = tmp_str.substr(1,52);
                    if (tmp_str.at(0)=='1'){
                        //cout<<"inside : "<<tmp_str<<endl;
                        new_msg.src = srcs[j];
                        new_msg.dest= dests[j];
                        if (j==10 or j==11 or j==12 or j==13)
                            new_msg.cmd=snp;
                        else if (j==8 or j==9 or j==16 )
                            new_msg.cmd=wb;
                        else if(tmp_str.substr(1,2)=="10")
                            new_msg.cmd=rd;
                        else if(tmp_str.substr(1,2)=="11")
                            new_msg.cmd=pwr;
                        else
                            new_msg.cmd=wt;
                        //write to msg file
                        msg_file<<new_msg.toString()<<"\n";
                        new_msg.addr = stol(tmp_str.substr(3,6));
                        //new_msg.addr=0;
                        trace.push_back(new_msg);
                        //cout<<new_msg.toString()<<endl;
                        num++;
                    }
                }
                
                tmp_str = line.substr(pl[16]+2,1);
                if (tmp_str.at(0)=='1') {
                    new_msg.src = srcs[17];
                    new_msg.dest = dests[17];
                    new_msg.cmd = wb;
                    msg_file<<new_msg.toString()<<"\n";
                    new_msg.addr = stol(tmp_str.substr(3,6));
                    //new_msg.addr=0;
                    trace.push_back(new_msg);
                    //cout<<new_msg.toString()<<endl;

                    num++;

                }
            }
            cout<<"finished"<<endl;
            trace_file.close();
            msg_file.close();
            
        }
        else {
            cout << "Unable to open file" << endl;
        }
        cout << "Info: read " << trace.size() << " messages." << endl;
    }
    
    vector<message_t> getMsgs(){
        return trace;
    }
};
