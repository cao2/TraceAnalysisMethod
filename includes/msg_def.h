#ifndef _MSG_H_
#define _MSG_H_

#include <strstream>
#include <string>

using namespace std;

typedef uint64_t config_t;
const config_t null_cfg = 0;


// Define indices for components.
const uint16_t cpu0 = 1;
const uint16_t cache0 = 2;
const uint16_t cpu1 = 3;
const uint16_t cache1 = 4;
const uint16_t membus = 5;
const uint16_t mem = 6;
const uint16_t bus0 =7;
const uint16_t bus1 =8;
const uint16_t pwr=9;
const uint16_t gfx=10;
const uint16_t audio=11;
const uint16_t usb=12;
const uint16_t uart=13;
string blk_vector[14] = {"-", "cpu0", "cache0", "cpu1", "cache1", "membus","mem","bus0","bus1","pwr","gfx","audio","usb","uart"};

// Define commands
typedef uint16_t command_t;
const command_t rd = 1;
const command_t wt = 2;
const command_t snp =3;
const command_t wb =4;
const command_t pwron=5;
const command_t pwroff=6;
const command_t uprd=7;
const command_t upwt=8;
const command_t rdres=9;
const command_t rdreq=10;
const command_t wtreq=11;
const command_t wtres=12;
const command_t snprd=13;
const command_t snpwt=14;
const command_t rdhit=15;
const command_t rdmiss=16;
const command_t wthit=17;
const command_t wtmiss=18;
string cmd_vector[19] = {"-", "rd", "wt", "snp", "wb","pwr_on","pwr_off","uprd","upwt","rdres","rdreq","wtreq","wtres","snprd","snpwt","rdhit","rdmiss","wthit","wtmiss"};

// Define the ranges of memory address spaces.
typedef uint32_t address_t;
const uint32_t NDEF = 0;
const uint32_t INTR = 1;
const uint32_t DMEM = 2;
const uint32_t MMIO = 3;
string addr_vector[4] = {"-", "INTR", "DMEM", "MMIO"};



class message_t
{
public:
    uint32_t src;
    uint32_t dest;
    command_t cmd;
    address_t addr;
    config_t pre_cfg, post_cfg;
    uint16_t tag;
    uint16_t channel;
    message_t() {
        src = dest = cmd = addr =tag =channel= NDEF;
        pre_cfg = post_cfg = null_cfg;
    };
    
    int get_src(string src){
        for (int i=0;i<14;i++)
            if (blk_vector[i].compare(src)==0)
                return i;
        cout<<"src doesn't look right: "<<src<<endl;
        return -1;
    }
    uint16_t get_tag(int x){
        if (x==0)
            return cpu0;
        else if (x==1)
            return gfx;
        else if (x==2)
            return uart;
        else if (x==3)
            return usb;
        else if (x==4)
            return audio;
        else if (x==5)
            return cpu1;
        else
            cout<<"ERROR!!! tag unrecognized "<<x<<endl;
        return cache0;
    }
    int get_cmd(string cmd){
        for (int i=0;i<19;i++)
            if (cmd_vector[i].compare(cmd)==0)
                return i;
        cout<<"ERROR!!!!!!!!!!!!!!!!!cmd doesn't look right: "<<cmd<<endl;
        return -1;
    }
    
    
    message_t(const message_t& other) {
        src = other.src;
        dest = other.dest;
        cmd = other.cmd;
        addr = other.addr;
        pre_cfg = other.pre_cfg;
        post_cfg = other.post_cfg;
        tag=other.tag;
        channel=other.channel;
    }
    
    message_t& operator=(const message_t& other) {
        src = other.src;
        dest = other.dest;
        cmd = other.cmd;
        addr = other.addr;
        pre_cfg = other.pre_cfg;
        post_cfg = other.post_cfg;
        tag=other.tag;
        channel=other.channel;
        return *this;
    }
    
    bool operator==(const message_t& other) const {
        return (src == other.src &&
                dest == other.dest &&
                cmd == other.cmd &&
                addr == other.addr &&
                pre_cfg == other.pre_cfg &&
                post_cfg == other.post_cfg&&
                tag==other.tag&&
                channel==other.channel);
    }

    bool operator <(const message_t &other) const {
        if (src == other.src &&
            dest == other.dest &&
            cmd == other.cmd &&
            addr == other.addr &&
            pre_cfg == other.pre_cfg &&
            post_cfg == other.post_cfg &&
            tag==other.tag)
            return false;
        else
            return 11*src*src*src+7*dest*dest+3*cmd*cmd+tag*tag<11*other.src*other.src*other.src+7*other.dest*other.dest+3*other.cmd*other.cmd+other.tag*other.tag;
    }
    void insert_pre_cfg(uint32_t i) {
        pre_cfg = pre_cfg | (1 << i);
    }
    
    void insert_post_cfg(uint32_t i) {
        post_cfg = post_cfg | (1 << i);
    }
    
    void set_msg(uint16_t source, uint16_t destination, command_t command, address_t address, uint16_t tag_i) {
        src = source;
        dest = destination;
        cmd = command;
        addr = address;
        tag=tag_i;
    }
    
    std::string toString(void) const
    {
        char str[50];
        sprintf( str, "%u", addr);
        //cout<<"addr "<<str;
        //cout<<str<<endl;
        return blk_vector[src] + ":" +  blk_vector[dest] + ":" + cmd_vector[cmd]+":"+blk_vector[tag] + ":" + str;
    }
};

#endif
