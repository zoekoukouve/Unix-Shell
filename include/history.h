#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <fcntl.h>
#pragma once


using namespace std;

void add_command_to_history(vector<string>&, const string&);
string get_previous_command(const vector<string>&, int&);
string get_next_command(const vector<string>&, int&);