#include <stdio.h>
#include <elf.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/mman.h>
#include <semaphore.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX_INPUT_SIZE 1024
#define MAX_PRIORITY 10
#define DEFAULT_PRIORITY 1

// Modify the Process structure to include priority
struct process {
    char* command;
    int pid;
    int priority; // Priority of the process
    struct process* next;
    int st;
};

// Function to get the priority from user input or use the default priority
int get_priority(char* command) {
    int priority = DEFAULT_PRIORITY;
    char* priority_flag = strstr(command, "-p ");
    if (priority_flag != NULL) {
        priority_flag += 3; // Skip "-p "
        priority = atoi(priority_flag);
        if (priority < 1 || priority > MAX_PRIORITY) {
            printf("Invalid priority value. Using default priority %d.\n", DEFAULT_PRIORITY);
            priority = DEFAULT_PRIORITY;
        }
    }
    return priority;
}

int NCPU, TSLICE;

struct shared_memory
{
    struct process* front;
    struct process* rear;
    int np;
    sem_t scheduler_sem;
};

struct shared_memory* q;

void initialize_queue()
{
    q->front = NULL;
    q->rear = NULL;
    q->np = 0;
    sem_init(&(q->scheduler_sem), 0, 0);
}

int isEmpty()
{
    return (q->front == NULL);
}

void enqueue( char* command)
{

    struct process* p = (struct process*)malloc(sizeof(struct process));
    int priority = get_priority(command);
    p->command=command;
    // p->command = strdup(command);  // Duplicate the command string
    p->next = NULL;
    p->st=0;
    p->priority = priority;

    if (isEmpty(q)) {
        q->front = p;
        q->rear = p;
        
    } else {
        // Find the appropriate position based on priority
        struct process* current = q->front;
        struct process* previous = NULL;
        while (current != NULL && priority <= current->priority) {
            previous = current;
            current = current->next;
        }
        if (previous == NULL) {
            // The new process has the highest priority
            p->next = q->front;
            q->front = p;
        } else {
            previous->next = p;
            p->next = current;
            if (current == NULL) {
                q->rear = p;
            }
        }
    }
    q->np += 1;
}

void rem(int pid)
{
    struct process* current = q->front;
    struct process* previous = NULL;

    while (current != NULL && current->pid != pid)
    {
        previous = current;
        current = current->next;
    }

    if (current == NULL)
    {
        printf("Process with PID %d not found.\n", pid);
        return;
    }

    if (previous == NULL)
    {
        // The process to be removed is the front of the queue
        q->front = current->next;
        if (q->front == NULL)
        {
            // The queue is now empty
            q->rear = NULL;
        }
    }
    else
    {
        previous->next = current->next;
        if (current == q->rear) {
            // The process to be removed is the rear of the queue
            q->rear = previous;
        }
    }
    q->np--;
    free(current->command);
    free(current); // Free the memory of the removed process

}
void print_queue() {
    struct process* current = q->front;

        int swapped;
        struct process* ptr1;
        struct process* lptr = NULL;
        do {
            swapped = 1;
            ptr1 = q->front;
            while (ptr1->next != lptr) {
                if (ptr1->priority > ptr1->next->priority) {
                    // Swap the processes for ascending priority
                    char* tmp_command = ptr1->command;
                    int tmp_priority = ptr1->priority;
                    int tmp_st = ptr1->st;
                    ptr1->command = ptr1->next->command;
                    ptr1->priority = ptr1->next->priority;
                    ptr1->st = ptr1->next->st;
                    ptr1->next->command = tmp_command;
                    ptr1->next->priority = tmp_priority;
                    ptr1->next->st = tmp_st;
                    swapped = 0;
                }
                ptr1 = ptr1->next;
            }
            lptr = ptr1;
        } while (!swapped);

    if (isEmpty(q)) {
        printf("Queue is empty.\n");
        return;
    }

    printf("Queue contents:\n");
    while (current != NULL) {
        printf("PID: %d, Command: %s\n", current->pid, current->command);
        current = current->next;
    }
}

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

struct cmd* head = NULL;

