/**
 * Operating Systems
 * Design and Implementation of a Scheduler Simulator
 * Process Scheduling
 *
 * @authors	Saja Fawagreh
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

// Defines a structure for the Process Control Block (PCB) with the necessary information
typedef struct pcb {	
    int pid;                     // Process ID
    int arrival_time;            // Arrival time of the process
    int total_CPU_time;          // Total CPU time required by the process
    int remaining_CPU_time;      // Remaining CPU time for the process
    int running_time;            // Time the process has spent in the running state
    int io_frequency;            // Frequency of I/O operations
    int io_duration;             // Duration of each I/O operation
    int waiting_time;            // Total time the process has spent waiting in the waiting state
    int priority;                // Priority of the process
    int start_timer_time;        // Start time when process is moved to ready state due to a timer interrupt
    int end_timer_time;          // End time when process is moved out of ready state due to a timer interrupt
    bool timer_interrupt;        // Flag indicating a timer interrupt
    int start_io_time;           // Start time when process is moved to ready state due to an I/O interrupt
    int end_io_time;             // End time when process is moved out of ready state due to an I/O interrupt
    bool io_interrupt;           // Flag indicating an I/O interrupt
    int ready_waiting_time;      // Total time the process has spent waiting in the ready state
    int start_time;              // Start time when the process started executing
    int end_time;                // End time of the process
} pcb_t;


// Defines a structure for a node to link the PCBs
typedef struct node {	
    pcb_t *pcb;
    struct node *next;
} node_t;

// Defines a structure for the queue
typedef struct {  
    node_t *front;
    node_t *rear;
    int size;
} queue_t;

// This function allocates a queue on the heap and returns a pointer to it
queue_t *alloc_queue(void) {
    queue_t *queue = malloc(sizeof(queue_t));  
    assert(queue != NULL);
    queue->front = NULL;  
    queue->rear = NULL;  
    queue->size = 0;  
    return queue;
}

/**
 * This function allocates a PCB structure on the heap and returns a pointer to it.
 *  
 * @param pid            The process ID of the PCB.
 * @param arrival_time   The arrival time of the PCB at the ready.
 * @param total_CPU_time The total CPU time of the PCB.
 * @param io_freq        The I/O frequency of the PCB.
 * @param io_dur         The I/O duration of the PCB.
 * @param priority       The priority of the PCB.
 * @return               A pointer to the PCB structure created.
 */
pcb_t *new_pcb(int pid, int arrival_time, int total_CPU_time, int io_freq, int io_dur, int priority) {
    pcb_t *pcb = malloc(sizeof(pcb_t));
    assert(pcb != NULL);
    pcb->pid = pid;
    pcb->arrival_time = arrival_time;
    pcb->total_CPU_time = total_CPU_time;
    pcb->remaining_CPU_time = total_CPU_time;
    pcb->running_time = 0;
    pcb->io_frequency = io_freq;
    pcb->io_duration = io_dur;
    pcb->waiting_time = 0;
    pcb->priority = priority;
    pcb->start_timer_time = 0;
    pcb->end_timer_time = 0;
    pcb->timer_interrupt = false;
    pcb->start_io_time = 0;
    pcb->end_io_time = 0;
    pcb->io_interrupt = false;
    pcb->ready_waiting_time = 0;
    pcb->start_time = -1;
    pcb->end_time = 0;
    return pcb;
}

/** 
 * This function enqueues (appends) a node to the end of the queue
 * 
 * @param queue   A pointer to the queue you want to enqueue a node to.
 * @param pcb     A pointer to the node you want to enqueue to the queue
 */ 
void enqueue(queue_t *queue, node_t *pcb)
{
	assert(queue!=NULL);
	if(queue->size == 0){
		queue->front = pcb;
	}
	else{
		queue->rear->next = pcb;
	}
    queue->rear = pcb;
	queue->size++;
}

/** 
 * This function dequeues (removes) the node from the front of the queue and returns a pointer to it.
 * 
 * @param queue   A pointer to the queue you want to dequeue from.
 * @return        A pointer to the dequeued node, or NULL if the queue is empty.
 */
