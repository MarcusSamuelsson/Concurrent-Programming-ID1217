#include <cstdio>
#include <cstdlib>
#include <ostream>
#include <sstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

using namespace std;

int genderInBathroom = 0;
int inBathroom = 0;

sem_t bathroomAccess;
sem_t bathroomInQueue;
sem_t writeLock;


struct thArgs{
    int index;
    int num;
    string sex;
};

void print(string s) {
    sem_wait(&writeLock);
    cout << s << endl;
    sem_post(&writeLock);
}

void bathroom(string id) {
    sem_wait(&bathroomAccess);
    inBathroom++;
    sem_post(&bathroomAccess);

    print(id + " is now taking a dump");

    sleep(rand() % 4 + 1);

    print(id + " has finished visit to bathroom");

    sem_wait(&bathroomAccess);
    inBathroom--;
    sem_post(&bathroomAccess);
}

void *work(void *args) {
    thArgs *th_args = (thArgs*)args;

    string name = th_args->sex + " " + to_string(th_args->num)
        + "(Thread " + to_string(th_args->index) + ")";

    while(true) {
        print(name + " started working now!");
        
        sleep(rand() % 15 + 5);

        print(name + " entered the bathroom queue");

        sem_wait(&bathroomInQueue);
        print(to_string(inBathroom) + " people in the bathroom");
        string temp = (genderInBathroom == 0 ? "The men" : "The women");
        print( temp + " are currently using the bathroom");
        if(genderInBathroom == 0){
            if(th_args->sex == "Man") {
                sem_post(&bathroomInQueue);
                bathroom(name);
            } else {
                while(inBathroom > 0);
                genderInBathroom = 1;
                sem_post(&bathroomInQueue);
                bathroom(name);
            }
        } else if (genderInBathroom == 1) {
            if(th_args->sex == "Man") {
                while(inBathroom > 0);
                genderInBathroom = 0;
                sem_post(&bathroomInQueue);
                bathroom(name);
            } else {
                sem_post(&bathroomInQueue);
                bathroom(name);
            }
        }
    }

    print("Work day is over!");

    return 0;
}

int main(int argc, char *argv[]) {
    sem_init(&bathroomAccess, 0, 1);
    sem_init(&bathroomInQueue, 0, 1);
    sem_init(&writeLock, 0, 1);

    int workers = atoi(argv[1]);
    cout << to_string(workers);
    thArgs *args[workers*2];

    pthread_t th[workers*2];

    for(int i = 0; i < workers; i++) {
        args[i] = new thArgs {
            (i+1),
            (i+1),
            "Man"
        };


        pthread_create(&th[i], 0, work, args[i]);
    }

    for(int i = 0; i < workers; i++) {
        args[i+workers-2] = new thArgs {
            (i+1+workers),
            (i+1),
            "Woman"
        };

        pthread_create(&th[i+workers], 0, work, args[i+workers]);
    }

    for(int i = 0; i < workers*2; i++) {
        pthread_join(th[i], NULL);
    }

    return 0;
}