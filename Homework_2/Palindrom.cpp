#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <time.h>
#include <sys/time.h>
#include <omp.h>
#include <cmath>

using namespace std;


bool iterateFile(function<void (const string & )> callback, string filepath) {
    ifstream wordfile(filepath);
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

void printResultToFile (vector<string>& res, string filename) {
    ofstream resultFile(filename);

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

    cout << "\nTotal palindromes: " << total;
}

int main(int argc, char *argv[]) {
    int workers = atoi(argv[1]);

    vector<vector<string>> vec;
    vector<vector<string>> result;

    vec.resize(3);
    result.resize(3);

    iterateFile([&](const string & str){vec.at(0).push_back(str);}, "./Homework_2/words");
    iterateFile([&](const string & str){vec.at(1).push_back(str);}, "/usr/share/dict/words");
    iterateFile([&](const string & str){vec.at(2).push_back(str);}, "./Homework_2/tinyWords");

    toLowerCase(vec.at(0));
    toLowerCase(vec.at(1));
    toLowerCase(vec.at(2));

    vector<double> timePerThread;
    timePerThread.resize(workers);



    for(int x = 1; x <= 3; x++) {
        cout << "\nTest for file" << x << "\n";
        for(int i = 1; i <= workers; i++) {
            double totalForThread = 0;

            for (int j = 0; j < 10; j++) {
                omp_set_num_threads(i*2);

                double start_time = omp_get_wtime();

                multiThreadSearch(vec.at(x-1), result.at(x-1));

                double end_time = omp_get_wtime();

                totalForThread += (end_time-start_time);
                cout << "\nTotal time attempt " << j << ": " << (end_time-start_time) << " s";
            }

            cout << "\nAverage time for " << i*2 << "number of threads: " << (totalForThread/10) << " s";
        }
    }

    printResultToFile(result.at(0), "result1");
    printResultToFile(result.at(1), "result2");
    printResultToFile(result.at(3), "result3");

    return 0;
}