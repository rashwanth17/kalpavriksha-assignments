#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

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
}ProcessState;

typedef struct
{
    char name[MAX_PROCESS_NAME_LENGTH];
    int pid;
    int arrival_time;
    int cpu_burst;
    int remaining_burst;
    int io_start;
    int io_duration;
    int io_remaining;
    int waiting_time;
    int turnaround_time;
    int completion_time;
    ProcessState state;
}PCB;

typedef struct
{
    PCB* items[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
}Queue;

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

void enqueue(Queue* queue,PCB* process)
{
    if(is_queue_full(queue))
    {
        return;
    }
    queue->rear=(queue->rear+1)%MAX_QUEUE_SIZE;
    queue->items[queue->rear]=process;
    queue->size++;
}

PCB* dequeue(Queue* queue)
{
    if(is_queue_empty(queue))
    {
        return NULL;
    }
    PCB* process=queue->items[queue->front];
    queue->front=(queue->front+1)%MAX_QUEUE_SIZE;
    queue->size--;
    return process;
}

PCB* create_process(char* name,int pid,int cpu_burst,int io_start,int io_duration)
{
    PCB* process=(PCB*)malloc(sizeof(PCB));
    if(process==NULL)
    {
        printf("Memory allocation failed for process %s\n",name);
        exit(1);
    }

    strncpy(process->name,name,MAX_PROCESS_NAME_LENGTH-1);
    process->name[MAX_PROCESS_NAME_LENGTH-1]='\0';

    process->pid=pid;
    process->arrival_time=0;
    process->cpu_burst=cpu_burst;
    process->remaining_burst=cpu_burst;
    process->io_start=io_start;
    process->io_duration=io_duration;
    process->io_remaining=0;
    process->waiting_time=0;
    process->turnaround_time=0;
    process->completion_time=0;
    process->state=NEW;

    return process;
}

int should_kill_process(int pid,int current_time,int kill_events[][2],int kill_count)
{
    for(int i=0;i<kill_count;i++)
    {
        if(kill_events[i][0]==pid && kill_events[i][1]==current_time)
        {
            return 1;
        }
    }
    return 0;
}

void schedule_processes(PCB* processes[],int process_count,int kill_events[][2],int kill_count)
{
    Queue ready_queue;
    Queue waiting_queue;
    Queue terminated_queue;

    initialize_queue(&ready_queue);
    initialize_queue(&waiting_queue);
    initialize_queue(&terminated_queue);

    for(int i=0;i<process_count;i++)
    {
        processes[i]->state=READY;
        enqueue(&ready_queue,processes[i]);
    }

    int current_time=0;
    PCB* current_process=NULL;
    int completed_processes=0;

    while(completed_processes<process_count)
    {
        if(!is_queue_empty(&waiting_queue))
        {
            int waiting_count=waiting_queue.size;
            for(int i=0;i<waiting_count;i++)
            {
                PCB* p=dequeue(&waiting_queue);
                p->io_remaining--;

                if(p->io_remaining<=0)
                {
                    p->state=READY;
                    enqueue(&ready_queue,p);
                }
                else
                {
                    enqueue(&waiting_queue,p);
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
            if(should_kill_process(current_process->pid,current_time,kill_events,kill_count))
            {
                current_process->state=KILLED;
                current_process->completion_time=current_time;
                current_process->turnaround_time=current_process->completion_time-current_process->arrival_time;
                current_process->waiting_time=current_process->turnaround_time-
                                                (current_process->cpu_burst-current_process->remaining_burst)-
                                                (current_process->io_duration-current_process->io_remaining);
                enqueue(&terminated_queue,current_process);
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
                enqueue(&terminated_queue,current_process);
                current_process=NULL;
                completed_processes++;
            }
            else if(current_process->cpu_burst-current_process->remaining_burst==current_process->io_start &&
                    current_process->io_duration>0)
            {
                current_process->state=WAITING;
                current_process->io_remaining=current_process->io_duration;
                enqueue(&waiting_queue,current_process);
                current_process=NULL;
            }
        }

        int ready_count=ready_queue.size;
        for(int i=0;i<ready_count;i++)
        {
            PCB* p=dequeue(&ready_queue);
            if(p->state==READY)
            {
                p->waiting_time++;
            }
            enqueue(&ready_queue,p);
        }

        current_time++;
    }
}

void print_process_table(PCB* processes[],int process_count)
{
    printf("\nPID\tName\tCPU\tIO\tStatus\t\tTurnaround\tWaiting\n");
    for(int i=0;i<process_count;i++)
    {
        PCB* p=processes[i];
        printf("%d\t%s\t%d\t%d\t",p->pid,p->name,p->cpu_burst,p->io_duration);
        if(p->state==KILLED)
        {
            printf("KILLED at %d\t-\t\t-\n",p->completion_time);
        }
        else
        {
            printf("OK\t\t%d\t\t%d\n",p->turnaround_time,p->waiting_time);
        }
    }
}

int main()
{
    PCB* processes[MAX_PROCESSES];
    int kill_events[MAX_PROCESSES][2];
    int process_count=0;
    int kill_count=0;
    char input[256];

    printf("Enter process details (Name PID CPU_Burst IO_Start IO_Duration):\n");
    printf("Use -1 -1 if no IO (or just give first 3 fields).\n");
    printf("Enter 'DONE' when finished.\n");

    while(1)
    {
        if(fgets(input,sizeof(input),stdin)==NULL)
        {
            break;
        }

        if(strncmp(input,"DONE",4)==0)
        {
            break;
        }

        if(strlen(input)==0 || input[0]=='\n')
        {
            continue;
        }

        if(process_count>=MAX_PROCESSES)
        {
            printf("Maximum process limit reached.\n");
            break;
        }

        char name[64];
        int pid;
        int cpu_burst;
        int io_start;
        int io_duration;

        int scanned=sscanf(input,"%63s %d %d %d %d",name,&pid,&cpu_burst,&io_start,&io_duration);

        if(scanned<3)
        {
            printf("Invalid input. Format: Name PID CPU_Burst [IO_Start IO_Duration]\n");
            continue;
        }

        if(cpu_burst<=0)
        {
            printf("CPU_Burst must be positive.\n");
            continue;
        }

        if(scanned==3)
        {
            io_start=-1;
            io_duration=0;
        }

        if(io_start<0)
        {
            io_start=-1;
            io_duration=0;
        }
        else
        {
            if(io_start>=cpu_burst)
            {
                printf("IO_Start must be less than CPU_Burst.\n");
                continue;
            }
            if(io_duration<0)
            {
                printf("IO_Duration must be non-negative.\n");
                continue;
            }
        }

        processes[process_count]=create_process(name,pid,cpu_burst,io_start,io_duration);
        process_count++;
    }

    if(process_count==0)
    {
        printf("No processes entered. Exiting.\n");
        return 0;
    }

    printf("\nEnter KILL events (KILL PID Time) or 'DONE':\n");

    while(1)
    {
        if(fgets(input,sizeof(input),stdin)==NULL)
        {
            break;
        }

        if(strncmp(input,"DONE",4)==0)
        {
            break;
        }

        if(strlen(input)==0 || input[0]=='\n')
        {
            continue;
        }

        if(kill_count>=MAX_PROCESSES)
        {
            printf("Maximum KILL events limit reached.\n");
            break;
        }

        char cmd[16];
        int pid;
        int time;

        if(sscanf(input,"%15s %d %d",cmd,&pid,&time)==3 && strcmp(cmd,"KILL")==0)
        {
            if(time<0)
            {
                printf("Kill time must be non-negative.\n");
                continue;
            }
            kill_events[kill_count][0]=pid;
            kill_events[kill_count][1]=time;
            kill_count++;
        }
        else
        {
            printf("Invalid KILL input. Format: KILL PID Time\n");
        }
    }

    schedule_processes(processes,process_count,kill_events,kill_count);
    print_process_table(processes,process_count);

    for(int i=0;i<process_count;i++)
    {
        free(processes[i]);
    }

    return 0;
}
