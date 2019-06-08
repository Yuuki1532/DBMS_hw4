#include <stdlib.h>
#include <string.h>
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