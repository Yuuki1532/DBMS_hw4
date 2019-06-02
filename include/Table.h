#ifndef TABLE_H
#define TABLE_H
#include <stdlib.h>
#include <stdio.h>
#include "User.h"
#include "Like.h"

#define INIT_TABLE_SIZE 10000
#define EXT_LEN 500

typedef struct UserTable {
    size_t capacity;
    size_t len;
    User_t *users;
    unsigned char *cache_map;
    /*FILE *fp;
    char *file_name;*/
} UserTable_t;

typedef struct LikeTable {
    size_t capacity;
    size_t len;
    Like_t *likes;
    unsigned char *cache_map;
} LikeTable_t;

UserTable_t *new_UserTable();
int add_User(UserTable_t *table, User_t *user);
//int archive_table(UserTable_t *table);
//int load_table(UserTable_t *table, char *file_name);
User_t* get_User(UserTable_t *table, size_t idx);

LikeTable_t *new_LikeTable();
int add_Like(LikeTable_t *table, Like_t *user);
Like_t* get_Like(LikeTable_t *table, size_t idx);

#endif
