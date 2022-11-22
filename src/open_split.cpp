#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <cassert>

#include "util.h"
#include "pgnlib/PGNGameCollection.h"

/*
int count_games(std::string pgn_fn)
{
	std::ifstream inf;
	std::string token = "";
	int game_cnt;

	inf.open(pgn_fn.c_str());
	game_cnt = 0;
	while (!inf.eof()) {
		token = "";
		inf >> token;
		if (token == "[Event") {
			game_cnt++;
		}
	}
	inf.close();

	std::cout << "Found " << game_cnt << " games!\n";

	return game_cnt;
}

void split_pgn(std::string pgn_fn, int n_split, std::string dir_path, std::string sub_pgn_fn[])
{
	std::ifstream inf;
	std::string token = "";
	int subpgn_size = 0;

	if (n_split <= 0) {
		std::cerr << "Error: Illegal split, number of subpgn should be larger than 0!" << std::endl;
		return;
	}

	// count the number of games
	int game_cnt = 0;
	game_cnt = count_games(pgn_fn);

	if (n_split > game_cnt) {
		std::cerr << "Error: Illegal split, number of games is less than n_split!" << std::endl;
		return;
	}

	// compute the number of games for each sub pgn
	subpgn_size = int(ceil(game_cnt / (float)n_split));

	std::ofstream subpgn_of;
	std::string first_fn = dir_path + "/" + "subopening-0.pgn";
	subpgn_of.open(first_fn.c_str());
	sub_pgn_fn[0] = first_fn;
	std::cout << "Writing " << first_fn << std::endl;

	inf.open(pgn_fn.c_str());
	if (!inf.good()) {
		std::cerr << "Something wrong with the pgn file: " << pgn_fn << std::endl;
		exit(1);
	}

	std::stringstream cached_pgn("");
	int game_index = 0;
	int sub_index = 0;
	while (!inf.eof()) {
		token = "";
		inf >> token;
		if (token == "[Event") {
			game_index++;
			if (game_index != 1) {
				subpgn_of << std::endl;
				subpgn_of << std::endl;
			}

			if (game_index % subpgn_size == 0) {
				// close last file
				subpgn_of.close();

				// open a new file
				sub_index++;
				if (sub_index < n_split) {
					std::string new_fn = dir_path + "/" + "subopening-" + int2str(sub_index) + ".pgn";
					sub_pgn_fn[sub_index] = new_fn;
					subpgn_of.open(new_fn.c_str());
					subpgn_of.clear(); // clear all flag
					std::cout << "Writing " << new_fn << std::endl;
				}
			}
		}
		subpgn_of << token;
		if (token[token.length() - 1] == ']') {
			subpgn_of << std::endl;
		} else {
			subpgn_of << " ";
		}
	}

	subpgn_of.close();
	inf.close();

	std::cout << "Found " << game_cnt << " games!\n";
}
*/

std::string get_subopening_name(int index) {
	return ("subopening-" + int2str(index) + ".pgn");
}

