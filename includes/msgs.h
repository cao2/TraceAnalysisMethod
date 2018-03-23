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
    
    uint32_t tst_srcs[32];
    uint32_t tst_dests[32];
    uint32_t lengths[75];
    message_t last_valid[75];
    
    message_t new_msg;
    bool addr_en;
    bool cmd;
    bool tag;
    bool uniq_en;
    //int offset;
    bool control;
    vector<int> traced_signal;
    vector<int> be_signal;
    bool rd_en, wt_en, pwron_en,pwroff_en;
    int id_offset;
public:
    msgs() {
        num_sigs=75;
        addr_en=true;
        cmd=true;
        tag=true;
        addr_en=true;
        //offset=21;
        control=false;
        uniq_en=false;
        id_offset=0;
        rd_en= wt_en=pwron_en=pwroff_en=true;
    }
    void set_cmd_en(bool x, bool y, bool a, bool b){
        rd_en=x;
        wt_en=y;
        pwron_en=a;
        pwroff_en=b;
    }
    void set_control(bool x){
        control=x;
    }
    void set_idoffset(int x){
        id_offset=x;
    }
    void set_addr_en(bool addr){
        addr_en=addr;
    }
    bool get_tag_en(){
        return tag;
    }
    uint16_t ip_state(string j){
        int x=stoi(j,nullptr,2);
        if (x==0)
            return cpu0;
        else if (x==1)
            return cpu1;
        else if (x==2)
            return cache0;
        else if (x==3)
            return cache1;
        else if (x==4)
            return membus;
        else if (x==5)
            return mem;
        else if (x==6)
            return gfx;
        else if (x==7)
            return pwr;
        else if (x==8)
            return audio;
        else if (x==9)
            return usb;
        else if (x==10)
            return uart;
        else if (x==11)
            return cache0+100;
        else if (x==12)
            return cache1+100;
        else if (x==13)
            return membus+100;
        else if (x==14)
            return gfx+50;
        else if (x==15)
            return audio+50;
        else if (x==16)
            return usb+50;
        else if (x==17)
            return uart+50;
        else
            return null_cfg;
        
    }
    void init_transaction(int selec){
        num_sigs=32;
        if (selec==22){
            traced_signal.push_back(0);
            traced_signal.push_back(1);
            traced_signal.push_back(2);
            traced_signal.push_back(3);
            cmd=true;
            tag=false;
            addr_en=false;
            cout<<"selected 22"<<endl;
        }
        else if (selec==0)
            for (int i=0;i<num_sigs;i++)
                traced_signal.push_back(i);
        else{
            //push the begining and ending event of each protocol
            traced_signal.push_back(1);
            traced_signal.push_back(2);
            traced_signal.push_back(3);
            traced_signal.push_back(4);
            for (uint16_t x=25; x<33;x++)
                traced_signal.push_back(x);
            //12 In total
            if (selec==1)
            {
                control=true;
                //select two cache response and up_snp_res
                traced_signal.push_back(7);
                traced_signal.push_back(8);
                traced_signal.push_back(14);
            }
            else if (selec==2)
            {
                //15 in total
                traced_signal.push_back(7);
                traced_signal.push_back(8);
                traced_signal.push_back(14);
                traced_signal.push_back(9);
                traced_signal.push_back(10);
                for(uint16_t i=15;i<25;i++)
                    traced_signal.push_back(i);
                
                
            }
            else if (selec==3){
                //13 in total
                traced_signal.push_back(7);
                traced_signal.push_back(8);
                traced_signal.push_back(14);
                for(uint16_t i=15;i<25;i++)
                    traced_signal.push_back(i);
            }
        }
        cout<<"traced_signal size is "<<traced_signal.size()<<endl;
        tst_srcs[0]=cpu0;       tst_dests[0]=cache0;        //cpu_req1
        tst_srcs[1]=cache0;     tst_dests[1]=cpu0;          //cpu_res1
        tst_srcs[2]=cpu1;       tst_dests[2]=cache1;        //cpu_req2
        tst_srcs[3]=cache1;     tst_dests[3]=cpu1;          //cpu_res2
        
        tst_srcs[4]=gfx;     tst_dests[4]=membus;       //gfx up req
        tst_srcs[5]=membus;     tst_dests[5]=gfx;        //gfx up res
        tst_srcs[6]=audio;     tst_dests[6]=membus;        //audio up req
        
        tst_srcs[7]=membus;     tst_dests[7]=audio;        //audio up res
        tst_srcs[8]=uart;     tst_dests[8]=membus;        //uart up req
        tst_srcs[9]=membus;     tst_dests[9]=uart;        //uart up res
        
        tst_srcs[10]=usb;    tst_dests[10]=membus;       //usb up req
        tst_srcs[11]=membus;    tst_dests[11]=usb;       //usb up res
        tst_srcs[12]=cache0;    tst_dests[12]=membus;       //bus req1
        
        tst_srcs[13]=cache1;    tst_dests[13]=membus;       //bus req 2
        tst_srcs[14]=membus;    tst_dests[14]=cache0;       //bus res1
        
        tst_srcs[15]=membus;    tst_dests[15]=cache1;       //bus res 2
        
        tst_srcs[16]=membus;    tst_dests[16]=mem;       //mem read
        tst_srcs[17]=membus;    tst_dests[17]=mem;          //mem write
        tst_srcs[18]=membus;    tst_dests[18]=gfx;          //gfx read
        tst_srcs[19]=membus;       tst_dests[19]=gfx;       //gfx write
        tst_srcs[20]=membus;       tst_dests[20]=audio;        //audio read
        
        tst_srcs[21]=membus;    tst_dests[21]=audio;          //audio write
        tst_srcs[22]=membus;    tst_dests[22]=usb;          //usb read
        tst_srcs[23]=membus;    tst_dests[23]=usb;       //usb write
        tst_srcs[24]=membus;    tst_dests[24]=uart;        //uart read
        tst_srcs[25]=membus;    tst_dests[25]=uart;          //uart write
        
        tst_srcs[26]=membus;    tst_dests[26]=cache0;          //up snop req
        tst_srcs[27]=cache0;    tst_dests[27]=membus;           //up snp res
        tst_srcs[28]=cache1;       tst_dests[28]=cache0;       //snp req1
        tst_srcs[29]=cache0;       tst_dests[29]=cache1;       //snp req2
        
        tst_srcs[30]=cache0;    tst_dests[30]=cache1;          //snp res1
        tst_srcs[31]=cache1;    tst_dests[31]=cache0;          //snp res2
    }
    
    
    void init(int selec){
        cout<<"at init "<<endl;
        if (selec==21){
            traced_signal.push_back(0);
            traced_signal.push_back(1);
        }
        else if (selec==22){
            traced_signal.push_back(0);
            traced_signal.push_back(1);
            traced_signal.push_back(2);
            traced_signal.push_back(3);
            cmd=true;
            tag=false;
            addr_en=false;
        }
        else {
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
            if (selec==0){
                for (int i=0;i<75;i++)
                    traced_signal.push_back(i);
                
            }
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
                    else if (selec==19){
                        cmd=false;
                        tag=true;
                        addr_en=true;
                    }
                    else if (selec==20){
                        cmd=false;
                        tag=true;
                        addr_en=true;
                        uniq_en=true;
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
                        tag=true;
                        addr_en=true;
                        uniq_en=true;
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
                        tag=true;
                        addr_en=true;
                        uniq_en=true;
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
    void parse_transaction(std::string line){
        
        vector<message_t>().swap(trace);//empty trace file
        int pl[num_sigs+3];
        //get the comma
        uint16_t state=1;
        pl[0]=-1;
        for (uint32_t i = 0; i < line.size(); i++)
            if (line.at(i) == ',') {
                pl[state] = i;
                state++;
            }
        string tmp_str;
        int num =0;
        for(uint16_t j=1;j<num_sigs+1;j++){
            //first check whether this message is selected
            if (std::find(std::begin(traced_signal), std::end(traced_signal),j)!=std::end(traced_signal))
            {
                tmp_str = line.substr(pl[j-1]+1,37);
                //parse_message(tmp_str);
//                //cout<<j<<": "<<tmp_str<<endl;
//                if (tmp_str.at(0)=='1'){//val
//                    new_msg.src=(tmp_str.substr(1,5))%50;
//                    new_msg.dest=(tmp_str.substr(6,5))%50;
//                    if (j<25 && j>14)//this is axi monitor{
//                    {
//                        if (j%2==0)//write
//                            new_msg.cmd=rd;
//                        else
//                            new_msg.cmd=wt;
//
//                    }
//                   else  if (tmp_str.substr(11,8)=="01000000"){
////                        if (new_msg.src>50)
////                            new_msg.cmd=rdreq;
////                        else if (new_msg.dest>50){
////                            if (control==true){
////                                //cout<<j<<" :" <<line.substr(pl[num_sigs])<<endl;
////                                if (line.substr(pl[num_sigs+j%5-2]+1,1)=="0")
////                                    new_msg.cmd=rdmiss;
////                                else
////                                    new_msg.cmd=rdhit;
////                            }
////                            else
////                                new_msg.cmd=rdres;
////                        }
////                        else
//                        if (j==5||j==6||j==13)
//                            new_msg.cmd= rdreq;
//                        else if (j==7 || j==8||j==14){
//                            if (control==true){
//                                //cout<<j<<" :" <<line.substr(pl[num_sigs])<<endl;
//                                if (line.substr(pl[num_sigs+j%5-2]+1,1)=="0")
//                                    new_msg.cmd=rdmiss;
//                                else
//                                    new_msg.cmd=rdhit;
//                            }
//                            else
//                                new_msg.cmd=rdres;
//                        }
//                        else if (j>24 and j<33)
//                            new_msg.cmd=uprd;
//                        else
//                            new_msg.cmd=rd;
//                    }
//                    else if (tmp_str.substr(11,8)=="00100000")
//                        new_msg.cmd =pwron;
//                    else if (tmp_str.substr(11,8)=="00010000")
//                        new_msg.cmd =pwroff;
//                    else if (tmp_str.substr(11,8)=="10000000")
//                    {
//                        if (j==5 or j==6 or j==13)
//                            new_msg.cmd=wtreq;
//                        else if (j==7 or j==8 or j==14){
//                            if (control==true){
//                                //cout<<j<<" :" <<line.substr(pl[num_sigs])<<endl;
//                                //cout<<(pl[num_sigs+j%5-2]+1,1)<<endl;
//                                if (line.substr(pl[num_sigs+j%5-2]+1,1)=="0")
//                                    new_msg.cmd=wtmiss;
//                                else
//                                    new_msg.cmd=wthit;
//                            }
//                            else
//                                new_msg.cmd=wtres;
//                        }
//                        else if (j>24 and j<33)
//                            new_msg.cmd=upwt;
//                        else
//                            new_msg.cmd=wt;
//                    }
//                    else
//                        cout<<"cmd doesn't looks right. Error 1   " <<tmp_str.substr(11,8)<<endl;
//
//                    //new_msg.adr=stoi(tmp_str(27,2),nullptr,2);
//                    //Currently we use the value of tag only, address is redundant
//
//
//                    if (j<25 && j>14){
//                        new_msg.tag=0;
//                        new_msg.addr=0;
//                    }
//                    else{
//                        new_msg.tag=new_msg.get_tag(stoi(tmp_str.substr(19,8),nullptr,2));
//                        new_msg.addr=stoi(tmp_str.substr(27,8),nullptr,2);
//                    }
                if (tmp_str.at(0)=='1'){
                    new_msg=parse_message(tmp_str);
                    if (j<25 && j>14)//this is axi monitor{
                    {
                         if (j%2==0)//write
                            new_msg.cmd=wt;
                         else
                            new_msg.cmd=rd;
                    }
                    trace.push_back(new_msg);
                    num++;
                    last_valid[j]=new_msg;
                    //                    cout<<j<<": "<<tmp_str<<endl;
                    //
                    //                    cout<<j<<": "<<new_msg.toString()<<endl;
                }
                //this is the type of tst_t
                
                //sender 5
                //receiver 5
                //cmd 8
                //tag 8
                //id 2
                //adr 2
                
            }
        }
        
    }
    
    message_t parse_message(std::string tmp_str){
        message_t new_msg;
        if (tmp_str.at(0)=='1'){//val
            
            uint32_t linkid= stoi(tmp_str.substr(1,5),nullptr,2);
            
            new_msg.src=tst_srcs[linkid];
            new_msg.dest=tst_dests[linkid];
                                  
            //check command
            //read
            //axi monitor
            if ((linkid>15) and (linkid <26)){
                if (linkid % 2==0)
                    new_msg.cmd=rd;
                else
                    new_msg.cmd=wt;
                if ((tmp_str.substr(6,8))=="00000001"){
                    new_msg.src=tst_srcs[linkid];
                    new_msg.dest=tst_dests[linkid];
                }
                else if ((tmp_str.substr(6,8))=="00000010"){
                    new_msg.src=tst_dests[linkid];
                    new_msg.dest=tst_srcs[linkid];
                }
                else
                    cout<<"axi cmd error"<<endl;
            }
            else if (tmp_str.substr(6,8)=="01000000"){
                if ((linkid==28) or (linkid==29) or (linkid==26))
                    new_msg.cmd=rdreq;
                else if ((linkid==30) or (linkid==31) or (linkid==27)){
                    new_msg.cmd=rdres;
                }
                else if ((linkid>3) and (linkid<12))
                    new_msg.cmd=uprd;
                else
                    new_msg.cmd=rd;
            }
            else if (tmp_str.substr(6,8)=="00100000")
                new_msg.cmd =pwron;
            else if (tmp_str.substr(6,8)=="00010000")
                new_msg.cmd =pwroff;
            else if (tmp_str.substr(6,8)=="10000000")
            {
                if ((linkid==28) or (linkid==29) or (linkid==26))
                    new_msg.cmd=wtreq;
                else if ((linkid==30) or (linkid==31) or (linkid==27)){
                    new_msg.cmd=wtres;
                }
                else if ((linkid>3) and (linkid<12))
                    new_msg.cmd=upwt;
                else
                    new_msg.cmd=wt;
            }
            else
                cout<<"cmd doesn't looks right. Error 1   " <<tmp_str.substr(11,8)<<endl;
            
            if (cmd==false)
                new_msg.cmd=0;
            if (tag==true)
                new_msg.tag=new_msg.get_tag(stoi(tmp_str.substr(14,8),nullptr,2));
            else
                new_msg.tag=0;
            if (addr_en==true)
                new_msg.addr=stoi(tmp_str.substr(22,8),nullptr,2)+1;
            else
                new_msg.addr=0;
            //last_valid[j]=new_msg;
           //cout<<"new_msg link id "<<linkid <<" sequence id "<< new_msg.addr <<" "<<tmp_str<<endl;
        }
        
        return new_msg;
    }
    void parse(std::string line){
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
        if (tmp_str.at(0)=='1'){
            new_msg.src = cpu0;
            new_msg.dest = cache0;
            new_msg.cmd=0;
            //new_msg //
            if (tmp_str.substr(1,8)=="01000000" &&rd_en)
                new_msg.cmd = rd;
            else if (tmp_str.substr(1,8)=="00100000"&& pwron_en)
                new_msg.cmd =pwron;
            else if (tmp_str.substr(1,8)=="00010000"&& pwroff_en)
                new_msg.cmd =pwroff;
            else if (tmp_str.substr(1,8)=="10000000" && wt_en)
                new_msg.cmd = wt;
            else
                cout<<"cmd doesn't looks right. Error 1"<<endl;
            //set address bits
            if (addr_en)
                new_msg.addr = stol(tmp_str.substr(17+id_offset,8-id_offset),nullptr,2)+1;
            else
                new_msg.addr=0;
            
            //set tag bits
            if (tag)
                new_msg.tag=new_msg.get_tag(stoi(tmp_str.substr(9,8),nullptr,2));
            else
                new_msg.tag=0;
            
            
            
            
            if (!cmd &&!uniq_en)
                new_msg.cmd=0;
            new_msg.channel=0;
            trace.push_back(new_msg);
            num++;
            last_valid[0]=new_msg;
        }
        for (j=1; j<num_sigs; j++)
            if (std::find(std::begin(traced_signal), std::end(traced_signal),j)!=std::end(traced_signal))
            {
                tmp_str = line.substr(pl[j-1]+1,lengths[j]);
                
                if (j>66 )
                    if (tmp_str.size()<lengths[j]){
                        break;}
                
                if (lengths[j]>=89 && tmp_str.at(0)=='1'){
                    
                    new_msg.src = srcs[j];
                    new_msg.dest = dests[j];
                    if (tmp_str.substr(1,8)=="01000000" &&rd_en){
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
                        
                    }
                    else if (tmp_str.substr(1,8)=="00100000" && pwron_en)
                        new_msg.cmd =pwron;
                    else if (tmp_str.substr(1,8)=="00010000" && pwroff_en)
                        new_msg.cmd =pwroff;
                    
                    else if (tmp_str.substr(1,8)=="10000000" &&wt_en){
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
                    else{
                        new_msg.cmd=0;
                        cout<<"cmd doesn't looks right. Error 1 "<<endl;
                    }
                    if (addr_en)
                        new_msg.addr = stol(tmp_str.substr(17+id_offset,8-id_offset),nullptr,2)+1;
                    else
                        new_msg.addr=0;
                    
                    //set tag bits
                    if (tag)
                        new_msg.tag=new_msg.get_tag(stoi(tmp_str.substr(9,8),nullptr,2));
                    else
                        new_msg.tag=0;
                    
                    bool uniq= uniq_en && std::find(std::begin(be_signal), std::end(be_signal),j)!=std::end(be_signal);
                    if (!cmd &&!uniq )
                        new_msg.cmd=0;
                    new_msg.channel=j;
                    trace.push_back(new_msg);
                    last_valid[j]=new_msg;
                    num++;
                    
                    
                }
                else if (lengths[j]==1 and tmp_str.at(0)=='1' and j!=6 and j!=9 and j!=12){
                    
                    new_msg.src=srcs[j];
                    new_msg.dest=dests[j];
                    new_msg.addr=0;
                    new_msg.tag=0;
                    new_msg.cmd=0;
                    bool valid=false;
                    //                        this part is commented because I can't find my tag and id anymore,
                    //                        maybe the file is lost or something, shit!
                    if (j==17 || j==19){
                        //cout<<j<<": "<<line.substr(pl[74],39)<<endl;
                        if (tag)
                            new_msg.tag=new_msg.get_tag(stoi(line.substr(pl[75]+1,8),nullptr,2));
                        if (addr_en){
                            new_msg.addr=stoi(line.substr(pl[74]+1+id_offset,8-id_offset),nullptr,2)+1;
                        }
                    }
                    else if (j==21 || j==23){
                        if (tag)
                            new_msg.tag=new_msg.get_tag(stoi(line.substr(pl[77]+1,8),nullptr,2));
                        if (addr_en){
                            new_msg.addr=stoi(line.substr(pl[76]+1,8),nullptr,2)+1;
                        }
                    }
                    if (j==17 || j==25||j==33 ||j==41||j==49 ){
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
                        if (!cmd )
                            new_msg.cmd=0;
                        new_msg.channel=j;
                        trace.push_back(new_msg);
                        cout<<j<<": "<<new_msg.toString()<<endl;
                        last_valid[j]=new_msg;
                        num++;
                        
                    }
                    
                }
                
            }
        
    }
    message_t * get_last(){
        return last_valid;
    }
    bool check_last_valid(const vector<message_t>& other){
        
        for(int i=0;i<75;i++){
            //not valid 6 9 12 17 19 21 23 25 27 29 31 33 35 37 39 41 43 45 47 49 51 53 55
            bool match_flag=false;
            if (other[i].src==last_valid[i].src)
                if (other[i].dest==last_valid[i].dest)
                    if (other[i].cmd==last_valid[i].cmd)
                        if (other[i].tag==last_valid[i].tag)
                            if (other[i].addr==last_valid[i].addr)
                                match_flag=true;
            if (match_flag==false){
                cout<< i<<": "<< other[i].toString() <<"::::::::::"<<last_valid[i].toString()<<endl;
                return false;
            }
        }
        cout<<"all message matched"<<endl;
        return true;
    }
    vector<message_t> getMsgs(){
        return trace;
    }
};

