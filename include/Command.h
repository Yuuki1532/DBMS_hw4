#ifndef COMMAND_H
#define COMMAND_H

enum { 
    UNRECOG_CMD,
    BUILT_IN_CMD,
    QUERY_CMD,
};

enum {
    INSERT_CMD = 100,
    SELECT_CMD,
    UPDATE_CMD,
    DELETE_CMD,
};

typedef struct {
    char name[256];
    int len;
    unsigned char type;
} CMD_t;

extern CMD_t cmd_list[];

typedef struct Conditions{
    char *field_name;
    char *realtional_op;
    char *rhs_literal;
} Conditions_t;

typedef struct WhereClauses{
    Conditions_t conditions[2];
    int conditions_len;
    char *logical_op;
} WhereClauses_t;

typedef struct JoinClauses{
    //table to join will always be `User` and `Like` (in order)
    //condition will always be one of id = id1 or id = id2
    int hasJoin;
    int like_field; // 1 or 2 to represent id1 or id2 respectively
} JoinClauses_t;

typedef struct SelectArgs {
    char *table;
    char **fields;
    size_t fields_len;
    int offset;
    int limit;
    int isAggr;
    WhereClauses_t where_args;
    JoinClauses_t join_args;
} SelectArgs_t;

typedef struct DeleteArgs {
    WhereClauses_t where_args;
} DeleteArgs_t;

typedef struct UpdateArgs{
    char *field_name;
    char *rhs_literal;
    WhereClauses_t where_args;
} UpdateArgs_t;

typedef union {
    SelectArgs_t sel_args;
    UpdateArgs_t update_args;
    DeleteArgs_t del_args;
} CmdArg_t;

typedef struct Command {
    unsigned char type;
    char **args;
    size_t args_len;
    size_t args_cap;
    CmdArg_t cmd_args;
} Command_t;

Command_t* new_Command();
int add_Arg(Command_t *cmd, const char *arg);
int add_select_field(Command_t *cmd, const char *argument);
void cleanup_Command(Command_t *cmd);

#endif
