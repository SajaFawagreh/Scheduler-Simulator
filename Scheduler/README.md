
# Scheduler Simulator

This folder contains a process scheduler simulator that implements various scheduling algorithms such as:
- **First-Come, First-Served (FCFS)**
- **Round Robin** with a 100ms time slice
- **Priority Scheduling**

The simulator is designed to evaluate the performance of these algorithms based on metrics like throughput, average turnaround time, and wait time.

## Contents

- **main.c**: The main source code for the scheduler simulator.
- **Test Cases**: A folder containing the input files, output results, and metrics for the test cases.
  - **Inputs**: Contains CSV files defining the process information for the test cases.
  - **Outputs**: Contains text files showing the output of the simulator for each test case.
  - **Metrics**: Contains the metrics results from the simulation runs.
  - **Scripts**: Contains batch scripts for running each test case automatically.

### Test Case Files
Each test case consists of:
- **Inputs**: CSV files (`test_case_1.csv` to `test_case_10.csv`) defining the processes, their arrival times, CPU bursts, I/O frequency, I/O duration, and priority.
- **Scripts**: Batch scripts (`test1.bat` to `test10.bat`) to automatically run the corresponding test case.
- **Outputs**: After running each test case, the simulator generates output files (`output_1.txt` to `output_10.txt`) showing the state transitions.
- **Metrics**: Metrics files (`calculations_1.txt` to `calculations_10.txt`) contain detailed performance analysis such as throughput, average turnaround time, and average waiting time.

## Input Format
The simulator expects input files in CSV format, where each line represents a process with the following fields:

```
PID, Arrival Time, Total CPU Time, I/O Frequency, I/O Duration, Priority
```

## Output
The simulator will produce an output log in text format that shows process transitions. Each line in the log will include:
- **Time**: The simulation time in milliseconds.
- **PID**: The process ID.
- **Old State / New State**: The transition between states (e.g., from **Ready** to **Running**).

### Sample Output:
```
Time: 0, PID: 1, Old State: Ready, New State: Running
Time: 50, PID: 1, Old State: Running, New State: Waiting (I/O)
...
```

## Metrics
Each simulation will produce metrics that include:
- **Throughput**: The number of processes completed per unit of time.
- **Average Turnaround Time**: The total time taken for each process to complete.
- **Average Waiting Time**: The total time spent by each process waiting in the ready queue.

### Sample Metrics:
```
Throughput      Average Turnaround Time   Average Waiting Time     
0.007273        355.500000                254.250000
```

## How to Run

1. **Compile the Program**:
   Use a C compiler (such as GCC) to compile the source code.
   ```bash
   gcc -o Scheduler main.c
   ```

2. **Run the Test Cases**:
   Navigate to the `Test Cases/Scripts` folder and run any of the provided batch files to execute the corresponding test case:
   ```bash
   ./test1.bat
   ```

3. **View the Results**:
   After running each test case, view the results in the corresponding `Outputs/output_x.txt` file and the calculations in `Metrics/calculations_x.txt`.

## Customization
You can create your own test files in the `Inputs` folder. When doing so, specify the processes (with arrival time, CPU bursts, I/O operations, and priority) and choose the scheduling algorithm (FCFS, Round Robin, or Priority) to observe how the simulator handles them.

To run the custom test, simply update or create a new batch file in the `Scripts` folder and specify the input file and scheduler in the command.
