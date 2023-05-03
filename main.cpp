// cd path, history velakia, anakatheuthunsh <<, prints &, mt systhmatos
#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <glob.h>           // glob(), globfree()
#include <map>
#include <fcntl.h>
#include <csignal>

using namespace std;

void mysh_loop();
int main_execution(vector<string>&, string&, map<string, string>&);
void execute(const vector<string>&);
void execute_bg(vector<string>&, string&, map<string, string>&);
void handle_wildcards(const string&);
void add_command_to_history(vector<string>&, const string&);
string get_previous_command(const vector<string>&, int&);
string get_next_command(const vector<string>&, int&);
void redirection_output(vector<string>&, string&, map<string, string>&);
void redirection_output_double(vector<string>&, string&, map<string, string>&);
void redirection_input(vector<string>&, string&, map<string, string>&);
void handle_pipes(vector<string>&, string&, map<string, string>& );
void execute_without_fork(const vector<string>&);
void handle_sigint(int);
void handle_sigtstp(int);
// int mysh_exit();


int main(int argc, char **argv) {
    //cout << "Hello World!";

    // int pid = fork();
    // if (pid == 0) {
    // // Run command loop.
    //     exit(EXIT_SUCCESS);
    // } else {
    //     // set siganl handlers
    //     signal(SIGINT, handle_sigint);
    //     signal(SIGTSTP, handle_sigtstp);
    //     waitpid(pid, NULL, 0);
    // }

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
        
        vector<string> tokens;                 // slpit line to tokens
        char* token = strtok(&line[0], " ");
        while(token != NULL) {
            tokens.push_back(token);
            token = strtok(NULL, " ");
        }
        
        // exit
        if(tokens[0] == "exit"){
            return;
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
            line = history[i];
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
                add_command_to_history(history, line);
        }
    } 
    
}

void redirection_input(vector<string>& tokens, string& ppath, map<string, string>& aliases){
         // open the appropriate file
        long unsigned int found = 0;
        for(long unsigned int i = 0; i < tokens.size(); ++i) {
            if (tokens[i] == "<"){
                found = i;
                break;
            }          
        }
        
        char name[300];
        strcpy(name, tokens[found + 1].c_str());

        int file_desc = open(name, O_RDONLY); // open the file only for read
        if (file_desc == -1) {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }

        // save the output to restore it to stdin later 
        int saved_stdout = dup(STDIN_FILENO);
        if (saved_stdout == -1) {
            perror("Error duplicating file descriptor");
            exit(EXIT_FAILURE);
        }    
        
        // redirect the input
        if (dup2(file_desc, STDIN_FILENO) == -1) {
            perror("Error redirecting input");
            exit(EXIT_FAILURE);;
        }

        // do what should be done
        vector<string> temp; // for history
        for (long unsigned int i = 0; i < found; ++i) {
            char cc[30000];
            strcpy(cc, tokens[i].c_str());
            temp.push_back((string)cc);;
        }
        int ret = main_execution(temp, ppath, aliases);
        if (ret == 1)
            execute(temp);       
        //execute(temp);                   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // restore it to stdin
        if (dup2(saved_stdout, STDIN_FILENO) == -1) {
            perror("Error restoring standard output");
            exit(EXIT_FAILURE);;
        }

        close(file_desc);
    }

