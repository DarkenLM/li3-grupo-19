#include "queries/queries.h"

// Queries: 1, 3, 4, 7, 8, 9

void query1(char flag, int argc, char** argv, void** catalogues, FILE* output_file) {
    IGNORE_ARG(flag);
    IGNORE_ARG(argc);
    IGNORE_ARG(argv);

    Q_INFO1 information;
    information.flight_info = NULL;
    information.reservation_info = NULL;
    information.user_info = NULL;
    information.type = -1;

    if (strncmp("Book", argv[0], 4) == 0) {
        QUERY1_RESERVATION reservation_info;
        information.reservation_info = &reservation_info;
        information.type = 2;
        void* reservation = catalog_search_in_int_hashtable(catalogues[3], atoi(argv[0] + 4));

        if (reservation == NULL) return;

        int nights = (get_reservation_end_date(reservation) - get_reservation_begin_date(reservation)) / (60 * 60 * 24);

        reservation_info.hotelID = get_reservation_hotelID(reservation);
        reservation_info.hotel_name = get_reservation_hotel_name(reservation);
        reservation_info.hotel_stars = get_reservation_hotel_stars(reservation);
        reservation_info.reservation_begin_date = get_reservation_begin_date(reservation);
        reservation_info.reservation_end_date = get_reservation_end_date(reservation);
        reservation_info.breakfast = get_reservation_includes_breakfast(reservation);
        reservation_info.nights = nights;
        reservation_info.total_price = (double)get_reservation_price_per_night(reservation) * (double)nights + (((double)get_reservation_price_per_night(reservation) * (double)nights) / 100) * (double)get_reservation_city_tax(reservation);

        output_query_info(1, flag, &information, output_file, 1);
        free(reservation_info.hotel_name);
    }
    if (atoi(argv[0]) > 0) {
        QUERY1_FLIGHT flight_info;
        information.flight_info = &flight_info;
        information.type = 1;

        void* flight = catalog_search_in_int_hashtable(catalogues[1], atoi(argv[0]));

        if (flight == NULL) return;

        flight_info.airline = get_flight_airline(flight);
        flight_info.airplane_model = get_flight_plane_model(flight);
        flight_info.origin = get_flight_origin(flight);
        flight_info.destination = get_flight_destination(flight);
        flight_info.schedule_departure_date = get_flight_schedule_departure_date(flight);
        flight_info.schedule_arrival_date = get_flight_schedule_arrival_date(flight);
        flight_info.passangers = get_flight_passengers(flight);
        flight_info.delay = get_flight_real_departure_date(flight) - information.flight_info->schedule_departure_date;

        output_query_info(1, flag, &information, output_file, 1);

        free(flight_info.airline);
        free(flight_info.airplane_model);
        free(flight_info.origin);
        free(flight_info.destination);
    } else {
        QUERY1_USER user_info;
        information.user_info = &user_info;
        information.type = 0;
        void* user = catalog_search_in_str_hashtable(catalogues[0], argv[0]);

        if (user == NULL) return;
        if (!get_user_account_status(user)) return;

        // int n_reservas;
        // double valor = calculate_user_total_spent(catalogues[3], argv[0], &n_reservas);

        user_info.name = get_user_name(user);
        user_info.sex = get_user_sex(user);
        user_info.age = get_user_age(user);
        user_info.country_code = get_user_country_code(user);
        user_info.passport = get_user_passport(user);
        user_info.n_flights = calculate_user_n_flights(catalogues[2], argv[0]);
        user_info.n_reservas = get_user_reservations_counter(user);
        user_info.total_spent = get_user_total_spent(user);

        output_query_info(1, flag, &information, output_file, 1);

        free(user_info.name);
        free(user_info.country_code);
        free(user_info.passport);
    }
}

