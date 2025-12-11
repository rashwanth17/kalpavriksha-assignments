#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_PROCESSES 100
#define MAX_PROCESS_NAME_LENGTH 50
#define MAX_QUEUE_SIZE 100

typedef enum
{
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED,
    KILLED
} ProcessState;

typedef struct
{
    int pid;
    char name[MAX_PROCESS_NAME_LENGTH];
    int cpu_burst;
    int remaining_burst;
    int io_start;
    int io_duration;
    int io_remaining;
    ProcessState state;
    int arrival_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
    int io_time;
} PCB;

typedef struct
{
    PCB* processes[MAX_QUEUE_SIZE];
    int front, rear;
    int size;
} Queue;

void initialize_queue(Queue* queue)
{
    queue->front=0;
    queue->rear=-1;
    queue->size=0;
}

int is_queue_empty(Queue* queue)
{
    return queue->size==0;
}

int is_queue_full(Queue* queue)
{
    return queue->size==MAX_QUEUE_SIZE;
}

void enqueue(Queue* queue, PCB* process)
{
    if(is_queue_full(queue))
    {
        printf("Queue is full\n");
        return;
    }
    queue->rear=(queue->rear+1)%MAX_QUEUE_SIZE;
    queue->processes[queue->rear]=process;
    queue->size++;
}

PCB* dequeue(Queue* queue)
{
    if(is_queue_empty(queue))
    {
        return NULL;
    }
    PCB* process=queue->processes[queue->front];
    queue->front=(queue->front+1)%MAX_QUEUE_SIZE;
    queue->size--;
    return process;
}

PCB* create_process(int pid, const char* name, int cpu_burst, int io_start, int io_duration)
{
    PCB* process=(PCB*)malloc(sizeof(PCB));
    if(process==NULL)
    {
        perror("Failed to allocate memory for process");
        exit(EXIT_FAILURE);
    }
    
    process->pid=pid;
    strncpy(process->name, name, MAX_PROCESS_NAME_LENGTH-1);
    process->name[MAX_PROCESS_NAME_LENGTH-1]='\0';
    process->cpu_burst=cpu_burst;
    process->remaining_burst=cpu_burst;
    process->io_start=io_start;
    process->io_duration=io_duration;
    process->io_remaining=0;
    process->state=NEW;
    process->arrival_time=0;
    process->completion_time=-1;
    process->waiting_time=0;
    process->turnaround_time=0;
    process->io_time=0;
    
    return process;
}

bool should_kill_process(int pid, int current_time, int kill_events[][2], int kill_count)
{
    for(int i=0; i<kill_count; i++)
    {
        if(kill_events[i][0]==pid && kill_events[i][1]==current_time)
        {
            return true;
        }
    }
    return false;
}

void schedule_processes(PCB* processes[], int process_count, int kill_events[][2], int kill_count)
{
    Queue ready_queue, waiting_queue, terminated_queue;
    initialize_queue(&ready_queue);
    initialize_queue(&waiting_queue);
    initialize_queue(&terminated_queue);
    
    for(int i=0; i<process_count; i++)
    {
        processes[i]->state=READY;
        enqueue(&ready_queue, processes[i]);
    }
    
    int current_time=0;
    PCB* current_process=NULL;
    int completed_processes=0;
    
    while(completed_processes<process_count)
    {
        if(!is_queue_empty(&waiting_queue))
        {
            int waiting_count=waiting_queue.size;
            for(int i=0; i<waiting_count; i++)
            {
                PCB* p=dequeue(&waiting_queue);
                p->io_remaining--;
                
                if(p->io_remaining<=0)
                {
                    p->state=READY;
                    enqueue(&ready_queue, p);
                }
                else
                {
                    enqueue(&waiting_queue, p);
                }
            }
        }
        
        if(current_process==NULL && !is_queue_empty(&ready_queue))
        {
            current_process=dequeue(&ready_queue);
            current_process->state=RUNNING;
        }
        
        if(current_process!=NULL)
        {
            if(should_kill_process(current_process->pid, current_time, kill_events, kill_count))
            {
                current_process->state=KILLED;
                current_process->completion_time=current_time;
                current_process->turnaround_time=current_process->completion_time-current_process->arrival_time;
                current_process->waiting_time=current_process->turnaround_time-
                                           (current_process->cpu_burst-current_process->remaining_burst)-
                                           (current_process->io_duration-current_process->io_remaining);
                enqueue(&terminated_queue, current_process);
                current_process=NULL;
                completed_processes++;
                current_time++;
                continue;
            }
            
            current_process->remaining_burst--;
            
            if(current_process->remaining_burst<=0)
            {
                current_process->state=TERMINATED;
                current_process->completion_time=current_time+1;
                current_process->turnaround_time=current_process->completion_time-current_process->arrival_time;
                current_process->waiting_time=current_process->turnaround_time-
                                           current_process->cpu_burst-
                                           (current_process->io_duration-current_process->io_remaining);
                enqueue(&terminated_queue, current_process);
                current_process=NULL;
                completed_processes++;
            }
            else if(current_process->cpu_burst-current_process->remaining_burst==current_process->io_start)
            {
                current_process->state=WAITING;
                current_process->io_remaining=current_process->io_duration;
                enqueue(&waiting_queue, current_process);
                current_process=NULL;
            }
        }
        
        int ready_count=ready_queue.size;
        for(int i=0; i<ready_count; i++)
        {
            PCB* p=dequeue(&ready_queue);
            if(p->state==READY)
            {
                p->waiting_time++;
            }
            enqueue(&ready_queue, p);
        }
        
        current_time++;
    }
}