void redirection_output_double(vector<string>& tokens, string& ppath, map<string, string>& aliases){
    // opern the appropriate file
        long unsigned int found = 0;
        for(long unsigned int i = 0; i < tokens.size(); ++i) {
            if (strcmp(tokens[i].c_str(),">>") == 0){
                found = i;
                break;
            }          
        }
        char name[300];
        strcpy(name, tokens[found + 1].c_str());            
        
       
       // save the output to restore it to stdout later 
        int file_desc = open(name, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
        if (file_desc == -1) {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }
        // redirect the output
        int saved_stdout = dup(STDOUT_FILENO);
        if (saved_stdout == -1) {
            perror("Error duplicating file descriptor");
            exit(EXIT_FAILURE);
        }    
        
        // redirect the output
        if (dup2(file_desc, STDOUT_FILENO) == -1) {
            perror("Error redirecting output");
            exit(EXIT_FAILURE);;
        }   
    
        // do what should be done
        vector<string> temp; // for history
        for (long unsigned int i = 0; i < found; ++i) {           
            char cc[30000];
            strcpy(cc, tokens[i].c_str());
            temp.push_back((string)cc);
        }

        int ret = main_execution(temp, ppath, aliases);
        if (ret == 1)
            execute(temp);  
        //execute(temp);                                      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // restore it to stdout 
        if (dup2(saved_stdout, STDOUT_FILENO) == -1) 
            perror("Error restoring standard output");
           
        close(file_desc);
}


void redirection_output(vector<string>& tokens, string& ppath, map<string, string>& aliases){
        
        // opern the appropriate file
        long unsigned int found = 0;
        for(long unsigned int i = 0; i < tokens.size(); ++i) {
            if (strcmp(tokens[i].c_str(),">") == 0){
                found = i;
                break;
            }          
        }
        char name[300];
        strcpy(name, tokens[found + 1].c_str());            
        
       
       // save the output to restore it to stdout later 
        int file_desc = open(name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (file_desc == -1) {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }
        // redirect the output
        int saved_stdout = dup(STDOUT_FILENO);
        if (saved_stdout == -1) {
            perror("Error duplicating file descriptor");
            exit(EXIT_FAILURE);
        }    
        
        // redirect the output
        if (dup2(file_desc, STDOUT_FILENO) == -1) {
            perror("Error redirecting output");
            exit(EXIT_FAILURE);;
        }   
    
        // do what should be done
        vector<string> temp; // for history
        for (long unsigned int i = 0; i < found; ++i) {           
            char cc[30000];
            strcpy(cc, tokens[i].c_str());
            temp.push_back((string)cc);
        }

        int ret = main_execution(temp, ppath, aliases);
        if (ret == 1)
            execute(temp);  

        // restore it to stdout 
        if (dup2(saved_stdout, STDOUT_FILENO) == -1) 
            perror("Error restoring standard output");
           
        close(file_desc);

    }

void add_command_to_history(vector<string>& history, const string& line){
    if (history.size() < 20){
        history.push_back(line);
    } else{
        history.erase(history.begin());
        history.push_back(line);
    }
}

string get_previous_command(const vector<string>& history, int& current_pos){
    if (current_pos > 0) {
        current_pos--;
        return history[current_pos];
    } else{
        return " ";
    }

    
}

string get_next_command(const vector<string>& history, int& current_pos){
    if (current_pos < (int)(history.size() - 1)) {
        current_pos++;
        return history[current_pos];
    } else{
        return " ";
    }
}

void execute(const vector<string>& vec_arg){
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

void execute_without_fork(const vector<string>& vec_arg){
    
    // Convert the vector of strings to an array of C-style strings
    cout << "eeep";
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

void execute_bg(vector<string>& vec_arg, string& ppath, map<string, string>& aliases){
    pid_t pid; 

    pid = fork();

    if (pid == -1) {    // Error forking
        perror("mysh");
        exit(EXIT_FAILURE);
    } 

    if (pid == 0) {   // Child process

        // Create a new session and process group for the child process.
        setsid();

        int ret = main_execution(vec_arg, ppath, aliases);
        if (ret == 1)
            execute_without_fork(vec_arg);
        exit(EXIT_SUCCESS);
    } else{
        return; 
        // parent doesn't wait for the child to finish
    }
}
    
void handle_wildcards(const string& pattern) {
  glob_t glob_result;

  int result = glob(pattern.c_str(), 0, NULL, &glob_result);
  
  if (result == 0) {

    for (long unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
      cout << glob_result.gl_pathv[i] << endl;
    }
    globfree(&glob_result);
  } else if (result == GLOB_NOMATCH) {

    cout << "No matches found.\n";
  } else {
    
        perror("Error: glob() failed");
  }
  
}

// pipes
void handle_pipes(vector<string>& tokens, string& ppath, map<string, string>& aliases){
    
    // find the two commands
    long unsigned int found = 0;    // flag to split commands
    vector<string> command1;
    vector<string> command2;

   // cout << "hiiiiiiiiiiiii";
    for(long unsigned int i = 0; i < tokens.size(); ++i) {
        if (found != 1){
            if (strcmp(tokens[i].c_str(),"|") == 0){
                found = 1;
                continue; 
            }   
        } 

        if (found == 0){
            char cc[30000];
            strcpy(cc, tokens[i].c_str());
            command1.push_back((string)cc);
        }else{
            char cc[30000];
            strcpy(cc, tokens[i].c_str());
            command2.push_back((string)cc);
        }
    }
    
    int fd[2];
    pipe(fd);

    int pid = fork();
    if (pid == 0) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);

        int ret = main_execution(command1, ppath, aliases);
        if (ret == 1)
            execute_without_fork(command1);
        exit(EXIT_SUCCESS);

    } else {
        int pid2 = fork();
        if (pid2 == 0) {
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);
            close(fd[1]);
            
            int ret = main_execution(command2, ppath, aliases);
            if (ret == 1)
                execute_without_fork(command2);     
            exit(EXIT_SUCCESS);                                        
        } else {
            close(fd[0]);
            close(fd[1]);
            waitpid(pid, NULL, 0);
            waitpid(pid2, NULL, 0);
        }
    }
}

// handle signals
void handle_sigint(int signal_c) {
    std::cout << "\nIgnoring SIGINT signal\n";
    fflush(stdout);
    // exit(0);
}

void handle_sigtstp(int signal_z) {
    cout << "path >";
    // pause();
}

int main_execution(vector<string>& tokens, string& ppath, map<string, string>& aliases){


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
        cout << "hi";
        execute_bg(tokens, ppath, aliases);
        return 0;
    }
    // redirection output
    else if(flag_output == 1){
        redirection_output(tokens, ppath, aliases);
        return 0;
    } 
    // redirection output
    else if(flag_output_app == 1){
        cout << "fhfjafkafk" << endl;
        redirection_output_double(tokens, ppath, aliases);
        return 0;
    } 
    // redirection input
    else if(flag_input == 1){
        redirection_input(tokens, ppath, aliases);
        return 0;
    } 
    // handle pipes
    else if(flag_pipe == 1){
        handle_pipes(tokens, ppath, aliases);
        return 0;
    } 
    // cd
    else if(tokens[0] == "cd"){
        if (tokens.size() == 1){
                // std::filesystem::path currentPath = std::filesystem::current_path();
                const char* path = "../syspro_project1";
                if (chdir(path) == -1) {
                    cout << "Error changing directory to " << path << endl;
                }
                ppath = (string)"mysh> ";
        } else{
                const char* path = tokens[1].c_str();
                if (chdir(path) == -1) {
                    cout << "Error changing directory to " << path << endl;
                }
                ppath = (string)"mysh> ";
                ppath.append((string)path);
                ppath.append((string)"$ ");

        }
        return 0;
            
    } 
    // wild characters
    else if(tokens[0] == "ls" &&  (tokens[tokens.size()-1].find("*") != string::npos || tokens[tokens.size()-1].find("?") != string::npos)){ 
        handle_wildcards(tokens[tokens.size()-1]);
                return 0;

    } 
    // alaising 
    else if (tokens[0] == "createalias" || tokens[0] == "destroyalias"){
        if (tokens[0] == "createalias"){            
            aliases[tokens[1]] = tokens[2];
        }else if(tokens[0] == "destroyalias"){
            aliases.erase(tokens[1]);
        }
        return 0;
    } // simple execution
    else
        return 1;
    //    execute(tokens);

}