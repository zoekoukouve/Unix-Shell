#include "../include/history.h"   

void add_command_to_history(vector<string>& history, const string& line){
    if (line == " " || line.size() == 0)
        return;
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