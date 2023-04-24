// cd, exit, history velakia, anakatheuthunsh <<, prints &
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

using namespace std;

void mysh_loop();
void execute(const vector<string>&);
void execute_bg(const vector<string>&);
void handle_wildcards(const string&);
void add_command_to_history(vector<string>&, const string&);
string get_previous_command(const vector<string>&, int&);
string get_next_command(const vector<string>&, int&);
void redirection_output(const vector<string>&);
void redirection_input(const vector<string>&);
void handle_pipes(const vector<string>&);

int main(int argc, char **argv) {
    //cout << "Hello World!";

    // Run command loop.
    mysh_loop();

    return EXIT_SUCCESS;
}


void mysh_loop(){
    string line;

    int status = 1;
    map<string, string> aliases; // for aliasing
    vector<string> history; // for history

    while(status != 0){
        printf("mysh> ");
        
        getline(cin, line);                   // read_line
        
        vector<string> tokens;                 // slpit line to tokens
        char* token = strtok(&line[0], " ");
        while(token != NULL) {
            tokens.push_back(token);
            token = strtok(NULL, " ");
        }
        
        // handle aliasing
        if (tokens[0] == "createalias"){            
            aliases[tokens[1]] = tokens[2];
            continue;
        }else if(tokens[0] == "destroyalias"){
            aliases.erase(tokens[1]);
            continue;
        }

        // find aliasing 
        for (long unsigned int i = 0; i < tokens.size(); ++i) {
            if (aliases.find(tokens[i]) != aliases.end()) {
                tokens[i] = aliases[tokens[i]];
            }
        }

        // proswrinos kwdikas //////////////////////////////////////////////////////////////////////////////////////////////////////
        int flag = 0;
        for (long unsigned int i = 0; i < tokens.size(); ++i) {
            if (tokens[i] == "<") {
                flag = 1;
                redirection_input(tokens);
                break;
            }
            if (tokens[i] == ">") {
                flag = 1;
                redirection_output(tokens);
                break;
            }
        }
        if (flag == 1) continue;


        int flag1 = 0;
        for (long unsigned int i = 0; i < tokens.size(); ++i) {
            if (tokens[i] == "|") {
                flag1 = 1;
                handle_pipes(tokens);
                break;
            }
            
        }
        if (flag1 == 1) continue;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // backround
        if (tokens[tokens.size()-1] == "&"){  
            tokens.erase(tokens.end());
            cout << "hi";
            execute_bg(tokens);

        } 
        // wild characters 
        else if(tokens[0] == "ls" &&  (tokens[tokens.size()-1].find("*") != string::npos || tokens[tokens.size()-1].find("?") != string::npos)){ 
            handle_wildcards(tokens[tokens.size()-1]);
        } 
        // simple execution
        else
            execute(tokens);


        // add that command to history 
        add_command_to_history(history, line);

    } 
    
}

void redirection_input(const vector<string>& tokens){
         // opern the appropriate file
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
        execute(temp);                   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // restore it to stdin
        if (dup2(saved_stdout, STDIN_FILENO) == -1) {
            perror("Error restoring standard output");
            exit(EXIT_FAILURE);;
        }

        close(file_desc);
    }

void redirection_output(const vector<string>& tokens){
        
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
        execute(temp);                                      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

void execute_bg(const vector<string>& vec_arg){
    pid_t pid; 

    pid = fork();

    if (pid == -1) {    // Error forking
        perror("mysh");
        exit(EXIT_FAILURE);
    } 

    if (pid == 0) {   // Child process

        // Create a new session and process group for the child process.
        setsid();
        
        // Convert the vector of strings to an array of C-style strings
        vector<char*> cargs(vec_arg.size() + 1);
        for (long unsigned int i = 0; i < vec_arg.size(); ++i) {
            cargs[i] = const_cast<char*>(vec_arg[i].c_str());
        }
        cargs[vec_arg.size()] = NULL;

        // print pid 
        cout << "\n" << getppid() << endl;
        // Call execvp() with the array of arguments
        if (execvp(cargs[0], cargs.data()) == -1) {
            perror("mysh");
            exit(EXIT_FAILURE);
        }
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
void handle_pipes(const vector<string>& tokens){
    
    // find the two commands
    long unsigned int found = 0;    // flag to split commands
    vector<string> command1;
    vector<string> command2;


    for(long unsigned int i = 0; i < tokens.size(); ++i) {
        if (strcmp(tokens[i].c_str(),"|") == 0){
            found = 1;
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
        execute(command1);                                              ///////////////////////////////////////////////////////////////////////
        //execvp(cmd1[0], cmd1);
    } else {
        int pid2 = fork();
        if (pid2 == 0) {
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);
            close(fd[1]);
            //execvp(cmd2[0], cmd2);
            execute(command2);                                              /////////////////////////////////////////////////////////////////////////////////
        } else {
            close(fd[0]);
            close(fd[1]);
            waitpid(pid, NULL, 0);
            waitpid(pid2, NULL, 0);
        }
    }
}
