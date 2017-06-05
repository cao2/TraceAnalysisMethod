//
//  vcd_msg.cpp
//  
//
//  Created by Yuting Cao on 4/19/16.
//
//
#include "msg_def.h"
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


class vcd_msg{


private:
    vector<message_t> trace;
public:
    static const uint32_t s_sig=18;
    static const uint32_t clk_time=2000;
    vcd_msg() {
    }

    vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }
    vector<string> split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        split(s, delim, elems);
        return elems;
    }
    
    
    
    void parse(char *filename){
        //define selection files
        uint32_t selection[s_sig]={9,10,11,12,13,14,15,16,17,18,21,22,23,24,25,26,39,40};
        string clk_s="!";
        string rest_s="F";
        string clk_v;
        string rest_v;
        //define signal sources and destinations
        uint32_t srcs[s_sig]={ membus ,         membus ,      cache0 ,    cache1 ,    cpu0 ,    cpu1 ,    membus ,    membus ,    cache0 ,    cache1 ,    cache0 , cache1 , cache0 , cache1 , mem ,   membus , membus,mem };
        uint32_t dests[s_sig]={ cache0 ,        cache1 ,      cpu0 ,       cpu1,      cache0 ,  cache1 ,  cache0 ,    cache1 ,    membus ,    membus ,    membus , membus , membus , membus , membus , mem , mem , membus };
        vector<string> symbol;
        // Open input trace file

        
        ifstream trace_file(filename);
        ofstream msg_file;
        msg_file.open ("vcd_msgs.txt",ios::trunc);
        
        if (trace_file.is_open()) {
            std::string line;
            message_t new_msg;
            bool notend=0;
            //first, get symbols for all
            int ite=0;
            int ind=0;
            bool varstart=0;
            while (getline(trace_file, line)){
                //cout<<line<<endl;
                if (line.substr(0,4)=="$var"){
                    varstart=1;
                    //cout<<"var"<<endl;
                    vector<std::string> x = split(line,' ');
                    if (ite==selection[ind]){
                        symbol.push_back(x.at(3));
                        //cout<<ind<<":"<<symbol.at(ind)<<endl;
                        ind++;
                        if (ind==s_sig)
                            break;
                    }
                    ite++;
                }
                else if (varstart==1)
                    break;
            }
            

            
            uint32_t cycl=0;
            string sigs[s_sig];
            uint32_t j;
            string empt="0000000000000000000000000000000000000000000000000000";
            for (j=0; j<s_sig; j++) {
                sigs[j]=empt;
            }
            
            while (line.at(0)!='#') {
                notend=std::getline(trace_file, line);
            }
            while (notend){
                if(line.at(0)=='#'&&stol(line.substr(1))%clk_time==0){
                    //cout<<"start:"<<line<<endl;
                    //cout<<line<<endl<<"********************************************"<<endl;
                    uint32_t tm = stol(line.substr(1))/clk_time;
                    //cout<<"bibi time: "<<tm<<" cycle:"<<cycl<<endl;
                    
                    if (cycl<tm) {
                        for ( ; cycl<tm; cycl++) {
                            if (clk_v.compare("1")==0 && rest_v.compare("0")==0)
                                for(j=0;j<s_sig;j++){
                                    //cout<<"clock is 1"<<endl;
                                    //cout<<"sig "<<j<<" "<<sigs[j]<<endl;
                                    if (sigs[j].at(0)=='b')
                                        sigs[j]=sigs[j].substr(1);
                                    if (sigs[j].size()>1 && (j==15 || j==14)) {
                                        sigs[j]=sigs[j].substr(1);
                                    }
                                    if (sigs[j].at(0)=='1'){
                                        new_msg.src= srcs[j];
                                        new_msg.dest = dests[j];
                                        if (j==6 ||j==7||j==10 ||j==11)
                                            new_msg.cmd=snp;
                                        else if (j==8 || j==9 || j==16 || j==17)
                                            new_msg.cmd=wb;
                                        else if (sigs[j].substr(1,2)=="10")
                                            new_msg.cmd=wt;
                                        else if (sigs[j].substr(1,2)=="01")
                                            new_msg.cmd=rd;
                                        else if (sigs[j].substr(1,2)=="11")
                                            new_msg.cmd=pwr;
                                        else//not supposed to happen
                                            assert(false);
                                        if (sigs[j].size()>6)
                                            new_msg.addr= stol(sigs[j].substr(3,6));
                                        else
                                            new_msg.addr=0;
                                        msg_file<<new_msg.toString()<<"\n";
                                        //cout<<new_msg.toString()<<endl;
                                        trace.push_back(new_msg);
                                    }
                                }
                        }
                    }
                    notend=getline(trace_file, line);
                    while (notend && line.at(0)!='#'){
                            vector<string> x=split(line,' ');
                            string sym;
                            string val;
                        //cout<<"jj"<<endl;
                            if (x.size()==1){
                                sym=line.substr(line.length()-1);
                                val=line.substr(0,line.length()-1);
                            }
                            else{
                                sym=x.at(1);
                                val=x.at(0);
                            }
                        //cout<<"dd"<<endl;
                            int pos= find(symbol.begin(),symbol.end(),sym)-symbol.begin();
                            //cout<<sym<<" index: "<<pos<<endl;
                            if (pos<symbol.size() && pos>=0) {
                                sigs[pos]=val;
                                //cout<<"write "<<pos<<"___"<<sigs[pos]<<endl;
                            }
                            else if (sym.compare(clk_s)==0)
                                clk_v=val;
                            
                            else if (sym.compare(rest_s)==0)
                                rest_v=val;
                            //finished reading one signal value
                            notend=getline(trace_file, line);
                        }
                    
                    
                    //cout<<"dfd"<<endl;
                    //now finish assigning all signals
                    //cout<<"mimi: clok: "<<clk_v<<" reset: "<<rest_v<<endl;
                    if (clk_v.compare("1")==0 && rest_v.compare("0")==0)
                        for(j=0;j<s_sig;j++){
                            //cout<<"clock is 1"<<endl;
                            //cout<<"sig "<<j<<" "<<sigs[j]<<endl;
                            if (sigs[j].at(0)=='b')
                                sigs[j]=sigs[j].substr(1);
                            if (sigs[j].size()>1 && (j==15 || j==14)) {
                                sigs[j]=sigs[j].substr(1);
                            }
                            //cout<<"here1"<<endl;
                            if (sigs[j].at(0)=='1'){
                                new_msg.src= srcs[j];
                                new_msg.dest = dests[j];
                                //cout<<"here2"<<endl;
                                if (j==6 ||j==7||j==10 ||j==11)
                                    new_msg.cmd=snp;
                                else if (j==8 || j==9 || j==16 || j==17)
                                    new_msg.cmd=wb;
                                else if (sigs[j].substr(1,2)=="10")
                                    new_msg.cmd=wt;
                                else if (sigs[j].substr(1,2)=="01")
                                    new_msg.cmd=rd;
                                else if (sigs[j].substr(1,2)=="11")
                                    new_msg.cmd=pwr;
                                else//not supposed to happen
                                    assert(false);
                                if (sigs[j].size()>6)
                                    new_msg.addr= stol(sigs[j].substr(4,7));
                                else
                                    new_msg.addr=0;
                                
                                msg_file<<new_msg.toString()<<"\n";
                                //cout<<new_msg.toString()<<endl;
                                trace.push_back(new_msg);
                            }
                        }
                    //end of clk and rest
                    cycl++;
                    
                    //cout<<"here3"<<endl;
                }
                else if (line.at(0)=='#'&&stol(line.substr(1))%clk_time!=0){
                    notend=getline(trace_file, line);
                    while (notend && line.at(0)!='#')
                        notend=getline(trace_file, line);
                }
            }

            cout<<"finished"<<endl;
            trace_file.close();
            msg_file.close();
        }
        else {
            cout << "Unable to open file" << endl;
        }
        //cout << "Info: read " << trace.size() << " messages." << endl;
    }
    
    vector<message_t> getMsgs(){
        return trace;
    }
};