node_t *dequeue(queue_t *queue)
{
    assert(queue!=NULL);
    assert(queue->size!=0);
    node_t *r = queue->front;
    queue->front = queue->front->next;
    if(queue->size == 1){
        queue->rear = NULL;  
    } 
	queue->size--;
	return r;
}

/** 
 * This function dequeues (removes) a specific node from the queue and returns a pointer to it.
 * 
 * @param queue          A pointer to the queue you want to dequeue the node from.
 * @param nodeToRemove   A pointer to the node you want to remove.
 * @return               A pointer to the removed node, or NULL if the node is not found or the queue is empty.
 */
node_t *dequeueNode(queue_t *queue, node_t *nodeToRemove) {
    assert(queue != NULL);
    assert(queue->size!=0);

    node_t *current = queue->front;
    node_t *prev = NULL;

    while (current != NULL) {
        if (current == nodeToRemove) {
            if (prev == NULL) {
                queue->front = current->next;
            } else {
                prev->next = current->next;
            }
            if (current == queue->rear) {
                queue->rear = prev;
            }
            queue->size--;
            current->next = NULL; 
            return current;
        }
        prev = current;
        current = current->next;
    }

    return NULL;
}

/** 
 * This function scans a CSV file, creates a PCB structure for each entry, enqueues them into a queue, and returns a pointer to the queue.
 * 
 * @param filename   The name of the input CSV file.
 * @return           A pointer to the queue containing PCB structures.
 */
queue_t *read_CSV_file(const char filename[]){
    pcb_t *pcb;
    node_t *p;
    queue_t *queue = alloc_queue();
    int pid, arrivalTime, totalCpuTime, ioFrequency, ioDuration, priority;

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Failed to open the file.\n");
        return NULL;
    }

    // Create a character buffer to store input
    char buffer[1024];
    // Read a line of text from file and store it in buffer
    fgets(buffer, sizeof(buffer), file);

    // Read data from the file in a loop as long as fscanf is able to extract all five integers from the line
    while (fscanf(file, "%d,%d,%d,%d,%d,%d", &pid, &arrivalTime, &totalCpuTime, &ioFrequency, &ioDuration, &priority) == 6) {
        p = malloc(sizeof(node_t));
        pcb = new_pcb(pid, arrivalTime, totalCpuTime, ioFrequency, ioDuration, priority); // Create a new PCB structure with the extracted values
        p->pcb = pcb; // Assigns the PCB value to PCB structure created
        p->next = NULL; // Assigns the next value to NULL since it will be the last element in the queue
        enqueue(queue, p); // Enqueue the node containing the PCB into the queue
    }

    fclose(file);

    return queue;
}

/**
 * This function writes the provided information for a PCB to the output file.
 *
 * @param filename   The name of the output file.
 * @param time       The time at which the PCB switches states.
 * @param pid        The process ID (PID).
 * @param oldState   The old state of the PCB.
 * @param newState   The new state of the PCB.
 */
void write_text_file(const char *filename, int time, int pid, const char *oldState, const char *newState) {
    FILE *file = fopen(filename, "a");

    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    fprintf(file, "%-5d %-4d %-10s %-10s\n", time, pid, oldState, newState);

    fclose(file);
}

/**
 * This function simulates an OS kernel, processing PCB state transitions.
 * 
 * @param new_queue    A pointer to the queue containing PCBs to process.
 * @param filename     The name of the output file for logging state transitions.
 * 
 * @return a pointer to the terminated queue.
 */
