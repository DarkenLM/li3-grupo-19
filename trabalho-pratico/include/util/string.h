#ifndef TP_UTIL_STRING_H
#define TP_UTIL_STRING_H

// Remove warning
#define __USE_XOPEN
#define _GNU_SOURCE

#include "common.h"
#include "error.h"
#include <strings.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>
// #include <wchar.h>
#include <time.h>
#include <math.h>

#include "parser/parser.h"

#define TIME_T_SYSTEM 1696118400

#define IS_NULL(ARG)         ((ARG) == NULL)
#define IS_STRING_NULL(ARG)  (IS_NULL((ARG)) || *(ARG) == '\0')

#define STRING_EQUAL(a, b) (strcmp((a), (b)) == 0)

#define CHAR_IS_LOWER(ch) ((ch) >= 97 && (ch) <= 122)
#define CHAR_IS_UPPER(ch) ((ch) >= 65 && (ch) <= 90)

static inline char char_tolower(char ch) {
    if (CHAR_IS_UPPER(ch)) return ch + ('a' - 'A');
    else return ch;
}

static inline char char_toupper(char ch) {
    if (CHAR_IS_LOWER(ch)) return ch + ('A' - 'a');
    else return ch;
}


char* isnprintf(const char *format, ...);

// /**
//  * @brief Converts a char string to a widechar string.
//  * 
//  * @param str The char array to be converted.
//  */
// wchar_t* char_str_to_wchar(char** str);

/**
 * @brief Splits a string into it's composing lines.
 * 
 * @param line The string.
 * @param len The length of the string.
 * @return A Tokens struct containing the lines within the string.
*/
Tokens get_lines(char* line, size_t len);

/**
 * @brief Verify if a certain char its a digit.
 * 
 * @param c Digit.
 * 
 * @return 1 if its a digit, otherwise 0.
*/
int is_digit(char c);

/**
 * @brief Verify if a certain char its a positive digit.
 * 
 * @param c Digit.
 * 
 * @return 1 if its a positive digit, otherwise 0.
*/
int is_digit_positive(char c);

/**
 * @brief Verify if a string contains a non negative number.
 * 
 * @param number String that contains the number.
 * 
 * @return 1 if it contains a non digit number, otherwise 0.
*/
int is_integer_positive_or_zero(char* number);

/**
 * @brief Verify if a string contains a positive number.
 * 
 * @param number String that contains the number.
 * 
 * @return 1 if it contains a positive digit number, otherwise 0.
*/
int is_integer_positive(char* number);

/**
 * @brief Verify if a string contains a number between 1 and 5.
 * 
 * @param number String that contains the number.
 * 
 * @return 1 if it contains a digit between 1 and 5, otherwise 0.
*/
int is_integer_between_one_and_five(char* number);

/**
 * @brief Verify if a matrix have white strings.
 * 
 * @param parameter Array of strings.
 * @param number_parameters Number of strings.
 * 
 * @return 1 if it doesn't contains white strings, otherwise 0.
*/
int have_whitespace(char* parameter[], int num_parameters);

/**
 * @brief Verify if a string represent an email.
 * 
 * @param parameter String that contains the email.
 * @param length Length of the string.
 * 
 * @return 1 if its an email, otherwise 0.
*/
int is_email(char* parameter, int length);

/**
 * @brief Verify if a string represent a boolean.
 * 
 * @param value String that contains the possible boolean.
 * 
 * @return 1 if it represents a boolean, otherwise 0.
*/
int is_boolean(char* value);

/**
 * @brief Verify a string length.
 * 
 * @param string String to be verified.
 * @param length Possible length of the string.
 * 
 * @return 1 if it has that length or more, otherwise 0.
*/
int is_length(char* string, int length);

/**
 * @brief Verify if a string represents a date.
 * 
 * @param string String to be verified.
 * 
 * @return 1 if it represents, otherwise 0.
*/
int is_date(char* string);

/**
 * @brief Verify if a string represents a date with time.
 * 
 * @param string String to be verified.
 * 
 * @return 1 if it represents, otherwise 0.
*/
int is_date_with_time(char* string);

/**
 * @brief Get the sex gender from the string.
 * 
 * @param parameter String containing the gender.
 * 
 * @return TRUE if its Male, FALSE otherwise.
*/
bool get_sex(char* parameter);

/**
 * @brief Get the account status from the string.
 * 
 * @param parameter String containing the account status.
 * 
 * @return TRUE if its active, FALSE otherwise.
*/
bool get_account_status(char* parameter);

/**
 * @brief Turns a string that represents a date to an int.
 * 
 * @param parameter String containing the date.
 * 
 * @return Date in int.
*/
int date_string_to_int(char* parameter);

/**
 * @brief Turns a string that represents a date with time to an int.
 * 
 * @param parameter String containing the date with time.
 * 
 * @return Date with time in int.
*/
int date_with_time_string_to_int(char* parameter);

/**
 * @brief Turns an that represents a date with time to a string.
 * 
 * @param parameter Integer containing the date with time.
 * 
 * @return String formatted with the date.
*/
char* date_int_to_string(int time);

/**
 * @brief Get the boolean from the string.
 * 
 * @param parameter String containing the boolean.
 * 
 * @return TRUE if its true, FALSE otherwise.
*/
bool get_boolean(char* parameter);

char* to_upper_string(char* parameter);

/**
 * @brief Converts a string to int.
 *
 * @param input The input string to be converted.
 *
 *     The following are inconvertible:
 *
 *     - empty string
 *     - leading whitespace
 *     - any trailing characters that are not part of the number
 *
 *     Cannot be NULL.
 *
 * @param base The base to interpret the string in. Same range as strtol (2 to 36).
 *
 * @return The int corresponding to the given string.
 */
int string_to_based_int(char* input, int base);

#endif