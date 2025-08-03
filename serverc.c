///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		Adam Moholkar			11/06/2024			server.c
//
//      Purpose: Server process sends and receives messages to 3 mult processes and populates a list, with
//      the smallest X values of the form 2^a * 3^b * 5^c, and displays this information back to the user.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h> 	    // similar to import java.io
#include <stdlib.h> 	// for library function prototypes
#include <unistd.h> 	// other miscellaneous prototypes
#include <sys/wait.h> 	// for wait() wrapper function prototype
#include <string.h>  	// provides function prototypes for all the built in C-style string library functions
#define QNAME 170       // base queue name = server

struct mess {
    long mtype;   // receives a 'type 2' message
    int nextInt;  // next value to use
} message;

// insertion sort which takes in an array and the number of valid elements - sorts an array from min -> max
void insertionSort(int array[], int numElements)
{
    int temp, j;
    for (int i = 1; i < numElements; i++){
        temp = array[i];
        j = i - 1;
        while (j >= 0 && array[j] > temp)  {
            array[j + 1] = array[j];
            j = j - 1;
        }
        array[j + 1] = temp;
    }
}

// function takes in an array and the number of valid elements in the array - removes and returns the first
// element in array.  Swaps element that was removed with -1 and array is shifted where -1 is at the end
// of the array. However, since numElements is decremented in the main program, all -1 values become out
// of scope
int popFront(int array[], int numElements)
{
    int temp;
    temp = array[0];
    for (int i = 0; i < numElements; i++) {
        array[i] = array[i+1];
    }
    array[numElements] = -1;
    return temp;
}

int main(int argc, char *argv[])
{
    int statusSnd,          // keeps track of whether or not message is successfully sent or not
        statusRcv,          // keeps track of whether or not message is successfully received or not
        statusDel,          // keeps track of whether or not message queue is successfully deleted or not
        qidServer,          // used to store the qid of server's message queue
        qidMultA,           // used to store the qid or multA's (2) message queue
        qidMultB,           // used to store the qid or multB's (3) message queue
        qidMultC,           // used to store the qid or multC's (5) message queue
        index,              // used to keep track of next available position in array
        numIterations,      // stores the number of values to print out (# of iterations)
        countMessages,      // used to ensure only 3 values are received
        checker,            // used as a boolean to check if a value is a duplicate in server's list
        tempMessage,        // used to keep track of the smallest value in server's list
        numValsOutputted;   // number printed as an index when outputting each value

    numIterations = atoi(argv[1]);      // extracts second argument from command
    int smallVals[numIterations*3];     // array keeps track of all values sent back from each mult process
    index = 0;

    qidServer = msgget(QNAME, 0600 | IPC_CREAT);     // get's server's message qid (Qname = 170)
    // Error handling for server's qid queue creation
    if (qidServer == -1) {
        perror("msgget error: error creating server's queue");
        return 0;
    }
    qidMultA = msgget(QNAME + 2, 0600 | IPC_CREAT);  // gets multA's message qid (Qname = 172)
    // Error handling for multA's qid queue creation
    if (qidMultA == -1) {
        perror("msgget error: error creating multA's queue");
        return 0;
    }
    qidMultB = msgget(QNAME + 3, 0600 | IPC_CREAT);  // gets multB's message qid (Qname = 173)
    // Error handling for multB's qid queue creation
    if (qidMultB == -1) {
        perror("msgget error: error creating multB's queue");
        return 0;
    }
    qidMultC = msgget(QNAME + 5, 0600 | IPC_CREAT);  // gets multC's message qid (Qname = 175)
    // Error handling for multC's qid queue creation
    if (qidMultC == -1) {
        perror("msgget error: error creating multC's queue");
        return 0;
    }
    numValsOutputted = 1;
    for (int i = 1; i <= numIterations; i++){
        message.mtype = 2;
        if (i > 1){ // message = smallest value in list after previous iteration
            message.nextInt = tempMessage;
        } else {    // message = 1, if this is the first iteration
            message.nextInt = 1;
        }
        // send message to the first multiplier
        statusSnd = msgsnd(qidMultA, &message, sizeof(message)-sizeof(long),0);
        // Error handling for message sent to multA's message queue
        if (statusSnd == -1) {
            perror("msgsnd error: error sending message to multA's queue");
            return 0;
        }
        // Error handling for message sent to multB's message queue
        statusSnd = msgsnd(qidMultB, &message, sizeof(message)-sizeof(long),0);
        if (statusSnd == -1) {
            perror("msgsnd error: error sending message to multB's queue");
            return 0;
        }
        // Error handling for message sent to multC's message queue
        statusSnd = msgsnd(qidMultC, &message, sizeof(message)-sizeof(long),0);
        if (statusSnd == -1) {
            perror("msgsnd error: error sending message to multC's queue");
            return 0;
        }
        countMessages = 0;
        while (countMessages < 3) { // receives a message from each mult processes
            statusRcv = msgrcv(qidServer, &message, sizeof(message)-sizeof(long), 2, 0);
            if (statusRcv == -1) {
                if (countMessages == 0){
                    printf("multA error: \n");
                } else if (countMessages == 1){
                    printf("multB error: \n");
                } else {
                    printf("multC error: \n");
                }
                perror("msgrcv error: error receiving message");
                return 0;
            }
            countMessages++;

            // checks to see if value received in message is already in server's list
            checker = 0;
            for (int j = 0; j < index; j++){
                if (smallVals[j] == message.nextInt){
                    checker = 1;
                }
            }
            if (checker == 0){  // if value isn't in list, add value into server's list
                smallVals[index] = message.nextInt;
                index++;
            }
        }  // end of while loop - receiving messages
        insertionSort(smallVals, index - 1);    // sorts the list from least -> greatest
        tempMessage = popFront(smallVals, index - 1);   // sets tempMessage = smallest value in list , and
                                                        // removes that value from the list
        index--;        // decremented because value was removed from list in popFront()
        printf("%d - smallest value = %d \n", numValsOutputted, tempMessage);
        numValsOutputted++;
    }
    // deletes server's message queue
    statusDel = msgctl(qidServer, IPC_RMID, NULL);
    // error handling for deleting server's message queue
    if (statusDel == -1){
        perror("msgctl error: error deleting server's message queue");
        return 0;
    }
} // end of main
