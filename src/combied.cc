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
	int tagoffset = 1;
	uint16_t tst_trace = 0;
	int msg_i = 0;

	//obtain process id for memory tracking
	unsigned int pid = getpid();
	//keep track of the max # of scenario
	uint32_t max = 0;
	//clear out the log file
	ofstream memfile;
	memfile.open("bin/log", ios::trunc);
	memfile.close();

	vector<lpn_t*> flow_spec;
	vector < uint32_t > noneed_monitors;

	vector < message_t > trace;
	string filename(argv[1]);
	ifstream trace_file(filename);
	cout << "trace name is " << filename << endl;

	//cout<<filename.substr(filename.length()-3)<<endl;
	if (filename.substr(filename.length() - 3) == "tst")
		tst_trace = 1;
	else if (filename.substr(filename.length() - 4) == "tstt")
		tst_trace = 2;
	//cout<<"tst_trace is "<<tst_trace<<endl;
	//read type of execution scenario from argv[2]
	int sc_type = atoi(argv[2]);
	if (sc_type == 3)
		order_en = true;
	cout << "type of execution scenario is " << sc_type << endl;

	//parse in the signal selection option from argv[3
	msgs* readmsg = new msgs();
	if (tst_trace > 0)
		readmsg->init_transaction(stoi(argv[3]));
	else
		readmsg->init(stoi(argv[3]));
	cout << "signal selection optin: " << stoi(argv[3]) << endl;

	//read in specification from argv[4]
	read_spec x;
	x.parse(argv[4]);
	init(x.get_flow_names(), x.get_num_flows());
	flow_spec = x.get_flow_spec();
	cout << "totoal flows " << flow_spec.size() << endl;

	if (trace_file.is_open()) {
		uint32_t max_msg_line = 0;
		std::string line;
		stack < scenario_t > s_stack;
		s_stack.push(scenario_t());
		stack < scenario_t > new_s_stack;
		//vector<scenario_t> *tmp;
		uint32_t tim = 0;
		vector < pair<vector<scenario_t>, uint32_t> > bad_scenario_vec;
		uint32_t old_size = 100;
		scenario_t new_scenario;
		uint32_t tri;

		vector < uint32_t > flow_spec_flag;
		scenario_t scenario;
		config_t new_cfg;

		string cfg_str;
		message_t new_msg;
		uint32_t total_data_drop = 0;
		uint32_t data_drop = 20;
		uint32_t drop_round = 0;
		uint16_t tmp_drop = 0;
		uint32_t round_i, round_tmp;
		int c_event_odd = 0;
		int c_event_even=0;
		bool tstt_start = false;
		while (getline(trace_file, line)) {

			if (tst_trace == 1) {
				readmsg->parse_transaction(line);
				trace = readmsg->getMsgs();
				tim++;
			} else if (tst_trace == 2) {
				//do something here
				//cout<<"new line "<<endl;
				if (line.at(32) == '1') {
					//this is the first event
					//vector<message_t>().swap(trace_tt);
					//cout<<"new_line"<<endl;
					trace = trace_tt;
					//cout<<"size of trace is "<<trace.size()<<endl;
					//cout<<"size of trace_tst is "<<trace_tt.size()<<endl;
					tim++;
					tstt_start = true;
					tmp_drop = stoi(line.substr(line.length() - 5), nullptr, 2);
					//cout<<"first line"<<endl;
					//cout<<line<<endl;
					if (line.at(0) == '1') {
						//cout<<line<<endl;
						if ((line.substr(6, 8)) == "11111111") {
							//cout << "****************************non msg******"
							//<< line << endl;

							//vector<message_t>().swap(trace);
						} else {
							new_msg = readmsg->parse_message(line);
							uint16_t linkid = stoi(line.substr(1, 5), nullptr,
									2);
							if (linkid < 12 ) {
								if (linkid %2==0)
									c_event_even++;
								else
									c_event_odd++;

							}
							//cout<<"first line "<<line<<endl;
						}
						if (tmp_drop > 0) {
							total_data_drop += tmp_drop;
							data_drop += tmp_drop;
							drop_round += 1;
							cout << "total_data drop: " << total_data_drop
									<< "   data_drop: " << data_drop
									<< "   round: " << drop_round << endl;
						}
					}
				}

				else if (line.at(0) == '1') {
					//cout<<line<<endl;
					new_msg = readmsg->parse_message(line);
					uint16_t linkid = stoi(line.substr(1, 5), nullptr, 2);
					if (linkid < 12 ) {
						if (linkid %2==0)
							c_event_even++;
						else
							c_event_odd++;

					}
					// if (new_msg.src==1 || new_msg.src==3 || new_msg.dest==1 || new_msg.dest==3)
					trace_tt.push_back(new_msg);
					//cout << "add to trace " << line << endl;
					tstt_start = false;
				} else
					tstt_start = false;

			} else if (tst_trace == 0) {
				readmsg->parse(line);
				trace = readmsg->getMsgs();
				tim++;
			}

			//cout<<"parsed one line " <<tim<<endl;
			//readmsg->set_idoffset(6);
			//readmsg->set_cmd_en(true,true,false,false);
			if (tstt_start == true || tst_trace != 2) {
				bool match = true;
				bool flag = false;
				bool fin = false;
				old_size = s_stack.size();
				if (tim % 100 == 0) {
					//dscen(s_stack);
					if (memcheck)
						getMemUsage(pid, "bin/log");
					//max_mem("bin/log");
					//break;
				}
				stack < scenario_t > tmp_stack;
				if (trace.size() > max_msg_line)
					max_msg_line = trace.size();
//				if (trace.size()>0)
//					print_scenario(s_stack.top());
				for (tri = 0; tri < trace.size(); tri++) {
					message_t msg(trace.at(tri));
					cout << tim << "***  " << msg_i++ << " " << msg.toString()
							<< " current scenarios: " << s_stack.size()
							<< " max:" << max << endl << endl;
//					print_scenario(s_stack);
					tmp_stack = s_stack;

					match = false;
					//last_valid[msg.channel]=msg;
					// Match the next message from trace against the current scenario.
					while (!s_stack.empty()) {
						scenario = s_stack.top();
						//if (newflag==false){
						for (uint32_t i = 0; i < scenario.active_t.size();
								i++) {

							flow_instance_t f = scenario.active_t.at(i);
							if ((f.flow_inst->get_tag() == msg.tag
									|| msg.tag == 0)
									&& (f.addr == msg.addr || f.addr == 0))
									//&&(msg.src==mem||msg.src==gfx||msg.src==usb||msg.src==uart||msg.src==audio))
									{
								for (round_i = 0; round_i < drop_round + 1;
										round_i++) {
									if (round_i > 1)
										cout << "drop rouond " << round_i
												<< endl;
									new_cfg = f.flow_inst->accept(msg, f.cfg,
											round_i);
									if (new_cfg != null_cfg && f.time != tim) {
//                                         cout<<"flow instance "<<i<<endl;
//                                cout << "matched "
//                                                << f.flow_inst->get_flow_name()
//                                                << " addr:" << f.addr << endl;

										uint32_t flow_index =
												f.flow_inst->get_index();
										new_scenario = scenario;
										new_scenario.last_valid.at(msg.channel) =
												msg;
										cfg_str = cfg_str_c(new_cfg);
										if (cfg_str
												== cfg_str_c(
														f.flow_inst->get_end_cfg())) {
											fin = true;
											//cout << "finished 17"
//													<< f.flow_inst->get_flow_name()
//													<< " " << cfg_str << endl;
											new_scenario.finished.at(flow_index) =
													new_scenario.finished.at(
															flow_index) + 1;
											//cout<<"1"<<endl;
											if (f.start==true){
												new_scenario.complete.at(flow_index)= new_scenario.complete.at(flow_index)+1;
												cout<<"a new complete flow instance "<<f.flow_inst->get_flow_name() <<":"<<f.addr<<endl;
											}
											if (order_en) {
												order_inst end;
												end.start = false;
												end.addr = f.addr;
												end.flow = flow_index;
												end.time = tim;
												end.idd = f.it;
												new_scenario.order.push_back(
														end);
											}
											new_scenario.active_t.erase(
													new_scenario.active_t.begin()
															+ i);
										} else {
											new_scenario.active_t.at(i).cfg =
													new_cfg;
											new_scenario.active_t.at(i).time =
													tim;
										}
										match = true;
										round_tmp = round_i;
										//cout<<" matched and break, current round_tmp is "<<round_tmp<<endl;
										new_s_stack.push(new_scenario);
										break;
									}
								}
							}
						}

						s_stack.pop();
					}
					if (match == false) {
						//cout<<"false match"<<endl;
						s_stack = tmp_stack;
						bool newflag = false;
						vector<int> newflow;
						vector < config_t > newflowcfg;
						for (round_i = 0; round_i < drop_round + 1; round_i++) {
							//find out if new msg can create a new flow_inst
							//cout<<"drop round "<<round_i<<endl;
							for (uint32_t i = 0; i < flow_spec.size(); i++) {
								lpn_t* f = flow_spec.at(i);

								uint16_t ct = round_i;
								config_t new_cfg = f->accept(msg, ct);
								if (new_cfg != null_cfg
										&& (f->get_tag() % tagoffset
												== msg.tag % tagoffset
												)) {
									cout << " matched new flow "
											<< f->get_flow_name() << " id is "
											<< msg.addr << "tag is "<<msg.tag<<endl;

									newflag = true;
									newflow.push_back(i);
									newflowcfg.push_back(new_cfg);
									//break;
								}
							}
							if (newflag == true) {
								round_tmp = round_i;
								break;
							}
						}
						if (newflag == true) {
							while (!s_stack.empty()) {
								scenario = s_stack.top();
								for (int i = 0; i < newflow.size(); i++) {
									new_scenario = scenario;
									new_scenario.last_valid.at(msg.channel) =
											msg;
									flow_instance_t new_f;
									new_f.flow_inst = flow_spec.at(
											newflow.at(i));
									new_f.cfg = newflowcfg.at(i);
									new_f.addr = msg.addr;
									new_f.time = tim;
									new_f.it = tim;
									if (round_i==0){
										new_f.start=true;
										cout<<"start event is observed here"<<new_f.flow_inst->get_flow_name() <<":"<<new_f.addr<<endl;
									}
									if (newflowcfg.at(i)== new_f.flow_inst->get_end_cfg()) {
//										cout << "finished 17"
//												<< new_f.flow_inst->get_flow_name()
//												<< " " << newflowcfg.at(i) << endl;
										new_scenario.finished.at(newflow.at(i)) =
												new_scenario.finished.at(
														newflow.at(i)) + 1;
										//cout<<"i"<<i<<"  "<<new_scenario.finished.at(newflow.at(i))<<endl;

										if (order_en) {
											order_inst end;
											end.start = false;
											end.addr = new_f.addr;
											end.flow = i;
											end.time = tim;
											end.idd = new_f.it;
											new_scenario.order.push_back(end);
										}

									} else {
										new_scenario.active_t.push_back(new_f);
										//cout<<"add new flow instance " <<flow_spec.at(newflow.at(i))->get_flow_name()<<endl;
										if (order_en) {
											order_inst end;
											end.start = true;
											end.addr = msg.addr;
											end.flow = newflow.at(i);
											end.time = tim;
											end.idd = tim;
											new_scenario.order.push_back(end);
										}
									}


									match = true;
									new_s_stack.push(new_scenario);
//									cout<<"push back new scenario "<<endl;
//									print_scenario(new_scenario);

								}

								s_stack.pop();
							}
							// s_stack.swap(new_s_stack);
							//cout<<"in side new si+stack size "<<s_stack.size()<<endl;
						}
					}
					if (match == true) {
						//cout<<"sucessful match for this msg "<<endl;
						s_stack.swap(new_s_stack);
						cout << "true round_tmp is " << round_tmp << endl;
						data_drop -= round_tmp;
						if (drop_round > data_drop)
							drop_round = data_drop;
						cout << "data dropped is " << data_drop << endl;
						//break;
					} else if (match == false) {
						flag = true;
						cout << "Info: " << trace.at(tri).toString()
								<< " not matched, backtrack." << endl;
						//cout <<"tag "<<trace.at(tri).tag<<endl;
						pair<vector<scenario_t>, uint32_t> tmp_bad;
						s_stack = tmp_stack;
						break;

						//errorfile<<trace.at(tri).toString()<<"line #:"<<tri<<"\n";

					}

					if (fin) {

						cout << "Flow Finished " << endl;

						//dscen(s_stack);
						//flag=true;
						// break;
					}

					if (s_stack.size() > max) {
						max = s_stack.size();
					}
					cout << "======================================" << endl;
					//end of tri for lopp
				}

				if (flag == true) {
					break;
				}
				if (tst_trace == 2) {
					vector<message_t>().swap(trace_tt);
					if (line.at(0) == '1'
							&& (line.substr(6, 8)) != "11111111") {
						trace_tt.push_back(new_msg);
						//cout<<"add to trace"<<endl;
					}
				}
			}
		}

		bool succ = false;
		cout << "===============================================" << endl;
		cout << "===============================================" << endl;
		cout << "===============================================" << endl;
		cout << "===============================================" << endl;
		if (s_stack.size() > 0) {

			//dscen(s_stack);
			for (uint32_t ctt = 0; ctt < s_stack.size(); ctt++) {
				scenario_t good_scen = s_stack.top();
				s_stack.pop();
				if (good_scen.active_t.size() == 0) {
					cout << endl << "***************" << endl
							<< "  Success -  the scenario that matches all messages is"
							<< endl;
					succ = true;
					cout << "maximum tracfic number is " << max_msg_line
							<< endl;
				}
				cout << "possiblity #" << ctt << endl;
				print_scenario(good_scen);

				//readmsg->check_last_valid(good_scen.last_valid);
				break;

				cout << endl;

			}

			if (succ == false)
				cout << "non of the scenarios have finished all its flow"
						<< endl;

		} else if (bad_scenario_vec.size() > 0) {
			cout << endl << "***  Failed - generating the partial scenarios"
					<< endl;
			pair<vector<scenario_t>, uint32_t> bad_scen = bad_scenario_vec.at(
					0);
			uint32_t msg_idx = bad_scen.second;
			message_t msg = trace.at(msg_idx);
			cout
					<< "***  the following partial scenario failed to match message ("
					<< msg_idx << ") " << msg.toString() << endl;

			for (uint32_t ctt = 0; ctt < s_stack.size(); ctt++) {
				scenario_t tmp_print = bad_scen.first.at(ctt);
				print_scenario(tmp_print);
				cout << endl;
			}
		}
		if (memcheck) {
			getMemUsage(pid, "bin/log");
			max_mem("bin/log");
		}
		cout << "max # of flow execution scenario: " << max << endl;
		cout << "***************************************************************************total data drop: " << total_data_drop << endl;
		cout << "current data drop " << data_drop << endl;
		cout << "************************************************************total events*************** "<<msg_i<<endl;
		cout << "************************************************************start events*************** " << c_event_even << endl;
		cout << "************************************************************end events*************** " << c_event_odd << endl;

	} else {
		cout << "ERROR 1: Trace file " << filename
				<< " cannot be opened!!!!!!!!" << endl;
	}

	return 0;

}