int create_process_and_run_term(char* command, int bckgrnd)
{
    int status = fork();

    if (status < 0)
    {
        printf("Fork failed\n");
        return -1;
    }
    else if (status == 0)
    {
        // Child process
        int val = execlp("/bin/sh", "sh", "-c", command, NULL);
        if (val == -1)
        {
            printf("\nWrong command\n");
            exit(0);
        }
    }
    else
    {
        // Parent process
        // if (bckgrnd == 0)
        // {
        //     int ret;
        //     if (waitpid(status, &ret, 0) == -1)
        //     {
        //         perror("Error waiting for child process");
        //         exit(EXIT_FAILURE);
        //     }
        //     if (WIFEXITED(ret))
        //     {
        //         int exit_status = WEXITSTATUS(ret);
        //     }
        // }
    }
    return status;
}

int launch_term(char* command)
{
    int status;
    int bckgrnd = 0;
    status = create_process_and_run_term(command, 1);
    return status;
}

char* read_command_term() {
    char* input = (char*)malloc(MAX_INPUT_SIZE * sizeof(char));  // Allocate memory
    if (input == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // Clear the input buffer
    // printf("clearing buffer\n");
    // int c;
    // while ((c = getchar()) != '\n' && c != EOF);
    // printf("cleared buffer\n");

    // Use fgets to read a line of input
    if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
        perror("Error in reading line");
        exit(EXIT_FAILURE);
    }

    // Remove the trailing newline character if present
    size_t length = strlen(input);
    if (length > 0 && input[length - 1] == '\n') {
        input[length - 1] = '\0';
    }

    printf("inpsent-%s\n", input);
    return input;
}


void shiftProcess() {
    if (isEmpty(q) || q->front == q->rear) {
        printf("Queue is empty or contains only one process. No shift performed.\n");
        return;
    }

    // Remove the front process and place it at the rear
    struct process* frontProcess = q->front;
    q->front = frontProcess->next;
    frontProcess->next = NULL;

    q->rear->next = frontProcess;
    q->rear = frontProcess;

    printf("Shifted process with PID %d to the back of the queue.\n", frontProcess->pid);
}


void start_scheduler()
{
    printf("\n--------------------scheduler%d----------------",getpid());
    int status;
    while (1)
    {
        // Wait for the semaphore (scheduler will be blocked if the queue is empty)
        // printf("semaph bef schd-%d\n",q->scheduler_sem);
        if(q->np==0)
        {
            printf("semaph waiting for queue-%p\n",(void*)&q->scheduler_sem);
            sem_wait(&(q->scheduler_sem));
            printf("queue done-%p\n",(void*)&q->scheduler_sem);
        }
        // printf("semaph aft schd-%d\n",q->scheduler_sem);
        printf("scheduler started\n");

        int swapped;
        struct process* ptr1;
        struct process* lptr = NULL;

        // Return if the queue is empty
        if (isEmpty(q)) {
            return;
        }

        while (!isEmpty(q))
        {
            struct process* head = q->front;
            // Run NCPU processes for TSLICE seconds
            for (int i = 0; i < NCPU && head != NULL; i++)
            {
                if (head->st == 0) //never ran it before
                {
                    // printf("com in schd never run before: %s\n", head->command);
                    status = create_process_and_run_term(head->command, 1);
                    printf("starting process-%d,pid-%d\n", i, status);
                    head->pid = status;
                    head->st = 1;
                }
                else //ran it earlier=> continue it now
                {
                    printf("continuing process-%d\n", head->pid);
                    int result = kill(head->pid, SIGCONT);
                    if (result == -1)
                    {
                        printf("Error resuming process\n");
                        exit(0);
                    }
                }
                head = head->next;
            }
            printf("\nqueue bef sleep--------------------%d\n",q->np);
            print_queue(q);
            sem_post(&(q->scheduler_sem));
            sleep(TSLICE); // Pause for TSLICE seconds
            sem_wait(&(q->scheduler_sem));
            printf("\nqueue after sleep------------------%d\n",q->np);
            print_queue(q);

            head = q->front;
            //pause them now
            for (int i = 0; i < NCPU && head != NULL; i++)
            {
                printf("\n%d processes process %d has st %d\n",q->np,i,head->st);
                if(head->st!=0)
                {
                    int result = waitpid(head->pid, &status, WNOHANG);
                    if (result == 0) //process still running
                    {
                        printf("pausing process-%d\n", head->pid);
                        int result = kill(head->pid, SIGSTOP);
                        if (result == -1)
                        {
                            printf("Error pausing process\n");
                            exit(0);
                        }
                    }
                    else if (result == head->pid) //completed
                    {
                        rem(head->pid);
                        printf("process removed\n");
                    }
                    else if (result == -1 && errno == ECHILD)
                    {
                        printf("No child process with PID %d found.\n", head->pid);
                    }
                    else
                    {
                        printf("Error in waitpid\n");
                    }
                }
                head = head->next;
            }

            // Shift one process to the back of the queue for each CPU
            for (int i = 0; i < MIN(NCPU, q->np); i++)
            {
                if (q->np > 1) // Ensure there's more than one process in the queue to shift
                {
                    shiftProcess(q);
                }
            }
            print_queue(q);
            sem_post(&(q->scheduler_sem));
        }
        printf("Queue is empty. Waiting for new elements...\n");
    }
}