queue_t *flow_process(queue_t *new_queue, const char *filename, const char *algorithm){
    queue_t *ready_queue = alloc_queue();
    queue_t *running_queue = alloc_queue();
    queue_t *waiting_queue = alloc_queue();
    queue_t *terminated_queue = alloc_queue();

    node_t *current;             // A pointer to the current node in the loop
    node_t *removed;             // A pointer to the node being removed
    node_t* highest_priority;    // A pointer to the node with the highest priority PCB (if Priority scheduling is chosen)
    int size = new_queue->size;  // The number of PCBs, which is the size of the new_queue
    int clock = 0;               // The timer
    
    // The while loop won't stop until all PCBs are terminated.
    while(terminated_queue->size < size){

        // Check for PCBs in the new queue and transitions them to the ready state if their arrival time matches the current clock time
        if(new_queue->size != 0){
            current =  new_queue->front;
            int new_size = new_queue->size;

            for(int i = 0; i < new_size; i++) {
                if (current->pcb->arrival_time == clock) {
                    node_t *next = current->next;
                    removed = dequeue(new_queue);
                    enqueue(ready_queue, removed);
                    write_text_file(filename, clock, removed->pcb->pid, "New", "Ready");
                    current = next;
                }

                else{
                    current = current->next;
                }
            }
        }

        // Check for PCBs in the waiting queue and transition them to the ready state if their waiting time matches their I/O duration
        if(waiting_queue->size != 0){
            current = waiting_queue->front;
            int waiting_size = waiting_queue->size;

            for(int i = 0; i < waiting_size; i++) {
                
                current->pcb->waiting_time++;

                if (current->pcb->io_duration == current->pcb->waiting_time) {
                    current->pcb->waiting_time = 0;
                    node_t *next = current->next;
                    removed = dequeueNode(waiting_queue, current);
                    enqueue(ready_queue, removed);
                    removed->pcb->start_io_time = clock; // Save the current clock time when moving from waiting to ready
                    removed->pcb->io_interrupt = true;   // Set a flag to indicate that there was a system call to an I/O service
                    write_text_file(filename, clock, removed->pcb->pid, "Waiting", "Ready");
                    current = next;
                }
               
                else{
                    current = current->next;
                }
            } 
        } 

        // Checks the PCB in the running queue and transitions them to the terminated state if their remaining CPU time is 0 
        if(running_queue->size != 0){
            running_queue->front->pcb->running_time++;
            running_queue->front->pcb->remaining_CPU_time--;
            
            if (running_queue->front->pcb->remaining_CPU_time == 0) {
                running_queue->front->pcb->end_time = clock;
                removed = dequeue(running_queue);
                enqueue(terminated_queue, removed);
                write_text_file(filename, clock, removed->pcb->pid, "Running", "Terminated");
            }
        
            // Checks the PCB in the running queue and transitions them to the waiting queue if their I/O frequency matches the running time
            else if(running_queue->front->pcb->running_time == running_queue->front->pcb->io_frequency){
                removed = dequeue(running_queue);
                enqueue(waiting_queue, removed);
                write_text_file(filename, clock, removed->pcb->pid, "Running", "Waiting");      
            }

            // If the algorithm is RoundRobin, the PCB will transition from the running queue to the ready queue when the running time equals the time quantum
            else if(strcmp(algorithm, "RoundRobin") == 0){
                removed = dequeue(running_queue);
                enqueue(ready_queue, removed);
                removed->pcb->start_timer_time = clock; // Save the current clock time when moving from running to ready
                removed->pcb->timer_interrupt = true; // Set a flag to indicate a timer interrupt occurred
                write_text_file(filename, clock, removed->pcb->pid, "Running", "Ready");      
            }
        }

        // Checks the ready queue for PCBs and transitions the first PCB (or the highest priority PCB if the algorithm is Priority) in the ready queue to the running state if the running state is available
        if(ready_queue->size != 0){
            if (running_queue->size == 0) {
                highest_priority = ready_queue->front;
                // If the algorithm is Priority, the highest priority PCB is transitioned to the running state if the running state is available
                if(strcmp(algorithm, "Priority") == 0){
                    current = ready_queue->front;
                    int ready_size = ready_queue->size;
                    for(int i = 0; i < ready_size; i++) {
                        if(highest_priority->pcb->priority > current->pcb->priority){
                            highest_priority = current;
                        }
                        current = current->next;
                    }
                }

                // Check if the PCB has not started executing yet
                if (highest_priority->pcb->start_time == -1) {
                    highest_priority->pcb->start_time = clock; // Record the start time when the PCB begins execution
                }

                // Check if the PCB experienced an I/O interrupt
                if (highest_priority->pcb->io_interrupt) {
                    highest_priority->pcb->io_interrupt = false; // Reset the I/O interrupt flag
                    highest_priority->pcb->end_io_time = clock; // Record the end time of the I/O interrupt
                    highest_priority->pcb->ready_waiting_time += highest_priority->pcb->end_io_time - highest_priority->pcb->start_io_time; // Update the total waiting time in the ready state due to I/O interrupt
                    highest_priority->pcb->running_time = 0; // Reset the running time counter to check for other I/O calls
                }

                // Check if the PCB experienced a timer interrupt
                if (highest_priority->pcb->timer_interrupt) {
                    highest_priority->pcb->timer_interrupt = false; // Reset the timer interrupt flag
                    highest_priority->pcb->end_timer_time = clock; // Record the end time of the timer interrupt
                    highest_priority->pcb->ready_waiting_time += highest_priority->pcb->end_timer_time - highest_priority->pcb->start_timer_time; // Update the total waiting time in the ready state due to timer interrupt
                }

                removed = dequeueNode(ready_queue, highest_priority);
                enqueue(running_queue, removed);
                write_text_file(filename, clock, removed->pcb->pid, "Ready", "Running");
            }
        }

        // Increments the timer after each loop
        clock++; 
    }
    
    return terminated_queue;
}

