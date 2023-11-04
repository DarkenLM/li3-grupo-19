#include "queries/queries.h"

Tokens tokenize_query(char* line, ssize_t len) {
    char* ptr = strndup(line, len + 1);
    if (ptr == NULL) exit(EXIT_FAILURE);

    if (ptr[len - 1] == '\n') {
        ptr[len - 1] = '\0';
    }

    int seps = 1;
    for (int i = 0; line[i]; i++) seps += (line[i] == ' ');

    char** arr = (char**)malloc(seps * sizeof(char*));

    char* token;
    int i = 0;
    while ((token = strsep(&ptr, " ")) != NULL) {
        int tokenLen = strlen(token);
        char* tokenData = strndup(token, tokenLen + 1);
        arr[i++] = tokenData;
    }

    // TODO: Separate possible flag on query. Concatenate string literals.

    Tokens ret = (Tokens)malloc(sizeof(TOKENS));
    ret->data = arr;
    ret->len = seps;

    free(ptr);
    return ret;
}

#pragma region Parser helpers
int query_verifier(Tokens tokens) {
    // As per the project document, the queries are guaranteed to be valid.
    // So, only a syntatic sanity check will be performed.

    for (int i = 0; i < tokens->len; i++) {
        if (tokens->data[i] == NULL) return 0;
    }

    return 1;
}

void* query_parser(Tokens tokens) {
    Query query = (Query)malloc(sizeof(QUERY));

    char id[QUERIES_CHAR_LEN] = { 0 };
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

    char* args[QUERIES_MAX_ARGS] = { 0 };

    // TODO: Concatenate strings on the args
    // j = 0;
    // for (int i = 1; i < tokens->len; i++) {
    //     if (tokens->data[i][0] == '\"') {
    //         strcpy(args[j], tokens->data[i]);

    //         i++;

    //         int len = strlen(tokens->data[i]);
    //         while(tokens->data[i][len - 1] != '\"') {
    //             strcat(args[j], tokens->data[i]);
    //             i++;
    //         }

    //         strcat(args[j], tokens->data[i]);
    //     } else {
    //         args[j] = tokens->data[i];
    //     }
        
    //     j++;
    // }

    j = 0;
    for (int i = 1; i < tokens->len; i++, j++) {
        if (tokens->data[i][0] == '\"') {
            int totalLen = 1;
            char* temp = tokens->data[i];
            int tempsize = strlen(temp);
            int start = i, end = i;

            while(temp[tempsize - 1] != '\"') {
                totalLen += tempsize + 1;
                end++;
                // i++;

                temp = tokens->data[i + (end - start)];
                tempsize = strlen(temp);
            }
            totalLen += tempsize;

            args[j] = (char*)malloc(totalLen * sizeof(char*));
            // while(temp[tempsize - 1] != '\"') {
            //     strcat(args[j], tokens->data[i++]);
            // }

            strcpy(args[j], tokens->data[i]);
            for (int k = start + 1; k <= end; k++) {
                strcat(args[j], " ");
                strcat(args[j], tokens->data[k]);
            }
            // strcat(args[j], tokens->data[i]);

            i += end - start;
        } else {
            args[j] = tokens->data[i];
        }
    }
    
    // query->id = id;
    strcpy(query->id, id);
    query->flag = flag;
    // query->args = args;
    memcpy(query->args, args, QUERIES_MAX_ARGS * sizeof(char*));

    return query;
}

void query_writer(void* raw_data) {
    volatile Query data = (Query)raw_data;

    printf("QUERY WRITE >>> ID: '%s' | FLAG: '%c' | ARGS:", data->id, data->flag);
    for (int i = 0; i < QUERIES_MAX_ARGS; i++) printf(" '%s' |", data->args[i]);

    printf("\n");
    fflush(stdout);
    return;
}

void query_discarder(Tokens tokens) {
    for (int i = 0; i < tokens->len; i++) free(tokens->data[i]);
    free(tokens);
}
#pragma endregion



void* query_execute(Query query, char* args) {
    IGNORE_ARG(query);
    IGNORE_ARG(args);
    return NULL;
};
void query_run_bulk(char* input_file, char* output_file) {
    parse_file(input_file, &tokenize_query, &query_verifier, &query_parser, &query_writer, &query_discarder);
    return;
};
void query_run_single(char* query, ssize_t len) {
    parse(query, len, &tokenize_query, &query_verifier, &query_parser, &query_writer, &query_discarder);
    return;
};