/*
 * util.cpp
 *
 *  Created on: Sep 1, 2013
 *      Author: mc
 */

#include <string>
#include <sstream>
#include <cstdio>
#include <cstdlib>


int str2int(std::string str)
{
    int n = 0;
    std::stringstream ss("");
    ss << str;
    ss >> n;
    return n;
}

std::string int2str(int n)
{
    std::string str;
    std::stringstream ss("");
    ss << n;
    ss >> str;
    return str;
}

double str2double(std::string str)
{
    double n;
    std::stringstream ss("");
    ss << str;
    ss >> n;
    return n;
}

bool valid_path(std::string fpath)
{
    FILE *fp;
    fp = fopen(fpath.c_str(), "r");
    if (fp != NULL) {
   	 return true;
    }
    return false;
}

std::string make_rmd_dirname()
{
	int i, part[4];
	std::string rmdname = "", rndnum;

	srand((unsigned)time(0));

	rndnum = "";
	for (i = 0; i < 3; i++) {
		part[i] = rand();
		rndnum += int2str(part[i]);
	}
	rmdname = "tmpdir-" + rndnum;
	return rmdname;
}
