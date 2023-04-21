// Your First C++ Program
#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

using namespace std;

void mysh_loop();
void execute(const vector<string>&);

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
        while (token != NULL) {
            words.push_back(token);
            token = strtok(NULL, " ");
        }

        execute(words);
    } 
}



void execute(const vector<string>& vec_arg){
    pid_t pid; //, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process

        // Convert the vector of strings to an array of C-style strings
        // Treat the vector as a NULL-terminated array of string pointers
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
    } else if (pid == -1) {
        // Error forking
        perror("mysh");
    } else {
        // Parent process
        //wpid = waitpid(pid, &status, 0);    
        waitpid(pid, &status, 0);   
    }

}