//
void split_pgn(std::string pgn_fn, int n_split, std::string dir_path, std::vector<std::string> &sub_pgn_fns)
{
	//std::ifstream inf;
	//std::string token = "";
	//int subpgn_size = 0;

	if (n_split <= 0) {
		std::cerr << "Error: Illegal split, number of subpgn should be larger than 0!" << std::endl;
		return;
	}
/*
	if (n_split == 1) {
		std::cerr << "n_split == 1, nothing to split" << std::endl;
		return;
	}
*/
	// load pgn file
	std::ifstream input_pgn(pgn_fn.c_str());
	pgn::GameCollection input_games;
	input_pgn >> input_games;

	// count the number of input games
	int game_cnt = input_games.size();

	if (n_split > game_cnt) {
		std::cerr << "Error: Illegal split, number of games is less than n_split!" << std::endl;
		return;
	}

	// compute the number of games for each sub pgn
	int subpgn_size = int(ceil(game_cnt / (float)n_split));

	// subfile name
	int sub_index = 0;
	std::string sub_opening_fn = dir_path + "/" + get_subopening_name(sub_index);//"subopening-0.pgn";
	sub_pgn_fns.push_back(sub_opening_fn);

	// initialize first sub pgn file stream
	std::ofstream subpgn_of;
	subpgn_of.open(sub_opening_fn.c_str());
	std::cout << "Writing " << sub_opening_fn << std::endl;

	// initialize sub file game number
	//int current_subpgn_size = subpgn_size;
	int end_index = std::min(0 + subpgn_size, game_cnt);

	try {
		//int cur_output_cnt = 0;
		int total_output_cnt = 0;

		pgn::GameCollection::iterator itr;
		for (itr = input_games.begin(); itr != input_games.end(); itr++) {

				// output current game
				subpgn_of << (*itr);

				//cur_output_cnt++;
				total_output_cnt++;

				if (total_output_cnt >= end_index) {
					// close last file
					subpgn_of.close();

					// open a new file
					sub_index++;
					if (sub_index < n_split) {
						std::string new_fn = dir_path + "/" + get_subopening_name(sub_index);
						sub_pgn_fns.push_back(sub_opening_fn);
						subpgn_of.open(new_fn.c_str());
						subpgn_of.clear(); // clear all flag
						std::cout << "Writing " << new_fn << std::endl;

						end_index = std::min(end_index + subpgn_size, game_cnt); // refresh end index
					}
				}
		}

	} catch (std::exception &e) {
		std::cerr << "exception: " << e.what() << std::endl;
		std::cerr << "Something wrong with the pgn file: " << pgn_fn << std::endl;
		exit(1);
	}
/*
	while (!inf.eof()) {
		token = "";
		inf >> token;
		if (token == "[Event") {
			game_index++;
			if (game_index != 1) {
				subpgn_of << std::endl;
				subpgn_of << std::endl;
			}

			if (game_index % subpgn_size == 0) {
				// close last file
				subpgn_of.close();

				// open a new file
				sub_index++;
				if (sub_index < n_split) {
					std::string new_fn = dir_path + "/" + "subopening-" + int2str(sub_index) + ".pgn";
					sub_pgn_fn[sub_index] = new_fn;
					subpgn_of.open(new_fn.c_str());
					subpgn_of.clear(); // clear all flag
					std::cout << "Writing " << new_fn << std::endl;
				}
			}
		}
		subpgn_of << token;
		if (token[token.length() - 1] == ']') {
			subpgn_of << std::endl;
		} else {
			subpgn_of << " ";
		}
	}
*/
	subpgn_of.close();

/*
	// compute the number of games for each sub pgn
	int subpgn_size = int(ceil(game_cnt / (float)n_split));

	// output sub-pgn files
	int output_cnt = 0;
	for (int sub_index = 0; sub_index < n_split; sub_index++) {
		int current_subpgn_size = subpgn_size;
		if (sub_index == (n_split - 1)) { // last one
			current_subpgn_size = game_cnt - output_cnt;
		}
		assert(current_subpgn_size > 0);

		// subfile name
		std::string sub_opening_fn = dir_path + "/" + get_subopening_name(sub_index);//"subopening-0.pgn";
		sub_pgn_fns.push_back(sub_opening_fn);

		std::ofstream subpgn_of;
		subpgn_of.open(sub_opening_fn.c_str());
		std::cout << "Writing " << sub_opening_fn << std::endl;
		for (int i = 0; i < current_subpgn_size; i++) {
			
		}
		subpgn_of.close();
	}
*/
	std::cout << "Found " << game_cnt << " games!\n";


/*
	inf.open(pgn_fn.c_str());
	if (!inf.good()) {
		std::cerr << "Something wrong with the pgn file: " << pgn_fn << std::endl;
		exit(1);
	}
*/

/*
	//std::stringstream cached_pgn("");
	int game_index = 0;
	int sub_index = 0;
	while (!inf.eof()) {
		token = "";
		inf >> token;
		if (token == "[Event") {
			game_index++;
			if (game_index != 1) {
				subpgn_of << std::endl;
				subpgn_of << std::endl;
			}

			if (game_index % subpgn_size == 0) {
				// close last file
				subpgn_of.close();

				// open a new file
				sub_index++;
				if (sub_index < n_split) {
					std::string new_fn = dir_path + "/" + "subopening-" + int2str(sub_index) + ".pgn";
					sub_pgn_fn[sub_index] = new_fn;
					subpgn_of.open(new_fn.c_str());
					subpgn_of.clear(); // clear all flag
					std::cout << "Writing " << new_fn << std::endl;
				}
			}
		}
		subpgn_of << token;
		if (token[token.length() - 1] == ']') {
			subpgn_of << std::endl;
		} else {
			subpgn_of << " ";
		}
	}

	subpgn_of.close();
	inf.close();

	std::cout << "Found " << game_cnt << " games!\n";*/
}

// Merge one pgn file into an output file stream
void merge_in_one_file(std::string sub_path, std::ofstream &tstream)
{
	std::string linestr;
	std::ifstream sub_log;
	sub_log.open(sub_path.c_str());
	while (!sub_log.eof()) {
		getline(sub_log, linestr);
		tstream << linestr << std::endl;
		if (sub_log.eof()) {
			break;
		}
	}
	sub_log.close();
}

// Merge multiple files into one
void file_merge(std::string final_f, std::vector<std::string> &all_subfile) //, int n_file)
{
	std::ofstream outf;
	int n_file = all_subfile.size();

	outf.open(final_f.c_str());
	for (int i = 0; i < n_file; i++) {
		std::string sub_path = all_subfile[i];
		std::cout << "Reading sub-pgn: " << sub_path << std::endl;
		if (valid_path(sub_path)) {
			merge_in_one_file(sub_path, outf);
		} else {
			std::cerr << "Can't find file: " << sub_path << std::endl;
		}
	}
	outf.close();
}
