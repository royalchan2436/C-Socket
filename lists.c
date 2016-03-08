#include "lists.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int asprintf(char **strp, const char *fmt, ...);
void free_memory(Poll *poll);

/* a wrapper function for malloc so we don't have to do this each time. */
void *Malloc(int size) {
    void *result;
    if ((result = malloc(size)) == NULL) {
        perror("malloc");
        exit(1);
    }
    return result;
}

/* Create a poll with this name and num_slots. 
 * Insert it into the list of polls whose head is pointed to by *head_ptr_add
 * Return 0 if successful, 1 if a poll by this name already exists in this list.
 */
int create_poll(char *name, char **slot_labels, int num_slots, 
                Poll **head_ptr_add) {

    // can't have duplicate named polls so first check if this poll exists
    if (find_poll(name, *head_ptr_add) != NULL) {
        return 1;
    }
    Poll *new_poll = Malloc(sizeof(struct poll));
    new_poll->num_slots = num_slots;

    // copy name in and ensure it is a string
    strncpy(new_poll->name, name, 31);
    new_poll->name[31] = '\0';

    // create the array of slot_labels and malloc space for each label
    new_poll->slot_labels = Malloc(sizeof(char *) * num_slots);
    int i;
    for (i=0; i < num_slots; i++) {
        new_poll->slot_labels[i] = Malloc(strlen(slot_labels[i])+1);
        strcpy(new_poll->slot_labels[i], slot_labels[i]);
    }

    // put new poll at tail of list so its next is NULL explicitly
    new_poll->comment = NULL;
    new_poll->avail = NULL;
    new_poll->next = NULL;
    // if this is first item to add it goes at front
    if (*head_ptr_add == NULL) {
        *head_ptr_add = new_poll;
    } else {
        // traverse the list to find the tail and add our new node
        Poll *prev = *head_ptr_add;
        while (prev->next != NULL) {
            prev = prev->next;
        }
        prev->next = new_poll;
    }
    return 0;
}


/* Return a pointer to the poll with this name in this list starting with head.
  Return NULL if there is no such poll.
 */
Poll *find_poll(char *name, Poll *head) {
    while (head != NULL) {
        if (!strcmp(head->name, name)) {
            return head;
        }
        head = head->next;
    }
    // if we get this far then we didn't find a poll with this name
    return NULL;
}



/* delete the poll by the name poll_name from the list at *head_ptr_add.
   Update the head of the list as appropriate and free all dynamically 
   allocated memory no longer used 
   Return 0 if successful and 1 if poll by this name is not found in list. 
*/
int delete_poll(char *poll_name, Poll **head_ptr_add) {

    Poll *poll_to_delete; 
    if ((poll_to_delete = find_poll(poll_name, *head_ptr_add)) == NULL) {
        return 1;
    }
    // need to find pointer to previous poll or to know if first
    Poll *prev = *head_ptr_add;
    // head must point to at least one poll or this function would have
    // already returned after unsuccessfully finding the poll
    if (!strcmp(prev->name, poll_name)) {
        // poll to delete is at the head of the list so go around it
        *head_ptr_add = prev->next;
    } else {
        while (prev->next != NULL) {
            if (!strcmp(prev->next->name, poll_name)) {
                break;
            }
            prev = prev->next;
        }
    }
    // now we have a pointer to poll_to_delete and to prev
    prev->next = poll_to_delete->next;
    // free the memory in poll_to_delete
    free_memory(poll_to_delete);
    return 0;
}

/* do all the freeing for a single poll */
void free_memory(Poll *poll) {
    // clean up participants
    Participant *cur = poll->participants;
    Participant *next;
    while (cur != NULL) {
        if (cur->comment != NULL) {
            free(cur->comment);
        }
        if (cur->availability != NULL) {
            free(cur->availability);
        }
        next = cur->next;
        free(cur);
        cur = next;
    }

    // clean up slot labels
    int i;
    for (i=0; i < poll->num_slots; i++) {
        free(poll->slot_labels[i]);
    }
    free(poll->slot_labels);
    free(poll);
}
      
/* 
 *  Print poll names one per line 
 */
char *print_polls(Poll *head) {
     
    char *null = Malloc(5);
    strcpy(null, "NULL");
    if(head == NULL){
        return null;
    }
    Poll *current = head;
    Poll *current_1 = head;
    int size = 0;
    char *result;
    while (current != NULL) {
        size += strlen(current->name)+2;
        current = current->next;
    }
    result = Malloc(size);
    while(current_1 != NULL){
        strcat(result, current_1->name);
        strcat(result, "\n");
        current_1 = current_1->next;
    }
    return result;
}


/* For the poll by the name poll_name from the list at head_ptr,
 * print the name, number of slots and each label and each participant.
 * For each participant, print name and availability.
 * Return 0 if successful and 1 if poll by this name is not found in list. 
 */
