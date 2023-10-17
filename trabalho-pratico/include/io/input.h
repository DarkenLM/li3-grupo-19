#ifndef INPUT_H
#define INPUT_H

// Verify if the parameters are valid
// parameter - The parameters to be verified of type char**
// Return 1 if it's everything correct, 0 otherwise 
int verify_user_tokens(char** parameter);

// Verify if the parameters are valid
// parameter - The parameters to be verified of type char**
// Return 1 if it's everything correct, 0 otherwise 
int verify_reservation_tokens(char** parameter);

// Verify if the parameters are valid
// parameter - The parameters to be verified of type char**
// Return 1 if it's everything correct, 0 otherwise 
int verify_flight_tokens(char** parameter);

// Verify if the parameters are valid
// parameter - The parameters to be verified of type char**
// Return 1 if it's everything correct, 0 otherwise 
int verify_passenger_tokens(char** parameter);

#endif