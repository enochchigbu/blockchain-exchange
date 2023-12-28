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

#include <queue>
#include <pthread.h>
#include <semaphore.h>

#include "tradecrypto.h"

typedef struct
{
    // Queue for requests, broker
    std::queue<RequestType> *broker;

    // Mutual exclusive lock for broker
    pthread_mutex_t broker_mutex;

    // Conditonal variables for threads
    pthread_cond_t producerCond, consumerCond;

    // Conditonal for bitcoin thread
    pthread_cond_t bitcoinCond;

    // Semaphore, default value 0
    sem_t *barrier;

    // amount of requests processed
    unsigned int requestsProcessed;

    // total bitcoin requests processed
    unsigned int totalBitcoinRequests;

    // amount of requests consumed
    unsigned int requestsConsumed;

    // total ethereum requests processed
    unsigned int totalEthereumRequests;

    // Amount of bitcoin transactions in queue, can't be more than 6
    unsigned int amountOfBitcoinTransactionsInQueue;

    // Amount of ethereum transactions in queue
    unsigned int amountOfEthereumTransactionsInQueue;

    // Amount of bitcoin transactions processed by blockchain X;
    unsigned int bitcoinTransactionsBlockchainX;

    // Amount of bitcoin transactions processed by blockchain Y;
    unsigned int bitcoinTransactionsBlockchainY;

    // Amount of ethereum transactions processed by blockchain X;
    unsigned int ethereumTransactionsBlockchainX;

    // Amount of ethereum transactions processed by blockchain Y;
    unsigned int ethereumTransactionsBlockchainY;

    // Total number of requests, defaults to 120 if command line arg isnt present
    unsigned int totalNumberOfRequests;

    // All args below default to 0 if no command line arg is given
    // Specifies the number of milliseconds N that the consumer using Blockchain X for processing a trade request and completing its transaction
    unsigned int blockchainXSleepTime;

    // Specifies the number of milliseconds N that the consumer using Blockchain Y for processing a trade request and completing its transaction
    unsigned int blockchainYSleepTime;

    // number of milliseconds required to produce and publish a Bitcoin request
    unsigned int bitcoinRequestTime;

    // number of milliseconds required to produce and publish a Ethereum request
    unsigned int ethereumRequestTime;
} SHARED_DATA;

// Structs for holding proudcer/consumer data and type
typedef struct
{
    Requests type;
    SHARED_DATA *sharedData;
} ProducerArg;

typedef struct
{
    Consumers type;
    SHARED_DATA *sharedData;
} ConsumerArg;