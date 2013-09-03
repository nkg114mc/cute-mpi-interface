#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>

#include "util.h"

using namespace std;

int count_games(string pgn_fn)
{
	ifstream inf;
	string token = "";
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

	cout << "Fount " << game_cnt << " games!\n";

	return game_cnt;
}

void split_pgn(string pgn_fn, int n_split, string dir_path, string sub_pgn_fn[])
{
	ifstream inf;
	string token = "";
	int subpgn_size = 0;

	if (n_split <= 0) {
		cerr << "Error: Illegal split, number of subpgn should be larger than 0!" << endl;
		return;
	}

	// count the number of games
	int game_cnt = 0;
	game_cnt = count_games(pgn_fn);

	if (n_split > game_cnt) {
		cerr << "Error: Illegal split, number of games is less than n_split!" << endl;
		return;
	}

	// compute the number of games for each sub pgn
	subpgn_size = int(ceil(game_cnt / (float)n_split));

	ofstream subpgn_of;
	string first_fn = dir_path + "/" + "subopening-0.pgn";
	subpgn_of.open(first_fn.c_str());
	sub_pgn_fn[0] = first_fn;
	cout << "Writing " << first_fn << endl;

	inf.open(pgn_fn.c_str());
	stringstream cached_pgn("");
	int game_index = 0;
	int sub_index = 0;
	while (!inf.eof()) {
		token = "";
		inf >> token;
		//cached_pgn << token;
		if (token == "[Event") {
			game_index++;
			if (game_index != 1) {
				subpgn_of << endl;
				subpgn_of << endl;
			}

			if (game_index % subpgn_size == 0) {
				// close last file
				subpgn_of.close();

				// open a new file
				sub_index++;
				if (sub_index < n_split) {
					string new_fn = dir_path + "/" + "subopening-" + int2str(sub_index) + ".pgn";
					sub_pgn_fn[sub_index] = new_fn;
					subpgn_of.open(new_fn.c_str());
					subpgn_of.clear(); // clear all flag
					cout << "Writing " << new_fn << endl;
				}
			}
		}
		subpgn_of << token;
		if (token[token.length() - 1] == ']') {
			subpgn_of << endl;
		} else {
			subpgn_of << " ";
		}
	}

	subpgn_of.close();
	inf.close();

	cout << "Fount " << game_cnt << " games!\n";
}

void merge_in_one_file(string sub_path, ofstream &tstream)
{
	string linestr;
	ifstream sub_log;
	sub_log.open(sub_path.c_str());
	while (!sub_log.eof()) {
		getline(sub_log, linestr);
		tstream << linestr << endl;
		if (sub_log.eof()) {
			break;
		}
	}
	sub_log.close();

}

void file_merge(string final_f, string all_subfile[], int n_file)
{
	int i;
	string sub_path = "";
	ofstream outf;

	outf.open(final_f.c_str());
	// ------------------
	for (i = 0; i < n_file; i++) {
		sub_path = all_subfile[i];
		cout << "Reading sub-pgn: " << sub_path << endl;
		if (valid_path(sub_path)) {
			merge_in_one_file(sub_path, outf);
		} else {
			cerr << "Can't find file: " << sub_path << endl;
		}
	}
	// -------------------
	outf.close();
}


