#ifndef DB_UTIL_H
#define DB_UTIL_H
#include "Command.h"
#include "Table.h"

typedef struct State {
    int saved_stdout;
} State_t;

State_t* new_State();
void print_prompt(State_t *state);
void print_user(User_t *user, SelectArgs_t *sel_args);
void print_users(UserTable_t *table, int *idxList, size_t idxListLen, Command_t *cmd);
void print_like(Like_t *like, SelectArgs_t *sel_args);
void print_likes(LikeTable_t *table, size_t idxList_len, Command_t *cmd);
int parse_input(char *input, Command_t *cmd);
void handle_builtin_cmd(UserTable_t *table, Command_t *cmd, State_t *state);
int handle_query_cmd(UserTable_t *user_table, LikeTable_t *like_table, Command_t *cmd);
int handle_insert_cmd(UserTable_t *user_table, LikeTable_t *like_table, Command_t *cmd);
int handle_select_cmd(UserTable_t *user_table, LikeTable_t *like_table, Command_t *cmd);
void print_help_msg();
void print_aggr(UserTable_t *table, int *idxList, size_t idxListLen, SelectArgs_t *sel_args);
void print_aggr(LikeTable_t *table, size_t idxListLen, SelectArgs_t *sel_args);
int* create_idxList(UserTable_t *user_table, LikeTable_t *like_table, Command_t *cmd, WhereClauses_t *where_args, int *idxList_len);
int handle_update_cmd(UserTable_t *table, Command_t *cmd);
int update_users(UserTable_t *table, int *idxList, size_t idxList_len, Command_t *cmd);
void update_user(User_t *user, UpdateArgs_t *update_args);
int handle_delete_cmd(UserTable_t *table, Command_t *cmd);
void delete_users(UserTable_t *table, int *idxList, size_t idxList_len, Command_t *cmd);
void handle_join_operation(UserTable_t *user_table, LikeTable_t *like_table, int *idxList, size_t idxList_len, Command_t *cmd);


#endif
