#include <iostream>
#include <sstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <assert.h>

#include "task.h"
#include "util.h"
#include "open_split.h"

using namespace std;



string parse_opening_fn(string open_info_str, opening_info_t &open_info)
{
	stringstream ss("");
	string token = "";
	string words[256];
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
	string fpath = "";
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

string build_task_cmd(option_t &opt, match_task_t &task)
{
	stringstream allcmd("");

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
	allcmd << "-engine " << opt.engine_info[task.player1] << " ";
	//   player2
	allcmd << "-engine " << opt.engine_info[task.player2] << " ";

	return allcmd.str();
}

void parse_engine_info()
{

}

int split_task(option_t &opt, int n_process, int &n_task, task_stack_t all_task[])
{
	int i, j;
	int n_engine = opt.n_engine;
	int n_pairs = 0;
	int n_round = 0;
	player_pair_t *possible_pairs;

	if (n_engine < 2) {
		cerr << "Error: at least two engine!" << endl;
		return 0;
	}

	// tournament type?
	if (opt.tournament_type == "round-robin") {
		n_pairs = (n_engine * (n_engine - 1)) / 2;
		possible_pairs = new player_pair_t[n_pairs];
		int pair_idx = 0;
		for (i = 0; i < (n_engine - 1); i++) {
			for (j = (i+1); j < n_engine; j++) {
				possible_pairs[pair_idx].player1 = i;
				possible_pairs[pair_idx].player2 = j;
				pair_idx++;
			}
		}
	} else if (opt.tournament_type == "gauntlet") {
		n_pairs = (n_engine - 1);
		possible_pairs = new player_pair_t[n_pairs];
		int pair_idx = 0;
		for (i = 1; i < (n_engine); i++) {
			possible_pairs[pair_idx].player1 = 0;
			possible_pairs[pair_idx].player2 = i;
			pair_idx++;
		}
	} else {
		cerr << "Error: unknown tournament type!: [" << opt.tournament_type << "]" << endl;
	}

	for (i = 0; i < n_pairs; i++) {
		cout << "pair " << possible_pairs[i].player1 << "-" << possible_pairs[i].player2 << endl;
	}

	// rounds
	n_round = opt.rounds;

	// total_games
	int total_games = n_round * n_pairs;
	cout << "Get " << total_games << " in total for this tournament!" << endl;

	assert(n_process > 0);
	if (n_round < n_process) {
		cerr << "Too less rounds, do not need to run parallely at all!\n";
	}

	// mkdir
	string tmpdir_path = make_rmd_dirname();
	string mkdir_cmd = string("mkdir ") + tmpdir_path;
	system(mkdir_cmd.c_str()); // make a tmp dir


	string opening_subpgn[1024];
	// Begin to distribute works =====================
	int n_round_per_proce = n_round / n_process;

	// split opening pgn into n_proc parts
	string pgn_fn = parse_opening_fn(opt.openning_info_str, opt.open_info);
	split_pgn(pgn_fn, n_process, tmpdir_path, opening_subpgn);

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
			task.pgnout_info = tmpdir_path + string("/subresult-") + int2str(j) + string(".pgn");
			task.cmd = build_task_cmd(opt, task);
			// insert
			all_task[j].stack.push_back(task);
			all_task[j].pgnout_path = task.pgnout_info;
		}
		// ===========
		cout << "Task for process[" << j << "] =====\n";
		//print_task_stack();
		for (int k = 0; k < all_task[j].stack.size(); k++) {
			cout << "Task " << k << ": " << all_task[j].stack[k].cmd << endl;
		}
		cout << "===================================\n";
	}

	// record all pgn output into option (for post-process)
	opt.n_sub_pgnout = n_process;
	for (i = 0; i < n_process; i++) {
		opt.sub_pgnout[i] = all_task[i].pgnout_path;
	}

	// End ===========================================
	n_task =  n_process;
	return n_task;
}

void run_task(option_t &opt, task_stack_t &alltask)
{
	int i;
	string cmd = "";

	// begin to run
	for (i = 0; i < alltask.stack.size(); i++) {
		cmd = alltask.stack[i].cmd;
		cout << "Process [" << opt.rank << "] is runing cmd: " << cmd << endl;
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

