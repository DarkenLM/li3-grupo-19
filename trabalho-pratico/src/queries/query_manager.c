/**
 * QUERY MANAGER
 *
 * This module manages and executes queries according to a source (line or file).
 */

#include "queries/queries.h"
#include "queries/output.h"
#include "util/string.h"
#include "tests/test.h"

#define NUM_QUERIES 10

#ifdef MAKE_TEST
// Refer to main.c
extern FILE* batch_test_test_report;

#define TIMERS_STORE_INDEX 4
#endif

void destroy_query(Query query) {
    for (int i = 0; i < query->argc; i++) free(query->argv[i]);
    free(query);
}

/**
 * @internal
 * 
 * @brief Tokenize strategy for the queries.
 * 
 * The parameters are described on @ref Tokenizer.
 */
Tokens tokenize_query(char* line, ssize_t len) {
    return tokenize_char_delim(line, len, " ");
}

// #pragma region Parser helpers
// #pragma GCC push_options
// #pragma GCC optimize ("O0")

/**
 * @internal
 * 
 * @brief Preprocessor strategy for the queries.
 * 
 * The parameters are described on @ref PreprocessFunction.
 */
void query_preprocessor(FILE* stream, ParserStore store, va_list args) {
    IGNORE_ARG(stream);

    gpointer null_element = NULL;

    // ------- OUTPUT DIRECTORY -------
    g_array_append_vals(store, &null_element, 1);  // Directory for outputs

    char* output_dir = va_arg(args, char*);

    // Using more verbose code
    char** s_output_dir = (char**)&g_array_index(store, void*, 0);
    *s_output_dir = resolve_to_cwd(output_dir);
    // Because the alternative is this abomination:
    // *((char**)(&(((void**) (void *) (store)->data) [(0)]))) = resolve_to_cwd(output_dir);

    // ------- QUERY NUMBER -------
    g_array_append_vals(store, &null_element, 1);  // Current query number

    int* query_num = (int*)malloc(sizeof(int));
    *query_num = 1;

    int** s_query_num = (int**)&g_array_index(store, void*, 1);
    *s_query_num = query_num;

    // ------- CATALOGUES -------
    g_array_append_vals(store, &null_element, 1);
    void** catalogues = va_arg(args, void**);

    void*** s_catalogues = (void***)&g_array_index(store, void*, 2);
    *s_catalogues = catalogues;

    // ------- WRITER -------
    QueryWriter qw = va_arg(args, QueryWriter);
    g_array_append_vals(store, &qw, 1);

// ------- TIMERS -------
#ifdef MAKE_TEST
    double* timers = (double*)malloc(sizeof(double) * NUM_QUERIES);
    memset(timers, 0, sizeof(double) * NUM_QUERIES);
    g_array_append_vals(store, &timers, 1);
#endif
}

int query_verify_raw(Query query, DatasetData dd, char** error) {
    void** catalogues = dataset_data_get_catalog_array(dd);

    // switch(query->id[0]) {
    //     case '1':  { return ((QueryVerifier)query1_verify)(query, catalogues, error); break; }
    //     case '2':  { return ((QueryVerifier)query2_verify)(query, catalogues, error); break; }
    //     case '3':  { return ((QueryVerifier)query3_verify)(query, catalogues, error); break; }
    //     case '4':  { return ((QueryVerifier)query4_verify)(query, catalogues, error); break; }
    //     case '5':  { return ((QueryVerifier)query5_verify)(query, catalogues, error); break; }
    //     case '6':  { return ((QueryVerifier)query6_verify)(query, catalogues, error); break; }
    //     case '7':  { return ((QueryVerifier)query7_verify)(query, catalogues, error); break; }
    //     case '8':  { return ((QueryVerifier)query8_verify)(query, catalogues, error); break; }
    //     case '9':  { return ((QueryVerifier)query9_verify)(query, catalogues, error); break; }
    //     case '10': { return ((QueryVerifier)query10_verify)(query, catalogues, error); break; }
    // 
    if (STRING_EQUAL(query->id, "1"))  { return ((QueryVerifier)query1_verify)(query, catalogues, error); }
    if (STRING_EQUAL(query->id, "2"))  { return ((QueryVerifier)query2_verify)(query, catalogues, error); }
    if (STRING_EQUAL(query->id, "3"))  { return ((QueryVerifier)query3_verify)(query, catalogues, error); }
    if (STRING_EQUAL(query->id, "4"))  { return ((QueryVerifier)query4_verify)(query, catalogues, error); }
    if (STRING_EQUAL(query->id, "5"))  { return ((QueryVerifier)query5_verify)(query, catalogues, error); }
    if (STRING_EQUAL(query->id, "6"))  { return ((QueryVerifier)query6_verify)(query, catalogues, error); }
    if (STRING_EQUAL(query->id, "7"))  { return ((QueryVerifier)query7_verify)(query, catalogues, error); }
    if (STRING_EQUAL(query->id, "8"))  { return ((QueryVerifier)query8_verify)(query, catalogues, error); }
    if (STRING_EQUAL(query->id, "9"))  { return ((QueryVerifier)query9_verify)(query, catalogues, error); }
    if (STRING_EQUAL(query->id, "10")) { return ((QueryVerifier)query10_verify)(query, catalogues, error); }
}

