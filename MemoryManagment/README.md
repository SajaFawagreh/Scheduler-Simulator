
# Memory Management Simulator

This folder contains a memory management simulator that implements partitioned memory management using the **First-Come, First-Served (FCFS)** scheduling algorithm for memory allocation. The simulator evaluates memory allocation based on fixed and variable partition sizes and tracks memory usage, fragmentation, and availability.

## Partition Sizes
The simulator allows you to choose between two predefined partition sizes:

1. **Size 1**:
   - Partition 1: 500MB
   - Partition 2: 250MB
   - Partition 3: 150MB
   - Partition 4: 100MB

2. **Size 2**:
   - Partition 1: 300MB
   - Partition 2: 300MB
   - Partition 3: 350MB
   - Partition 4: 50MB

You can specify the desired partition size when running the test cases.

## Contents

- **main.c**: The main source code for the memory management simulator.
- **Test Cases**: A folder containing the input files, output results, and batch scripts for the test cases.
  - **Inputs**: CSV files defining the memory management scenarios.
  - **Outputs**: Text files showing the results of the memory management simulation for each test case.
  - **Scripts**: Batch scripts for running each test case automatically.

### Test Case Files
Each test case consists of:
- **Inputs**: CSV files (`test_case_1.csv` to `test_case_10.csv`) defining memory requirements and process details.
- **Scripts**: Batch scripts (`test1.bat` to `test10.bat`) to automatically run the corresponding test case.
- **Outputs**: After running each test case, the simulator generates output files (`output_1.txt` to `output_10.txt`) showing the memory allocation results.

## Input Format
The simulator expects input files in CSV format, where each line represents a process with the following fields:

```
Pid, Arrival Time, Total CPU Time, I/O Frequency, I/O Duration, Memory Needed
```

## Output
The simulator will produce an output log in text format that shows the memory allocation status for each process. Each line in the log will include:

- **Time**: The simulation time in milliseconds.
- **PID**: The process ID.
- **Partition # of Entry**: The partition number where the process is placed.
- **Partition # of Removal**: The partition number where the process is removed.
- **Total Used Memory**: The total memory used in the system at the time.
- **Total Free Memory**: The total free memory in the system.
- **Total Usable Memory**: The total memory available for allocation.
- **Used Partitions**: The number of partitions currently being used.
- **Free Partitions**: The number of partitions available.
- **Old State / New State**: The state transition for the process (e.g., from **Waiting** to **Running**).

### Sample Output:
```
Time: 0      PID: 1      Partition # of Entry: 1      Partition # of Removal: -      Total Used Memory: 500MB      Total Free Memory: 500MB      Total Usable Memory: 500MB      Used Partitions: 1      Free Partitions: 3      Old State: Ready      New State: Running
Time: 50     PID: 1      Partition # of Entry: -      Partition # of Removal: 1      Total Used Memory: 0MB        Total Free Memory: 1000MB     Total Usable Memory: 1000MB     Used Partitions: 0      Free Partitions: 4      Old State: Running    New State: Terminated
```

## How to Run

1. **Compile the Program**:
   Use a C compiler (such as GCC) to compile the source code.
   ```bash
   gcc -o MemoryManager main.c
   ```

2. **Run the Test Cases**:
   Navigate to the `Test Cases/Scripts` folder and run any of the provided batch files to execute the corresponding test case:
   ```bash
   ./Scripts/test1.bat
   ```

3. **View the Results**:
   After running each test case, view the results in the corresponding `Outputs/output_x.txt` file.

## Customization
You can create your own test files in the `Inputs` folder. When doing so, specify the processes (with arrival time and memory requirements) and observe how the simulator handles memory allocation. To run the custom test, simply update or create a new batch file in the `Scripts` folder and specify the input file in the command.

You can also create custom partition sizes by modifying the code. The default partition sizes are defined in the source code, but you can update them to your preferred values in this [section](https://github.com/SajaFawagreh/Scheduler-Simulator/blob/375bce0a52c01be4e80cfccf0bc75a81801b02ae/MemoryManagment/main.c#L446C5-L456C6). Change these values to your desired partition sizes, then recompile the program to use the updated sizes.
