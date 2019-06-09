#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "Util.h"
#include "Command.h"
#include "Table.h"
#include "SelectState.h"
#include "OptimizedTest.h"

///
/// Allocate State_t and initialize some attributes
/// Return: ptr of new State_t
///
State_t* new_State() {
    State_t *state = (State_t*)malloc(sizeof(State_t));
    state->saved_stdout = -1;
    return state;
}

///
/// Print shell prompt
///
void print_prompt(State_t *state) {
    if (state->saved_stdout == -1) {
        printf("db > ");
    }
}

///
/// Print the user in the specific format
///
void print_user(User_t *user, SelectArgs_t *sel_args) {
    size_t idx;
    printf("(");
    for (idx = 0; idx < sel_args->fields_len; idx++) {
        if (!strncmp(sel_args->fields[idx], "*", 1)) {
            printf("%d, %s, %s, %d", user->id, user->name, user->email, user->age);
        } else {
            if (idx > 0) printf(", ");

            if (!strncmp(sel_args->fields[idx], "id", 2)) {
                printf("%d", user->id);
            } else if (!strncmp(sel_args->fields[idx], "name", 4)) {
                printf("%s", user->name);
            } else if (!strncmp(sel_args->fields[idx], "email", 5)) {
                printf("%s", user->email);
            } else if (!strncmp(sel_args->fields[idx], "age", 3)) {
                printf("%d", user->age);
            }
        }
    }
    printf(")\n");
}

void print_like(Like_t *like, SelectArgs_t *sel_args){
    size_t idx;
    printf("(");
    for (idx = 0; idx < sel_args->fields_len; idx++) {
        if (!strncmp(sel_args->fields[idx], "*", 1)) {
            printf("%d, %d", like->id1, like->id2);
        } else {
            if (idx > 0) printf(", ");

            if (!strncmp(sel_args->fields[idx], "id1", 3)) {
                printf("%d", like->id1);
            } else if (!strncmp(sel_args->fields[idx], "id2", 3)) {
                printf("%d", like->id2);
            }
        }
    }
    printf(")\n");

}

///
/// Print the users for given offset and limit restriction
///
void print_users(UserTable_t *table, int *idxList, size_t idxList_len, Command_t *cmd) {
    size_t idx;
    int limit = cmd->cmd_args.sel_args.limit;
    int offset = cmd->cmd_args.sel_args.offset;

    if (offset == -1) {
        offset = 0;
    }

    //if (idxList) {
        if (cmd->cmd_args.sel_args.isAggr == 1){ //Aggregation function
            if (offset == 0 && limit != 0)
                print_aggr(table, idxList, idxList_len, &(cmd->cmd_args.sel_args));
        }
        else {
            int ub = idxList_len;

            if (limit != -1)
                if (offset + limit < ub)
                    ub = offset + limit;

            for (idx = offset; idx < ub; idx++) {
                /*if (limit != -1 && (idx - offset) >= limit) {
                    break;
                }*/
                print_user(get_User(table, idxList[idx]), &(cmd->cmd_args.sel_args));
            }
        }
    /*} else {
        if (cmd->cmd_args.sel_args.isAggr == 1 && offset == 0 && limit > 0){ //Aggregation function
            print_aggr(table, NULL, 0, &(cmd->cmd_args.sel_args));
        }
        else {
            for (idx = offset; idx < table->len; idx++) {
                if (limit != -1 && (idx - offset) >= limit) {
                    break;
                }
                print_user(get_User(table, idx), &(cmd->cmd_args.sel_args));
            }
        }
    }*/
}

void print_likes(LikeTable_t *table, size_t idxList_len, Command_t *cmd) {
    size_t idx;
    int limit = cmd->cmd_args.sel_args.limit;
    int offset = cmd->cmd_args.sel_args.offset;

    if (offset == -1) {
        offset = 0;
    }

    if (cmd->cmd_args.sel_args.isAggr == 1){ //Aggregation function
        if (offset == 0 && limit != 0)
            print_aggr(table, idxList_len, &(cmd->cmd_args.sel_args));
    }
    else {
        int ub = idxList_len;

        if (limit != -1)
            if (offset + limit < ub)
                ub = offset + limit;
        
        for (idx = offset; idx < ub; idx++) {
            /*if (limit != -1 && (idx - offset) >= limit) {
                break;
            }*/
            print_like(get_Like(table, idx), &(cmd->cmd_args.sel_args));
        }
    }
}