/**
 * @internal
 * 
 * @brief Verify strategy for the queries.
 * 
 * The parameters are described on @ref VerifyFunction.
 */
int query_verifier_tokens(Tokens tokens, ParserStore store) {
    // As per the project document, the queries are guaranteed to be valid.
    // So, only a syntatic sanity check will be performed.

    // TODO: Actually check if the queries are correct, because we know that the 
    // user will always try to fuck our shit up.

    IGNORE_ARG(store);

    for (int i = 0; i < tokens->len; i++) {
        if (tokens->data[i] == NULL) return 0;
    }

    return 1;
}


/**
 * @internal
 * 
 * @brief Parser strategy for the queries.
 * 
 * The parameters are described on @ref ParseFunction.
 */
void* query_parser(Tokens tokens) {
    Query query = (Query)malloc(sizeof(QUERY));
    memset(query, 0, sizeof(QUERY));

    char id[QUERIES_CHAR_LEN] = {0};
    char flag = 0;

    int j = 0;
    for (int i = 0; tokens->data[0][i] != '\0'; i++) {
        if (tokens->data[0][i] >= '0' && tokens->data[0][i] <= '9') {
            id[j++] = tokens->data[0][i];
        } else {
            flag = tokens->data[0][i];
        }
    }

    id[j] = '\0';

    char* args[QUERIES_MAX_ARGS] = {0};

    j = 0;
    for (int i = 1; i < tokens->len; i++, j++) {
        if (tokens->data[i][0] == '\"') {
            int totalLen = 1;
            char* temp = tokens->data[i];
            int tempsize = strlen(temp);
            int start = i, end = i;

            while (temp[tempsize - 1] != '\"') {
                totalLen += tempsize + 1;
                end++;

                temp = tokens->data[i + (end - start)];
                tempsize = strlen(temp);
            }
            totalLen += tempsize;

            args[j] = (char*)malloc(totalLen * sizeof(char*));

            strcpy(args[j], tokens->data[i] + 1);
            for (int k = start + 1; k <= end; k++) {
                strcat(args[j], " ");
                strcat(args[j], tokens->data[k]);
            }
            args[j][totalLen - 3] = '\0';

            query->argc++;

            i += end - start;
        } else {
            args[j] = strdup(tokens->data[i]);
            if (tokens->data[i][0] != '\0') query->argc++;
        }
    }

    strcpy(query->id, id);
    query->flag = flag;
    memcpy(query->argv, args, QUERIES_MAX_ARGS * sizeof(char*));

    return query;
}

/**
 * @internal
 * 
 * @brief Writer strategy for the bulk mode query execution.
 * 
 * The parameters are described on @ref WriteFunction.
 */
