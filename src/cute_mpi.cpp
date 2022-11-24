#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <assert.h>
#include <sys/stat.h>

#include "task.h"
#include "util.h"
#include "open_split.h"


std::string parse_opening_fn(std::string open_info_str, opening_info_t &open_info)
{
	std::stringstream ss("");
	std::string token = "";
	std::string words[256];
	int n_word, j;
	bool bb = false;

	// remove all '='
	for (j = 0; j < open_info_str.length(); j++) {
		if (open_info_str[j] == '=') open_info_str[j] = ' ';
	}

	// split
	ss << open_info_str;
	n_word = 0;
	token = "";
	while (ss >> token) {
		if (token != "") {
			words[n_word] = token;
			n_word++;
		}
		token = "";
	}

	// init
	open_info.file = "";
	open_info.format = "";
	open_info.order = "";
	open_info.plies = "";
	open_info.start = "";

	// find the file path
	int i;
	std::string fpath = "";
	for (i = 0; i < n_word; i++) {
		if (words[i] == "file") {
			fpath = words[i + 1];
			open_info.file = fpath;
		} else if (words[i] == "format") {
			open_info.format = words[i + 1];
		} else if (words[i] == "order") {
			open_info.order = words[i + 1];
		} else if (words[i] == "plies") {
			open_info.plies = words[i + 1];
		} else if (words[i] == "start") {
			open_info.start = words[i + 1];
		}
	}

	return fpath;
}

std::string build_task_cmd(option_t &opt, match_task_t &task)
{
	std::stringstream allcmd("");

	// cutechess-cli exec
	allcmd << opt.cute_exe_path << " ";

	// all other option
	allcmd << opt.other_option << " ";

	// tournament type
	allcmd << "-tournament " << opt.tournament_type << " ";

	// pgnout
	allcmd << "-pgnout " << task.pgnout_info << " ";

	// opening
	allcmd << "-openings ";
	if (opt.open_info.file != "") allcmd << "file="<< task.opening_info << " format=pgn"; // currently only support pgn
	if (opt.open_info.plies != "") allcmd << " plies=" << opt.open_info.plies;
	if (opt.open_info.order != "") allcmd << " order=" << opt.open_info.order;
	if (opt.open_info.start != "") allcmd << " start=" << opt.open_info.start;
	allcmd << " ";

	// rounds
	allcmd << "-rounds " << task.rounds << " ";

	// engin infos
	//   player1
	allcmd << "-engine " << opt.engine_infos[task.player1] << " ";
	//   player2
	allcmd << "-engine " << opt.engine_infos[task.player2] << " ";

	return allcmd.str();
}

void parse_engine_info()
{

}

int split_task(option_t &opt, int n_process, int &n_task, task_stack_t all_task[])
{
	int i, j;
	int n_pairs = 0;
	int n_round = 0;
	std::vector<player_pair_t> possible_pairs;

	const int n_engine = opt.get_n_engine();
	if (n_engine < 2) {
		std::cerr << "Error: at least two engine!" << std::endl;
		return 0;
	}

	// tournament type?
	if (opt.tournament_type == "round-robin") {
		n_pairs = (n_engine * (n_engine - 1)) / 2;
		for (i = 0; i < (n_engine - 1); i++) {
			for (j = (i+1); j < n_engine; j++) {
				player_pair_t new_pair;
				new_pair.player1 = i;
				new_pair.player2 = j;
				possible_pairs.push_back(new_pair);
			}
		}
	} else if (opt.tournament_type == "gauntlet") {
		n_pairs = (n_engine - 1);
		for (i = 1; i < (n_engine); i++) {
			player_pair_t new_pair;
			new_pair.player1 = 0;
			new_pair.player2 = i;
			possible_pairs.push_back(new_pair);
		}
	} else {
		std::cerr << "Error: unknown tournament type!: [" << opt.tournament_type << "]" << std::endl;
	}

	for (i = 0; i < n_pairs; i++) {
		std::cout << "pair " << possible_pairs[i].player1 << "-" << possible_pairs[i].player2 << std::endl;
	}

	// rounds
	n_round = opt.rounds;

	// total_games
	int total_games = n_round * n_pairs;
	std::cout << "Get " << total_games << " games in total for this tournament!" << std::endl;

	assert(n_process > 0);
	if (n_round < n_process) {
		std::cerr << "Too less rounds, do not need to run parallely at all!\n";
	}

	// mkdir
	std::string tmpdir_path = make_rmd_dirname();
	std::string mkdir_cmd = std::string("mkdir ") + tmpdir_path;
	std::cout << mkdir_cmd << std::endl;
	mkdir(tmpdir_path.c_str(), 0777); // make a tmp dir


	// Begin to distribute works =====================

	// split opening pgn into n_proc parts
	std::vector<std::string> opening_subpgn;
	std::string pgn_fn = parse_opening_fn(opt.openning_info_str, opt.open_info);
	split_pgn(pgn_fn, n_process, tmpdir_path, opening_subpgn);

	int n_round_per_proce = n_round / n_process;

	// assign task for each process
	int remain_rounds = n_round;
	for (j = 0; j < n_process; j++) {

		// a task for process j
		// ===========
		// 1) how many rounds for this process?
		int rounds_for_curr_proc = 0;//n_round;
		if (j < (n_process - 1)) {
			rounds_for_curr_proc = n_round_per_proce;
			remain_rounds -= n_round_per_proce;
		} else {
			rounds_for_curr_proc = remain_rounds;
		}
		// 2) do such number of rounds for each pair
		for (int pair_idx = 0; pair_idx < n_pairs; pair_idx++) {
			match_task_t task;
			task.player1 = possible_pairs[pair_idx].player1;
			task.player2 = possible_pairs[pair_idx].player2;
			task.rounds = rounds_for_curr_proc;
			task.opening_info = opening_subpgn[j];
			task.pgnout_info = tmpdir_path + std::string("/subresult-") + int2str(j) + std::string(".pgn");
			task.cmd = build_task_cmd(opt, task);
			// insert
			all_task[j].stack.push_back(task);
			all_task[j].pgnout_path = task.pgnout_info;
		}
		// ===========
		std::cout << "Task for process[" << j << "] =====\n";
		//print_task_stack();
		for (int k = 0; k < all_task[j].stack.size(); k++) {
			std::cout << "Task " << k << ": " << all_task[j].stack[k].cmd << std::endl;
		}
		std::cout << "===================================\n";
	}

	// record all pgn output into option (for post-process)
	for (i = 0; i < n_process; i++) {
		opt.sub_pgnout.push_back(all_task[i].pgnout_path);
	}

	// End ===========================================
	n_task =  n_process;
	return n_task;
}

void run_task(int worker_rank, task_stack_t &alltask)
{
	int i;
	std::string cmd = "";

	// begin to run
	for (i = 0; i < alltask.stack.size(); i++) {
		cmd = alltask.stack[i].cmd;
		std::cout << "Process [" << worker_rank << "] is runing cmd: " << cmd << std::endl;
		system(cmd.c_str());
	}
}


int serialize_task_stack(match_task_t &task, unsigned char serial_buffer[])
{
	int s = 0, i;
	for (i = 0; i < task.cmd.length(); i++) {
		serial_buffer[i] = task.cmd[i];
	}
	serial_buffer[task.cmd.length()] = '\0';
	return task.cmd.length();
}