void shell_loop_term()
{
    int status;
    while (1)
    {
        printf("> ");
        char* command = read_command_term();
        // if (strcmp("history", command) == 0)
        // {
        //     // show_hist(); // Implement this if needed
        // }
        if (strcmp("startsch", command) == 0)//will be called only once per code run
        {
            status = fork();
            if (status < 0)
            {
                printf("scheduler didn't start");
            }
            else if (status == 0)
            {
                //child
                start_scheduler(q);
                printf("scheduler ended");
            }
        }
        else if(strcmp("pqueue", command) == 0)
        {
            print_queue(q);
            // Signal the semaphore to resume the scheduler
            // sem_post(&(q->scheduler_sem));
        }
        else
        {
            char* com = (char*)malloc((strlen(command) - 6) * sizeof(char));
            for (int i = 7; i < strlen(command); i++)
            {
                com[i - 7] = command[i];
            }
            com[strlen(command) - 7] = '\0';
            enqueue(com);
            // Signal the semaphore when a new element is enqueued
            // printf("semaph bef subm-%d\n",q->scheduler_sem);
            sem_post(&(q->scheduler_sem));
            // printf("semaph aft subm-%d\n",q->scheduler_sem);
        }
        // printf("aaa\n");
    }
}

// void freelist(struct cmd* tmp)
// {
//     if (tmp == NULL)
//     {
//         return;
//     }
//     freelist(tmp->next);
//     free(tmp->arg);
//     free(tmp);
// }

// void handle_sigint(int signum)
// {
//     // show_hist_full(); // Implement this if needed
//     freelist(head);
//     printf("\nLinked List freed\n");
//     exit(EXIT_SUCCESS);
// }

int main(int argc, char* argv[]) {
    printf("Enter the number of cpu's:");
    scanf("%d", &NCPU);
    printf("Enter the time quantum:");
    scanf("%d", &TSLICE);

    printf("clearing buffer\n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf("cleared buffer\n");

    int shm_fd = shm_open("shmfile", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
     // Size of shared memory
    int shm_size = sizeof(struct shared_memory);

    // Set the size of the shared memory
    ftruncate(shm_fd, shm_size);

    // Map the shared memory
    q = (struct shared_memory*)mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (q == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    initialize_queue(q);

    // struct sigaction sa;
    // sa.sa_handler = handle_sigint;
    // sa.sa_flags = 0;
    // sigemptyset(&sa.sa_mask);
    // sigaction(SIGINT, &sa, NULL);

    if (argc != 2) {
        printf("\nStarting terminal...\n");
        shell_loop_term(q);
        munmap(q, shm_size);
        close(shm_fd);

        // Remove the shared memory object
        shm_unlink("shmfile");
        return 1;
    }
    return 0;
}