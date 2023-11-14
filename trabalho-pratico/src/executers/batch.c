#include "executers/batch.h"

void test_preprocessor(FILE* stream, ParserStore store, va_list args) {
    gpointer null_element = NULL;
    g_array_append_vals(store, &null_element, 1); // Discard file
    default_csv_preprocessor(stream, store, args);   // File header
    //cvs_preprocessor_helper(stream, store);

    Catalog* catalogo = va_arg(args, Catalog*);
    g_array_append_vals(store, &catalogo, 1);
}

void test_preprocessor_reservation(FILE* stream, ParserStore store, va_list args) {
    gpointer null_element = NULL;
    g_array_append_vals(store, &null_element, 1); // Discard file
    default_csv_preprocessor(stream, store, args);   // File header
    //cvs_preprocessor_helper(stream, store);

    Catalog* catalogo = va_arg(args, Catalog*);
    g_array_append_vals(store, &catalogo, 1);

    Catalog* catalogo2 = va_arg(args, Catalog*);
    g_array_append_vals(store, &catalogo2, 1);
}

void test_preprocessor_passenger(FILE* stream, ParserStore store, va_list args) {
    gpointer null_element = NULL;
    g_array_append_vals(store, &null_element, 1); // Discard file
    default_csv_preprocessor(stream, store, args);   // File header
    //cvs_preprocessor_helper(stream, store);

    Catalog* catalogo = va_arg(args, Catalog*);
    g_array_append_vals(store, &catalogo, 1);

    Catalog* catalogo2 = va_arg(args, Catalog*);
    g_array_append_vals(store, &catalogo2, 1);

    Catalog* catalogo3 = va_arg(args, Catalog*);
    g_array_append_vals(store, &catalogo3, 1);
}
void default_csv_destructor_reservation(FILE* stream, ParserStore store) {
    FILE* discarder = g_array_index(store, FILE*, 0);
    if(discarder != NULL)  CLOSE_FILE(discarder);

    void** file_header = g_array_index(store, void**, 1);
    free(file_header);

    g_array_free(store, TRUE);
}

void test_parser(Tokens tokens) {
    return;
}

void test_writer(void* raw_data, FILE** store) {
    fflush(stdout);
}

void batch(char* arg1, char* arg2) {

    Catalog* user_catalog = catalog_init(&user_tree_compare_func, g_str_hash, g_str_equal, free);
    char* userdata_path = join_paths(2, arg1, "users.csv");
    parse_file(
        userdata_path,     
        &tokenize_csv,
        &test_preprocessor,
        &verify_user_tokens, 
        &parse_user, 
        &write_user, 
        &discard_user,
        &default_csv_destructor,
        user_catalog
    );  
    free(userdata_path);
    // catalog_print_hash_table(user_catalog, &print_user_key_value);

    Catalog* flight_catalog = catalog_init(&flight_tree_compare_func, g_direct_hash, g_direct_equal, NULL);
    char* flightsdata_path = join_paths(2, arg1, "flights.csv");
    parse_file(
        flightsdata_path,
        &tokenize_csv,
        &test_preprocessor,
        &verify_flight_tokens, 
        &parse_flight, 
        &write_flight, 
        &discard_flight,
        &default_csv_destructor,
        flight_catalog
    );
    free(flightsdata_path);
    // catalog_print_hash_table(flight_catalog, &print_flights_key_value);

    Catalog* passengers_catalog = catalog_init(&passenger_tree_compare_func, NULL, NULL, NULL);
    char* passengersdata_path = join_paths(2, arg1, "passengers.csv");
    parse_file(
        passengersdata_path,
        &tokenize_csv,
        &test_preprocessor_passenger,
        &verify_passenger_tokens,
        &parse_passenger, 
        &write_passenger, 
        &discard_passenger,
        &default_csv_destructor_reservation,
        user_catalog,
        flight_catalog,
        passengers_catalog
    );
    free(passengersdata_path);
    // catalog_print_tree(passengers_catalog, &passenger_print_tree);

    Catalog* reservation_catalog = catalog_init(&reservation_tree_compare_func, g_direct_hash, g_direct_equal, NULL);
    char* reservationsdata_path = join_paths(2, arg1, "reservations.csv");
    parse_file(
        reservationsdata_path,
        &tokenize_csv,
        &test_preprocessor_reservation,
        &verify_reservation_tokens, 
        &parse_reservation, 
        &write_reservation, 
        &discard_reservation,
        &default_csv_destructor_reservation,
        user_catalog,
        reservation_catalog
    );//TODO: passar o reservation_catalog, pq preciso de guardar os valores das reservas nele
    free(reservationsdata_path);



    // query_run_bulk(arg2, "ignore"); //TODO: change ignore to path where output files will be






    // Cleanup cwd
    g_free(get_cwd()->str);
    g_free(get_cwd());

    catalog_destroy(user_catalog);
    catalog_destroy(flight_catalog);
    catalog_destroy(passengers_catalog);
    catalog_destroy(reservation_catalog);

    return;
}