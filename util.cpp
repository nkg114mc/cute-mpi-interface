/*
 * util.cpp
 *
 *  Created on: Sep 1, 2013
 *      Author: mc
 */

#include <string>
#include <sstream>
#include <cstdio>
#include <cstdlib> >


using namespace std;

int str2int(string str)
{
    int n = 0;
    stringstream ss("");
    ss << str;
    ss >> n;
    return n;
}

string int2str(int n)
{
    string str;
    stringstream ss("");
    ss << n;
    ss >> str;
    return str;
}

double str2double(string str)
{
    double n;
    stringstream ss("");
    ss << str;
    ss >> n;
    return n;
}

bool valid_path(string fpath)
{
    FILE *fp;
    fp = fopen(fpath.c_str(), "r");
    if (fp != NULL) {
   	 return true;
    }
    return false;
}

string make_rmd_dirname()
{
	int i, part[4];
	string rmdname = "", rndnum;

	srand((unsigned)time(0));

	rndnum = "";
	for (i = 0; i < 3; i++) {
		part[i] = rand();
		rndnum += int2str(part[i]);
	}
	rmdname = "tmpdir-" + rndnum;
	return rmdname;
}


