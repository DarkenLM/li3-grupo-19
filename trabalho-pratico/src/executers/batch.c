#include "executers/batch.h"

#include "locale.h"
#include "tests/test.h"

gint flights2_compare(gconstpointer flight_A, gconstpointer flight_B) {
    const Flight Flight1 = *(const Flight *)flight_A;
    const Flight Flight2 = *(const Flight *)flight_B;

    int date1 = get_flight_schedule_departure_date(Flight1);
    int date2 = get_flight_schedule_departure_date(Flight2);
    if(date1 > date2) return 1;
    if(date1 < date2) return -1;
    return 0;
}

void csv_destructor(FILE* stream, ParserStore store) {
    IGNORE_ARG(stream);

    FILE* discarder = g_array_index(store, FILE*, 0);
    if (discarder != NULL) CLOSE_FILE(discarder);

    void** file_header = g_array_index(store, void**, 1);
    free(file_header);

    g_array_free(store, TRUE);
}

void batch(const char* arg1, const char* arg2) {
#ifdef MAKE_TEST
    char* output_path = join_paths(2, get_cwd()->str, "Resultados/test_report.txt");
    FILE* test_report = OPEN_FILE(output_path, "w");
    double sorting_time = 0;
    double parsing_time = 0;
    printf("\n----===[  CATALOGS SETUP METRICS  ]===----\n\n");
    fprintf(test_report, "\n----===[  CATALOGS SETUP METRICS  ]===----\n\n");
    clock_t start_time = clock();
#endif

    // TODO Verificar onde vai ficar a inicialização desta estrutura
    GArray* pointer_to_generic_catalog = generate_genCat();

    Catalog* user_catalog = catalog_init(g_str_hash, g_str_equal, free);
    char* userdata_path = join_paths(2, arg1, "users.csv");
    parse_file(
        userdata_path,
        &tokenize_csv,
        &preprocessor_user,
        &verify_user_tokens,
        &parse_user,
        &usersCatalog_write_to_catalog,
        &discard_user,
        &csv_destructor,
        user_catalog,
        pointer_to_generic_catalog
        );

#ifdef MAKE_TEST
    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    parsing_time += elapsed_time;
    printf(" - Execution time for parsing users: %.4f seconds\n", elapsed_time);
    fprintf(test_report, " - Execution time for parsing users: %.4f seconds\n", elapsed_time);
#endif
    free(userdata_path);

    TEST_EXPR(start_time = clock();)
    catalog_sort(user_catalog, (GCompareFunc)&usersCatalog_full_compare_func);
#ifdef MAKE_TEST
    end_time = clock();
    elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    sorting_time += elapsed_time;
    printf(" - Execution time for sorting users: %.4f seconds\n\n", elapsed_time);
    fprintf(test_report, " - Execution time for sorting users: %.4f seconds\n\n", elapsed_time);

    start_time = clock();
#endif

    Catalog* flight_catalog = catalog_init(g_direct_hash, g_direct_equal, NULL);
    char* flightsdata_path = join_paths(2, arg1, "flights.csv");
    parse_file(
        flightsdata_path,
        &tokenize_csv,
        &preprocessor_flight,
        &verify_flight_tokens,
        &parse_flight,
        &flightsCatalog_write_to_catalog,
        &discard_flight,
        &csv_destructor,
        flight_catalog,
        pointer_to_generic_catalog
        );


#ifdef MAKE_TEST
    end_time = clock();
    elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    parsing_time += elapsed_time;
    printf(" - Execution time for parsing flights: %.4f seconds\n", elapsed_time);
    fprintf(test_report, " - Execution time for parsing flights: %.4f seconds\n", elapsed_time);
#endif
    free(flightsdata_path);

    TEST_EXPR(start_time = clock();)
    catalog_sort(flight_catalog, (GCompareFunc)&flightsCatalog_full_compare_func);

#ifdef MAKE_TEST
    end_time = clock();
    elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    sorting_time += elapsed_time;
    printf(" - Execution time for sorting flights: %.4f seconds\n\n", elapsed_time);
    fprintf(test_report, " - Execution time for sorting flights: %.4f seconds\n\n", elapsed_time);

    start_time = clock();
#endif

    Catalog* passengers_catalog = catalog_init(NULL, NULL, NULL);
    char* passengersdata_path = join_paths(2, arg1, "passengers.csv");
    parse_file(
        passengersdata_path,
        &tokenize_csv,
        &preprocessor_passenger,
        &verify_passenger_tokens,
        &parse_passenger,
        &passengersCatalog_write_to_catalog,
        &discard_passenger,
        &csv_destructor,
        user_catalog,
        flight_catalog,
        passengers_catalog,
        pointer_to_generic_catalog
        );

#ifdef MAKE_TEST
    end_time = clock();
    elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    parsing_time += elapsed_time;
    printf(" - Execution time for parsing passengers: %.4f seconds\n", elapsed_time);
    fprintf(test_report, " - Execution time for parsing passengers: %.4f seconds\n", elapsed_time);
#endif
    free(passengersdata_path);

    TEST_EXPR(start_time = clock();)
    // catalog_sort(passengers_catalog, (GCompareFunc)&passengersCatalog_full_compare_func);

#ifdef MAKE_TEST
    end_time = clock();
    elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    sorting_time += elapsed_time;
    printf(" - Execution time for sorting passengers: %.4f seconds\n\n", elapsed_time);
    fprintf(test_report, " - Execution time for sorting passengers: %.4f seconds\n\n", elapsed_time);

    start_time = clock();
#endif

    Catalog* reservation_catalog = catalog_init(g_direct_hash, g_direct_equal, NULL);
    char* reservationsdata_path = join_paths(2, arg1, "reservations.csv");
    parse_file(
        reservationsdata_path,
        &tokenize_csv,
        &preprocessor_reservation,
        &verify_reservation_tokens,
        &parse_reservation,
        &reservationsCatalog_write_to_catalog,
        &discard_reservation,
        &csv_destructor,
        user_catalog,
        reservation_catalog,
        pointer_to_generic_catalog
        );

#ifdef MAKE_TEST
    end_time = clock();
    elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    parsing_time += elapsed_time;
    printf(" - Execution time for parsing reservations: %.4f seconds\n", elapsed_time);
    fprintf(test_report, " - Execution time for parsing reservations: %.4f seconds\n", elapsed_time);
#endif
    free(reservationsdata_path);

    TEST_EXPR(start_time = clock();)
    catalog_sort(reservation_catalog, (GCompareFunc)&reservationsCatalog_full_compare_func);

#ifdef MAKE_TEST
    end_time = clock();
    elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    sorting_time += elapsed_time;
    printf(" - Execution time for sorting reservations: %.4f seconds\n\n", elapsed_time);
    fprintf(test_report, " - Execution time for sorting reservations: %.4f seconds\n\n", elapsed_time);
#endif

    TEST_EXPR(start_time = clock();)
    Stats_info stats_info = create_stats_info(user_catalog, flight_catalog, pointer_to_generic_catalog);

#ifdef MAKE_TEST
    end_time = clock();
    elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    sorting_time += elapsed_time;
    printf(" - Execution time for inicializing statistics: %.4f seconds\n", elapsed_time);
    fprintf(test_report, " - Execution time for inicializing statistics: %.4f seconds\n", elapsed_time);
#endif

    // Run queries
    void** catalogues = (void**)malloc(6 * sizeof(void*));
    catalogues[0] = (Catalog*)user_catalog;
    catalogues[1] = (Catalog*)flight_catalog;
    catalogues[2] = (Catalog*)passengers_catalog;
    catalogues[3] = (Catalog*)reservation_catalog;
    catalogues[4] = (Stats_info)stats_info;

    TEST_EXPR(printf("\n----===[  QUERY EXECUTION METRICS  ]===----\n\n");)
    TEST_EXPR(fprintf(test_report, "\n----===[  QUERY EXECUTION METRICS  ]===----\n\n");)
    TEST_EXPR(CLOSE_FILE(test_report);)
    query_run_bulk((char*)arg2, "Resultados", catalogues);

    TEST_EXPR(test_report = OPEN_FILE(output_path, "a");)
    TEST_EXPR(printf(" -> Execution time for parsing all collections datasets: %.4f seconds.\n", parsing_time);)
    TEST_EXPR(fprintf(test_report, " -> Execution time for parsing all collections datasets: %.4f seconds.\n", parsing_time);)
    TEST_EXPR(printf(" -> Execution time for sorting all collections datasets: %.4f seconds.\n", sorting_time);)
    TEST_EXPR(fprintf(test_report, " -> Execution time for sorting all collections datasets: %.4f seconds.\n", sorting_time);)
    TEST_EXPR(CLOSE_FILE(test_report);)

    // Cleanup cwd
    // g_free(get_cwd()->str);
    // g_free(get_cwd());

    // TODO Ver melhor maneira de fazer este free
    for (int i = 0; i < catalog_get_item_count(user_catalog); i++) {
        User user = (User)catalog_search_in_array(user_catalog, i);
        g_array_free(get_user_reservations(user), TRUE);
        g_array_free(get_user_flights(user), TRUE);
    }

    catalog_destroy(user_catalog);
    catalog_destroy(flight_catalog);
    catalog_destroy(passengers_catalog);
    catalog_destroy(reservation_catalog);
    stats_destroy(stats_info);


    return;
}