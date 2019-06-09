#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "Command.h"
#include "Table.h"
#include "SelectState.h"
#include "OptimizedTest.h"


int optimized_t3(UserTable_t *table, Command_t *cmd){
    //handle the special case (t3): select count(*) from user where age <= {upper} and age >= {lower}
    //lower, upper in [0, 200]
    const char *query[] = {"select", "count(*)", "from", "user", "where", "age", "<=", 0, "and", "age", ">="};
    const int qs_len[] = {6, 8, 4, 4, 5, 3, 2, 0, 3, 3, 2};
    int match_t3 = 1;
    if (cmd->args_len == 12){

        for (int i = 0; i < 11; i++){
            if (strncmp(cmd->args[i], query[i], qs_len[i])){
                match_t3 = 0;
                break;
            }
        }
        if (match_t3){
            int upper = atoi(cmd->args[7]), lower = atoi(cmd->args[11]);
            if (upper <= 200 && lower <= upper){
                int sum = 0;
                for (int i = lower; i <= upper; i++)
                    sum += table->age_count[i];
                printf("(%d)\n", sum);
                return 1;
            }
                
        }
    }
    return 0;
}

void *p_t4(void *args){
    t4_args *argv = (t4_args*)args;
    UserTable_t *user_table = argv->user_table;
    LikeTable_t *like_table = argv->like_table;
    int k = argv->k;
    int count = 0;
    int start = (user_table->capacity / DEGREE) * k;
    int end = k == DEGREE - 1 ? user_table->capacity : (user_table->capacity / DEGREE) * (k + 1);

    User_t *user = NULL;
    for (int idx = start; idx < end; idx++){
        user = get_User(user_table, idx);
        if (user && !strcmp(user->name, argv->name)){
            if (user->id <= INIT_TABLE_SIZE){
                if (like_table->id1_count[user->id])
                    count ++;
            }
            else{
                if (like_table->big_id1_count.count(user->id))
                    count ++;
            }
        }

    }

    argv->ans[k] = count;
    pthread_exit(0);
}

int optimized_t4(UserTable_t *user_table, LikeTable_t *like_table, Command_t *cmd){
    //handle the special case (t4):
    //select count(*) from user join like on id = id1 where name = "{target_user}"
    const char *query[] = {"select", "count(*)", "from", "user", "join", "like", "on",
                            "id", "=", "id1", "where", "name", "="};
    const int qs_len[] = {6, 8, 4, 4, 4, 4, 2, 2, 1, 3, 5, 4, 1};
    int match_t4 = 1;
    if (cmd->args_len == 14){

        for (int i = 0; i < 13; i++){
            if (strncmp(cmd->args[i], query[i], qs_len[i])){
                match_t4 = 0;
                break;
            }
        }
        if (match_t4){
            pthread_t tid[DEGREE];
            pthread_attr_t attr;
	        pthread_attr_init(&attr);
            int ans[DEGREE] = {0};
            int count = 0;

            for (int k = 0; k < DEGREE; k++){
                t4_args *p = new t4_args(user_table, like_table, cmd->args[13], ans, k);
                pthread_create(&tid[k], &attr, p_t4, p);
            }

            for (int k = 0; k < DEGREE; k++){
                pthread_join(tid[k], NULL);
            }

            for (int k = 0; k < DEGREE; k++)
                count += ans[k];

            printf("(%d)\n", count);
            return 1;
        }
    }

    return 0;
}

void *p_t5(void *args){
    t5_args *argv = (t5_args*)args;
    UserTable_t *user_table = argv->user_table;
    LikeTable_t *like_table = argv->like_table;
    int k = argv->k;
    int count = 0;
    int start = (user_table->capacity / DEGREE) * k;
    int end = k == DEGREE - 1 ? user_table->capacity : (user_table->capacity / DEGREE) * (k + 1);

    User_t *user = NULL;
    for (int idx = start; idx < end; idx++){
        user = get_User(user_table, idx);
        if (user && user->age < argv->age){
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

    argv->ans[k] = count;
    pthread_exit(0);
}

int optimized_t5(UserTable_t *user_table, LikeTable_t *like_table, Command_t *cmd){
    //handle the special case (t4):
    //select count(*) from user join like on id = id2 where age < {target_age}
    const char *query[] = {"select", "count(*)", "from", "user", "join", "like", "on",
                            "id", "=", "id2", "where", "age", "<"};
    const int qs_len[] = {6, 8, 4, 4, 4, 4, 2, 2, 1, 3, 5, 3, 1};
    int match_t5 = 1;
    if (cmd->args_len == 14){

        for (int i = 0; i < 13; i++){
            if (strncmp(cmd->args[i], query[i], qs_len[i])){
                match_t5 = 0;
                break;
            }
        }
        if (match_t5){
            pthread_t tid[DEGREE];
            pthread_attr_t attr;
	        pthread_attr_init(&attr);
            int ans[DEGREE] = {0};
            int count = 0;

            for (int k = 0; k < DEGREE; k++){
                t5_args *p = new t5_args(user_table, like_table, atoi(cmd->args[13]), ans, k);
                pthread_create(&tid[k], &attr, p_t5, p);
            }

            for (int k = 0; k < DEGREE; k++){
                pthread_join(tid[k], NULL);
            }

            for (int k = 0; k < DEGREE; k++)
                count += ans[k];

            printf("(%d)\n", count);
            return 1;
        }
    }

    return 0;
}