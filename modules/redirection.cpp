#include "../include/redirection.h"          
#include "../include/execution.h"   

void redirection_input(vector<string>& tokens,  vector<string>& history){
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

    // save the input to restore it to stdin later 
    int saved_stdin = dup(STDIN_FILENO);
    if (saved_stdin == -1) {
        perror("Error duplicating file descriptor");
        exit(EXIT_FAILURE);
    }    
    
    // redirect the input
    if (dup2(file_desc, STDIN_FILENO) == -1) {
        perror("Error redirecting input");
        exit(EXIT_FAILURE);;
    }

    // do what should be done
    vector<string> temp; 
    for (long unsigned int i = 0; i < found; ++i) {
        char cc[30000];
        strcpy(cc, tokens[i].c_str());
        temp.push_back((string)cc);;
    }
    int ret = main_execution(temp, history);
    if (ret == 1)
        execute(temp);       

    // restore it to stdin
    if (dup2(saved_stdin, STDIN_FILENO) == -1) {
        perror("Error restoring standard output");
        exit(EXIT_FAILURE);;
    }

    close(file_desc);
}

void redirection_output_double(vector<string>& tokens,  vector<string>& history){
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

    int ret = main_execution(temp, history);
    if (ret == 1)
        execute(temp);  
    //execute(temp);                                      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // restore it to stdout 
    if (dup2(saved_stdout, STDOUT_FILENO) == -1) 
        perror("Error restoring standard output");
        
    close(file_desc);
}

void redirection_output(vector<string>& tokens,  vector<string>& history){
    
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

    int ret = main_execution(temp, history);
    if (ret == 1)
        execute(temp);  

    // restore it to stdout 
    if (dup2(saved_stdout, STDOUT_FILENO) == -1) 
        perror("Error restoring standard output");
        
    close(file_desc);

}