char * print_poll_info(char *poll_name, Poll *head_ptr) {

    Poll *poll; 
    char *null = Malloc(5);
    strcpy(null, "NULL");
    if ((poll = find_poll(poll_name, head_ptr)) == NULL) {
        return null;
    }
    
    int size = 0;
    char *result;
    int comment = 0;
    int avail = 0;
    // start by printing the poll name
    size += strlen(poll_name)+1;

    // add the slot labels each on a separate line
    int label;
    for (label=0; label < poll->num_slots; label++) {
        size += strlen(poll->slot_labels[label])+1;
    }
    if(poll->comment){
        comment += strlen(poll->comment)+1;
    }
    if(poll->avail){
        avail += strlen(poll->avail)+1;
    }
    size += comment;
    size += avail;
    result = Malloc(size);
    strcat(result, poll_name);
    strcat(result, " ");
    for (label=0; label < poll->num_slots; label++) {
        strcat(result, poll->slot_labels[label]);
        strcat(result, " ");
    }
    strcat(result, "\n");
    if(comment != 0){
        strcat(result, poll->comment);
        strcat(result, "\n");
    }
    if(avail != 0){
        strcat(result, poll->avail);
        strcat(result, "\n");
    }
    free(null);
    return result;
}


int vote(char *poll_name, Poll *head_ptr, char *avail){

     Poll *current = find_poll(poll_name, head_ptr);

     if(current == NULL){
        return 1;
     }
     if(current->num_slots == strlen(avail)){
        free(current->avail);
        current->avail = Malloc(strlen(avail)+1);
        strcpy(current->avail, avail);
        return 0;
     }else{
         return 1;
     }
}


int comment(char *poll_name, Poll *head_ptr, char *comment){

    Poll *current = find_poll(poll_name, head_ptr);

     if(current == NULL){
        return 1;
     }

     free(current->comment);
     current->comment = Malloc(strlen(comment)+1);
     strcpy(current->comment, comment);
     return 0;
}

char * process_args(int cmd_argc, char **cmd_argv, Poll **poll_list_ptr) {

    // some commands need the poll_list head and others
    // require the address of that head
    Poll *poll_list = *poll_list_ptr; 
    char *ans5 = malloc(4);
    strcpy(ans5, "sus");
    char *ans4 = malloc(5);
    strcpy(ans4, "NULL");

    if (strcmp(cmd_argv[0], "list_polls") == 0 && cmd_argc == 1) {
        char *buf;
        buf = print_polls(poll_list);
        return buf;
    } else if (strcmp(cmd_argv[0], "create_poll") == 0 && cmd_argc >= 3) {
        int label_count = cmd_argc - 2;
        int result = create_poll(cmd_argv[1], &cmd_argv[2], label_count,
                     poll_list_ptr);
        if (result == 1) {
            char *ans = malloc(13);
            strcpy(ans, "Poll exists");
            return ans;
        }
        
        return ans5;

   } else if (strcmp(cmd_argv[0], "vote") == 0 && cmd_argc == 3) {

        char *poll_name = cmd_argv[1];        // better name for clarity of code below
        // try to add participant to this poll  
        int ans6 = vote(poll_name, poll_list, cmd_argv[2]);
        if(ans6 != 1){
           return ans5;
        }else{
            char *ans7 = Malloc(20);
            strcpy(ans7, "Length Not Match\n");
            return ans7;
        }
        
        

    } else if (strcmp(cmd_argv[0], "delete_poll") == 0 && cmd_argc == 2) {
        if (delete_poll(cmd_argv[1], poll_list_ptr) == 1) {
            char *ans2 = malloc(15);
            strcpy(ans2, "No poll exists");
            return ans2;
        }
        return ans5;

    } else if (strcmp(cmd_argv[0], "poll_info") == 0 && cmd_argc == 2) {
        char *buffer;
        buffer = print_poll_info(cmd_argv[1], poll_list);
        return buffer;

    } else if(strcmp(cmd_argv[0], "comment") == 0 && cmd_argc >= 3){
        
        int space_needed = 0;
        int i;
        for (i=2; i<cmd_argc; i++) {
            space_needed += strlen(cmd_argv[i]) + 1;
        }

        char *comment_1 = malloc(space_needed);
        if (comment_1 == NULL) {
            perror("malloc");
            exit(1);
        }

        strcpy(comment_1, cmd_argv[3]);
        for (i=4; i<cmd_argc; i++) {
            strcat(comment_1, " ");
            strcat(comment_1, cmd_argv[i]);
        }

        comment(cmd_argv[1], poll_list, comment_1);
        return ans5;
        
    }else{
        char *ans1 = malloc(20);
        strcpy(ans1, "Incorrect syntax\n");
        return ans1;
    }
    return ans4;
}
