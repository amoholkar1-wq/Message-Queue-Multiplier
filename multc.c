///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		Adam Moholkar			11/06/2024			mult.c
//
//      Purpose: Mult process sends receives a message from the server process and multiplies that
//      message's value by their multiplier value and will then send back the result to the server process.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>     // for library function prototypes
#include <stdio.h> 	    // similar to import java.io
#define QNAME 170       // base queue name = server

struct mess {
    long mtype;   // set up below to receive a 'type 2' message
    int nextInt;  // next value to use
} message;

int main(int argc, char *argv[])
{
    int qidServer,      // used to store the qid of server's message queue
        qidMult,        // used to store the qid of mult's message queue
        statusSnd,      // keeps track of whether or not message is successfully sent or not
        statusRcv,      // keeps track of whether or not message is successfully received or not
        statusDel,      // keeps track of whether or not message queue is successfully deleted or not
        queueName,      // keeps track of correct queue name for associated mult program
        multID,         // keeps track of the value to be added to base queue name to get correct queue name
        multiplier,     // keeps track of the value sent from server to mult's queue
        result,         // keeps track of the result to be sent back from mult to server's queue
        numIterations;  // keeps track of number of messages to be sent back to server

    multID = atoi(argv[1]);
    numIterations = atoi(argv[2]);  // to keep track of number of 'X's to print out - maybe change name
    queueName = QNAME + multID;     // gets correct queue name for the specified multiplier

    // get access to mult's qid
    qidMult = msgget(queueName, 0600 | IPC_CREAT);  // qid = 170
    if (qidMult == -1) {
        printf("Error with %d : \n", queueName);
        perror("msgget error: error creating mult's queue \n");
        return 0;
    }
    // get access to server's qid
    qidServer = msgget(QNAME, 0600 | IPC_CREAT);  // qid = 170
    if (qidServer == -1) {
        printf("Error with %d : \n", QNAME);
        perror("msgget error: error creating server's queue \n");
        return 0;
    }
    for (int i = 0; i < numIterations; i++){
        // receive message from server and save result into mult's queue
        statusRcv = msgrcv(qidMult, &message, sizeof(message)-sizeof(long), 2, 0);
        if (statusRcv == -1) {
            perror("msgrcv error");
            return 0;
        }
        multiplier = message.nextInt;   // get server's message value
        result = multiplier*multID;     // get new value to send back to server
        message.mtype = 2;
        message.nextInt = result;       // populate message with new value
        // send new message back to server's message queue
        statusSnd = msgsnd(qidServer, &message, sizeof(message)-sizeof(long),0);
        if (statusSnd == -1) {
            perror("msgsnd error");
            return 0;
        }
    }
    statusDel = msgctl(qidMult, IPC_RMID, NULL);
    // error handling for deleting server's message queue
    if (statusDel == -1){
        perror("msgctl error: error deleting server's message queue");
        return 0;
    }
} // end of main