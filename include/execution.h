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

int main_execution(vector<string>&, vector<string>&);
void execute(const vector<string>&);
void execute_bg(vector<string>&, vector<string>&);
void execute_without_fork(const vector<string>&);