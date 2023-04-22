// Your First C++ Program
#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <glob.h>           // glob(), globfree()

using namespace std;

void mysh_loop();
void execute(const vector<string>&);
void execute_bg(const vector<string>&);
void handle_wildcards(string&);

int main(int argc, char **argv) {
    //cout << "Hello World!";
    


    // Run command loop.
    mysh_loop();

    return EXIT_SUCCESS;
}


void mysh_loop(){
    string line;
    //string* args;
    int status = 1;

    while(status != 0){
        printf("mysh> ");
        
        getline(cin, line);                   // read_line
        
        vector<string> words;                 // slpit line to tokens
        char* token = strtok(&line[0], " ");
        while(token != NULL) {
            words.push_back(token);
            token = strtok(NULL, " ");
        }

        if (words[words.size()-1] == "&"){ // backround
            words.erase(words.end());
            cout << "hi";
            execute_bg(words);
        } else if(words[0] == "ls" &&  (words[words.size()-1].find("*") != string::npos || words[words.size()-1].find("?") != string::npos)){ 
            // wild characters 
            handle_wildcards(words[words.size()-1]);
        } 
        else
            execute(words);
    } 
}



void execute(const vector<string>& vec_arg){
    pid_t pid; //, wpid;
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
    pid_t pid; //, wpid;

    pid = fork();

    if (pid == -1) {    // Error forking
        perror("mysh");
        exit(EXIT_FAILURE);
    } 

    if (pid == 0) {   // Child process

        // Create a new session and process group
        // for the child process.
        setsid();
        
        // Convert the vector of strings to an array of C-style strings
        vector<char*> cargs(vec_arg.size() + 1);
        for (long unsigned int i = 0; i < vec_arg.size(); ++i) {
            cargs[i] = const_cast<char*>(vec_arg[i].c_str());
        }
        cargs[vec_arg.size()] = NULL;

        // print pid 
        cout << "\n[1] " << getppid() << endl;
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
    


void handle_wildcards(string& pattern) {
  glob_t glob_result;

  int ret = glob(pattern.c_str(), 0, NULL, &glob_result);
  
  if (ret == 0) {

    for (long unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
      cout << glob_result.gl_pathv[i] << endl;
    }
    globfree(&glob_result);
  } else if (ret == GLOB_NOMATCH) {

    cout << "No matches found.\n";
  } else {
    
        perror("Error: glob() failed");
  }
  
}
