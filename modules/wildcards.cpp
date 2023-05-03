#include "../include/wildcards.h"          

void handle_wildcards(const string& pattern) {
  	glob_t glob_result;

	int result = glob(pattern.c_str(), 0, NULL, &glob_result);

	if (result == 0) {								// match found
		for (long unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
			cout << glob_result.gl_pathv[i] << endl;
		}
		globfree(&glob_result);
	} else if (result == GLOB_NOMATCH) {			// match not found

		cout << "No matches found.\n";
	} else {
		perror("Error: glob() failed");				// error
	}
  
}