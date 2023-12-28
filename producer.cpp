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
#define MAX_AMOUNT_OF_BITCOIN_TRANSACTIONS_IN_BROKER 6
#define MAX_AMOUNT_OF_REQUESTS_IN_BROKER 15

void *producer(void *ptr)
{
    ProducerArg *producerArg = static_cast<ProducerArg *>(ptr);

    RequestType type = producerArg->type;
    SHARED_DATA *sharedData = producerArg->sharedData;

    while (true)
    {
        // Sleep for specified time
        if (type == 0)
            usleep(sharedData->bitcoinRequestTime * MILLISECONDS);
        else
            usleep(sharedData->ethereumRequestTime * MILLISECONDS);

        // Mutual exclusive access
        pthread_mutex_lock(&sharedData->broker_mutex);

        // Check if queue is full, if it is then conditional must wait
        while (sharedData->broker->size() >= MAX_AMOUNT_OF_REQUESTS_IN_BROKER)
            pthread_cond_wait(&sharedData->producerCond, &sharedData->broker_mutex);

        // Process Bitcoin or Ethereum transaction
        // Bitcoin
        if (type == 0)
        {
            // Check if 6 Bitcoin transactions are in queue
            while (sharedData->amountOfBitcoinTransactionsInQueue >= MAX_AMOUNT_OF_BITCOIN_TRANSACTIONS_IN_BROKER)
                pthread_cond_wait(&sharedData->bitcoinCond, &sharedData->broker_mutex);

            // Add bitcoin request to queue, increment bitcoin transactions to queue
            sharedData->broker->push(Bitcoin);
            sharedData->amountOfBitcoinTransactionsInQueue++;
            sharedData->totalBitcoinRequests++;
        }
        // Ethereum
        else
        {
            // Add ethereum request to queue, increment ether count
            sharedData->broker->push(Ethereum);
            sharedData->amountOfEthereumTransactionsInQueue++;
            sharedData->totalEthereumRequests++;
        }

        // Log report
        unsigned int produced[] = {sharedData->totalBitcoinRequests, sharedData->totalEthereumRequests};
        unsigned int inRequestQueue[] = {sharedData->amountOfBitcoinTransactionsInQueue, sharedData->amountOfEthereumTransactionsInQueue};
        report_request_added(type, produced, inRequestQueue);

        // Signal the consumer that an element is in queue
        pthread_cond_signal(&sharedData->consumerCond);

        // Unlock mutex access
        pthread_mutex_unlock(&sharedData->broker_mutex);

        // Increment and Check if all requests processed
        sharedData->requestsProcessed++;
        if (sharedData->requestsProcessed >= sharedData->totalNumberOfRequests - 1)
            break;
    }

    // Exit thread
    pthread_exit(NORMAL_EXIT);
};