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
- update Makefile ()
    - change `CC` in `Makefile` to `g++`
    - add flags `-std=c++11 -O2`to `CFLAGS`
