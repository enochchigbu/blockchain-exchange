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

#include <fcntl.h>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <getopt.h>
#include <queue>

#include "tradecrypto.h"
#include "report.h"
#include "shared.h"
#include "producer.h"
#include "consumer.h"

#define DEFAULT_NUMBER_OF_REQUESTS 120
#define DEFAULT_VALUE_OPTIONAL_ARG 0
#define NORMAL_EXIT 0

int main(int argc, char **argv)
{
    // Intiialization of shared data (default arguments)
    SHARED_DATA sharedData;
    sharedData.requestsProcessed = 0;
    sharedData.requestsConsumed = 0;
    sharedData.totalBitcoinRequests = 0;
    sharedData.totalEthereumRequests = 0;
    sharedData.amountOfBitcoinTransactionsInQueue = 0;
    sharedData.amountOfEthereumTransactionsInQueue = 0;
    sharedData.bitcoinTransactionsBlockchainX = 0;
    sharedData.bitcoinTransactionsBlockchainY = 0;
    sharedData.ethereumTransactionsBlockchainX = 0;
    sharedData.ethereumTransactionsBlockchainY = 0;
    sharedData.totalNumberOfRequests = DEFAULT_NUMBER_OF_REQUESTS;
    sharedData.blockchainXSleepTime = DEFAULT_VALUE_OPTIONAL_ARG;
    sharedData.blockchainYSleepTime = DEFAULT_VALUE_OPTIONAL_ARG;
    sharedData.bitcoinRequestTime = DEFAULT_VALUE_OPTIONAL_ARG;
    sharedData.ethereumRequestTime = DEFAULT_VALUE_OPTIONAL_ARG;

    sharedData.broker = new std::queue<RequestType>();
    pthread_mutex_init(&sharedData.broker_mutex, nullptr);
    pthread_cond_init(&sharedData.producerCond, nullptr);
    pthread_cond_init(&sharedData.consumerCond, nullptr);
    pthread_cond_init(&sharedData.bitcoinCond, nullptr);

    // Ensures that a new semaphore is created
    sem_unlink("/barrier");
    sharedData.barrier = sem_open("/barrier", O_CREAT, 0644, 0);

    // optional arg processing
    int option;
    while ((option = getopt(argc, argv, "n:x:y:b:e:")) != -1)
    {
        switch (option)
        {
        case 'n':
            if (atoi(optarg) < 0)
            {
                std::cout << "Total number of trade requests must be greater than 0" << std::endl;
                exit(NORMAL_EXIT);
            }
            sharedData.totalNumberOfRequests = atoi(optarg);
            break;
        case 'x':
            if (atoi(optarg) < 0)
            {
                std::cout << "Number of milliseconds for sleep time with Blockchain X must be greater than 0" << std::endl;
                exit(NORMAL_EXIT);
            }
            sharedData.blockchainXSleepTime = atoi(optarg);
            break;
        case 'y':
            if (atoi(optarg) < 0)
            {
                std::cout << "Number of milliseconds for sleep time with Blockchain Y must be greater than 0" << std::endl;
                exit(NORMAL_EXIT);
            }
            sharedData.blockchainYSleepTime = atoi(optarg);
            break;
        case 'b':
            if (atoi(optarg) < 0)
            {
                std::cout << "Number of milliseconds for sleep time among Bitcoin requests must be greater than 0" << std::endl;
                exit(NORMAL_EXIT);
            }
            sharedData.bitcoinRequestTime = atoi(optarg);
            break;
        case 'e':
            if (atoi(optarg) < 0)
            {
                std::cout << "Number of milliseconds for sleep time among Ethereum requests must be greater than 0" << std::endl;
                exit(NORMAL_EXIT);
            }
            sharedData.ethereumRequestTime = atoi(optarg);
            break;
        default:
            std::cout << "Invalid Argument, ending program" << std::endl;
            exit(NORMAL_EXIT);
        }
    }

    // create producer (Bitcoin, Ethereum) threads
    pthread_t bitcoinThread;
    pthread_t ethereumThread;

    // create consumer (Blockchain X, Y) threads
    pthread_t blockchainXThread;
    pthread_t blockchainYThread;

    // Create arguments for producer threads
    ProducerArg bitcoinArg = {Bitcoin, &sharedData};
    ProducerArg ethereumArg = {Ethereum, &sharedData};

    // Create arguments for consumer threads
    ConsumerArg blockchainXArg = {BlockchainX, &sharedData};
    ConsumerArg blockchainYArg = {BlockchainY, &sharedData};

    // start producer threads
    if (pthread_create(&bitcoinThread, NULL, &producer, &bitcoinArg))
    {
        // error handling
        std::cout << "bitcoin thread failed" << std::endl;
        exit(NORMAL_EXIT);
    }
    if (pthread_create(&ethereumThread, NULL, &producer, &ethereumArg))
    {
        // error handling
        std::cout << "ethereum thread failed" << std::endl;
        exit(NORMAL_EXIT);
    }

    // start consumer threads
    if (pthread_create(&blockchainXThread, NULL, &consumer, &blockchainXArg))
    {
        // error handling
        std::cout << "Blockchain X thread failed" << std::endl;
        exit(NORMAL_EXIT);
    }
    if (pthread_create(&blockchainYThread, NULL, &consumer, &blockchainYArg))
    {
        // error handling
        std::cout << "Blockchain Y thread failed" << std::endl;
        exit(NORMAL_EXIT);
    }

    // Wait for producer threads to finish
    pthread_join(bitcoinThread, NULL);
    pthread_join(ethereumThread, NULL);

    // Wait for the signal from a consumer thread
    sem_wait(sharedData.barrier);

    // Log
    unsigned int produced[] = {sharedData.totalBitcoinRequests, sharedData.totalEthereumRequests};
    unsigned int **consumed = new unsigned int *[2];

    for (int i = 0; i < 2; i++)
    {
        consumed[i] = new unsigned int[2];
    }

    consumed[0][0] = sharedData.bitcoinTransactionsBlockchainX;
    consumed[0][1] = sharedData.ethereumTransactionsBlockchainX;
    consumed[1][0] = sharedData.bitcoinTransactionsBlockchainY;
    consumed[1][1] = sharedData.ethereumTransactionsBlockchainY;

    report_production_history(produced, consumed);

    // Exit Program
    return 0;
}
