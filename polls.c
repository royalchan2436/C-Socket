#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lists.h"

#define INPUT_BUFFER_SIZE 256
#define INPUT_ARG_MAX_NUM 12
#define DELIM " \n"


/* 
 * Print a formatted error message to stderr.
 */
void error(char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
}

/* 
 * Read and process poll commands
 * Return:  -1 for quit command
 *          0 otherwise
 */
char * process_args(int cmd_argc, char **cmd_argv, Poll **poll_list_ptr) {

    // some commands need the poll_list head and others
    // require the address of that head
    Poll *poll_list = *poll_list_ptr; 

    if (strcmp(cmd_argv[0], "list_polls") == 0 && cmd_argc == 1) {
        char *buf;
        buf = print_polls(poll_list);
        printf("%s", buf);
        free(buf);
    } else if (strcmp(cmd_argv[0], "create_poll") == 0 && cmd_argc >= 3) {
        int label_count = cmd_argc - 2;
        int result = create_poll(cmd_argv[1], &cmd_argv[2], label_count,
                     poll_list_ptr);
        if (result == 1) {
            char *ans = malloc(13);
            strcpy(ans, "Poll exists");
            return ans;
        }

   } else if (strcmp(cmd_argv[0], "vote") == 0 && cmd_argc == 3) {

        char *poll_name = cmd_argv[1];        // better name for clarity of code below

        // try to add participant to this poll  
        vote(poll_name, poll_list, cmd_argv[2]);
        

    } else if (strcmp(cmd_argv[0], "delete_poll") == 0 && cmd_argc == 2) {
        if (delete_poll(cmd_argv[1], poll_list_ptr) == 1) {
            char *ans2 = malloc(15);
            strcpy(ans2, "No poll exists");
            return ans2;
        }

    } else if (strcmp(cmd_argv[0], "poll_info") == 0 && cmd_argc == 2) {
        char *buffer;
        buffer = print_poll_info(cmd_argv[1], poll_list);
        return buffer;
    } else if(strcmp(cmd_argv[0], "comment") == 0 && cmd_argc == 3){
        
        comment(cmd_argv[1], poll_list, cmd_argv[2]);
        
    }else{
        char *ans1 = malloc(17);
        strcpy(ans1, "Incorrect syntax");
        return ans1;
    }
    char *ans4 = malloc(5);
    strcpy(ans4, "NULL");
    return ans4;
}

int main(void){
    return 0;
}
