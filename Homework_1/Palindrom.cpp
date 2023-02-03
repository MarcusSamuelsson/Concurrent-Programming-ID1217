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

pthread_mutex_t mutex;

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
        pthread_mutex_lock(&mutex);
        res.push_back(str);
        pthread_mutex_unlock(&mutex);
        return true;
    }

    for(int i = 0; i < vec.size(); i++) {
        if(reverse == vec.at(i)) {
            pthread_mutex_lock(&mutex);
                res.push_back(str);
            pthread_mutex_unlock(&mutex);
            return true;
        }
    }
    
    return false;
}

 typedef struct THARGS{
    int index;
    vector<string> vec;
    vector<string> &res;
    int start;
    int end;
    int &tot;
} thArgs;

void *multiSearch (void *args) {
    thArgs *th_args = (thArgs*)args;

    int total;

    for(int i = th_args->start; i < th_args->end; i++) {
        if(palindromCheck(th_args->vec.at(i), th_args->vec, th_args->res)) {
            total++;
        }
    }

    cout << "Thread " << th_args->index << " found a total of " << total << " palindromes" << "\n";

    pthread_mutex_lock(&mutex);
        th_args->tot += total;
    pthread_mutex_unlock(&mutex);

    return 0;
}

void multiThreadSearch (vector<string> &vec, vector<string> &res, int totThreads, int &tot) {
    pthread_t th[totThreads];
    thArgs *args[totThreads];

    int vecSizePerTh = (int)vec.size()/totThreads;

    pthread_mutex_init(&mutex, NULL);
    
    for(int i = 0; i < totThreads; i++) {
        args[i] = new thArgs {
            i,
            vec,
            res,
            vecSizePerTh*i,
            vecSizePerTh*(i+1),
            tot
        };

        pthread_create(&th[i], NULL, multiSearch, args[i]);
    }

    for(int i = 0; i < totThreads; i++) {
        pthread_join(th[i], NULL);
    }
}

int main(int argc, char *argv[]) {
    double start_time, end_time; /* start and end times */

    int workers = atoi(argv[1]);

    vector<string> vec;
    vector<string> result;

    bool res = iterateFile([&](const string & str){vec.push_back(str);});

    toLowerCase(vec);

    int total = 0;

    start_time = read_timer();

    multiThreadSearch(vec, result, workers, total);

    end_time = read_timer();

    cout << "Total palindromes: " << total;
    cout << "\nTotal time: " << (end_time-start_time) << " s";

    printResultToFile(result);

    return 0;
}