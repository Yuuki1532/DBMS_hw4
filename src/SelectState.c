#include <string.h>
#include <stdlib.h>
#include "Command.h"
#include "SelectState.h"

void field_state_handler(Command_t *cmd, size_t arg_idx) {
    cmd->cmd_args.sel_args.fields = NULL;
    cmd->cmd_args.sel_args.fields_len = 0;
    cmd->cmd_args.sel_args.limit = -1;
    cmd->cmd_args.sel_args.offset = -1;
    cmd->cmd_args.sel_args.isAggr = 0;
    cmd->cmd_args.sel_args.where_args.conditions_len = 0;

    while(arg_idx < cmd->args_len) {
        if (!strncmp(cmd->args[arg_idx], "*", 1)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "id", 2)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "name", 4)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "email", 5)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "age", 3)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "avg(",4)) {
            add_select_field(cmd, cmd->args[arg_idx]);
            cmd->cmd_args.sel_args.isAggr = 1;
        } else if (!strncmp(cmd->args[arg_idx], "count(", 6)) {
            add_select_field(cmd, cmd->args[arg_idx]);
            cmd->cmd_args.sel_args.isAggr = 1;
        } else if (!strncmp(cmd->args[arg_idx], "sum(", 4)) {
            add_select_field(cmd, cmd->args[arg_idx]);
            cmd->cmd_args.sel_args.isAggr = 1;
        } else if (!strncmp(cmd->args[arg_idx], "from", 4)) {
            table_state_handler(cmd, arg_idx+1);
            return;
        } else {
            cmd->type = UNRECOG_CMD;
            return;
        }
        arg_idx += 1;
    }
    cmd->type = UNRECOG_CMD;
    return;
}

