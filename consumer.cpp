/*
    Single Programmer Affidavit
    I the undersigned promise that the submitted assignment is my own work. While I was
    free to discuss ideas with others, the work contained is my own. I recognize that
    should this not be the case; I will be subject to penalties as outlined in the course
    syllabus.
    Name: Enoch Chigbu
    Red ID: 827078077

    Name: Farhan Talukder
    Red ID: 827061320
*/

#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include "report.h"
#include "tradecrypto.h"
#include "shared.h"

#define NORMAL_EXIT 0
#define MILLISECONDS 1000

void *consumer(void *ptr)
{
    ConsumerArg *consumerArg = static_cast<ConsumerArg *>(ptr);

    Consumers type = consumerArg->type;
    SHARED_DATA *sharedData = consumerArg->sharedData;

    while (true)
    {
        // Mutual exclusive access
        pthread_mutex_lock(&sharedData->broker_mutex);

        // Wait until the queue is not empty
        while (sharedData->broker->size() == 0)
            pthread_cond_wait(&sharedData->consumerCond, &sharedData->broker_mutex);

        // Take request off queue, take off queue
        RequestType request = sharedData->broker->front();
        sharedData->broker->pop();

        // Signal the producer that a new slot is avaliable in the queue
        pthread_cond_signal(&sharedData->producerCond);

        // Decrement amount of bitcoin transactions in queue if request taken off is bitcoin
        if (request == Bitcoin)
        {
            // Increment based on blockchain x or y
            if (type == 0)
                sharedData->bitcoinTransactionsBlockchainX++;
            else
                sharedData->bitcoinTransactionsBlockchainY++;

            sharedData->amountOfBitcoinTransactionsInQueue--;
            pthread_cond_signal(&sharedData->bitcoinCond);
        }
        else
        {
            sharedData->amountOfEthereumTransactionsInQueue--;
            // Increment based on blockchain x or y
            if (type == 0)
                sharedData->ethereumTransactionsBlockchainX++;
            else
                sharedData->ethereumTransactionsBlockchainY++;
        }
        sharedData->requestsConsumed++;

        unsigned int consumed[RequestTypeN];

        // Log
        if (type == 0)
        {
            consumed[0] = sharedData->bitcoinTransactionsBlockchainX;
            consumed[1] = sharedData->ethereumTransactionsBlockchainX;
        }
        else
        {
            consumed[0] = sharedData->bitcoinTransactionsBlockchainY;
            consumed[1] = sharedData->ethereumTransactionsBlockchainY;
        }

        unsigned int inRequestQueue[] = {sharedData->amountOfBitcoinTransactionsInQueue, sharedData->amountOfEthereumTransactionsInQueue};
        report_request_removed(type, request, consumed, inRequestQueue);

        // Unlock mutex access
        pthread_mutex_unlock(&sharedData->broker_mutex);

        // Signal barrier if max amount of requests have been processed & broker has either one element or is empty
        if ((sharedData->requestsConsumed >= sharedData->totalNumberOfRequests) && (sharedData->broker->size() == 0))
        {
            sem_post(sharedData->barrier);
            break;
        }

        // Sleep for specified time
        if (type == 0)
            usleep(sharedData->blockchainXSleepTime * MILLISECONDS);
        else
            usleep(sharedData->blockchainYSleepTime * MILLISECONDS);
    }

    // Exit Consumer Thread
    pthread_exit(NORMAL_EXIT);
};