void query2(char flag, int argc, char** argv, void** catalogues, FILE* output_file) {
    IGNORE_ARG(flag);
    IGNORE_ARG(argc);
    IGNORE_ARG(argv);
    IGNORE_ARG(catalogues);
    IGNORE_ARG(output_file);

    Q_INFO2 information;
    output_query_info(2, flag, &information, output_file, 1);
}

void query3(char flag, int argc, char** argv, void** catalogues, FILE* output_file) {
    IGNORE_ARG(flag);
    IGNORE_ARG(argc);
    IGNORE_ARG(argv);

    Q_INFO3 information;

    guint matched_index = 0;
    int hotel_id = atoi(argv[0] + 3);
    gboolean exists = catalog_exists_in_array(catalogues[3], GINT_TO_POINTER(hotel_id), &reservationsCatalog_hotelID_compare_func, &matched_index);
    if (exists) {
        int matched_index_down = matched_index;

        void* data1 = catalog_search_in_array(catalogues[3], matched_index_down);
        while (get_reservation_hotelID((Reservation)data1) == hotel_id && matched_index_down > 0) {
            data1 = catalog_search_in_array(catalogues[3], --matched_index_down);
        };
        if (get_reservation_hotelID(data1) != hotel_id) matched_index_down++;

        int matched_index_up = matched_index;
        void* data2 = catalog_search_in_array(catalogues[3], matched_index_up);
        while (get_reservation_hotelID((Reservation)data2) == hotel_id && matched_index_up < catalog_get_item_count(catalogues[3]) - 1) {
            data2 = catalog_search_in_array(catalogues[3], ++matched_index_up);
        };
        if (get_reservation_hotelID(data2) != hotel_id) matched_index_up--;

        int i = matched_index_down;
        double rating = 0;
        int quantidade_a_percorrer = (matched_index_up - matched_index_down + 1);
        while (0 < quantidade_a_percorrer) {
            const Reservation reservation_temp = (const Reservation)(catalog_search_in_array(catalogues[3], i));
            rating += get_reservation_rating(reservation_temp);
            i++;
            quantidade_a_percorrer--;
        };
        rating /= (matched_index_up - matched_index_down + 1);

        information.rating = rating;

        output_query_info(3, flag, &information, output_file, 1);

    } else {
        fprintf(output_file, "ERRO na query3 (Reservation with that hotel id not found)\n");
    }
}

void query4(char flag, int argc, char** argv, void** catalogues, FILE* output_file) {
    IGNORE_ARG(flag);
    IGNORE_ARG(argc);
    IGNORE_ARG(argv);

    Q_INFO4 information;

    GArray* arrTemp = g_array_new(FALSE, FALSE, sizeof(gpointer));
    guint matched_index = 0;
    int hotel_id = atoi(argv[0] + 3);
    gboolean exists = catalog_exists_in_array(catalogues[3], GINT_TO_POINTER(hotel_id), &reservationsCatalog_hotelID_compare_func, &matched_index);
    if (exists) {
        void* data = catalog_search_in_array(catalogues[3], matched_index);
        g_array_append_val(arrTemp, data);
        int matched_index_down = matched_index - 1;
        int matched_index_up = matched_index + 1;
        void* data1 = catalog_search_in_array(catalogues[3], matched_index_down);
        void* data2 = catalog_search_in_array(catalogues[3], matched_index_up);
        while (hotel_id == get_reservation_hotelID((Reservation)data1) && matched_index_down >= 0) {
            data1 = catalog_search_in_array(catalogues[3], matched_index_down);
            g_array_append_val(arrTemp, data1);
            // print_flight(data1);
            matched_index_down--;
            if (matched_index_down < 0) break;
            data1 = catalog_search_in_array(catalogues[3], matched_index_down);
            // data1 = catalog_search_in_array(catalog, matched_index_down);
            // print_flight(data1);
        };
        while (hotel_id == get_reservation_hotelID((Reservation)data2) && matched_index_up != catalog_get_item_count(catalogues[3])) {
            // g_array_append_val(arrTemp,data2);
            data2 = catalog_search_in_array(catalogues[3], matched_index_up);
            g_array_append_val(arrTemp, data2);
            matched_index_up++;
            // print_flight(data2);
            data2 = catalog_search_in_array(catalogues[3], matched_index_up);
        };

        g_array_sort(arrTemp, &reservationsCatalog_date_compare_func);

        int count = 1;
        for (int i = 0; i < (int)arrTemp->len; i++) {
            const Reservation reservation_temp = (const Reservation)(g_array_index(arrTemp, gpointer, i));

            int first_day = get_reservation_begin_date(reservation_temp);
            int last_day = get_reservation_end_date(reservation_temp);

            double days = (last_day - first_day);
            days /= 60 * 60 * 24;

            double price = get_reservation_price_per_night(reservation_temp) * days + ((get_reservation_price_per_night(reservation_temp) * days) / 100) * get_reservation_city_tax(reservation_temp);

            information.reservationID = get_reservation_id(reservation_temp);
            information.reservation_begin_date = first_day;
            information.reservation_end_date = last_day;
            information.userID = get_reservation_userID(reservation_temp);
            information.reservation_rating = get_reservation_rating(reservation_temp);
            information.price = price;

            output_query_info(4, flag, &information, output_file, count);

            free(information.userID);

            count++;
        };
    };
    g_array_free(arrTemp, TRUE);
}

