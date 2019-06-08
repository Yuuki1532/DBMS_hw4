# Simple DBMS                                                                                                   

This project is a simple DBMS implemented using C, as the education material of the Introduction to Database Systems in National Chiao Tung University

### Table schema
#### `User` table
| field_name | data_type |
|---|---|
| id | unsigned int |
| name | char[255] |
| email | char[255] |
| age | unsigned int |

#### `Like` table
| field_name | data_type |
|---|---|
| id1 | unsigned int |
| id2 | unsigned int |

### log
- initial commit (9d76308)
    - temporarily fixed compability for new testdata with `user table`
- update readme.md (e07f944)
- update Makefile (90e1072)
    - <font color="green">**add**</font> flags `-std=c++11 -O2` to `CFLAGS`
    - <font color="orange">**change**</font> `CC` in `Makefile` to `g++`
- define 2 tables: `UserTable` , `LikeTable` (1dde43e)
    - <font color="green">**add**</font> patch 0.0.013
    - <font color="green">**add**</font> `Like.h` , `Like.c`
    - <font color="green">**add**</font> functions `new_Like` `command_to_Like` `new_LikeTable` `add_Like` `get_Like` by the same means as those of `User`
    - <font color="green">**add**</font> `insert` query for `Like` table
    - <font color="orange">**rename**</font> `Table_t` to `UserTable_t`
    - <font color="orange">**rename**</font> related `Table` strings in function which actually inferred to `User` to `UserTable`
    - <font color="orange">**change**</font> flags to `-g` (removed `-O2`) for debugging
    - <font color="red">**remove**</font> unit tests
    - <font color="red">**remove**</font> `file mode` , `.load`
- add query parsing for `join` (be33a4b)
    - <font color="green">**add**</font> `Like` table to `table_state_handler`
    - <font color="green">**add**</font> modifications on query structs to support `Like` table
    - <font color="green">**add**</font> `join_state_handler`
    - <font color="orange">**change**</font>: simplify `update_table_state_handler` `where_state_handler` to ignore string bonus
- add select (without join) for `Like` table (843c240)
    - <font color="green">**add**</font> `select` (without join) for `Like` table
    - <font color="green">**add**</font> `print_like` `print_likes` `print_aggr (overload)`
- add join select (5e91902)
    - <font color="green">**add**</font> `join` operation to `select`
<font color="blue">**The program now functions but with high time complexity**</font>
---
- add some index and special cases optimization(1fe0c2a)
    - <font color="green">**add**</font> index (std::map) on `user.id`
    - <font color="green">**add**</font> count of each value (std::map) on `like.id1` `like.id2`
    - <font color="green">**add**</font> use the map above to speedup `insert` `delete` `update` `join`
    - <font color="red">**remove**</font> the old fashion primary key check and join condition check which takes O(n)
    - <font color="green">**add**</font> `-Wno-sign-compare` flag to `Makefile`
    - <font color="green">**add**</font> `OptimizedTest.h` `OptimizedTest.c`
    - <font color="green">**add**</font> `age_count` to `User` , `optimized_t3`
    - <font color="orange">**change**</font> all `.c` files to `.cpp` files
    - <font color="green">**add**</font>: use `id_index` array + `big_id_index` map for index on `id` to perform faster insertions on general testcases
- optimized index on Like table()
    - <font color="green">**add**</font>: use the same method of `id_index` + `big_id_index` to construct `id1_count` `big_id1_count` `id2_count` `big_id2_count` on `Like` table to speedup
