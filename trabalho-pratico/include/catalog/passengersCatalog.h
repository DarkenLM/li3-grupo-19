#ifndef PASSENGERS_CATALOG
#define PASSENGERS_CATALOG

#include "collections/passenger.h"
#include "common.h"

/**
 * @file passengers_catalog.h
 * @brief Header file for Passengers Catalog.
 */

/**
 * @brief Comparison function for the Passengers Catalog.
 *
 * @param a The first item to compare.
 * @param b The second item to compare.
 * @return An integer less than, equal to, or greater than zero if a is found, respectively, to be less than, equal to, or greater than b.
 */
gint passengersCatalog_full_compare_func(gconstpointer a, gconstpointer b);

gint passenger_flightID_compare_func(gconstpointer a, gconstpointer b);

/**
 * @brief Writes a passenger to the Passengers Catalog.
 *
 * @param passenger The passenger to be added.
 * @param store The parser store.
 */
void passengersCatalog_write_to_catalog(void* _passenger, ParserStore store);

#endif /* PASSENGERS_CATALOG */
