#include "../include/wildcards.h"        
#include "../include/execution.h"   


void exec_wildcards(vector<string>& tokens, glob_t& glob_result){

	// special case - cd command
	if (tokens[0] == "cd"){
		glob_t glob_result_dir;
		int result = glob(tokens[tokens.size()-1].c_str(), GLOB_ONLYDIR, NULL, &glob_result_dir);

		if (result == 0) {								// match found
			if (glob_result_dir.gl_pathc == 1){ // execute command
				string directory = (string) glob_result_dir.gl_pathv[0];
				if (chdir(directory.c_str()) != 0) {
					cerr << "Error: " << directory << " is not a valid directory\n";
				}	
			} else {
				perror("Error: cd wildcard, too many arguments");
			}
			globfree(&glob_result_dir);
			
		} else if (result == GLOB_NOMATCH) {			// match not found
			cout << "No matches found.\n";
		} else {
			perror("Error: glob() failed");				// error
		}

		return;
	}


	// find the command
	vector<string> tokenst = tokens;
	tokenst.pop_back();

	// execute the command foreach result
	for (long unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
		tokenst.push_back(glob_result.gl_pathv[i]);
		execute(tokenst);
		tokenst.pop_back();
	}
}

void handle_wildcards(vector<string>& tokens){
	
	glob_t glob_result;

	int result = glob(tokens[tokens.size()-1].c_str(), 0, NULL, &glob_result);


	if (result == 0) {								// match found
		exec_wildcards(tokens, glob_result);
		globfree(&glob_result);
	} else if (result == GLOB_NOMATCH) {			// match not found
		cout << "No matches found.\n";
	} else {
		perror("Error: glob() failed");				// error
	}
}