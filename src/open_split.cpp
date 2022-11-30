#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <cassert>
#include <cstdio>
#include <cstdlib>

#include "util.h"
#include "pgnlib/PGNGameCollection.h"


unsigned int get_random_int32() {
	int a = rand();
	int b = rand();
	int c = rand();
	unsigned int result = (c << 30) | (b << 15) | a;
	return result;
}

void shuffle_game_collection(pgn::GameCollection &input_games, 
							 pgn::GameCollection &shuffled_games) {
	time_t t;
	srand((unsigned) time(&t));
	int n_games = input_games.size();

	if (n_games <= 1) {
		return; // nothing to shuffule
	}

	std::vector<int> indices(n_games);
	for (int i = 0; i < n_games; i++) {
		indices[i] = i;
	}

	std::cout << "Start shuffling " << n_games << " opening games.\n";
	for (int i = 0; i < (n_games - 1); i++) {
		int remain_cnt = n_games - (i + 1);
		unsigned int j = get_random_int32() % remain_cnt + (i + 1);
		// swap i, j
		int tmp = indices[i];
		indices[i] = indices[j];
		indices[j] = tmp;
	}
	shuffled_games.clear();
	for (int i = 0; i < n_games; i++) {
		int j = indices[i];
		shuffled_games.push_back(input_games[j]);
	}
	std::cout << "Done shuffled opening " << n_games << " games!\n";
}

std::string get_subopening_name(int index) {
	return ("subopening-" + int2str(index) + ".pgn");
}

// split the opening pgn files
void split_pgn(std::string pgn_fn, int n_split, bool need_shuffle, std::string dir_path, 
			   std::vector<std::string> &sub_pgn_fns)
{
	if (n_split <= 0) {
		std::cerr << "Error: Illegal split, number of subpgn should be larger than 0!" << std::endl;
		return;
	}

	pgn::GameCollection *pgn_to_process;

	// load pgn file
	std::ifstream input_pgn(pgn_fn.c_str());
	pgn::GameCollection input_games;
	input_pgn >> input_games;
	pgn_to_process = &input_games;

	// count the number of input games
	int game_cnt = input_games.size();

	if (n_split > game_cnt) {
		std::cerr << "Error: Illegal split, number of games is less than n_split!" << std::endl;
		return;
	}

	// shuffle input if needed
	pgn::GameCollection shuffled_games;
	if (need_shuffle) {
		shuffle_game_collection(input_games, shuffled_games);
		pgn_to_process = &shuffled_games;
	}

	// compute the number of games for each sub pgn
	int subpgn_size = int(ceil(game_cnt / (float)n_split));

	// subfile name
	int sub_index = 0;
	std::string sub_opening_fn = dir_path + "/" + get_subopening_name(sub_index);
	sub_pgn_fns.push_back(sub_opening_fn);

	// initialize first sub pgn file stream
	std::ofstream subpgn_of;
	subpgn_of.open(sub_opening_fn.c_str());
	std::cout << "Writing " << sub_opening_fn << std::endl;

	// initialize sub file game number
	int end_index = std::min(0 + subpgn_size, game_cnt);

	try {
		int total_output_cnt = 0;

		pgn::GameCollection::iterator itr;
		for (itr = pgn_to_process->begin(); itr != pgn_to_process->end(); itr++) {

			// output current game
			subpgn_of << (*itr);

			total_output_cnt++;

			if (total_output_cnt >= end_index) {
				// close last file
				subpgn_of.close();
				// open a new file
				sub_index++;
				if (sub_index < n_split) {
					std::string new_fn = dir_path + "/" + get_subopening_name(sub_index);
					sub_pgn_fns.push_back(new_fn);
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
	
	subpgn_of.close();

	std::cout << "Found " << game_cnt << " games!\n";
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

