#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <time.h>
#include <sys/time.h>
#include <omp.h>

using namespace std;


bool iterateFile(function<void (const string & )> callback) {
    ifstream wordfile("./Homework_1/words");
    if(!wordfile) {
        cout << "No file found";
        return false;
    }

    string line;
    while (getline(wordfile, line)) {
        callback(line);
    }

    wordfile.close();
    return true;
}

string reverseString (string str) {
    string final = "";

    for(int i = 0; i < str.length(); i++) {
        final += str.at(str.length()-i-1);
    }

    return final;
}

void toLowerCase (vector<string>& vector) {
    for(int i = 0; i < vector.size(); i++) {
        for(char &c : vector[i]) {
            c = tolower(c);
        }
    }
}

void printResultToFile (vector<string>& res) {
    ofstream resultFile("result");

    for(int i = 0; i < res.size(); i++) {
        resultFile << res.at(i) << "\n";
    }
    
    resultFile.close();
}


bool palindromCheck (string str, vector<string> vec, vector<string>& res) {

        string reverse = reverseString(str);

        if(reverse == str){
            
            #pragma omp critical 
            {
                res.push_back(str);
            }
            
            return true;
        }

        for(int i = 0; i < vec.size(); i++) {
            if(reverse == vec.at(i)) {
                
                #pragma omp critical 
                {
                    res.push_back(str);
                }
                
                return true;
            }
        }  

        return false;
}


void multiThreadSearch (vector<string> &vec, vector<string> &res) {
    int total = 0;

    #pragma omp parallel 
    {
        #pragma omp for 
        for(int i = 0; i < vec.size(); i++) {
            if(palindromCheck(vec.at(i), vec, res)) {
                #pragma omp critical 
                {
                    total++;
                }
            }
        }
    }

    cout << "Total palindromes: " << total;
}

int main(int argc, char *argv[]) {
    int workers = atoi(argv[1]);

    vector<string> vec;
    vector<string> result;

    bool res = iterateFile([&](const string & str){vec.push_back(str);});

    toLowerCase(vec);

    omp_set_num_threads(workers);

    double start_time = omp_get_wtime();

    multiThreadSearch(vec, result);

    double end_time = omp_get_wtime();

    cout << "\nTotal time: " << (end_time-start_time) << " s";

    printResultToFile(result);

    return 0;
}