/**
 * Calculates the average turnaround time for processes in the queue.
 *
 * @param queue     Pointer to the queue containing PCBs.
 * @return          The average turnaround time of the processes in the queue.
 */
float average_turnaround_time(queue_t *queue){
    float sum = 0;
    for (node_t *current = queue->front; current != NULL; current = current->next) {
        sum += (current->pcb->end_time - current->pcb->arrival_time);
    }
    float turnaround = sum / (queue->size);
    return turnaround;
}

/**
 * Calculates the average waiting time for processes in the queue.
 *
 * @param queue     Pointer to the queue containing PCBs.
 * @return          The average waiting time of the processes in the queue.
 */
float average_waiting_time(queue_t *queue){
    float sum = 0;
    for (node_t *current = queue->front; current != NULL; current = current->next) {
        sum += (current->pcb->start_time - current->pcb->arrival_time) + current->pcb->ready_waiting_time;
    }
    float waiting = sum / (queue->size);
    return waiting;
}

/**
 * Calculates the throughput of the processes in the queue.
 *
 * @param queue     Pointer to the queue containing PCBs.
 * @return          The throughput of the processes in the queue.
 */
float throughput(queue_t *queue){
    float throughput = (float)(queue->size) / (queue->rear->pcb->end_time);
    return throughput;
}

int main(int argc, char *argv[]){

    queue_t *new_queue;
    queue_t *terminated_queue;
    float turnaround_time;
    float waiting_time;
    float throughput_fraction;

    new_queue = read_CSV_file(argv[1]);

    FILE *file1 = fopen(argv[2], "w");
    fprintf(file1, "%-5s %-4s %-10s %-10s\n", "Time", "PID", "Old State", "New State");
    fclose(file1);

    terminated_queue = flow_process(new_queue, argv[2], argv[4]);

    // Calculate the average turnaround time for all of the PCBs
    turnaround_time = average_turnaround_time(terminated_queue);

    // Calculate the average waiting time for all of the PCBs
    waiting_time = average_waiting_time(terminated_queue);

    // Calculate the throughput fraction for all of the PCBs
    throughput_fraction = throughput(terminated_queue);

    FILE *file2 = fopen(argv[3], "w");
    fprintf(file2, "%-15s %-25s %-25s\n", "Throughput", "Average Turnaround Time", "Average Waiting Time");
    fprintf(file2, "%-15f %-25f %-25f\n", throughput_fraction, turnaround_time, waiting_time);
    fclose(file2);

    free(new_queue);
    free(terminated_queue);
    free(file1);
    free(file2);

    return 0;
}