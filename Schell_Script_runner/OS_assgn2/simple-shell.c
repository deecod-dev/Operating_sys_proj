#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<time.h>
#include<sys/time.h>

#define MAX_INPUT_SIZE 1024

struct cmd
{
    char* arg;
    int eles;
    struct tm st;
    struct timeval start;
    struct timeval end;
    unsigned long t;
    int cpid;
    struct cmd* next;
};
struct cmd* head=NULL;

void show_hist()
{
    if(head==NULL){return;}
    struct cmd* tmp=head;
    printf("\ncommand-arguments------\n");
    while(tmp!=NULL)
    {
        printf("    %s\n",tmp->arg);
        tmp=tmp->next;
    }
}
void show_hist_full()
{
    if(head==NULL){return;}
    struct cmd* tmp=head;
    printf("\nCtrl-C pressed...\n");
    printf("\ncommand-arguments----------------------------------------------------------------\n");
    while(tmp!=NULL)
    {
        printf("    %s      process-pid: %d      start-time: %d-%02d-%02d %02d:%02d:%02d      total-time: %luμsec\n", tmp->arg , tmp->cpid , tmp->st.tm_year + 1900, tmp->st.tm_mon + 1, tmp->st.tm_mday, tmp->st.tm_hour, tmp->st.tm_min, tmp->st.tm_sec , tmp->t);
        tmp=tmp->next;
    }
    printf("---------------------------------------------------------------------------------\n");
}


int create_process_and_run(char* command,int bckgrnd) {
    int status = fork();
    int status_child;

    if(status < 0) {
        printf("Fork failed\n");
    } else if(status == 0) {
        // child process
        // char* args[50];
        // int argCount = 0;

        // // Tokenize the command
        // char* token = strtok((char*)command, " ");
        // while (token != NULL){
        //     args[argCount] = token;
        //     token = strtok(NULL, " ");
        //     argCount++;
        // }
        // args[argCount] = NULL;

        // execute command
        execlp("/bin/sh", "sh", "-c", command, NULL);
        perror("error due to execlp");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process
        struct cmd* var=(struct cmd*)malloc(sizeof(struct cmd));
        if (var == NULL) {
            perror("Error allocating memory for struct cmd");
            exit(EXIT_FAILURE);
        }
        var->next=NULL;
        // printf("!!!!!!!!!!%s",command);
        var->arg = (char*)malloc(strlen(command) + 1);
        if (var->arg == NULL) {
            perror("Error allocating memory for argument");
            free(var); // Free the previously allocated memory for struct cmd
            exit(EXIT_FAILURE);
        }
        strcpy(var->arg, command);
        if (bckgrnd == 0) {
            int ret;
            if ((ret = waitpid(status, NULL, 0)) == -1) {
                perror("Error waiting for child process");
                free(var->arg); // Free memory allocated for argument
                free(var);      // Free memory allocated for struct cmd
                exit(EXIT_FAILURE);
            }
            if (WIFEXITED(ret)) {
                int exit_status = WEXITSTATUS(ret);
                // printf("\nretstat%d\n", exit_status);
            }
        }
        var->cpid=status;

        if(head==NULL)
        {
            head=var;
        }
        else
        {
            struct cmd *temp=head;
            while(temp!=NULL)
            {
                if(temp->next==NULL)
                {
                    temp->next=var;
                    break;
                }
                temp=temp->next;
            }
        }
    }
    return 0;
}
int create_process_and_run_term(char* command,int bckgrnd) {
    int status = fork();

    if (status < 0)
    {
        printf("Fork failed\n");
        return -1;
    }
    else if (status == 0)
    {
        // Child process
        int val=execlp("/bin/sh", "sh", "-c", command, NULL);
        if(val==-1)
        {
            printf("\nWrong command\n");
            exit(0);
        }
    }
    else
    {
        // Parent process
        time_t t = time(NULL);

        struct cmd* var=(struct cmd*)malloc(sizeof(struct cmd));
        if (var == NULL) {
            perror("Error allocating memory for struct cmd");
            exit(EXIT_FAILURE);
        }
        var->st=*localtime(&t);

        var->next=NULL;
        gettimeofday(&(var->start), NULL);
        var->arg = (char*)malloc(strlen(command) + 1);
        if (var->arg == NULL) {
            perror("Error allocating memory for argument");
            free(var); // Free the previously allocated memory for struct cmd
            exit(EXIT_FAILURE);
        }
        strcpy(var->arg, command);
        if(bckgrnd==0)
        {
            int ret;
            // waitpid(status,&ret,0);
            if (waitpid(status, &ret, 0) == -1) {
                perror("Error waiting for child process");
                exit(EXIT_FAILURE);
            }
            if(WIFEXITED(ret))
            {
                int exit_status = WEXITSTATUS(ret);
                // printf("\nretstat%d\n",exit_status);
            }
        }

        gettimeofday(&(var->end), NULL);
        var->t=(var->end.tv_sec - var->start.tv_sec) * 1000000 + var->end.tv_usec - var->start.tv_usec;
        var->cpid=status;

        if(head==NULL)
        {
            head=var;
        }
        else
        {
            struct cmd *temp=head;
            while(temp!=NULL)
            {
                if(temp->next==NULL)
                {
                    temp->next=var;
                    break;
                }
                temp=temp->next;
            }
        }
    }
    return 0;
}