void query_writer(void* raw_data, ParserStore store) {
    Query data = (Query)raw_data;

    // ------- Fetch store arguments -------
    char** s_output_dir = (char**)&g_array_index(store, void*, 0);
    int* s_query_num = g_array_index(store, void*, 1);
    void** catalogues = ((void**)g_array_index(store, void*, 2));
    QueryWriter writer = g_array_index(store, QueryWriter, 3);

    TEST_EXPR(double* timers = (double*)g_array_index(store, double*, TIMERS_STORE_INDEX);)

    // ------- Process required variables -------
    char* output_file = isnprintf("command%d_output.txt", *s_query_num);
    char* path = join_paths(2, *s_output_dir, output_file);

    // ------- Execute query -------
    FILE* retFile = OPEN_FILE(path, "w");

    TEST_EXPR(clock_t start_time = clock();)
    query_execute(data, catalogues, retFile, writer);
    TEST_EXPR(clock_t end_time = clock();)
    TEST_EXPR(double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;)
    TEST_EXPR(timers[atoi(data->id) - 1] += elapsed_time;)

    CLOSE_FILE(retFile);
    // ------- Free memory -------
    free(output_file);
    free(path);

    // ------- Destroy query -------
    // for (int i = 0; i < data->argc; i++) free(data->argv[i]);
    // free(data);
    destroy_query(data);

    // ------- Write flags -------
    *s_query_num = *s_query_num + 1;

    return;
}

/**
 * @internal
 * 
 * @brief Writer strategy for the single mode query execution.
 * 
 * The parameters are described on @ref WriteFunction.
 */
void query_writer_single(void* raw_data, ParserStore store) {
    Query data = (Query)raw_data;

    void** catalogues = g_array_index(store, void**, 0);
    QueryWriter writer = g_array_index(store, QueryWriter, 1);
    GArray* arr = g_array_index(store, GArray*, 2);

    query_execute(data, catalogues, (FILE*)arr, writer);

    // for (int i = 0; i < data->argc; i++) free(data->argv[i]);
    // free(data);
    destroy_query(data);
}
void output_query_to_array(int query, char flag, void* query_info, FILE* output_file, int n_element) {
    // I won't even begin to beg for forgiveness, for the sins I'm about to commit will make the demons shiver.
    GArray* arr = (GArray*)output_file;

    char* output = output_query_to_str(query, flag, query_info, NULL, n_element);
    g_array_append_val(arr, output);
}

/**
 * @internal
 * 
 * @brief Discarder strategy for the queries. Currently unused.
 * 
 * The parameters are described on @ref WriteFunction.
 */
void query_discarder(void* raw_data, ParserStore store) {
    // Queries are always correct. No discarding needed.
    IGNORE_ARG(raw_data);
    IGNORE_ARG(store);
}

/**
 * @internal
 * 
 * @brief Destructor strategy for the queries.
 * 
 * The parameters are described on @ref DestructFunction.
 */
void query_destructor(FILE* stream, ParserStore store) {
    IGNORE_ARG(stream);

// ------- Fetch store timers for display -------
#ifdef MAKE_TEST
    // char* output_path = join_paths(2, get_cwd()->str, "Resultados/test_report.txt");
    // FILE* test_report = OPEN_FILE(output_path, "a");

    double* timers = g_array_index(store, double*, TIMERS_STORE_INDEX);
    double total_time = 0;

    for (int i = 0; i < NUM_QUERIES; i++) {
        test_trace(" - Execution time for query %2d: %.4f seconds.\n", i + 1, timers[i]);
        fprintf(batch_test_test_report, " - Execution time for query %2d: %.4f seconds.\n", i + 1, timers[i]);
        total_time += timers[i];
    }

    test_trace("\n----===[  GENERAL PROGRAM METRICS  ]===----\n\n");
    fprintf(batch_test_test_report, "\n----===[  GENERAL PROGRAM METRICS  ]===----\n\n");

    test_trace(" -> Execution time for solving all queries: %.4f seconds.\n", total_time);
    fprintf(batch_test_test_report, " -> Execution time for solving all queries: %.4f seconds.\n", total_time);

    // CLOSE_FILE(batch_test_test_report);
#endif

    // ------- Free Memory -------
    // for (guint i = 0; i < store->len; ++i) {
    //     void* element = g_array_index(store, void*, i);
    //     free(element);
    // }
    free(g_array_index(store, void*, 0)); // Directory for outputs
    free(g_array_index(store, void*, 1)); // Current query number
    // Do not free the memory for the catalogues.
    // The code explodes if such is done with the new implementation.

    g_array_free(store, TRUE);
}
// #pragma GCC pop_options
// #pragma endregion