void query5(char flag, int argc, char** argv, void** catalogues, FILE* output_file) {
    IGNORE_ARG(flag);
    IGNORE_ARG(argc);
    IGNORE_ARG(argv);

    Q_INFO5 information;

    GArray* arrTemp = g_array_new(FALSE, FALSE, sizeof(gpointer));
    guint matched_index = 0;
    gboolean exists = catalog_exists_in_array(catalogues[1], argv[0], &flightsCatalog_origin_compare_func, &matched_index);
    // void *data1, *data2;
    char* orig;

    if (exists) {
        void* data = catalog_search_in_array(catalogues[1], matched_index);
        g_array_append_val(arrTemp, data);

        int matched_index_down = matched_index - 1;
        int matched_index_up = matched_index + 1;

        void* data1 = catalog_search_in_array(catalogues[1], matched_index_down);
        void* data2 = catalog_search_in_array(catalogues[1], matched_index_up);

        orig = get_flight_origin((Flight)data1);
        while (strcasecmp(argv[0], orig) == 0 && matched_index_down >= 0) {
            data1 = catalog_search_in_array(catalogues[1], matched_index_down);
            g_array_append_val(arrTemp, data1);

            matched_index_down--;
            if (matched_index_down < 0) break;

            data1 = catalog_search_in_array(catalogues[1], matched_index_down);

            free(orig);
            orig = get_flight_origin((Flight)data1);
        };
        free(orig);

        orig = get_flight_origin((Flight)data2);
        while (strcasecmp(argv[0], orig) == 0 && matched_index_up + 1 < catalog_get_item_count(catalogues[1])) {
            data2 = catalog_search_in_array(catalogues[1], matched_index_up);
            g_array_append_val(arrTemp, data2);

            matched_index_up++;

            data2 = catalog_search_in_array(catalogues[1], matched_index_up);

            free(orig);
            orig = get_flight_origin((Flight)data2);
        };
        free(orig);

        g_array_sort(arrTemp, &flightsCatalog_full_compare_func);
        // bool activated = FALSE;
        int count = 1;
        for (int i = 0; i < (int)arrTemp->len; i++) {
            const Flight flight_temp = (const Flight)(g_array_index(arrTemp, gpointer, i));
            if (date_string_withtime_to_int(argv[1]) <= get_flight_schedule_departure_date(flight_temp) && get_flight_schedule_departure_date(flight_temp) <= date_string_withtime_to_int(argv[2])) {
                // activated = TRUE;

                information.flight_id = get_flight_id(flight_temp);
                information.schedule_departure_date = get_flight_schedule_departure_date(flight_temp);
                information.destination = get_flight_destination(flight_temp);
                information.airline = get_flight_airline(flight_temp);
                information.plane_model = get_flight_plane_model(flight_temp);

                output_query_info(5, flag, &information, output_file, count);

                free(information.destination);
                free(information.airline);
                free(information.plane_model);

                count++;
            }
            // if(i == (int)arrTemp->len - 1 && activated == TRUE)   fprintf(output_file, "\n");
            // PAULO - NÃO SEI SE ISTO VAI SER UTIL POR ISSO NÃO APAGO
        };
    };

    g_array_free(arrTemp, TRUE);
}

