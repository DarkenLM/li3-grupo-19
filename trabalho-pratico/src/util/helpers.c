#include "util/helpers.h"
#include "stdlib.h"

int get_age(int date) {
    date = abs(date);
    date = date/(60*60*24);

    return (date/365);
}
