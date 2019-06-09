#ifndef OPTTEST_H
#define OPTTEST_H
#include "Command.h"
#include "Table.h"
#define DEGREE 20

struct t4_args{
    UserTable_t *user_table;
    LikeTable_t *like_table;
    char *name;
    int *ans;
    int k;
    t4_args(UserTable_t *user_table, LikeTable_t *like_table, char *name, int *ans, int k)
        :user_table(user_table), like_table(like_table), name(name), ans(ans), k(k) {};
};

struct t5_args{
    UserTable_t *user_table;
    LikeTable_t *like_table;
    int age;
    int *ans;
    int k;
    t5_args(UserTable_t *user_table, LikeTable_t *like_table, int age, int *ans, int k)
        :user_table(user_table), like_table(like_table), age(age), ans(ans), k(k) {};
};

void *p_t4(void *args);
void *p_t5(void *args);
int optimized_t3(UserTable_t *table, Command_t *cmd);
int optimized_t4(UserTable_t *user_table, LikeTable_t *like_table, Command_t *cmd);
int optimized_t5(UserTable_t *user_table, LikeTable_t *like_table, Command_t *cmd);

#endif