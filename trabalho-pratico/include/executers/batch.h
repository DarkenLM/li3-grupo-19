#ifndef TP_EXECUTERS_BATCH_H
#define TP_EXECUTERS_BATCH_H

#include "collections/user.h"
#include "collections/reservation.h"
#include "collections/flight.h"
#include "collections/passenger.h"

void test_preprocessor(FILE* stream, ParserStore store);
void test_parser(Tokens tokens);
void test_writer(void* raw_data, FILE** store);

void batch(char* arg1, char* arg2);

#endif