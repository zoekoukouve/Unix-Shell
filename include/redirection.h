#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <csignal>
#pragma once


using namespace std;

void redirection_output(vector<string>&, vector<string>&);
void redirection_output_double(vector<string>&, vector<string>&);
void redirection_input(vector<string>&, vector<string>&);