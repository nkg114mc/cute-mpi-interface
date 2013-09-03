/*
 * open_split.h
 *
 *  Created on: Sep 1, 2013
 *      Author: mc
 */

#ifndef OPEN_SPLIT_H_
#define OPEN_SPLIT_H_

#include <string>

using namespace std;


void split_pgn(string pgn_fn, int n_split, string dir_path, string sub_pgn_fn[]);
void file_merge(string final_f, string all_subfile[], int n_file);

#endif /* OPEN_SPLIT_H_ */
