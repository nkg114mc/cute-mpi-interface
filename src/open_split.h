/*
 * open_split.h
 *
 *  Created on: Sep 1, 2013
 *      Author: mc
 */

#ifndef OPEN_SPLIT_H_
#define OPEN_SPLIT_H_

#include <string>
#include <vector>
#include "pgnlib/PGNGameCollection.h"

void split_pgn(std::string pgn_fn, 
               int n_split, 
               bool need_shuffle, 
               std::string dir_path, 
               std::vector<std::string> &sub_pgn_fns);
void file_merge(std::string final_f, 
                std::vector<std::string> &all_subfile);
void shuffle_game_collection(pgn::GameCollection &input_games, 
							 pgn::GameCollection &shuffled_games);

#endif /* OPEN_SPLIT_H_ */