void print_aggr(UserTable_t *table, int *idxList, size_t idxList_len, SelectArgs_t *sel_args){
    size_t idx;
    printf("(");

    for (idx = 0; idx < sel_args->fields_len; idx++) {
        
        if (idx > 0) printf(", ");
        
        if (!strncmp(sel_args->fields[idx], "avg(", 4) || !strncmp(sel_args->fields[idx], "sum(", 4)) {
            int sum = 0;
            User_t *user;

            if (!strncmp(sel_args->fields[idx]+4, "id)", 3)){

                for (int i = 0; i < idxList_len; i++){
                    user = get_User(table, idxList[i]);
                    sum += user->id;
                }
            }

            else if (!strncmp(sel_args->fields[idx]+4, "age)", 4)){

                for (int i = 0; i < idxList_len; i++){
                    user = get_User(table, idxList[i]);
                    sum += user->age;
                }
            }

            if(!strncmp(sel_args->fields[idx], "sum(", 4)){
                printf("%d", sum);
            }
            else if(!strncmp(sel_args->fields[idx], "avg(", 4)){
                double avg = sum;
                if (idxList_len == 0)
                    avg = 0;
                else
                    avg /= idxList_len;
                printf("%.3lf", avg);
            }

        }
        else if (!strncmp(sel_args->fields[idx], "count(", 6)) {
            printf("%d", (int)idxList_len);

        }
    }

    printf(")\n");

}

void print_aggr(LikeTable_t *table, size_t idxList_len, SelectArgs_t *sel_args){
    size_t idx;
    printf("(");
    for (idx = 0; idx < sel_args->fields_len; idx++) {
        
        if (idx > 0) printf(", ");
        
        if (!strncmp(sel_args->fields[idx], "avg(", 4) || !strncmp(sel_args->fields[idx], "sum(", 4)) {
            int sum = 0;
            Like_t *like;

            if (!strncmp(sel_args->fields[idx]+4, "id1)", 4)){

                for (int i = 0; i < idxList_len; i++){
                    like = get_Like(table, i);
                    sum += like->id1;
                }
            }

            else if (!strncmp(sel_args->fields[idx]+4, "id2)", 4)){

                for (int i = 0; i < idxList_len; i++){
                    like = get_Like(table, i);
                    sum += like->id2;
                }
            }

            if(!strncmp(sel_args->fields[idx], "sum(", 4)){
                printf("%d", sum);
            }
            else if(!strncmp(sel_args->fields[idx], "avg(", 4)){
                double avg = sum;
                if (idxList_len == 0)
                    avg = 0;
                else
                    avg /= idxList_len;
                printf("%.3lf", avg);
            }

        }
        else if (!strncmp(sel_args->fields[idx], "count(", 6)) {
            printf("%d", (int)idxList_len);

        }
    }

    printf(")\n");

}

///
/// This function received an output argument
/// Return: category of the command
///
int parse_input(char *input, Command_t *cmd) {
    char *token;
    int idx;
    token = strtok(input, " ,\n");
    for (idx = 0; strlen(cmd_list[idx].name) != 0; idx++) {
        if (!strncmp(token, cmd_list[idx].name, cmd_list[idx].len)) {
            cmd->type = cmd_list[idx].type;
        }
    }
    while (token != NULL) {
        add_Arg(cmd, token);
        token = strtok(NULL, " ,\n");
    }
    return cmd->type;
}

///
/// Handle built-in commands
/// Return: command type
///
void handle_builtin_cmd(UserTable_t *table, Command_t *cmd, State_t *state) {
    if (!strncmp(cmd->args[0], ".exit", 5)) {
        //archive_table(table);
        exit(0);
    } else if (!strncmp(cmd->args[0], ".output", 7)) {
        if (cmd->args_len == 2) {
            if (!strncmp(cmd->args[1], "stdout", 6)) {
                close(1);
                dup2(state->saved_stdout, 1);
                state->saved_stdout = -1;
            } else if (state->saved_stdout == -1) {
                int fd = creat(cmd->args[1], 0644);
                state->saved_stdout = dup(1);
                if (dup2(fd, 1) == -1) {
                    state->saved_stdout = -1;
                }
                __fpurge(stdout); //This is used to clear the stdout buffer
            }
        }
    } /*else if (!strncmp(cmd->args[0], ".load", 5)) {
        if (cmd->args_len == 2) {
            load_table(table, cmd->args[1]);
        }
    }*/ else if (!strncmp(cmd->args[0], ".help", 5)) {
        print_help_msg();
    }
}

