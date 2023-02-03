#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

using namespace std;

/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

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
        res.push_back(str);
        return true;
    }

    for(int i = 0; i < vec.size(); i++) {
        if(reverse == vec.at(i)) {
            res.push_back(str);
            return true;
        }
    }
    
    return false;
}

int main() {
    double start_time, end_time; /* start and end times */

    vector<string> vec;
    vector<string> result;

    bool res = iterateFile([&](const string & str){vec.push_back(str);});

    toLowerCase(vec);


    int total = 0;

    start_time = read_timer();

    for(int i = 0; i < vec.size(); i++) {
        if(palindromCheck(vec.at(i), vec, result)) {
            total++;
        }
    }

    end_time = read_timer();

    cout << "Total palindromes: " << total;
    cout << "\nTotal time: " << (end_time-start_time) << " s";

    printResultToFile(result);

    return 0;
}