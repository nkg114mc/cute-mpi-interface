/*
 * task.h
 *
 *  Created on: Sep 1, 2013
 *      Author: mc
 */

#ifndef TASK_H_
#define TASK_H_

#include <string>
#include <vector>

using namespace std;

struct opening_info_t {
	string file;
	string format;
	string order;
	string plies;
	string start;
};

struct option_t {
	int rounds;
	string tournament_type;
	int n_engine;
	string engine_info[1024];
	string openning_info_str;
	opening_info_t open_info;
	string other_option;
	string pgnout;

	// mpi size and rank
	int size;
	int rank;

	// path of cutechess-cli
	string cute_exe_path;

	// all subpgn
	int n_sub_pgnout;
	string sub_pgnout[1024];
};

struct match_task_t {
	int player1;
	int player2;
	int rounds;
	string opening_info;
	string pgnout_info;
	string cmd;
	//int type_flag;
};

struct player_pair_t {
	int player1;
	int player2;
};


struct task_stack_t {
	vector<match_task_t> stack;
	string pgnout_path;
};

#endif /* TASK_H_ */
