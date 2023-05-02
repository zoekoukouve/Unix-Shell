




void main_execution(){

    
    int flag_output = 0;
    int flag_input = 0;
    int flag_pipe = 0;
    
    for (long unsigned int i = 0; i < tokens.size(); ++i) {
            if (tokens[i] == "<") {
                flag_input = 1;
                // redirection_input(tokens);
                // break;
            }
            if (tokens[i] == ">") {
                flag_output = 1;
                // redirection_output(tokens);
                // break;
            }
            if (tokens[i] == "|") {
                flag_pipe = 1;
                // handle_pipes(tokens);
                // break;
            }
            
    }
    
    if(tokens[tokens.size()-1] == "&"){  
        tokens.erase(tokens.end());
        cout << "hi";
        execute_bg(tokens);

    } else if(flag_output == 1){
        redirection_output(tokens);
    } else if(flag_input == 1){
        redirection_input(tokens);
    } else if(flag_output == 1){
        handle_pipes(tokens);
    } else if(tokens[0] == "exit"){
            return;
        }
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
            
    } else if(tokens[0] == "ls" &&  (tokens[tokens.size()-1].find("*") != string::npos || tokens[tokens.size()-1].find("?") != string::npos)){ 
            handle_wildcards(tokens[tokens.size()-1]);
        } 
        // simple execution
        else
            execute(tokens);

}