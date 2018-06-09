//
//  main.cpp
//  project1
//
//  Created by Cheng Xu on 1/12/18.
//  Copyright © 2018 Cheng Xu. All rights reserved.
//

#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <deque>
#include "getopt.h"

using namespace std;

struct option longOpts[] = {
    { "stack",  no_argument, nullptr, 's'},
    { "queue",  no_argument, nullptr, 'q'},
    { "change", no_argument, nullptr, 'c'},
    { "swap",   no_argument, nullptr, 'p'},
    { "length", no_argument, nullptr, 'l'},
    { "output", required_argument, nullptr, 'o'},
    { "begin",  required_argument, nullptr, 'b'},
    { "end",    required_argument, nullptr, 'e'},
    { "help",   no_argument, nullptr, 'h'}
};

class Dictionary{
public:
    Dictionary(): word(""), prev(-1),position(-1), letter(' '), change_mode(' '),used(false) {}    //for initializing
    Dictionary(string word_in) :word(word_in), prev(-1), position(-1), letter(' '), change_mode(' '),used(false) {}    //for inserting word
    string word;
    int prev;
    int position;
    char letter;
    char change_mode;
    bool used;
    
};

bool swap_or_change(string &current_word, Dictionary &dictionary, char output_mode, bool can_swap, bool can_change);
bool length_word(string &current_word, Dictionary &dictionary, char output_mode);

