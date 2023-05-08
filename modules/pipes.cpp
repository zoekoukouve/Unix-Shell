#include "../include/pipes.h"   
#include "../include/execution.h"   

void handle_pipes(vector<string>& tokens,  vector<string>& history){
    
    // find the two commands
    long unsigned int found = 0;    // flag to split commands
    vector<string> command1;
    vector<string> command2;

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
        } else{
            char cc[30000];
            strcpy(cc, tokens[i].c_str());
            command2.push_back((string)cc);
        }
    }
    
    int fd[2]; // pipe
    pipe(fd);

    int pid = fork();
    if (pid == 0) {                     // execute command1
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);

        int ret = main_execution(command1, history);
        if (ret == 1)
            execute_without_fork(command1);
        exit(EXIT_SUCCESS);

    } else {                            // execute command2
        int pid2 = fork();
        if (pid2 == 0) {
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);
            close(fd[1]);
            
            int ret = main_execution(command2, history);
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