/*
6 <year> <N>
nome;passageiros
*/
gint sort_Q6(gconstpointer a, gconstpointer b){
    Q_info6 value1 = (Q_info6)a;
    Q_info6 value2 = (Q_info6)b;

    if(value1->passangers > value2->passangers) return -1;
    if(value1->passangers < value2->passangers) return 1;
    return(strcmp(value1->origin, value2->origin));
}

void query6(char flag, int argc, char** argv, void** catalogues, FILE* output_file) {
    //IGNORE_ARG(flag);
    IGNORE_ARG(argc);
    //IGNORE_ARG(argv);
    //IGNORE_ARG(catalogues);
    //IGNORE_ARG(output_file);

    Q_INFO6 information;
    GArray* arr_temp = g_array_new(FALSE, FALSE, sizeof(Q_INFO6));

    GHashTableIter iter;
    gpointer key, value;

    Stats_info stats = catalogues[4];
    int year = atoi(argv[0]);
    g_hash_table_iter_init(&iter, stats->aeroports);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        strcpy(information.origin, key);
        information.passangers = calculate_aeroport_n_passengers(catalogues[1], information.origin, &year);
        g_array_append_val(arr_temp, information);
    }
    g_array_sort(arr_temp, (GCompareFunc) sort_Q6);
    //arr_temp tem o numero de passageiros por aeroporto no array
    // colocar ano e todos os origins na função...
    
    // dar append do valor retornado a um array com origin
    // ordenar o valor retornado por ordem crescente
    // enviar a informação para o modulo de outputs
    for(int i = 0; i < (int)arr_temp->len && i < (atoi(argv[1])); i++){
        information = g_array_index(arr_temp, Q_INFO6, i);
        output_query_info(6, flag, &information, output_file, i+1);
    }
}

struct q7_index {
    char* origin;
    int median;
};

gint compare_q7_indices(gconstpointer a, gconstpointer b, gpointer user_data) {
    IGNORE_ARG(user_data);

    const struct q7_index* index_a = (const struct q7_index*)a;
    const struct q7_index* index_b = (const struct q7_index*)b;

    return index_b->median - index_a->median;
}

