//
//  msgs.cpp
//  
//
//  Created by Yuting Cao on 4/1/17.
//
//

#include "msg_def.h"
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <fstream>

class msgs{


private:
    vector<message_t> trace;
    uint32_t num_sigs;
    uint32_t srcs[75];
    uint32_t dests[75];
    uint32_t lengths[75];
    message_t new_msg;
    bool addr_en;
    bool cmd;
    bool tag;
    //int offset;
    bool control;
    vector<int> traced_signal;
    vector<int> be_signal;
public:
    msgs() {
        num_sigs=75;
        addr_en=true;
        cmd=true;
        tag=true;
        addr_en=true;
        //offset=21;
        control=false;
    }
    void set_control(bool x){
        control=x;
    }
    
    void set_addr_en(bool addr){
        addr_en=addr;
    }
    bool get_tag_en(){
        return tag;
    }
    void init(int selec){
        cout<<"selection: "<<selec<<endl;
        be_signal.push_back(0);
        be_signal.push_back(1);
        be_signal.push_back(2);
        be_signal.push_back(3);
        be_signal.push_back(57);
        be_signal.push_back(58);
        be_signal.push_back(59);
        be_signal.push_back(60);
        be_signal.push_back(61);
        be_signal.push_back(62);
        be_signal.push_back(63);
        be_signal.push_back(64);
        if (selec==0)
            for (int i=0;i<75;i++)
                traced_signal.push_back(i);
        else{
            
                traced_signal.push_back(0);
                traced_signal.push_back(1);
                traced_signal.push_back(2);
                traced_signal.push_back(3);
                traced_signal.push_back(57);
                traced_signal.push_back(58);
                traced_signal.push_back(59);
                traced_signal.push_back(60);
                traced_signal.push_back(61);
                traced_signal.push_back(62);
                traced_signal.push_back(63);
                traced_signal.push_back(64);
                //12 In total
            
            if (selec==1||selec>14)
            {
                control=true;
                cout<<"here"<<endl;
                traced_signal.push_back(5);
                traced_signal.push_back(8);
                traced_signal.push_back(11);
                if (selec==15)
                {
                    cmd=true;
                    tag=true;
                    addr_en=false;
                }
                else if (selec==16){
                    cmd=true;
                    tag=false;
                    addr_en=false;
                }
                else if (selec==17){
                    cmd=true;
                    tag=false;
                    addr_en=true;
                }
                else if (selec==18){
                    cmd=true;
                    tag=true;
                    addr_en=true;
                }

            }
            else if (selec%2==0)
            {
                //15 in total
                traced_signal.push_back(5);
                traced_signal.push_back(8);
                traced_signal.push_back(11);
                traced_signal.push_back(13);
                traced_signal.push_back(15);
                traced_signal.push_back(17);
                traced_signal.push_back(21);
                traced_signal.push_back(25);
                traced_signal.push_back(29);
                traced_signal.push_back(33);
                traced_signal.push_back(37);
                traced_signal.push_back(41);
                traced_signal.push_back(45);
                traced_signal.push_back(49);
                traced_signal.push_back(53);
            if (selec ==4){
                    cmd=false;
                    tag=false;
                    addr_en=true;
                }
                else if (selec ==6){
                    cmd=false;
                    tag=true;
                    addr_en=true;
                }
                else if (selec ==8){
                    cmd=true;
                    tag=false;
                    addr_en=true;
                }
                else if (selec ==10){
                    cmd=true;
                    tag=true;
                    addr_en=true;
                }
                else if (selec == 12){
                    cmd=true;
                    tag=true;
                    addr_en=false;
                }
                
            }
            else if (selec%2==1){
                //13 in total
                traced_signal.push_back(5);
                traced_signal.push_back(8);
                traced_signal.push_back(11);
                traced_signal.push_back(17);
                traced_signal.push_back(21);
                traced_signal.push_back(25);
                traced_signal.push_back(29);
                traced_signal.push_back(33);
                traced_signal.push_back(37);
                traced_signal.push_back(41);
                traced_signal.push_back(45);
                traced_signal.push_back(49);
                traced_signal.push_back(53);
                if (selec ==5){
                    cmd=false;
                    tag=false;
                    addr_en=true;
                }
                else if (selec ==7){
                    cmd=false;
                    tag=true;
                    addr_en=true;
                }
                else if (selec ==9){
                    cmd=true;
                    tag=false;
                    addr_en=true;
                }
                else if (selec ==11){
                    cmd=true;
                    tag=true;
                    addr_en=true;
                }
                else if (selec == 13){
                    cmd=true;
                    tag=true;
                    addr_en=false;
                }
                
            }
        }
        
        srcs[0]=cpu0;       dests[0]=cache0;        lengths[0]=89; //cpu_req1
        srcs[1]=cache0;     dests[1]=cpu0;          lengths[1]=89;//cpu_res1
        srcs[2]=cpu1;       dests[2]=cache1;        lengths[2]=89;//cpu_req2
        srcs[3]=cache1;     dests[3]=cpu1;          lengths[3]=89;//cpu_res2
        
        srcs[4]=cache1;     dests[4]=cache0;        lengths[4]=89;//snp_req1
        srcs[5]=cache0;     dests[5]=cache1;        lengths[5]=89;//snp_res1
        srcs[6]=cache0;     dests[6]=cache1;        lengths[6]=1;//snp_hit1
        
        srcs[7]=cache0;     dests[7]=cache1;        lengths[7]=89;//snp_req2
        srcs[8]=cache1;     dests[8]=cache0;        lengths[8]=89;//snp_res2
        srcs[9]=cache1;     dests[9]=cache0;        lengths[9]=1;//snp_hit2
        
        srcs[10]=membus;    dests[10]=cache0;       lengths[10]=89; //up_snp_req
        srcs[11]=cache0;    dests[11]=membus;       lengths[11]=89;//up_snp_res
        srcs[12]=cache0;    dests[12]=membus;       lengths[12]=1;//up_snp_hit
        
        srcs[13]=cache0;    dests[13]=membus;       lengths[13]=89;//bus_req1
        srcs[14]=membus;    dests[14]=cache0;       lengths[14]=569;//bus_res1
        
        srcs[15]=cache1;    dests[15]=membus;       lengths[15]=89;//bus_req2
        srcs[16]=membus;    dests[16]=cache1;       lengths[16]=569;//bus_res2
        
        srcs[17]=membus;    dests[17]=mem;          lengths[17]=1;//rvalid
        srcs[18]=membus;    dests[18]=mem;          lengths[18]=32;//raddr
        srcs[19]=mem;       dests[19]=membus;       lengths[19]=1;//rres
        srcs[20]=mem;       dests[20]=membus;        lengths[20]=1;//rlast
        
        srcs[21]=membus;    dests[21]=mem;          lengths[21]=1;//wvalid
        srcs[22]=membus;    dests[22]=mem;          lengths[22]=32;//waddr
        srcs[23]=mem;       dests[23]=membus;       lengths[23]=1;//wrsp
        srcs[24]=mem;       dests[24]=membus;        lengths[24]=1;//wlast
        
        
        srcs[25]=membus;    dests[25]=gfx;          lengths[25]=1;//rvalid_gfx
        srcs[26]=membus;    dests[26]=gfx;          lengths[26]=32;//raddr
        srcs[27]=gfx;       dests[27]=membus;       lengths[27]=1;//rdvakud_gfx
        srcs[28]=gfx;       dests[28]=membus;       lengths[28]=1;//rlast_gfx
        
        srcs[29]=membus;    dests[29]=gfx;          lengths[29]=1;//wvalid_gfx
        srcs[30]=membus;    dests[30]=gfx;          lengths[30]=32;//waddr_gfx
        srcs[31]=gfx;       dests[31]=membus;       lengths[31]=1;//wrsp_gfx
        srcs[32]=gfx;       dests[33]=membus;       lengths[32]=1;//rlast_gfx
        
        srcs[33]=membus;    dests[33]=uart;         lengths[33]=1;//rvalid_uart
        srcs[34]=membus;    dests[34]=uart;          lengths[34]=32;//raddr
        srcs[35]=uart;      dests[35]=membus;       lengths[35]=1;//rres_uart
        srcs[36]=uart;      dests[36]=membus;       lengths[36]=1;//rlast_uart
        
        
        srcs[37]=membus;    dests[37]=uart;         lengths[37]=1;//wvalid_uart
        srcs[38]=membus;    dests[38]=uart;         lengths[38]=32; //waddr_uart
        srcs[39]=uart;      dests[39]=membus;       lengths[39]=1;//wrsp_uart
        srcs[40]=uart;      dests[40]=membus;       lengths[40]=1;//wlast_uart
        
        
        srcs[41]=membus;    dests[41]=usb;          lengths[41]=1;//rvalid_usb
        srcs[42]=membus;    dests[42]=usb;          lengths[42]=32;//raddr
        srcs[43]=usb;       dests[43]=membus;       lengths[43]=1;//rres_usb
        srcs[44]=usb;       dests[44]=membus;       lengths[44]=1;//rlast_usb
        
        
        srcs[45]=membus;    dests[45]=usb;          lengths[45]=1;//wvalid_usb
        srcs[46]=membus;    dests[46]=usb;          lengths[46]=32;//waddr_usb
        srcs[47]=usb;       dests[47]=membus;       lengths[47]=1;//wrsp_usb
        srcs[48]=usb;       dests[48]=membus;       lengths[48]=1;//wlast_usb
        
        
        srcs[49]=membus;    dests[49]=audio;        lengths[49]=1;//rvalid_audio
        srcs[50]=membus;    dests[50]=audio;        lengths[50]=32;//raddr
        srcs[51]=audio;     dests[51]=membus;       lengths[51]=1;//rres_audio
        srcs[52]=audio;     dests[52]=membus;       lengths[52]=1;//rlast_audio
        
        srcs[53]=membus;    dests[53]=audio;        lengths[53]=1;//wvalid_audio
        srcs[54]=membus;    dests[54]=audio;        lengths[54]=32;//waddr_audio
        srcs[55]=audio;     dests[55]=membus;       lengths[55]=1;//wrsp_audio
        srcs[56]=audio;     dests[56]=membus;       lengths[56]=1;//wlast_audio
        
        
        srcs[57]=gfx;       dests[57]=membus;       lengths[57]=89;//gfx_upreq
        srcs[58]=membus;    dests[58]=gfx;          lengths[58]=89;//gfx_upres
        
        srcs[59]=uart;       dests[59]=membus;       lengths[59]=89;//usb_upreq
        srcs[60]=membus;    dests[60]=uart;          lengths[60]=89;//gfx_upres
        
        srcs[61]=usb;      dests[61]=membus;       lengths[61]=89;//gfx_upreq
        srcs[62]=membus;    dests[62]=usb;         lengths[62]=89;//gfx_upres
        
        srcs[63]=audio;     dests[63]=membus;       lengths[63]=89;//gfx_upreq
        srcs[64]=membus;    dests[64]=audio;        lengths[64]=89;//gfx_upres
        
        srcs[65]=membus;    dests[65]=pwr;          lengths[65]=89;//pwr req
        srcs[66]=pwr;       dests[66]=membus;       lengths[66]=89;//pwr res
        srcs[67]=pwr;       dests[67]=gfx;          lengths[67]=89;//gfx pwr req
        srcs[68]=gfx;       dests[68]=pwr;          lengths[68]=89;//gfx pwr res
        srcs[69]=pwr;       dests[69]=uart;          lengths[69]=89;//uart pwr req
        srcs[70]=uart;       dests[70]=pwr;          lengths[70]=89;//uart pwr res
        srcs[71]=pwr;       dests[71]=usb;          lengths[71]=89;//usb pwr req
        srcs[72]=usb;       dests[72]=pwr;          lengths[72]=89;//usb pwr res
        srcs[73]=pwr;       dests[73]=audio;          lengths[73]=89;//audio pwr req
        srcs[74]=audio;       dests[74]=pwr;          lengths[74]=89;//audio pwr res
    }
    void parse(std::string line){
        
        //cout<<"begin"<<endl;
        vector<message_t>().swap(trace);
        int pl[num_sigs+4];
        int num =0;
        uint32_t state=0;
        for (uint32_t i = 0; i < line.size(); i++)
            if (line.at(i) == ',') {
                        pl[state] = i;
                        state++;
                    }
        uint32_t j =0;
        string tmp_str = line.substr(0, lengths[0]);
        //cout<<tmp_str<<endl;
        if (tmp_str.at(0)=='1'){
                    new_msg.src = cpu0;
                    new_msg.dest = cache0;
                    //new_msg //
                    if (tmp_str.substr(1,8)=="01000000")
                        new_msg.cmd = rd;
                    else if (tmp_str.substr(1,8)=="00100000")
                        new_msg.cmd =pwron;
                    else if (tmp_str.substr(1,8)=="00010000")
                        new_msg.cmd =pwroff;
                    else if (tmp_str.substr(1,8)=="10000000")
                        new_msg.cmd = wt;
                    else
                        cout<<"cmd doesn't looks right. Error 1"<<endl;
           //set address bits
            if (addr_en)
                new_msg.addr = stol(tmp_str.substr(17,8),nullptr,2);
            else
                new_msg.addr=0;
            
            //set tag bits
            if (tag)
                new_msg.tag=new_msg.get_tag(stoi(tmp_str.substr(9,8),nullptr,2));
            else
                new_msg.tag=0;
            
            
            
            
            if (!cmd )
                new_msg.cmd=0;
                
                    //new_msg.addr=0;
                    //msg_file<<new_msg.toString()<<"|";
                    trace.push_back(new_msg);
                    //cout<<"0 : "<<new_msg.toString()<<endl;
                    num++;
                }
                for (j=1; j<num_sigs; j++)
                if (std::find(std::begin(traced_signal), std::end(traced_signal),j)!=std::end(traced_signal))
                    {
                       // cout<<j<<":"<<endl;
                    tmp_str = line.substr(pl[j-1]+1,lengths[j]);
                    
                    if (j>66 )
                        //cout<<j<<": "<<tmp_str<<endl;
                    if (tmp_str.size()<lengths[j]){
                        break;}
                    
                    if (lengths[j]>=89 && tmp_str.at(0)=='1'){
                       
                        new_msg.src = srcs[j];
                        new_msg.dest = dests[j];
                        //new_msg //
//                        if (j==10 ||j ==11){
//                            if (tmp_str.substr(1,8)=="01000000")
//                                new_msg.cmd=snprd;
//                            else if (tmp_str.substr(1,8)=="10000000")
//                                new_msg.cmd=snpwt;
//                            else
//                                cout<<"ERROR SNP RD?WT"<<endl;
//                        
//                        }
                        //else
                        if (tmp_str.substr(1,8)=="01000000"){
                            if (j>56)
                                new_msg.cmd = uprd;
                            else if (j==4||j==7||j==10)
                                new_msg.cmd= rdreq;
                            else if (j==5 || j==8||j==11){
                                if (control){
                                if (line.substr(pl[j]+1,1)=="1")
                                    new_msg.cmd=rdhit;
                                else if (line.substr(pl[j]+1,1)=="0")
                                    new_msg.cmd=rdmiss;
                                }
                                else
                                    new_msg.cmd=rdres;
                            }
                            else
                                new_msg.cmd=rd;
                            //cout<<"1"<<endl;
                        
                        }
                        else if (tmp_str.substr(1,8)=="00100000")
                            new_msg.cmd =pwron;
                        else if (tmp_str.substr(1,8)=="00010000")
                            new_msg.cmd =pwroff;
                        
                        else if (tmp_str.substr(1,8)=="10000000"){
                            if (j>56)
                                new_msg.cmd = upwt;
                            else if (j==4||j==7||j==10)
                                new_msg.cmd= wtreq;
                            else if (j==5 || j==8||j==11){
                                if (control){
                                if (line.substr(pl[j]+1,1)=="1")
                                    new_msg.cmd=wthit;
                                else if (line.substr(pl[j]+1,1)=="0")
                                    new_msg.cmd=wtmiss;
                                }
                                else
                                    new_msg.cmd=wtres;
                            }
                            else
                                new_msg.cmd =wt;
                        }
                        else
                            cout<<"cmd doesn't looks right. Error 1: "<<tmp_str<<endl;
                        if (addr_en)
                            new_msg.addr = stol(tmp_str.substr(17,8),nullptr,2);
                        else
                            new_msg.addr=0;
                        
                        //set tag bits
                        if (tag)
                            new_msg.tag=new_msg.get_tag(stoi(tmp_str.substr(9,8),nullptr,2));
                        else
                            new_msg.tag=0;
                        
                        //msg_file<<new_msg.toString()<<"|";
                        //cout<<"addr :"<<new_msg.addr<<endl;
                        if (!cmd )
                            new_msg.cmd=0;
                        trace.push_back(new_msg);
                        num++;
                        //cout<<j<<": "<<tmp_str<<endl;
                        //if (j==25)
                        //cout<<j<<": "<<new_msg.toString()<<endl;
                        
                    }
                    else if (lengths[j]==1 and tmp_str.at(0)=='1' and j!=6 and j!=9 and j!=12){
                        
                        new_msg.src=srcs[j];
                        new_msg.dest=dests[j];
                        new_msg.addr=0;
                        new_msg.tag=0;
                        bool valid=false;
                        if (j==17 ){
                            if (tag)
                                new_msg.tag=new_msg.get_tag(stoi(line.substr(pl[74]+1,8),nullptr,2));
                            if (addr_en){
                                new_msg.addr=stoi(line.substr(pl[75]+1,8),nullptr,2);
                            }
                        }
                        else if (j==21 ){
                            if (tag)
                                new_msg.tag=new_msg.get_tag(stoi(line.substr(pl[76]+1,8),nullptr,2));
                            if (addr_en){
                                new_msg.addr=stoi(line.substr(pl[77]+1,8),nullptr,2);
                            }
                        }
                        if (j==17 || j==25||j==33 ||j==41||j==49 ){
                            //cout<<j<<": "<<line.substr(pl[j-1]+1,36);
                            new_msg.cmd = rd;
                            valid=true;
                            j++;
                        }
                        else if (j==19 ||j==27||j==35||j==43||j==51){
                            if (line.substr(pl[j]+1,1)=="1"){
                                new_msg.cmd=rd;
                                valid=true;
                            }
                            j++;
                        }
                        else{
                            new_msg.cmd = wt;
                            if (j==21|| j==29||j==37|| j==45  || j==53){
                                valid=true;
                                j++;
                                
                            }
                            if (j==23||j==31||j==39||j==47||j==55){
                                if (line.substr(pl[j]+1,1)=="1"){
                                    new_msg.cmd=wt;
                                    valid=true;
                                }
                                j++;
                            }
                            
                        }
                        if (valid==true){
                            //msg_file<<new_msg.toString()<<"|";
                            if (!cmd )
                                new_msg.cmd=0;
                            trace.push_back(new_msg);
                            num++;
                            //cout<<j<<": "<<line.substr(pl[j-1]+1,10)<<endl;
                            //cout<<j<<" lala: "<<new_msg.toString()<<endl;
                        }
                        
                    }
                    
                }
                //msg_file<<"\n";
            //}
           //cout<<"finished"<<endl;
           // trace_file.close();
           //msg_file.close();
        //cout<<"line finished"<<endl;
            
        //}
        //else {
         //   cout << "Unable to open file" << endl;
        //}
        //cout << "Info: read " << trace.size() << " messages." << endl;
    }
    
    vector<message_t> getMsgs(){
        return trace;
    }
};
