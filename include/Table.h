#ifndef TABLE_H
#define TABLE_H
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <vector>
#include "User.h"
#include "Like.h"

#define INIT_TABLE_SIZE 135000
#define EXT_LEN 50000

typedef struct UserTable {
    size_t capacity;
    size_t len;
    User_t *users;
    unsigned char *cache_map;
    /*FILE *fp;
    char *file_name;*/
    //std::map<unsigned int, User_t*> id_index;
    User_t* id_index[INIT_TABLE_SIZE + 1]; //index entries whose id is less then INIT_TABLE_SIZE
    std::map<unsigned int, User_t*> big_id_index;
    int age_count[201];
} UserTable_t;

typedef struct LikeTable {
    size_t capacity;
    size_t len;
    Like_t *likes;
    unsigned char *cache_map;
    int id1_count[INIT_TABLE_SIZE + 1];
    int id2_count[INIT_TABLE_SIZE + 1];
    std::map<unsigned int, int> big_id1_count;
    std::map<unsigned int, int> big_id2_count;
} LikeTable_t;

UserTable_t *new_UserTable();
int add_User(UserTable_t *table, User_t *user);
//int archive_table(UserTable_t *table);
//int load_table(UserTable_t *table, char *file_name);
inline User_t* get_User(UserTable_t *table, size_t idx);

LikeTable_t *new_LikeTable();
int add_Like(LikeTable_t *table, Like_t *user);
inline Like_t* get_Like(LikeTable_t *table, size_t idx);

///
/// Return the user in table by the given index
///
inline User_t* get_User(UserTable_t *table, size_t idx) {
    if (!table->cache_map[idx]) {
        return NULL;
    }
    return table->users+idx;
}

inline Like_t* get_Like(LikeTable_t *table, size_t idx) {
    if (!table->cache_map[idx]) {
        return NULL;
    }
    return table->likes+idx;
}

#endif