int launch (char *command) {
    int status;
    int bckgrnd=0;
    // printf("\ncomms: %s    %ld\n",command,strlen(command));
    if(command[strlen(command)-1]=='&')
    {
        bckgrnd=1;
    }
    status = create_process_and_run(command,bckgrnd);
    return status;
}

int launch_term (char *command) {
    int status;
    int bckgrnd=0;
    // printf("\ncomms: %s    %ld\n",command,strlen(command));
    if(command[strlen(command)-1]=='&')
    {
        bckgrnd=1;
    }
    status = create_process_and_run_term(command,bckgrnd);
    return status;
}

char* read_command(FILE* file) {
    char* input = NULL;
    size_t input_size = 0;

    // Read a line from the file
    ssize_t bytes_read = getline(&input, &input_size, file);

    if (bytes_read == -1) {
        free(input); // Free memory allocated by getline
        return NULL; // End of file
    }

    // Remove the trailing newline character
    if (input[bytes_read - 1] == '\n') {
        input[bytes_read - 1] = '\0';
    }

    return input;
}
char* read_command_term()
{
    char* input = NULL;
    size_t input_size = 0;

    // Read user input using getline()
    ssize_t bytes_read = getline(&input, &input_size, stdin);
    
    if (bytes_read == -1)
    {
        perror("Error in reading line");//byme changed string
        exit(EXIT_FAILURE);
    }

    // Remove the trailing newline character
    if (input[bytes_read - 1] == '\n')
    {
        input[bytes_read - 1] = '\0';
    }
    return input;
}

void shell_loop(FILE* script_file) {
    int status;
    while (1) {
        char* command = read_command(script_file);
        // printf("command:  %s\n",command);
        if (command == NULL) {
            break; // End of file
        }
        if(strcmp("history",command)==0)
        {
            show_hist();
        }
        else
        {
            status = launch(command);
        }
        free(command); // Free memory allocated by read_command_from_file
    }
}

void shell_loop_term()
{
    int status;
    while(1)
    {
        printf("> ");
        char* command = read_command_term();
        // printf("command:  %s\n",command);
        if(strcmp("history",command)==0)
        {
            show_hist();
        }
        else
        {
            status = launch_term(command);
        }
    }
}

void freelist(struct cmd* tmp)
{
    if(tmp==NULL)
    {
        return;
    }
    freelist(tmp->next);
    free(tmp->arg);
    free(tmp);
}
void handle_sigint(int signum) {
    show_hist_full();
    freelist(head);
    printf("\nLinked List freed\n");
    exit(EXIT_SUCCESS);
}
int main(int argc, char* argv[]) {
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    
    if (argc != 2) {
        // fprintf(stderr, "Usage: %s <script_file>\n", argv[0]);
        printf("\nStarting terminal...\n");
        shell_loop_term();
        return 1;
    }

    // Open the shell script file for reading
    FILE* script_file = fopen(argv[1], "r");
    if (script_file == NULL) {
        perror("Unable to open script file");
        return 1;
    }

    shell_loop(script_file);

    fclose(script_file); // Close the script file
    return 0;
}