int main(int argc, char * argv[]) {
    ios_base::sync_with_stdio(false);
    ostringstream os;
    char dictionary_type;
    int dictionary_size;
    int option_index = 0, option = 0;
    bool routing_mode = true;  //true represents stack mode, false represents queue mode
    int num_mode = 0;   //number of routing mode from the command line
    int num_change_mode = 0;    //number of change mode from the command line
    bool start_word_specified = false;
    bool end_word_specified = false;
    bool output_mode_specified = false;
    bool can_change = false, can_swap = false, can_length = false;
    string start_word, end_word;
    char output_mode;
    int start_index=-1 , end_index=-1;  //-1 is used to judge whether the specified word exists in the dictionary
    string word;
    vector<Dictionary> dictionary;
    deque<int> index;
    cin>>dictionary_type;
    cin>>dictionary_size;
    while ((option = getopt_long(argc, argv, "sqcplo:b:e:h", longOpts, &option_index)) != -1){
        switch (option){
            case 's':
                routing_mode = true;
                num_mode++;
                break;
            case 'q':
                routing_mode = false;
                num_mode++;
                break;
            case 'c':
                can_change = true;
                num_change_mode++;
                break;
            case 'p':
                can_swap = true;
                num_change_mode++;
                break;
            case 'l':
                can_length = true;
                num_change_mode++;
                break;
            case 'o':
                output_mode = optarg[0];
                output_mode_specified = true;
                break;
            case 'b':
                start_word = optarg;
                start_word_specified = true;
                break;
            case 'e':
                end_word = optarg;
                end_word_specified = true;
                break;
            case 'h':
                os<< "EECS281 Project1!\n" ;
                exit(0);
                break;
        }
    }
    
    if(!output_mode_specified){
        output_mode = 'W';
    }
    
    //illegal command input
    if(num_mode == 0 || num_mode == 2){
        cerr<<"Must type in exactly one routing scheme.\n";
        exit(1);
    }
    if(num_change_mode == 0){
        cerr<<"Must type in at least one changing skill.\n";
        exit(1);
    }
    if(output_mode != 'W' && output_mode != 'M'){
        cerr<<"Must type in a correct output mode.\n";
        exit(1);
    }
    if(!start_word_specified || !end_word_specified){
        cerr<<"Must specify a start word and an end word.\n";
        exit(1);
    }
    
    if(dictionary_type == 'S'){
        //push all the words in simple mode into dictionary
        dictionary.reserve(dictionary_size);
        getline(cin,word);
        if(can_length){
            while(getline(cin,word)){
                if(word.find("//") != string::npos){
                    //skip command lines
                    continue;
                }
                dictionary.push_back(Dictionary(word));
                if(word==start_word){
                    start_index = int(dictionary.size())-1;
                }
                else if(word == end_word){
                    end_index = int(dictionary.size())-1;
                }
            }
        }
        else{
            while(getline(cin,word)){
                if(word.find("//") != string::npos){
                    //skip command lines
                    continue;
                }
                if(word.length() == start_word.length()){
                    dictionary.push_back(Dictionary(word));
                    if(word==start_word){
                        start_index = int(dictionary.size())-1;
                    }
                    else if(word == end_word){
                        end_index = int(dictionary.size())-1;
                    }
                }
            }
            dictionary.resize(dictionary.size());
        }
    }
    else{
        //push all the words in complex mode into dictionary
        dictionary.reserve(3*dictionary_size);
        getline(cin,word);
        while(getline(cin,word)){
            //Since in complex mode there will be more words, we need to increase the capacity of dictionary
            bool spec_char_found = false;
            if(word.find("//") != string::npos){
                //skip command lines
                continue;
            }
            for(int i = 0; i<int(word.length()); i++){
                if( word[i] == '&' ){
                    //case of reverse: '&' will always appear in the last position
                    spec_char_found = true;
                    word.erase(i,1);
                    dictionary.push_back(Dictionary(word));
                    if(word == start_word){
                        start_index = int(dictionary.size())-1;
                    }
                    else if(word == end_word){
                        end_index = int(dictionary.size())-1;
                    }
                    reverse(word.begin(),word.end());
                    dictionary.push_back(Dictionary(word));
                    if(word == start_word){
                        start_index = int(dictionary.size())-1;
                    }
                    else if(word == end_word){
                        end_index = int(dictionary.size())-1;
                    }
                    break;
                }
                else if( word[i] == '['){
                    //case of insert-each: []
                    int second_bracket_index;
                    string insert_chars;
                    spec_char_found = true;
                    second_bracket_index= int(word.find(']'));
                    insert_chars = word.substr(i+1, second_bracket_index-i-1);
                    word.erase(i,second_bracket_index-i+1);
                    for(int j = 0; j<int(insert_chars.length()); j++){
                        //insert one character into the word
                        word.insert(i,1,insert_chars[j]);
                        dictionary.push_back(Dictionary(word));
                        if(word == start_word){
                            start_index = int(dictionary.size())-1;
                        }
                        else if(word == end_word){
                            end_index = int(dictionary.size())-1;
                        }
                        //restore to the original word
                        word.erase(i,1);
                    }
                    break;
                }
                else if( word[i] == '!'){
                    //case of swap
                    string swap_word;
                    spec_char_found = true;
                    word.erase(i,1);
                    //swap the two characters before '!'
                    swap_word = word;
                    swap_word[i-1] = word[i-2];
                    swap_word[i-2] = word[i-1];
                    //put in-order word into dictionary
                    dictionary.push_back(Dictionary(word));
                    if(word == start_word){
                        start_index = int(dictionary.size())-1;
                    }
                    else if(word == end_word){
                        end_index = int(dictionary.size())-1;
                    }
                    //put swapped word into dictionary
                    dictionary.push_back(Dictionary(swap_word));
                    if(swap_word == start_word){
                        start_index = int(dictionary.size())-1;
                    }
                    else if(swap_word == end_word){
                        end_index = int(dictionary.size())-1;
                    }
                    break;
                }
                else if( word[i] == '?'){
                    //put original word into dictionary
                    spec_char_found = true;
                    word.erase(i,1);
                    dictionary.push_back(Dictionary(word));
                    if(word == start_word){
                        start_index = int(dictionary.size())-1;
                    }
                    else if(word == end_word){
                        end_index = int(dictionary.size())-1;
                    }
                    //put double-char word into dictionary
                    word.insert(i,1,word[i-1]);
                    dictionary.push_back(Dictionary(word));
                    if(word == start_word){
                        start_index = int(dictionary.size())-1;
                    }
                    else if(word == end_word){
                        end_index = int(dictionary.size())-1;
                    }
                    break;
                }
            }
            if(!spec_char_found){
                //if no special character is found, then just put the original word into dictionary
                dictionary.push_back(Dictionary(word));
                if(word == start_word){
                    start_index = int(dictionary.size())-1;
                }
                else if(word == end_word){
                    end_index = int(dictionary.size())-1;
                }
            }
            //delete the unused capacity of the dictionary
            dictionary.resize(dictionary.size());
        }
    }
    
    //illegal command input
    if(start_index==-1 || end_index==-1){
        cerr<<"Must specify a word existing in the dictionary for start word and end word.\n";
        exit(1);
    }
    if(!can_length && start_word.length()!=end_word.length()){
        cerr<<"Must specify the start word and end word that match in length without typing the length command.\n";
        exit(1);
    }
    //track the routine to find end word
    int current_index;
    int checked_word = 0;
    index.push_front(start_index);
    checked_word++;
    dictionary[start_index].used=true;
    while(!index.empty()){
        current_index = index.front();
        index.pop_front();
        for(int i = 0; i<int(dictionary.size()); i++){
            if(!dictionary[i].used){
                if(can_swap || can_change){
                    if(swap_or_change(dictionary[current_index].word, dictionary[i], output_mode, can_swap, can_change)){
                        dictionary[i].used = true;
                        dictionary[i].prev = current_index;
                        if(routing_mode){//stack mode
                            index.push_front(i);
                        }
                        else{//queue mode
                            index.push_back(i);
                        }
                        checked_word++;
                        continue;
                    }
                }
                if(can_length){
                    if(length_word(dictionary[current_index].word, dictionary[i], output_mode)){
                        dictionary[i].used = true;
                        dictionary[i].prev = current_index;
                        if(routing_mode){//stack mode
                            index.push_front(i);
                        }
                        else{//queue mode
                            index.push_back(i);
                        }
                        checked_word++;
                    }
                }
            }
            if(dictionary[end_index].used){
                break;
            }
        }
        if(dictionary[end_index].used){
            break;
        }
    }
    if(dictionary[end_index].used){
        deque<int> output_index;
        int num_in_deque = 0;
        int word_index = end_index;
        while(word_index != start_index){
            output_index.push_front(word_index);
            num_in_deque++;
            word_index = dictionary[word_index].prev;
        }
        output_index.push_front(word_index);
        num_in_deque++;
        os<<"Words in morph: "<<num_in_deque<<"\n";
        os<<dictionary[output_index.front()].word<<"\n";
        output_index.pop_front();
        while(!output_index.empty()){
            if(output_mode == 'W'){
                os<<dictionary[output_index.front()].word<<"\n";
                output_index.pop_front();
            }
            else if(output_mode == 'M'){
                os<<dictionary[output_index.front()].change_mode<<","<<dictionary[output_index.front()].position;
                if(dictionary[output_index.front()].letter != ' '){
                    os<<","<<dictionary[output_index.front()].letter<<"\n";
                }
                else{
                    os<<"\n";
                }
                output_index.pop_front();
            }
        }
    }
    else{
        os<<"No solution, "<<checked_word<<" words checked.\n";
    }
    cout<<os.str();
    return 0;
}

