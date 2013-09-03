/*
 * util.h
 *
 *  Created on: Sep 1, 2013
 *      Author: mc
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <string>

using namespace std;



int str2int(string str);
string int2str(int n);
double str2double(string str);
bool valid_path(string fpath);
string make_rmd_dirname();

#endif /* UTIL_H_ */
