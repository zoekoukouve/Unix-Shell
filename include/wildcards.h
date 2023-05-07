#include <glob.h>           
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <csignal>
#pragma once

using namespace std;


void exec_wildcards(vector<string>&, glob_t&);

void handle_wildcards(vector<string>&);