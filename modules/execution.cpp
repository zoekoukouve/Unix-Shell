#include "../include/redirection.h"   
#include "../include/pipes.h"   
#include "../include/wildcards.h"   
#include "../include/execution.h"   

void execute(vector<string>& vec_arg){
    pid_t pid; 
    int status;

    pid = fork();

    if (pid == -1) {    // Error forking
        perror("mysh");
        exit(EXIT_FAILURE);
    } 

    if (pid == 0) {         // Child process

        // Convert the vector of strings to an array of C-style strings
        vector<char*> cargs(vec_arg.size() + 1);
        for (long unsigned int i = 0; i < vec_arg.size(); ++i) {
            cargs[i] = const_cast<char*>(vec_arg[i].c_str());
        }
        cargs[vec_arg.size()] = NULL;

        // Call execvp() with the array of arguments
        if (execvp(cargs[0], cargs.data()) == -1) {
            perror("mysh");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);

    } else {        // Parent process
        // parent wait for the child to finish
        waitpid(pid, &status, 0);   
    }

}

void execute_without_fork(vector<string>& vec_arg){
    
    // Convert the vector of strings to an array of C-style strings
    vector<char*> cargs(vec_arg.size() + 1);
    for (long unsigned int i = 0; i < vec_arg.size(); ++i) {
        cargs[i] = const_cast<char*>(vec_arg[i].c_str());
    }
    cargs[vec_arg.size()] = NULL;

        // Call execvp() with the array of arguments
    if (execvp(cargs[0], cargs.data()) == -1) {
        perror("mysh");
        exit(EXIT_FAILURE);
    }
    return;
}

void execute_bg(vector<string>& vec_arg,  vector<string>& history){
    pid_t pid; 

    pid = fork();

    if (pid == -1) {    // Error forking
        perror("mysh");
        exit(EXIT_FAILURE);
    } 

    if (pid == 0) {   // Child process

        // Create a new session and process group for the child process.
        setsid();

        int ret = main_execution(vec_arg, history);
        if (ret == 1)
            execute_without_fork(vec_arg);
        exit(EXIT_SUCCESS);
    } else{
        return; 
        // parent doesn't wait for the child to finish
    }
}

int main_execution(vector<string>& tokens,  vector<string>& history){

    // in case that a part of a complex command that the mysh_loop send is "type myHistory i"
    // myHistorty i - find the i-th command of the history vector
            string linet;
            int flag_myHistoryI = 0;
            if(tokens[0] == "myHistory" && tokens.size() == 2){
                long unsigned int i = stoi(tokens[1]);
                if ((i >= 0 && i < history.size())){
                    linet = history[history.size() -1 -i];
                    flag_myHistoryI = 1;
                }          
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
    
    int flag_output = 0;
    int flag_output_app = 0;
    int flag_input = 0;
    int flag_pipe = 0;
    
    for (long unsigned int i = 0; i < tokens.size(); ++i) {
            if (tokens[i] == "<") {
                flag_input = 1;
            }
            if (tokens[i] == ">>") {
                flag_output_app = 1;
            }
            if (tokens[i] == ">") {
                flag_output = 1;
            }
            if (tokens[i] == "|") {
                flag_pipe = 1;
            }
            
    }
    
    // backround
    if(tokens[tokens.size()-1] == "&"){  
        tokens.erase(tokens.end());
        execute_bg(tokens, history);
        return 0;
    }
    // handle pipes
    else if(flag_pipe == 1){
        handle_pipes(tokens, history);
        return 0;
    } 
    // redirection output
    else if(flag_output == 1){
        redirection_output(tokens, history);
        return 0;
    } 
    // redirection output
    else if(flag_output_app == 1){
        redirection_output_double(tokens, history);
        return 0;
    } 
    // redirection input
    else if(flag_input == 1){
        redirection_input(tokens, history);
        return 0;
    } 
    // myHistory
    else if (tokens[0] == "myHistory" && tokens.size() == 1){
        for (long unsigned int i = 0; i < history.size(); ++i) {  // handle aliasing
            cout << history.size() -1 -i << ". " << history[i] << endl; 
        }       
    }
    // wild characters
    else if(tokens[0] == "ls" &&  (tokens[tokens.size()-1].find("*") != string::npos || tokens[tokens.size()-1].find("?") != string::npos)){ 
        handle_wildcards(tokens[tokens.size()-1]);
                return 0;

    } 
    // aliasing 
    else if (tokens[0] == "createalias" || tokens[0] == "destroyalias"){
        // executed in the mysh_loop
    }
    // simple execution
    else
        return 1;

    return 0;
}