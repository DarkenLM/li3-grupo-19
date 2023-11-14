#ifndef USER_CATALOG
#define USER_CATALOG

#include "common.h"
#include "collections/user.h"

gint user_tree_compare_func(gconstpointer a, gconstpointer b);
void user_print_tree(gpointer data, gpointer user_data);
void write_user(USER *user, ParserStore store);
void print_user_key_value(void* key, void* value);

#endif