void table_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len
            && !strncmp(cmd->args[arg_idx], "user", 4)) {

        arg_idx++;
        if (arg_idx == cmd->args_len) {
            return;
        } else if (!strncmp(cmd->args[arg_idx], "where", 5)) {
            where_state_handler(cmd, &(cmd->cmd_args.sel_args.where_args), arg_idx+1);
            return;
        } else if (!strncmp(cmd->args[arg_idx], "offset", 6)) {
            offset_state_handler(cmd, arg_idx+1);
            return;
        } else if (!strncmp(cmd->args[arg_idx], "limit", 5)) {
            limit_state_handler(cmd, arg_idx+1);
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}

void delete_state_handler(Command_t *cmd, size_t arg_idx) {
    cmd->cmd_args.del_args.where_args.conditions_len = 0;

    if (arg_idx < cmd->args_len
        && !strncmp(cmd->args[arg_idx], "from", 4)) {

        arg_idx++;
        if (arg_idx < cmd->args_len
            && !strncmp(cmd->args[arg_idx], "user", 4)) {

            arg_idx++;

            if (arg_idx == cmd->args_len) return;

            if (!strncmp(cmd->args[arg_idx], "where", 5)) {
                where_state_handler(cmd, &(cmd->cmd_args.del_args.where_args), arg_idx+1);
                return;
            }

        }
    }


    cmd->type = UNRECOG_CMD;
    return;
}

void update_table_state_handler(Command_t *cmd, size_t arg_idx) {
    cmd->cmd_args.update_args.where_args.conditions_len = 0;

    if (arg_idx < cmd->args_len
        && !strncmp(cmd->args[arg_idx], "user", 4)) {

        arg_idx++;
        if (arg_idx < cmd->args_len
            && !strncmp(cmd->args[arg_idx], "set", 3)) {

            arg_idx++;

            //handle field=value part

            int current_term = 0; //lhs or rhs

            while (arg_idx < cmd->args_len){
                char *cmd_dup = strdup(cmd->args[arg_idx]);
                char *assign_op_ptr = NULL;

                assign_op_ptr = strstr(cmd_dup, "=");
                if (assign_op_ptr) //assign op found
                    strncpy(assign_op_ptr, "\\", 1);

                char *token = strtok(cmd_dup, "\\");
                while (token){
                    if (current_term == 0){
                        cmd->cmd_args.update_args.field_name = strdup(token);
                        current_term += 1;
                    }
                    else if (current_term == 1){
                        cmd->cmd_args.update_args.rhs_literal = strdup(token);
                        current_term += 1;
                        break;
                    }

                    token = strtok(NULL, "\\");
                }

                arg_idx += 1;

                if (current_term == 2)
                    break;
            }

            if (arg_idx == cmd->args_len) return;

            if (!strncmp(cmd->args[arg_idx], "where", 5)) {
                where_state_handler(cmd, &(cmd->cmd_args.update_args.where_args), arg_idx+1);
                return;
            }


        }
    }


    cmd->type = UNRECOG_CMD;
    return;
}

void where_state_handler(Command_t *cmd, WhereClauses_t *where_args, size_t arg_idx) {

    //handle a single condition
    //condition_state_handler(cmd, arg_idx);
    //op: >, <, >=, <=, =, !=
    const int op_num = 6;
    const char *op_str[] = {">=", "<=", "!=", ">", "<", "="};

    int condition_idx = where_args->conditions_len;
    int current_term = 0; //lhs or rhs

    while (arg_idx < cmd->args_len){
        char *cmd_dup = strdup(cmd->args[arg_idx]);
        //unsigned int op_idx = strcspn(cmd_dup, "><=!");
        char *op_ptr = NULL;
        for (int i = 0; i < op_num; i++){
            op_ptr = strstr(cmd_dup, op_str[i]);
            if (op_ptr){
                where_args->conditions[condition_idx].realtional_op = strdup(op_str[i]);
                if (strlen(op_str[i]) == 1)
                    strncpy(op_ptr, "\\", 1);
                else
                    strncpy(op_ptr, "\\\\", 2);

                break;
            }
        }

        char *token = strtok(cmd_dup, "\\");
        while (token){
            if (current_term == 0){
                where_args->conditions[condition_idx].field_name = strdup(token);
                current_term += 1;
            }
            else if (current_term == 1){
                where_args->conditions[condition_idx].rhs_literal = strdup(token);
                current_term += 1;
                where_args->conditions_len += 1;
                break;
            }

            token = strtok(NULL, "\\");
        }

        arg_idx += 1;

        if (current_term == 2)
            break;
    }

    if (arg_idx == cmd->args_len) return;

   
    if (!strncmp(cmd->args[arg_idx], "and", 3)){
        if (where_args->conditions_len != 1){
            cmd->type = UNRECOG_CMD;
            return;
        }
        where_args->logical_op = strdup("and");
        where_state_handler(cmd, where_args, arg_idx+1);
        return;

    } else if (!strncmp(cmd->args[arg_idx], "or", 2)){
        if (where_args->conditions_len != 1){
            cmd->type = UNRECOG_CMD;
            return;
        }
        where_args->logical_op = strdup("or");
        where_state_handler(cmd, where_args, arg_idx+1);
        return;

    } else if (!strncmp(cmd->args[arg_idx], "offset", 6)){
        offset_state_handler(cmd, arg_idx+1);
        return;
    } else if (!strncmp(cmd->args[arg_idx], "limit", 5)){
        limit_state_handler(cmd, arg_idx+1);
        return;
    } else {
        cmd->type = UNRECOG_CMD;
        return;
    }

    cmd->type = UNRECOG_CMD;
    return;
}

void offset_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len) {
        cmd->cmd_args.sel_args.offset = atoi(cmd->args[arg_idx]);

        arg_idx++;

        if (arg_idx == cmd->args_len) {
            return;
        } else if (arg_idx < cmd->args_len
                && !strncmp(cmd->args[arg_idx], "limit", 5)) {

            limit_state_handler(cmd, arg_idx+1);
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}

void limit_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len) {
        cmd->cmd_args.sel_args.limit = atoi(cmd->args[arg_idx]);

        arg_idx++;

        if (arg_idx == cmd->args_len) {
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}
