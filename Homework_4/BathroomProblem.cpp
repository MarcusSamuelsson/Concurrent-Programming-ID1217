#include <cstdio>
#include <cstdlib>
#include <ostream>
#include <sstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <chrono>
#include <ctime>

using namespace std;

class monitor {
private:
    int occupiedGender; //Men = 0, Women = 1
    
    int inBathroom;
    
    int inQueue;
 
    // condition variable to check whether reader can read
    pthread_cond_t bathroomInQueue;
 
    // condition variable to check whether writer can write
    pthread_cond_t bathroomAccess;
 
    // mutex for synchronization
    pthread_mutex_t condlock;
 
public:
    monitor() {
        occupiedGender = 0;
        inBathroom = 0;
        inQueue = 0;
 
        pthread_cond_init(&bathroomInQueue, NULL);
        pthread_cond_init(&bathroomAccess, NULL);
        pthread_mutex_init(&condlock, NULL);
    }
    
    void manEnter (string name) {
        pthread_mutex_lock(&condlock);
        if(inQueue > 0) {
            print("\n" + to_string(inQueue) + " people in the queue");
            print(to_string(inBathroom) + " people in the bathroom");
            string temp; 
            
            if(occupiedGender == 0) {
                temp = "The bathroom is empty\n";
            } else {
                temp = (occupiedGender == 1 ? "The men are currently using the bathroom\n" 
                : "The women are currently using the bathroom\n");
            }
            
            print(temp);
            inQueue++;
            (name + " is waiting in queue");

            pthread_cond_wait(&bathroomAccess, &condlock);
        }else {
            inQueue++;
        }

        if(occupiedGender == 2) {
            pthread_cond_wait(&bathroomAccess, &condlock);
            occupiedGender = 1;
            inBathroom++;
            print(name + " is now taking a dump");
        } else {
            occupiedGender = 1;
            inBathroom++;
            print(name + " is now taking a dump");
        }

        inQueue--;

        pthread_mutex_unlock(&condlock);
    }

    void manExit (string name) {
        pthread_mutex_lock(&condlock);
        print(name + " has finished visit to bathroom");
        inBathroom--;

        if(inBathroom == 0) {
            occupiedGender = 0;
            pthread_cond_signal(&bathroomAccess);
        }
        pthread_mutex_unlock(&condlock);
    }

    void womanEnter (string name) {
        pthread_mutex_lock(&condlock);
        if(inQueue > 0) {
            print("\n" + to_string(inQueue) + " people in the queue");
            print(to_string(inBathroom) + " people in the bathroom");
            string temp; 
            
            if(occupiedGender == 0) {
                temp = "The bathroom is empty\n";
            } else {
                temp = (occupiedGender == 1 ? "The men are currently using the bathroom\n" 
                : "The women are currently using the bathroom\n");
            }
            
            print(temp);

            inQueue++;
            print(name + " is waiting in queue");
            pthread_cond_wait(&bathroomAccess, &condlock);
        } else {
            inQueue++;
        }

        if(occupiedGender == 1) {
            pthread_cond_wait(&bathroomAccess, &condlock);
            occupiedGender = 2;
            inBathroom++;
            print(name + " is now taking a dump");
        } else {
            occupiedGender = 2;
            inBathroom++;
            print(name + " is now taking a dump");
        }

        inQueue--;
        
        pthread_mutex_unlock(&condlock);
    }

    void womanExit (string name) {
        pthread_mutex_lock(&condlock);
        print(name + " has finished visit to bathroom");
        inBathroom--;

        if(inBathroom == 0) {
            occupiedGender = 0;
            pthread_cond_signal(&bathroomAccess);
        }
        pthread_mutex_unlock(&condlock);
    }

    void print(string t) {
        time_t my_time = time(NULL);
        cout << ctime(&my_time) << t << "\n";
    }
}

// global object of monitor class
M;


struct thArgs{
    int index;
    int num;
    string sex;
};

void print(string t) {
        time_t my_time = time(NULL);
        cout << ctime(&my_time) << t << "\n";
    }

void *work(void *args) {
    thArgs *th_args = (thArgs*)args;

    string name = th_args->sex + " " + to_string(th_args->num)
        + "(Thread " + to_string(th_args->index) + ")";

    while(true) {
        print(name + " started working now!");
        
        sleep(rand() % 15 + 5);

        print(name + " has to take a bathroombreak");
        if(th_args->sex == "Man") {
            M.manEnter(name);
        } else {
            M.womanEnter(name);
        }
        
        sleep(rand() % 5 + 1);

        if(th_args->sex == "Man") {
            M.manExit(name);
        } else {
            M.womanExit(name);
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    int men = atoi(argv[1]);
    int women = atoi(argv[2]);

    thArgs *args[men+women];

    pthread_t th[men+women];

    for(int i = 0; i < men; i++) {
        args[i] = new thArgs {
            (i+1),
            (i+1),
            "Man"
        };


        pthread_create(&th[i], 0, work, args[i]);
    }

    for(int i = 0; i < women; i++) {
        args[i+men] = new thArgs {
            (i+1+men),
            (i+1),
            "Woman"
        };

        pthread_create(&th[i+men], 0, work, args[i+men]);
    }

    for(int i = 0; i < men+women; i++) {
        pthread_join(th[i], NULL);
    }

    return 0;
}