/**
 * @internal
 * 
 * @brief Executes queries.
 * 
 * @param query The information about the query to execute.
 * @param catalogues The catalogues to be used for the query.
 * @param output_file THe output file for the queries.
 * @param writer The writer strategy to use.
 */
void query_execute(Query query, void** catalogues, FILE* output_file, QueryWriter writer) {
    switch (atoi(query->id)) {
        case 1:  { query1 (query->flag, query->argc, query->argv, catalogues, output_file, writer); break; }
        case 2:  { query2 (query->flag, query->argc, query->argv, catalogues, output_file, writer); break; }
        case 3:  { query3 (query->flag, query->argc, query->argv, catalogues, output_file, writer); break; }
        case 4:  { query4 (query->flag, query->argc, query->argv, catalogues, output_file, writer); break; }
        case 5:  { query5 (query->flag, query->argc, query->argv, catalogues, output_file, writer); break; }
        case 6:  { query6 (query->flag, query->argc, query->argv, catalogues, output_file, writer); break; }
        case 7:  { query7 (query->flag, query->argc, query->argv, catalogues, output_file, writer); break; }
        case 8:  { query8 (query->flag, query->argc, query->argv, catalogues, output_file, writer); break; }
        case 9:  { query9 (query->flag, query->argc, query->argv, catalogues, output_file, writer); break; }
        case 10: { query10(query->flag, query->argc, query->argv, catalogues, output_file, writer); break; }
    }
};

void query_run_bulk(char* input_file, char* output_dir, void** catalogues) {
    parse_file(
        input_file,
        &tokenize_query,
        &query_preprocessor,
        &query_verifier_tokens,
        &query_parser,
        &query_writer,
        &query_discarder,
        &query_destructor,
        output_dir,
        catalogues,
        output_query_to_file
    );
    return;
};

/**
 * @internal
 * 
 * @brief Creates the @ref ParserStore required by the single-execution query parsers.
 * 
 * @param dd The data of the currently loaded datasets,
 */
ParserStore __make_query_single_store(DatasetData dd) {
    // ======= CREATE STORE =======
    ParserStore store = makeStore();

    // ------- CATALOGUES -------
    void** catalogues = dataset_data_get_catalog_array(dd);
    g_array_append_vals(store, &catalogues, 1);

    // ------- WRITER -------
    QueryWriter qw = output_query_to_array;
    g_array_append_vals(store, &qw, 1);

    GArray* arr = g_array_new(FALSE, FALSE, sizeof(char*));
    g_array_append_vals(store, &arr, 1);

    return store;
}

GArray* query_run_single(char* query, ssize_t len, DatasetData dd) {
    // // ======= CREATE STORE =======
    // ParserStore store = makeStore();

    // // ------- CATALOGUES -------
    // void** catalogues = dataset_data_get_catalog_array(dd);
    // g_array_append_vals(store, &catalogues, 1);

    // // ------- WRITER -------
    // QueryWriter qw = output_query_to_array;
    // g_array_append_vals(store, &qw, 1);

    // GArray* arr = g_array_new(FALSE, FALSE, sizeof(char*));
    // g_array_append_vals(store, &arr, 1);

    ParserStore store = __make_query_single_store(dd);

    parse(query, len, &tokenize_query, &query_verifier_tokens, &query_parser, &query_writer_single, &query_discarder, store);

    GArray* arr = g_array_index(store, GArray*, 2);
    g_array_free(store, TRUE);

    return arr;
};

GArray* query_run_single_raw(Query query, DatasetData dd) {
    ParserStore store = __make_query_single_store(dd);

    query_writer_single((void*)query, store);

    GArray* arr = g_array_index(store, GArray*, 2);
    g_array_free(store, TRUE);

    return arr;
}