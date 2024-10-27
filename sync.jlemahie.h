//
// Created by James on 10/26/2024.
//

#ifndef SYNCHRONIZATION_SYNC_JLEMAHIE_H
#define SYNCHRONIZATION_SYNC_JLEMAHIE_H

#define NUM_RECEIVERS 4
#define MAX_MESSAGE_LEN 31

#include <stdbool.h>
#include <pthread.h>

typedef struct {
    bool done; // whether or not the program is done
    pthread_mutex_t mutex; // synchronization for this variable
} ControlInfo;

typedef struct {
    int recipientID; // id of intended recipient
    char message[MAX_MESSAGE_LEN+1]; // the message for the recipient
    char reply[MAX_MESSAGE_LEN+1]; // the reply from the recipient
    bool messageReady; // whether data is ready for the recipient
    bool replyReady; // whether reply is ready for the sender
    pthread_mutex_t mutex; // synchronization for these variables
} DataInfo;

typedef struct {
    int myID; // id of sender or receiver
    DataInfo *data; // the data
    ControlInfo *control; // control
} ThreadInfo;

void *receiver(void *param);

void *sender(void *param);

void *controller(void *param);

void spin(int val);
#endif //SYNCHRONIZATION_SYNC_JLEMAHIE_H