void print_process_table(PCB* processes[], int process_count)
{
    printf("\n%5s %-15s %8s %8s %15s %12s %12s\n", 
           "PID", "Name", "CPU", "I/O", "Status", "Turnaround", "Waiting");
    printf("-------------------------------------------------------------\n");
    
    for(int i=0; i<process_count; i++)
    {
        PCB* p=processes[i];
        char status[20];
        
        if(p->state==KILLED)
        {
            snprintf(status, sizeof(status), "KILLED at %d", p->completion_time);
        }
        else
        {
            strcpy(status, "OK");
        }
        
        printf("%5d %-15s %8d %8d %15s %12d %12d\n", 
               p->pid, p->name, p->cpu_burst, p->io_duration, 
               status, p->turnaround_time, p->waiting_time);
    }
}

int main()
{
    printf("FCFS Process Scheduler Simulator\n");
    printf("Enter process details (name pid cpu_burst io_start io_duration), one per line\n");
    printf("Enter 'DONE' when finished entering processes\n");
    printf("Example: chrome 101 10 4 3\n");
    
    PCB* processes[MAX_PROCESSES];
    int process_count=0;
    char input[256];
    int kill_events[MAX_PROCESSES][2];
    int kill_count=0;
    
    while(process_count<MAX_PROCESSES)
    {
        printf("> ");
        if(fgets(input, sizeof(input), stdin)==NULL)
        {
            break;
        }
        
        if(strncmp(input, "DONE", 4)==0)
        {
            break;
        }
        else if(strncmp(input, "KILL", 4)==0)
        {
            int pid, time;
            if(sscanf(input+4, "%d %d", &pid, &time)==2)
            {
                kill_events[kill_count][0]=pid;
                kill_events[kill_count][1]=time;
                kill_count++;
            }
            else
            {
                printf("Invalid KILL command. Use: KILL pid time\n");
            }
            continue;
        }
        
        char name[50];
        int pid, cpu_burst, io_start=-1, io_duration=0;
        
        if(sscanf(input, "%s %d %d %d %d", name, &pid, &cpu_burst, &io_start, &io_duration)>=3)
        {
            if(cpu_burst<=0)
            {
                printf("Error: CPU burst must be positive\n");
                continue;
            }
            if(io_start<0)
            {
                io_start=-1;
                io_duration=0;
            }
            else if(io_duration<=0)
            {
                printf("Error: I/O duration must be positive\n");
                continue;
            }
            
            processes[process_count]=create_process(pid, name, cpu_burst, io_start, io_duration);
            process_count++;
        }
        else
        {
            printf("Invalid input. Use: name pid cpu_burst [io_start io_duration]\n");
        }
    }
    
    if(process_count==0)
    {
        printf("No processes to schedule.\n");
        return 0;
    }
    
    schedule_processes(processes, process_count, kill_events, kill_count);
    print_process_table(processes, process_count);
    
    for(int i=0; i<process_count; i++)
    {
        free(processes[i]);
    }
    
    return 0;
}