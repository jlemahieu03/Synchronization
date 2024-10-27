//
// Created by James on 10/26/2024.
//

#include "sync.jlemahie.h"
#include <stdio.h>
#include <stdlib.h>

int main(){
    ControlInfo control;
    control.done = false;
    pthread_mutex_init(&control.mutex, NULL);

    DataInfo data;
    pthread_mutex_init(&data.mutex, NULL);
    data.recipientID = -1;
    data.message[0] = '\0';
    data.reply[0] = '\0';
    data.messageReady = false;
    data.replyReady = false;

    ThreadInfo threadInfo[1 + NUM_RECEIVERS];

    for (int i = 0; i < NUM_RECEIVERS + 1; ++i){
        threadInfo[i].myID =  i;
        threadInfo[i].data = &data;
        threadInfo[i].control = &control;
    }

    pthread_t sender_thread;
    pthread_create(&sender_thread, NULL, sender, &threadInfo[0]);

    pthread_t receiver_threads[NUM_RECEIVERS];
    for (int i = 0; i < NUM_RECEIVERS; ++i){
        pthread_create(&receiver_threads[i], NULL, receiver, &threadInfo[i+1]);
    }

    pthread_t controller_thread;
    pthread_create(&controller_thread, NULL, controller, &control);

    pthread_join(sender_thread, NULL);

    for (int i = 0; i < NUM_RECEIVERS; i++) {
        pthread_join(receiver_threads[i], NULL);
    }

    pthread_join(controller_thread, NULL);
    return 0;
}

void spin(int val){
    for (int i = 0; i < val; ++i){
        for (int j = 0; j < val; ++j){
        }
    }
}

void *controller(void *param) {
    ControlInfo *control = (ControlInfo *) param;

    getchar();
    pthread_mutex_lock(&control->mutex);
    control->done = true;
    pthread_mutex_unlock(&control->mutex);

    return NULL;
}

void *sender (void *param){
    ThreadInfo *sender = (ThreadInfo *) param;
    srand(time(0));
    bool done = false;

    while(!done){
        spin(10000);
        pthread_mutex_lock(&sender->data->mutex);
        if (sender->data->replyReady){
            printf("reply: %s\n", sender->data->reply);
            sender->data->replyReady = false;
        }
        if (!(sender->data->messageReady)){
            int messageInt = rand() % NUM_RECEIVERS;
            sender->data->recipientID = messageInt;
            snprintf(sender->data->message, MAX_MESSAGE_LEN + 1, "message for %d", messageInt);
            sender->data->messageReady = true;
        }
        pthread_mutex_unlock(&sender->data->mutex);

        pthread_mutex_lock(&sender->control->mutex);
        if (sender->control->done){
            done = true;
        }
        pthread_mutex_unlock(&sender->control->mutex);
    }
    pthread_exit(NULL);

    return NULL;
}

void *receiver (void *param){
    ThreadInfo *receiver = (ThreadInfo *) param;
    bool done = false;

    while(!done){
        pthread_mutex_lock(&receiver->data->mutex);
        if (receiver->data->messageReady){
            if (receiver->data->recipientID == receiver->myID){
                printf("%s\n", receiver->data->message);
                receiver->data->messageReady = false;
                snprintf(receiver->data->reply, MAX_MESSAGE_LEN + 1, "%s - read by %d", receiver->data->message, receiver->myID);
                receiver->data->replyReady = true;
            }
        }
        pthread_mutex_unlock(&receiver->data->mutex);

        pthread_mutex_lock(&receiver->control->mutex);
        if(receiver->control->done){
            done = true;
        }
        pthread_mutex_unlock(&receiver->control->mutex);
    }

    pthread_exit(NULL);

    return NULL;
}