void query7(char flag, int argc, char** argv, void** catalogues, FILE* output_file) {
    IGNORE_ARG(argc);

    Q_INFO7 information;

    Catalog* flights = catalogues[1];
    int size = catalog_get_item_count(flights);

    GSequence* sequence = g_sequence_new(NULL);

    for (int i = 0; i < size; i++) {
        Flight flight = (Flight)catalog_search_in_array(flights, i);
        char* flight_origin = get_flight_origin(flight);

        int calculated = 0;

        GSequenceIter* iter = g_sequence_get_begin_iter(sequence);
        while (!g_sequence_iter_is_end(iter)) {
            const struct q7_index* ind = g_sequence_get(iter);
            if (strcmp(ind->origin, flight_origin) == 0) {
                calculated = 1;
                break;
            }

            iter = g_sequence_iter_next(iter);
        }

        if (calculated) {
            free(flight_origin);
            continue;
        }

        struct q7_index* ind = (struct q7_index*)malloc(sizeof(struct q7_index));
        ind->origin = flight_origin;

        // This function does the heavy lifting. It has cost both me and Paulo most of our sanities.
        ind->median = calculate_flight_delay_median(flights, flight_origin);

        g_sequence_insert_sorted(sequence, ind, compare_q7_indices, NULL);
    }

    GSequenceIter* iter = g_sequence_get_begin_iter(sequence);
    GSequenceIter* iter_root = iter;

    int count = 0;

    while (!g_sequence_iter_is_end(iter) && count < atoi(argv[0])) {
        struct q7_index* ind = g_sequence_get(iter);

        information.origin = ind->origin;
        information.median = ind->median;

        output_query_info(7, flag, &information, output_file, count + 1);

        iter = g_sequence_iter_next(iter);
        count++;
    }

    iter = iter_root;
    while (!g_sequence_iter_is_end(iter)) {
        struct q7_index* ind = g_sequence_get(iter);
        free(ind->origin);
        free(ind);
        iter = g_sequence_iter_next(iter);
    }

    g_sequence_free(sequence);
}

void query8(char flag, int argc, char** argv, void** catalogues, FILE* output_file) {
    IGNORE_ARG(flag);
    IGNORE_ARG(argc);
    IGNORE_ARG(argv);
    IGNORE_ARG(catalogues);
    IGNORE_ARG(output_file);

    Q_INFO8 information;

    GArray* arrTemp = g_array_new(FALSE, FALSE, sizeof(gpointer));
    guint matched_index = 0;
    int hotel_id = atoi(argv[0] + 3);
    int begin_date = date_string_notime_to_int(argv[1]);
    int end_date = date_string_notime_to_int(argv[2]);
    gboolean exists = catalog_exists_in_array(catalogues[3], GINT_TO_POINTER(hotel_id), &reservationsCatalog_hotelID_compare_func, &matched_index);
    if (exists) {
        int matched_index_down = matched_index;
        void* data1 = catalog_search_in_array(catalogues[3], matched_index_down);

        g_array_append_val(arrTemp, data1);

        while (
            hotel_id == get_reservation_hotelID((Reservation)data1) &&
            matched_index_down > 0) {
            data1 = catalog_search_in_array(catalogues[3], --matched_index_down);
            g_array_append_val(arrTemp, data1);
        };
        if(hotel_id != get_reservation_hotelID((Reservation)data1)) g_array_remove_index(arrTemp, arrTemp->len - 1);

        int matched_index_up = matched_index;
        void* data2 = catalog_search_in_array(catalogues[3], matched_index_up);
        while (
            hotel_id == get_reservation_hotelID((Reservation)data2) &&
            (int)matched_index_up < catalog_get_item_count(catalogues[3])-1) {
            data2 = catalog_search_in_array(catalogues[3], ++matched_index_up);
            g_array_append_val(arrTemp, data2);
        };
        if(hotel_id != get_reservation_hotelID((Reservation)data2)) g_array_remove_index(arrTemp, arrTemp->len - 1);

        // g_array_sort(arrTemp, &reservation_date_compare_func);
    }

    int resolution = 0;
    for (int i = 0; i < (int)arrTemp->len; i++) {
        const Reservation reservation_temp = (const Reservation)(g_array_index(arrTemp, gpointer, i));
        // print_reservation(reservation_temp);
        int start_reservation = get_reservation_begin_date(reservation_temp);
        int end_reservation = get_reservation_end_date(reservation_temp);
        if (start_reservation <= end_date && end_reservation >= begin_date) {
            if (start_reservation <= begin_date) start_reservation = begin_date;
            if (end_reservation >= end_date) end_reservation = end_date;
            int nights = difftime(end_reservation, start_reservation);
            nights /= 3600 * 24;
            if (get_reservation_end_date(reservation_temp) - end_date > 0) nights++;
            // int res = difftime(end_date - DATE_OFFSET , begin_date - DATE_OFFSET );
            // printf("%d\n", res/(3600*24));
            // if(res/(3600*24) < 31) nights++;
            // print_reservation(reservation_temp);
            // printf("Nights: %d - Start: %s End: %s , RealStart: %s RealEnd: %s\n", nights, date_int_notime_to_string(start_reservation), date_int_notime_to_string(end_reservation), date_int_notime_to_string(get_reservation_begin_date(reservation_temp)), date_int_notime_to_string(get_reservation_end_date(reservation_temp)));
            resolution += get_reservation_price_per_night(reservation_temp) * (nights);
        }
    };

    information.revenue = resolution;

    output_query_info(8, flag, &information, output_file, 1);
    g_array_free(arrTemp, TRUE);
}

