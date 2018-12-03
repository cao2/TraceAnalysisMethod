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
bool memcheck = false;
bool order_en = true;
//vector<message_t> uniq_msg;
set<message_t> uniq_msg;
vector<message_t> trace_tt;

int main(int argc, char *argv[]) {
	vector < message_t > trace;
	string filename(argv[1]);
	ifstream trace_file(filename);
	cout << "trace name is " << filename << endl;

	if (trace_file.is_open()) {
		message_t new_msg;
		std::string line;
		ofstream myfile ("bin/translated_lpn.txt");
		ofstream myfile1 ("bin/abstract_trace.txt");
		msgs* readmsg = new msgs();
		readmsg->init_transaction(0);
		while (getline(trace_file, line)) {
				if (line.at(32) == '1') {
					//First file happend at one clock cycle
					if (line.at(0) == '1') {
						if ((line.substr(6, 8)) != "11111111") {
							new_msg = readmsg->parse_message(line);
							myfile << new_msg.toString() << "	" << "----" << endl;
							int msg_index = readmsg->get_msg_index(line);
							myfile1 << msg_index << "	" << "----" << endl;
						}
					}
				}
				else if (line.at(0) == '1') {
					//Same clock cycle as the previous msg
					new_msg = readmsg->parse_message(line);
					myfile<<new_msg.toString()<<endl;
					int msg_index = readmsg->get_msg_index(line);
					myfile1<<msg_index<<endl;
				}
		}
		myfile.close();
		myfile1.close();
		cout << "===============================================" << endl;
		cout << "===============================================" << endl;
		cout << "===============================================" << endl;
		cout << "===============================================" << endl;
		
	} else {
		cout << "ERROR 1: Trace file " << filename
				<< " cannot be opened!!!!!!!!" << endl;
	}

	return 0;

}

