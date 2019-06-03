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
- add join select ()
    - <font color="green">**add**</font> `join` operation to `select`
<font color="blue">**The program now functions but with high time complexity**</font>
---