///
/// Handle query type commands
/// Return: command type
///
int handle_query_cmd(UserTable_t *user_table, LikeTable_t *like_table, Command_t *cmd) {
    if (!strncmp(cmd->args[0], "insert", 6)) {
        handle_insert_cmd(user_table, like_table, cmd);
        return INSERT_CMD;
    } else if (!strncmp(cmd->args[0], "select", 6)) {
        handle_select_cmd(user_table, like_table, cmd);
        return SELECT_CMD;
    } else if (!strncmp(cmd->args[0], "update", 6)) {
        handle_update_cmd(user_table, cmd);
        return UPDATE_CMD;
    } else if (!strncmp(cmd->args[0], "delete", 6)) {
        handle_delete_cmd(user_table, cmd);
        return DELETE_CMD;
    } else {
        return UNRECOG_CMD;
    }
}

int handle_delete_cmd(UserTable_t *table, Command_t *cmd) {
    cmd->type = DELETE_CMD;
    delete_state_handler(cmd, 1);

    int idxList_len;
    int *idxList = create_idxList(table, NULL, cmd, &(cmd->cmd_args.del_args.where_args), &idxList_len);
    delete_users(table, idxList, idxList_len, cmd);
    free(idxList);

    return idxList_len;

}

void delete_users(UserTable_t *table, int *idxList, size_t idxList_len, Command_t *cmd) {
    size_t idx;
    User_t *user = NULL;

    for (idx = 0; idx < idxList_len; idx++) {
        user = get_User(table, idxList[idx]);
        if (user->id <= INIT_TABLE_SIZE){
            table->id_index[user->id] = NULL;
        }
        else{
            table->big_id_index[user->id] = NULL;
        }
        
        table->cache_map[idxList[idx]] = 0;
        
    }

}

int handle_update_cmd(UserTable_t *table, Command_t *cmd) {
    cmd->type = UPDATE_CMD;
    update_table_state_handler(cmd, 1);

    int idxList_len;
    int *idxList = create_idxList(table, NULL, cmd, &(cmd->cmd_args.update_args.where_args), &idxList_len);
    int ret = update_users(table, idxList, idxList_len, cmd);
    free(idxList);

    return ret;

}

int update_users(UserTable_t *table, int *idxList, size_t idxList_len, Command_t *cmd) {
    size_t idx;

    if (!strncmp(cmd->cmd_args.update_args.field_name, "id", 2)){ //modifying primary key
        if (idxList_len >= 2){ //making multiple rows have the same primary key
            return 0;
        }

        int rhs = atoi(cmd->cmd_args.update_args.rhs_literal);
        int idxFound = -1;
        //User_t *usr_ptr;

        // Check if id exists in the table
        /*for (idx = 0; idx < table->len; idx++) {
            usr_ptr = get_User(table, idx);
            if (usr_ptr != NULL && usr_ptr->id == rhs) {
                idxFound = idx;
                break;
            }
        }*/
        //find if id exists
        if (rhs <= INIT_TABLE_SIZE){
            if (table->id_index[rhs] != NULL)
                idxFound = table->id_index[rhs] - table->users;
        }
        else{ //find big_id_index
            if (table->big_id_index.size() != 0){
                auto usr_ptr = table->big_id_index.find(rhs);
                if (usr_ptr != table->big_id_index.end() && usr_ptr->second != NULL){
                    idxFound = usr_ptr->second - table->users;
                }
            }
        }
        

        //check if the row whose id found is the one to update, i.e. no error
        if (idxFound != -1 && idxFound != idxList[0]){
            return 0;
        }


    }

    for (idx = 0; idx < idxList_len; idx++) {
        
        update_user(get_User(table, idxList[idx]), &(cmd->cmd_args.update_args));
    }
    return idxList_len;
}

void update_user(User_t *user, UpdateArgs_t *update_args){
    char *lhs = update_args->field_name;
    char *rhs = update_args->rhs_literal;

    if (!strncmp(lhs, "id", 2)){
        user->id = atoi(rhs);
        //map
    }
    else if (!strncmp(lhs, "name", 4))
        strncpy(user->name, rhs, MAX_USER_NAME);
    else if (!strncmp(lhs, "email", 5))
        strncpy(user->email, rhs, MAX_USER_EMAIL);
    else if (!strncmp(lhs, "age", 3))
        user->age = atoi(rhs);

}

