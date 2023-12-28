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

#ifndef REPORT_H
#define REPORT_H

#include "tradecrypto.h"

/**
 * @brief Show that a request has been added to the request queue and
 *        print the current status of the broker request queue.
 *
 * @param type              What kind of request was produced?
 * @param produced          Array of number of requests of each type that have been
 *                          produced
 * @param inRequestQueue    Array of number of requests of each type that are
 *                          in the request queue and have not yet been consumed.
 *                          (inRequestQueue[Bitcoin] and inRequestQueue[Ethereum])
 *
 * produced and inRequestQueue reflect numbers *after* adding the current request.
 */
void report_request_added(RequestType type,
                          unsigned int produced[],
                          unsigned int inRequestQueue[]);

/**
 * @brief   Show that an item has been removed from the request queue
 *          and print the current status of the broker request queue.

 * @param consumers         Who removed and processed the request?
 * @param type              What kind of request was removed?  As each consumer consumes
 *                          one type of request, this is the same as the producer.
 * @param consumed          Array of number of requests of each type that have been
 *                          consumed.
 * @param inRequestQueue    Array of number of requests of each type that are
 *                          in the request queue and have not yet been consumed.
 *                          (inRequestQueue[Bitcoin] and inRequestQueue[Ethereum])
 *
 * Counts reflect numbers *after* the request has been removed
 */
void report_request_removed(Consumers consumers, RequestType type,
                            unsigned int consumed[], unsigned int inRequestQueue[]);

/**
 * @brief   Show how many requests of each type produced.
 *          Show how many requests consumed by each consumer.
 *
 * @param produced   count for each RequestType produced
 * @param consumed   array of pointers to consumed arrays for each consumer
 *                   e.g. consumed[BlockchainA] points to an array that
 *                   is indexed by request type
 *                   (it is an 2-D array, consumed[BlockchainA][Bitcoin] is
 *                    the number for bitcoin trade requests that were
 *                    transacted on BlockchainA)
 */
void report_production_history(unsigned int produced[], unsigned int *consumed[]);

#endif
