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


void split_pgn(std::string pgn_fn, 
               int n_split, 
               std::string dir_path, 
               std::vector<std::string> &sub_pgn_fns);
void file_merge(std::string final_f, 
                std::vector<std::string> &all_subfile);

#endif /* OPEN_SPLIT_H_ */