///
/// The return value is the number of rows insert into table
/// If the insert operation success, then change the input arg
/// `cmd->type` to INSERT_CMD
///
int handle_insert_cmd(UserTable_t *user_table, LikeTable_t *like_table, Command_t *cmd) {
    int ret = 0;
    if(!strncmp(cmd->args[2], "user", 4)){
        User_t *user = command_to_User(cmd);
        if (user) {
            ret = add_User(user_table, user);
            if (ret > 0) {
                cmd->type = INSERT_CMD;
            }
        }
    }
    else if(!strncmp(cmd->args[2], "like", 4)){
        Like_t *like = command_to_Like(cmd);
        if (like) {
            ret = add_Like(like_table, like);
            if (ret > 0) {
                cmd->type = INSERT_CMD;
            }
        }
    }

   
    return ret;
}

///
/// The return value is the number of rows select from table
/// If the select operation success, then change the input arg
/// `cmd->type` to SELECT_CMD
///
int handle_select_cmd(UserTable_t *user_table, LikeTable_t *like_table, Command_t *cmd) {
    cmd->type = SELECT_CMD;
    field_state_handler(cmd, 1);
    int idxList_len = 0;

    if (cmd->cmd_args.sel_args.join_args.hasJoin){
        //optimize t4
        if (!optimized_t4(user_table, like_table, cmd) && !optimized_t5(user_table, like_table, cmd)){
            int *idxList = create_idxList(user_table, NULL, cmd, &(cmd->cmd_args.sel_args.where_args), &idxList_len);
            handle_join_operation(user_table, like_table, idxList, idxList_len, cmd);
            free(idxList);
        }
    }
    else if (!strncmp(cmd->cmd_args.sel_args.table, "like", 4)){
        print_likes(like_table, like_table->len, cmd);
        idxList_len = like_table->len;
    }
    else if (!strncmp(cmd->cmd_args.sel_args.table, "user", 4)){
        //optimize t3
        if (!optimized_t3(user_table, cmd)){
            int *idxList = create_idxList(user_table, NULL, cmd, &(cmd->cmd_args.sel_args.where_args), &idxList_len);
            print_users(user_table, idxList, idxList_len, cmd);
            free(idxList);
        }
    }

    
    return idxList_len;

}

int* create_idxList(UserTable_t *user_table, LikeTable_t *like_table, Command_t *cmd, WhereClauses_t *where_args, int *idxList_len) {
    int *idxList = (int*)malloc(user_table->len * sizeof(int));
    memset(idxList, 0, user_table->len * sizeof(int));
    *idxList_len = 0;

    for (int idx = 0; idx < user_table->len; idx++){
        if(user_table->cache_map[idx]){ //row exists
            //check for where conditions
            User_t *user = get_User(user_table, idx);
            int relation_result[2], final_result = 0;

            for (int i = 0; i < where_args->conditions_len; i++){

                char *lhs = where_args->conditions[i].field_name;
                char *op = where_args->conditions[i].realtional_op;
                char *rhs = where_args->conditions[i].rhs_literal;

                if (!strncmp(lhs, "id", 2) || !strncmp(lhs, "age", 3)){ // int type
                    double lhsi, rhsi = atof(rhs);

                    if (!strncmp(lhs, "id", 2))
                        lhsi = user->id;

                    else/* (!strncmp(lhs, "age", 3))*/
                        lhsi = user->age;

                    
                    if (!strncmp(op, ">=", 2))
                        relation_result[i] = lhsi >= rhsi ? 1 : 0;
                    else if (!strncmp(op, "<=", 2))
                        relation_result[i] = lhsi <= rhsi ? 1 : 0;
                    else if (!strncmp(op, "!=", 2))
                        relation_result[i] = lhsi != rhsi ? 1 : 0;
                    else if (!strncmp(op, ">", 1))
                        relation_result[i] = lhsi > rhsi ? 1 : 0;
                    else if (!strncmp(op, "<", 1))
                        relation_result[i] = lhsi < rhsi ? 1 : 0;
                    else if (!strncmp(op, "=", 1))
                        relation_result[i] = lhsi == rhsi ? 1 : 0;
                    

                }

                if (!strncmp(lhs, "name", 4) || !strncmp(lhs, "email", 5)){ // str type
                    char *lhss, *rhss = rhs;

                    if (!strncmp(lhs, "name", 4))
                        lhss = user->name;

                    else if (!strncmp(lhs, "email", 5))
                        lhss = user->email;

                    if (!strncmp(op, "!=", 2))
                        relation_result[i] = strcmp(lhss, rhss) ? 1 : 0;
                    else if (!strncmp(op, "=", 1))
                        relation_result[i] = strcmp(lhss, rhss) ? 0 : 1;
                    

                }

            }

            //logicl op
            if (where_args->conditions_len == 0){
                final_result = 1;
            }
            else if (where_args->conditions_len == 1){
                final_result = relation_result[0];
            }
            else /*(where_args->conditions_len == 2)*/{
                char *logical_op = where_args->logical_op;

                if (!strncmp(logical_op, "and", 3))
                    final_result = relation_result[0] && relation_result[1];
                if (!strncmp(logical_op, "or", 2))
                    final_result = relation_result[0] || relation_result[1];

            }

            //add to list
            if (final_result)
                idxList[(*idxList_len)++] = idx;
        }

    }

    return idxList;
}

