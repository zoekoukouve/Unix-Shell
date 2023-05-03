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

void handle_pipes(vector<string>&, vector<string>&);