bool swap_or_change(string &current_word,Dictionary &dictionary, char output_mode, bool can_swap, bool can_change){
    int char_diff = 0;
    int first_diff =0 , second_diff= 0;
    if(dictionary.word.length() == current_word.length()){
        for(int i = 0; i<int(current_word.length()); i++){
            if(current_word[i] != dictionary.word[i]){
                char_diff++;
                if(char_diff == 1){
                    first_diff = i;
                }
                else if(char_diff == 2){
                    second_diff = i;
                }
                else return false;
            }
        }
        if(char_diff == 1){//change
            if(can_change){
                if(output_mode == 'M'){
                    dictionary.position = first_diff;
                    dictionary.letter = dictionary.word[first_diff];
                    dictionary.change_mode = 'c';
                }
                return true;
            }
            else return false;
        }
        else{
            if(can_swap){
                if(current_word[first_diff]==dictionary.word[second_diff] && current_word[second_diff]==dictionary.word[first_diff] && second_diff-first_diff == 1){
                    if(output_mode == 'M'){
                        dictionary.position = first_diff;
                        dictionary.change_mode = 's';
                    }
                    return true;
                }
                else return false;
            }
            else return false;
        }
    }
    else return false;
}
bool length_word(string &current_word, Dictionary &dictionary, char output_mode){
    if(dictionary.word.length() == current_word.length()-1){//current_word's length is longer:maybe delete
        int char_diff = 0;
        for(int i=0; i<int(current_word.length()); i++){
            if(dictionary.word[i] != current_word[i]){
                char_diff = i;
                break;
            }
        }
        for(int i=char_diff+1; i<int(current_word.length()); i++){
            if(dictionary.word[i-1] != current_word[i]){
                return false;
            }
        }
        if(output_mode == 'M'){
            dictionary.position = char_diff;
            dictionary.change_mode = 'd';
        }
        return true;
    }
    else if(dictionary.word.length() == current_word.length()+1){//current_word's length is shorter:maybe insert
        int char_diff = 0;
        for(int i=0; i<int(dictionary.word.length()); i++){
            if(dictionary.word[i] != current_word[i]){
                char_diff = i;
                break;
            }
        }
        for(int i= char_diff+1; i<int(dictionary.word.length()); i++){
            if(dictionary.word[i] != current_word[i-1]){
                return false;
            }
        }
        if(output_mode == 'M'){
            dictionary.position = char_diff;
            dictionary.letter = dictionary.word[char_diff];
            dictionary.change_mode = 'i';
        }
        return true;
    }
    else return false;
}

