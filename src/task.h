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


struct opening_info_t {
	std::string file;
	std::string format;
	std::string order;
	std::string plies;
	std::string start;
};

struct option_t {
	int rounds;
	std::string tournament_type;
	std::vector<std::string> engine_infos;
	std::string openning_info_str;
	opening_info_t open_info;
	std::string other_option;
	std::string pgnout;

	// path of cutechess-cli
	std::string cute_exe_path;

	// all subpgn
	std::vector<std::string> sub_pgnout;

	inline int get_n_engine() {
		return engine_infos.size();
	}
};

struct match_task_t {
	int player1;
	int player2;
	int rounds;
	std::string opening_info;
	std::string pgnout_info;
	std::string cmd;
};

struct player_pair_t {
	int player1;
	int player2;
};


struct task_stack_t {
	std::vector<match_task_t> stack;
	std::string pgnout_path;
};

#endif /* TASK_H_ */
