#include <sstream>
#include <map>

#include "include/execution.h"
#include "include/history.h"

void mysh_loop();

using namespace std;

int main(int argc, char **argv) {
    mysh_loop();
    return EXIT_SUCCESS;
}

void mysh_loop(){
    string line;

    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    int status = 1;
    map<string, string> aliases;    // for aliasing
    vector<string> history;         // for history

    while(status != 0){
        
        cout << "mysh> ";

        line = " ";                            // initializing the line so that in case a siganal is received, the line is not uninitialized 
        getline(cin, line);                   // read_line

       
        vector<string> comm;                    // slpit line to commands
        char* token = strtok(&line[0], ";");
        while(token != NULL) {
            comm.push_back(token);
            token = strtok(NULL, ";");
        }
        
        // Execute each command command 
        for (long unsigned int j = 0; j < comm.size(); j++){   
            
            // Parser
            vector<string> tokens;                  // slpit line to tokens
            string linet = comm[j];
            string token;
            stringstream tempss(linet);
            
            while (tempss >> token){
                if (token[0] == '\"'){          // treat " " substrings as one token
                    string temp;
                    while (tempss >> temp){
                        token += " " + temp;
                        if (temp.back() == '\"')
                            break;
                    }
                    token = token.substr(1, token.length() - 2); // erase ""
                }
                tokens.push_back(token);
            }
            
            /////////////////////////////////////////////// Commands //////////////////////////////////////////////

            // exit
            if(tokens[0] == "exit"){
                return;
            }

            // myHistorty i - find the i-th command of the history vector
            vector<string> tokenst; 
            if(tokens[0] == "myHistory" && tokens.size() == 2){
                long unsigned int i = stoi(tokens[1]);
                if (!(i >= 0 && i < (history.size() -1)))
                    continue;
                string temp = history[history.size() -1 -i];
                                    // slpit line to tokens
                char* tokent = strtok(&temp[0], " ");
                while(tokent != NULL) {
                    tokenst.push_back(tokent);
                    tokent = strtok(NULL, " ");
                }
                tokens = tokenst;
                linet = history[history.size() -1 -i];
            }

            // cd 
            if(tokens[0] == "cd"){
                string directory = tokens[1];
                if (chdir(directory.c_str()) != 0) {
                    cerr << "Error: " << directory << " is not a valid directory\n";
                }
                add_command_to_history(history, linet);    
                continue;   
            } 
            
            // aliasing 
            for (long unsigned int i = 0; i < tokens.size(); ++i) {  // handle aliasing
                if(tokens[i] == "destroyalias"){
                    aliases.erase(tokens[i+1]);  
                    continue;
                }
            }
            
            // find aliasing 
            for (long unsigned int i = 0; i < tokens.size(); ++i) {
                if (aliases.find(tokens[i]) != aliases.end()) {
                    tokens[i] = aliases[tokens[i]];
                }
            }

            for (long unsigned int i = 0; i < tokens.size(); ++i) {  // handle aliasing
                if (tokens[i] == "createalias"){            
                    aliases[tokens[i+1]] = tokens[i+2];
                }
            }
            
            
            int pid = fork();
            if (pid == 0) {
                // Run command loop.
                signal(SIGINT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);
                int ret = main_execution(tokens, history); 
                if (ret == 1)
                        execute(tokens);
                exit(EXIT_SUCCESS);
            } else {
                    // setted siganl handlers
                waitpid(pid, NULL, 0);
            }
            
                // add that command to history 
            if (!(tokens[0] == "myHistory" && tokens.size() == 1))
                add_command_to_history(history, linet);
            
        } 
    }
}

