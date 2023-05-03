// cd path, history velakia, anakatheuthunsh <<, prints &, mt systhmatos
#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <glob.h>           
#include <map>
#include <fcntl.h>
#include <csignal>

using namespace std;

void mysh_loop();
int main_execution(vector<string>&, string&, map<string, string>&);
void execute(const vector<string>&);
void execute_bg(vector<string>&, string&, map<string, string>&);
void add_command_to_history(vector<string>&, const string&);
string get_previous_command(const vector<string>&, int&);
string get_next_command(const vector<string>&, int&);
void execute_without_fork(const vector<string>&);




int main(int argc, char **argv) {
    
    mysh_loop();
    return EXIT_SUCCESS;
}

void mysh_loop(){
    string line;

    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    int status = 1;
    map<string, string> aliases; // for aliasing
    vector<string> history; // for history
    string ppath= (string)"mysh> ";

    while(status != 0){
        
        //printf("mysh> ");
        cout << ppath;

        getline(cin, line);                   // read_line

        vector<string> comm;                 // slpit line to commands
        char* token = strtok(&line[0], ";");
        while(token != NULL) {
            comm.push_back(token);
            token = strtok(NULL, ";");
        }
        
        for (long unsigned int j = 0; j < comm.size(); j++){   
            vector<string> tokens;                  // slpit line to tokens
            string linet = comm[j];
            string token;
            //char* token = strtok(&linet[0], " ");
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
            
            // exit
            if(tokens[0] == "exit"){
                return;
            }

            // cd 
            if(tokens[0] == "cd"){
               // if (tokens.size() == 1){
                        // std::filesystem::path currentPath = std::filesystem::current_path();
                        // const char* path = "../syspro_project1";
                        // if (chdir(path) == -1) {
                        //     cout << "Error changing directory to " << path << endl;
                        // }
                        // ppath = (string)"mysh> ";
               // } else{
                        const char* path = tokens[1].c_str();
                        string directory = tokens[1];
                        if (chdir(directory.c_str()) != 0) {
                            std::cerr << "Error: " << directory << " is not a valid directory\n";
                            //return 1;
                        }
                        ppath = (string)"mysh> ";
                        ppath.append((string)path);
                        ppath.append((string)"$ ");
                      //  char cwd[1024];
                        // if (getcwd(cwd, sizeof(cwd)) == nullptr) {
                        //     std::cerr << "Error getting current working directory\n";
                        //     //return 1;
                        // }

                        // string program_path("main");
                        // // Compute the relative path of the working folder
                        // std::string working_folder = ".";
                        // if (program_path.rfind('/') != std::string::npos) {
                        //     working_folder = program_path.substr(0, program_path.rfind('/'));
                        // }
                        // if (chdir(working_folder.c_str()) != 0) {
                        //     std::cerr << "Error changing working directory to " << working_folder << "\n";
                        //     //return 1;
                        // }
                        // if (chdir(cwd) != 0) {
                        //     std::cerr << "Error changing working directory to " << cwd << "\n";
                        //    // return 1;
                        // }
                        // std::string relative_path = ".";
                        // if (program_path.rfind('/') != std::string::npos) {
                        //     relative_path = cwd + program_path.substr(program_path.rfind('/'));
                        // }
                        // path = relative_path.c_str();
                        // cout << path;

               // }
                continue;   
            } 
            
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


            int flag_history = 0;
            // myHistorty
            vector<string> tokenst; 
            if(tokens[0] == "myHistory" && tokens.size() == 1){
                for (long unsigned int i = 0; i < history.size(); ++i) {  // handle aliasing
                    cout << history[i] << endl; 
                }
                flag_history = 1;
            } else if(tokens[0] == "myHistory" && tokens.size() > 1){
                int i = stoi(tokens[1]);
                string temp = history[i];
                                    // slpit line to tokens
                char* tokent = strtok(&temp[0], " ");
                while(tokent != NULL) {
                    tokenst.push_back(tokent);
                    tokent = strtok(NULL, " ");
                }
                tokens = tokenst;
                linet = history[i];
            }
            
            if (flag_history == 0) {
                int pid = fork();
                if (pid == 0) {
                // Run command loop.
                    signal(SIGINT, SIG_DFL);
                    signal(SIGTSTP, SIG_DFL);
                    int ret = main_execution(tokens, ppath, aliases);   ////////////// provlhmatiko cd////////////////////////////
                    if (ret == 1)
                        execute(tokens);
                    exit(EXIT_SUCCESS);
                } else {
                    // setted siganl handlers
                    waitpid(pid, NULL, 0);
                }
            
                // add that command to history    
                    add_command_to_history(history, linet);
            }
        } 
    }
}