void query9(char flag, int argc, char** argv, void** catalogues, FILE* output_file) {
    IGNORE_ARG(flag);
    IGNORE_ARG(argc);
    IGNORE_ARG(argv);

    Q_INFO9 information;

    GArray* arrTemp = g_array_new(FALSE, FALSE, sizeof(gpointer));
    guint matched_index = 0;
    gboolean exists = catalog_exists_in_array(catalogues[0], *argv, &usersCatalog_name_compare_func, &matched_index);

    // void *data1, *data2;
    char* user_name;

    if (exists) {
        int matched_index_down = matched_index;
        void* data1 = catalog_search_in_array(catalogues[0], matched_index);
        user_name = get_user_name((User)data1);

        g_array_append_val(arrTemp, data1);

        while (strncasecmp(*argv, user_name, strlen(*argv)) == 0 && matched_index_down > 0) {
            data1 = catalog_search_in_array(catalogues[0], --matched_index_down);
            g_array_append_val(arrTemp, data1);
            free(user_name);
            user_name = get_user_name((User)data1);
        };
        if (strncasecmp(*argv, user_name, strlen(*argv)) != 0) g_array_remove_index(arrTemp, arrTemp->len - 1);
        free(user_name);

        int matched_index_up = matched_index;
        void* data2 = catalog_search_in_array(catalogues[0], matched_index_up);
        user_name = get_user_name((User)data2);
        while (strncasecmp(*argv, user_name, strlen(*argv)) == 0 && (int)matched_index_up < catalog_get_item_count(catalogues[0]) - 1) {
            data2 = catalog_search_in_array(catalogues[0], ++matched_index_up);
            g_array_append_val(arrTemp, data2);

            free(user_name);
            user_name = get_user_name((User)data2);
        };
        if (strncasecmp(*argv, user_name, strlen(*argv)) != 0) g_array_remove_index(arrTemp, arrTemp->len - 1);
        free(user_name);

        g_array_sort(arrTemp, &usersCatalog_strcoll_compare_func);

        // bool activated = FALSE;
        int count = 1;
        for (int i = 0; i < (int)arrTemp->len; i++) {
            const User user_temp = (const User)(g_array_index(arrTemp, gpointer, i));

            if (get_user_account_status(user_temp) == TRUE) {
                // activated = TRUE;

                user_name = get_user_name(user_temp);
                information.userID = get_user_id(user_temp);
                information.user_name = user_name;

                output_query_info(9, flag, &information, output_file, count);

                free(information.userID);
                free(user_name);

                count++;
            }
            // if(i == (int)arrTemp->len - 1 && activated == TRUE) fprintf(output_file, "\n");
        };
    }

    g_array_free(arrTemp, TRUE);
}

void query10(char flag, int argc, char** argv, void** catalogues, FILE* output_file) {
    IGNORE_ARG(flag);
    IGNORE_ARG(argc);
    IGNORE_ARG(argv);
    IGNORE_ARG(catalogues);
    IGNORE_ARG(output_file);

    Q_INFO10 information;
    output_query_info(10, flag, &information, output_file, 1);
}