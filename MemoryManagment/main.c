/**
 * SYSC 4001 - Operating Systems
 * Lab section: A2
 * Assignment 2 Process Scheduling, Memory Management, Part 2
 * Design and Implementation of a Scheduler Simulator
 *
 * @authors	Saja Fawagreh
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

// Defines a structure for the PCB with the necessary information
typedef struct pcb {	
    int pid;                    
    int arrival_time;
    int total_CPU_time;
    int remaining_CPU_time;
    int io_frequency;
    int io_duration;
    int waiting_time;
    int memory_needed;
    int memory_position;
} pcb_t;

// Defines a structure for a node to link the PCBs
typedef struct node{	
    pcb_t *pcb;
    struct node *next;
} node_t;

// Defines a structure for the queue
typedef struct {  
    node_t *front;
    node_t *rear;
    int size;
} queue_t;

// Defines a structure to represent memory management information
typedef struct {
    int position;
    int memory_size;
    int partition_free;
    int partition_used;
    bool used;
} memory_t;

/**
 * Allocates and initializes an array of memory_t structures to manage memory partitions (main memory table).
 *
 * @param size An array containing the sizes of memory partitions to be managed.
 * @return     A pointer to an array of pointers to memory_t structures representing the main memory table.
 */
memory_t** MemoryManagement(int size[]) {
    memory_t** lst = (memory_t**)malloc(4 * sizeof(memory_t*));
    if (lst == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    for(int i = 0; i < 4; i++) {
        lst[i] = (memory_t*)malloc(sizeof(memory_t));
        if (lst[i] == NULL) {
            printf("Memory allocation failed for element %d.\n", i);
            return NULL;
        }
        lst[i]->position = i;
        lst[i]->memory_size = size[i];
        lst[i]->partition_free = size[i];
        lst[i]->partition_used = 0;
        lst[i]->used = false;
    }

    return lst;
}

/**
 * Allocates memory for a PCB.
 *
 * @param memory_management Pointer to the memory structure to allocate memory to.
 * @param pcb               Pointer to the PCB to allocate memory for.
 */
void allocate_memory(memory_t* memory_management, pcb_t* pcb){
    memory_management->partition_used = pcb->memory_needed;
    memory_management->partition_free = memory_management->memory_size - memory_management->partition_used;
    memory_management->used = true;
    pcb->memory_position = memory_management->position;
}

/**
 * Deallocates memory in the memory management structure.
 *
 * @param memory_management Pointer to the memory structure to deallocate memory from.
 */
void deallocate_memory(memory_t* memory_management){
    memory_management->partition_used = 0;
    memory_management->partition_free = memory_management->memory_size - memory_management->partition_used;
    memory_management->used = false;
}

/**
 * This function allocates a queue on the heap and returns a pointer to it
 * 
 * @return A pointer to the queue structure created.
 */
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
 * @param memory_needed  The memory needed for the PCB.
 * @return               A pointer to the PCB structure created.
 */
pcb_t *new_pcb(int pid, int arrival_time, int total_CPU_time, int io_freq, int io_dur, int memory_needed) {
    pcb_t *pcb = malloc(sizeof(pcb_t));
    assert(pcb != NULL);
    pcb->pid = pid;
    pcb->arrival_time = arrival_time;
    pcb->total_CPU_time = total_CPU_time;
    pcb->remaining_CPU_time = total_CPU_time;
    pcb->io_frequency = io_freq;
    pcb->io_duration = io_dur;
    pcb->waiting_time = 0;
    pcb->memory_needed = memory_needed;
    pcb->memory_position = -1;
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
    int pid, arrivalTime, totalCpuTime, ioFrequency, ioDuration, memoryNeeded;

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
    while (fscanf(file, "%d,%d,%d,%d,%d,%d", &pid, &arrivalTime, &totalCpuTime, &ioFrequency, &ioDuration, &memoryNeeded) == 6) {
        p = malloc(sizeof(node_t));
        pcb = new_pcb(pid, arrivalTime, totalCpuTime, ioFrequency, ioDuration, memoryNeeded); // Create a new PCB structure with the extracted values
        p->pcb = pcb; // Assigns the PCB value to PCB structure created
        p->next = NULL; // Assigns the next value to NULL since it will be the last element in the queue
        enqueue(queue, p); // Enqueue the node containing the PCB into the queue
    }

    fclose(file);

    return queue;
}

/**
 * Writes information about the main memory and PCB states to the specified output file at specific times.
 *
 * @param filename           The name of the output file.
 * @param time               The time at which the PCB switches states.
 * @param pcb                Pointer to the PCB.
 * @param MemoryManagement   A pointer to the memory management array.
 * @param oldState           The old state of the PCB.
 * @param newState           The new state of the PCB.
 */
void write_text_file(const char *filename, int time, pcb_t* pcb, memory_t** MemoryManagement, const char *oldState, const char *newState) {
    FILE *file = fopen(filename, "a");
    int usedMemory = 0, freeMemory = 0, usableMemory = 0;
    char usedPartitionStr[50] = "";
    char freePartitionStr[50] = "";
    
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    for (int i = 0; i < 4; i++) {
        freeMemory += MemoryManagement[i]->partition_free;
        if (MemoryManagement[i]->partition_used == 0) {
            usableMemory += MemoryManagement[i]->memory_size;
            sprintf(freePartitionStr + strlen(freePartitionStr), "%d ", i);
        } else {
            usedMemory += MemoryManagement[i]->partition_used;
            sprintf(usedPartitionStr + strlen(usedPartitionStr), "%d ", i);
        }
    }

    if(pcb->remaining_CPU_time){
        fprintf(file, "%-8d %-8d %-25d %-25s %-20d %-20d %-23d %-18s %-18s %-15s %-15s\n", time, pcb->pid, pcb->memory_position, "", usedMemory, freeMemory, usableMemory, usedPartitionStr, freePartitionStr, oldState, newState);
    }
    else{
        fprintf(file, "%-8d %-8d %-25s %-25d %-20d %-20d %-23d %-18s %-18s %-15s %-15s\n", time, pcb->pid, "", pcb->memory_position, usedMemory, freeMemory, usableMemory, usedPartitionStr, freePartitionStr, oldState, newState); 
    }

    fclose(file);
}


/**
 * This function simulates an OS kernel, processing PCB state transitions.
 * 
 * @param new_queue    A pointer to the queue containing PCBs to process.
 * @param filename     The name of the output file for logging state transitions.
 */
void flow_process(queue_t *new_queue, const char *filename, int size[]){
    queue_t *ready_queue = alloc_queue();
    queue_t *running_queue = alloc_queue();
    queue_t *waiting_queue = alloc_queue();
    queue_t *terminated_queue = alloc_queue();

    memory_t** memory_management;   // A pointer to the memory table
    node_t *current;                // A pointer to the current node in the loop
    node_t *removed;                // A pointer to the node being removed
    int running_time = 0;           // The running time of the PCB in the running state
    int pcb_num = new_queue->size;  // The number of PCBs, which is the size of the new_queue
    int clock = 0;                  // The timer
    
    memory_management = MemoryManagement(size);

    // The while loop won't stop until all PCBs are terminated.
    while(terminated_queue->size < pcb_num){

        // Check for PCBs in the new queue and transition them to the ready state if their arrival time is less than or equal to the current clock time 
        // and there is available memory for the process
        if (new_queue->size != 0) {
            current = new_queue->front;
            int new_size = new_queue->size;

            for (int i = 0; i < new_size; i++) {
                // Check if the PCBs arrival time allows it to move to the ready state
                if (current->pcb->arrival_time <= clock) {
                    bool done = false; // A boolean variable to indicate if memory has been allocated for the PCB
                    for (int j = 0; j < 4; j++) {
                        // Memory will be allocated only when:
                        // 1. There is sufficient space in one of the partitions
                        // 2. The partition does not currently contain a PCB
                        // 3. The process has not already been allocated memory
                        if ((current->pcb->memory_needed <= memory_management[j]->memory_size) && 
                            (memory_management[j]->used == false) && 
                            (done == false)) {
                            // Move the PCB from the new queue to the ready queue
                            node_t *next = current->next;
                            removed = dequeueNode(new_queue, current);
                            enqueue(ready_queue, removed);

                            // Allocate memory for the PCB
                            allocate_memory(memory_management[j], removed->pcb);
                            write_text_file(filename, clock, removed->pcb, memory_management, "New", "Ready");
                            current = next; // Move to the next PCB after memory allocation
                            done = true;
                            break;
                        }
                    }

                    // If no memory is allocated for the PCB, move to the next one
                    if (done == false) {
                        current = current->next;
                    }
                } 
                
                // If the PCBs arrival time does not allow it to move to the ready state, move to the next one
                else {
                    current = current->next;
                }
            }
        }


        // Check for PCBs in the waiting queue and transition them to the ready state if their waiting time matches their I/O duration
        if(waiting_queue->size != 0){
            current =  waiting_queue->front;
            int waiting_size = waiting_queue->size;

            for(int i = 0; i < waiting_size; i++) {
                
                current->pcb->waiting_time++;

                if (current->pcb->io_duration == current->pcb->waiting_time) {
                    current->pcb->waiting_time = 0;
                    node_t *next = current->next;
                    removed = dequeueNode(waiting_queue, current);
                    enqueue(ready_queue, removed);
                    current = next;
                }
               
                else{
                    current = current->next;
                }
            } 
        } 

        // Checks the PCB in the running queue and transitions them to the terminated state if their remaining CPU time is 0 
        if(running_queue->size != 0){
            running_time++;
            running_queue->front->pcb->remaining_CPU_time--;
            
            if (running_queue->front->pcb->remaining_CPU_time == 0) {
                removed = dequeue(running_queue);
                enqueue(terminated_queue, removed);

                // Deallocate memory associated with the terminated PCB
                int index = removed->pcb->memory_position;
                deallocate_memory(memory_management[index]);
                write_text_file(filename, clock, removed->pcb, memory_management, "Running", "Terminated");
                // Reset the memory position to -1
                removed->pcb->memory_position = -1;
            }
        
            // Checks the PCB in the running queue and transitions them to the waiting queue if their I/O frequency matches the running time
            else if(running_time == running_queue->front->pcb->io_frequency){
                removed = dequeue(running_queue);
                enqueue(waiting_queue, removed);    
            }
        }

        // Checks the ready queue for PCBs and transitions the first PCB in the ready queue to the running state if the running state is available
        if(ready_queue->size != 0){
            if (running_queue->size == 0) {
                removed = dequeue(ready_queue);
                enqueue(running_queue, removed);
                running_time = 0;
            }
        }

        // Increments the timer after each loop
        clock++; 
    }

    // Deallocates memory after jobs are terminated 
    free(ready_queue);
    free(waiting_queue);
    free(running_queue);
    free(terminated_queue);
    free(current);
    free(removed);
}

int main(int argc, char *argv[]){

    queue_t *new_queue = alloc_queue();
    new_queue = read_CSV_file(argv[1]);
    FILE *file = fopen(argv[2], "w");
    fprintf(file, "%-8s %-8s %-25s %-25s %-20s %-20s %-23s %-18s %-18s %-15s %-15s\n", "Time", "PID", "Partition # of Entry", "Partition # of Removal", "Total Used Memory", "Total Free Memory", "Total Usable Memory", "Used Partitions", "Free Partitions", "Old State", "New State");
    fclose(file);

    int size[4];

    if (strcmp(argv[3], "size1") == 0) {
        size[0] = 500;
        size[1] = 250;
        size[2] = 150;
        size[3] = 100;
    } else {
        size[0] = 300;
        size[1] = 300;
        size[2] = 350;
        size[3] = 50;
    }

    flow_process(new_queue, argv[2], size);

    free(new_queue);
    free(file);

    return 0;
}