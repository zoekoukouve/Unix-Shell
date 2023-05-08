#include <sstream>
#include <map>

#include "execution.h"
#include "history.h"
#include "wildcards.h"


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
        
        // Execute each command  
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
            
            /////////////////////////////////////////////// Commands ////////////////////////////////////////////////

            // exit
            if(tokens[0] == "exit"){
                return;
            }

            // myHistorty i - find the i-th command of the history vector
            int flag_myHistoryI = 0;
            if(tokens[0] == "myHistory" && tokens.size() == 2){
                long unsigned int i = stoi(tokens[1]);
                if (!(i >= 0 && i < history.size())){
                    cout << "My history error" << endl;
                    continue;
                }
                linet = history[history.size() -1 -i];
                flag_myHistoryI = 1;
            }

            // if myHistorty i happen, retokenize
            if (flag_myHistoryI == 1){
                tokens.clear();
                string token1;
                stringstream tempss1(linet);
                
                while (tempss1 >> token1){
                    if (token1[0] == '\"'){          // treat " " substrings as one token
                        string temp;
                        while (tempss1 >> temp){
                            token1 += " " + temp;
                            if (temp.back() == '\"')
                                break;
                        }
                        token1 = token1.substr(1, token1.length() - 2); // erase ""
                    }
                    tokens.push_back(token1);
                }
            }

            
            // aliasing 
            int flag_alias = 0;

            for (long unsigned int i = 0; i < tokens.size(); ++i) {  // handle aliasing
                if(tokens[i] == "destroyalias"){
                    aliases.erase(tokens[i+1]);  
                    continue;
                }
            }
            
            // find aliasing 
            for (long unsigned int i = 0; i < tokens.size(); ++i) {
                if (aliases.find(tokens[i]) != aliases.end()) {
                    //tokens[i] = aliases[tokens[i]];
                    size_t pos = linet.find(tokens[i]);
                    linet.replace(pos, tokens[i].length(), aliases[tokens[i]]);
                    flag_alias = 1;
                }
            }

            // if alias found, retokenize
            if (flag_alias == 1){
                tokens.clear();
                string token1;
                stringstream tempss1(linet);
                
                while (tempss1 >> token1){
                    if (token1[0] == '\"'){          // treat " " substrings as one token
                        string temp;
                        while (tempss1 >> temp){
                            token1 += " " + temp;
                            if (temp.back() == '\"')
                                break;
                        }
                        token1 = token1.substr(1, token1.length() - 2); // erase ""
                    }
                    tokens.push_back(token1);
                }
            }

            for (long unsigned int i = 0; i < tokens.size(); ++i) {  // handle aliasing
                if (tokens[i] == "createalias"){            
                    aliases[tokens[i+1]] = tokens[i+2];
                }
            }
            
            // cd 
            if(tokens[0] == "cd"){
                // cd with wildcards
                if(tokens[tokens.size()-1].find("*") != string::npos || tokens[tokens.size()-1].find("?") != string::npos){ 
                    handle_wildcards(tokens);
                } 
                // simple cd 
                else{
                    string directory = tokens[1];
                    if (chdir(directory.c_str()) != 0) {
                        cerr << "Error: " << directory << " is not a valid directory\n";
                    }
                }
                add_command_to_history(history, linet);    
                continue;   
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
                waitpid(pid, NULL, WUNTRACED);
            }
            
                // add that command to history 
            if (!(tokens[0] == "myHistory" && tokens.size() == 1))
                add_command_to_history(history, linet);
            
        } 
    }
}

