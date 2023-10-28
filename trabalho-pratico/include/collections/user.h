#ifndef TP_COLLECTIONS_USER_H
#define TP_COLLECTIONS_USER_H

#include "debug.h"
#include "util/collection.h"
#include "util/error.h"
#include "util/string.h"
#include <stdbool.h>
#include <stdint.h>

#define MAX_USER_ID_LEN 60

// Lenght of the largest portuguese proper noun: 
// "Manuel Maria Filipe Carlos Amélio Luís Miguel Rafael Gabriel Gonzaga Xavier "
// "Francisco de Assis Eugénio de Bragança Orleães Sabóia e Saxe-Coburgo-Gotha"
#define MAX_NAME_LEN 156

// #define UserId(_id) char _id[MAX_USER_ID_LEN]
#define UserId(name) TypedArrayVariable(char, name, MAX_USER_ID_LEN)
#define CountryCode(name) TypedArrayVariable(char, name, 2)

typedef struct user {
    UserId(id);
    char name[MAX_NAME_LEN];
    bool sex;
    // char country_code[2];
    CountryCode(country_code);
    int account_creation; // Offset from Base Date
    bool account_status;

    // Statistics
    uint8_t age;
} USER, *User;

USER makeUser(
    // UserId(id),
    // char name[MAX_NAME_LEN]
    char* id,
    char* name,



    bool sex, 
    CountryCode(country_code), 
    int account_creation, 
    bool account_status
);

USER parseUserFromLine(char* line, int len);
#endif