void handle_join_operation(UserTable_t *user_table, LikeTable_t *like_table, int *idxList, size_t idxList_len, Command_t *cmd){
    size_t idx;
    int limit = cmd->cmd_args.sel_args.limit;
    int offset = cmd->cmd_args.sel_args.offset;

    if (offset == -1) {
        offset = 0;
    }

    if (offset != 0 || limit == 0) // only count()
        return;
    
    int count = 0;
    //int like_table_len = like_table->len;
    User_t *user = NULL;
    //Like_t *like = NULL;
    for (idx = 0; idx < idxList_len; idx ++){
        user = get_User(user_table, idxList[idx]);

        //for each user in the list, find if join condition matched
        if (cmd->cmd_args.sel_args.join_args.like_field == 1){ //id = id1
            //since id1 is primary key of Like, stop when we find one occurrence
            /*for (int i = 0; i < like_table_len; i++){
                like = get_Like(like_table, i);
                if (user->id == like->id1){
                    count ++;
                    break;
                }
            }*/
            if (user->id <= INIT_TABLE_SIZE){
                if (like_table->id1_count[user->id])
                    count ++;
            }
            else{
                if (like_table->big_id1_count.count(user->id))
                    count ++;
            }
            
        }
        else{ // id = id2
            /*for (int i = 0; i < like_table_len; i++){
                like = get_Like(like_table, i);
                if (user->id == like->id2){
                    count ++;
                }
            }*/
            if (user->id <= INIT_TABLE_SIZE){
                if (like_table->id2_count[user->id])
                    count += like_table->id2_count[user->id];
            }
            else{
                auto like_ptr = like_table->big_id2_count.find(user->id);
                if (like_ptr != like_table->big_id2_count.end())
                    count += like_ptr->second;
            }
            
        }

    }

    printf("(%d)\n", count);

}

///
/// Show the help messages
///
void print_help_msg() {
    const char msg[] = "# Supported Commands\n"
    "\n"
    "## Built-in Commands\n"
    "\n"
    "  * .exit\n"
    "\tThis cmd archives the table, if the db file is specified, then exit.\n"
    "\n"
    "  * .output\n"
    "\tThis cmd change the output strategy, default is stdout.\n"
    "\n"
    "\tUsage:\n"
    "\t    .output (<file>|stdout)\n\n"
    "\tThe results will be redirected to <file> if specified, otherwise they will display to stdout.\n"
    "\n"
    "  * .load\n"
    "\tThis command loads records stored in <DB file>.\n"
    "\n"
    "\t*** Warning: This command will overwrite the records already stored in current table. ***\n"
    "\n"
    "\tUsage:\n"
    "\t    .load <DB file>\n\n"
    "\n"
    "  * .help\n"
    "\tThis cmd displays the help messages.\n"
    "\n"
    "## Query Commands\n"
    "\n"
    "  * insert\n"
    "\tThis cmd inserts one user record into table.\n"
    "\n"
    "\tUsage:\n"
    "\t    insert <id> <name> <email> <age>\n"
    "\n"
    "\t** Notice: The <name> & <email> are string without any whitespace character, and maximum length of them is 255. **\n"
    "\n"
    "  * select\n"
    "\tThis cmd will display all user records in the table.\n"
    "\n";
    printf("%s", msg);
}

