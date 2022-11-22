/*
 * util.h
 *
 *  Created on: Sep 1, 2013
 *      Author: mc
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <string>


int str2int(std::string str);
std::string int2str(int n);
double str2double(std::string str);
bool valid_path(std::string fpath);
std::string make_rmd_dirname();

#endif /* UTIL_H_ */
