
# Scheduler and Memory Management Simulator

This repository contains two distinct parts for the Operating Systems course project:

1. **Scheduler**: A process scheduling simulator that supports multiple scheduling algorithms such as First-Come, First-Served (FCFS), Round Robin, and Priority Scheduling. The simulator is designed to evaluate the performance of these algorithms based on various metrics such as throughput, average turnaround time, and wait time.

2. **Memory Management**: A memory management simulator that simulates partitioned memory management. The system tracks memory usage and allocation for processes, using different partitioning schemes. It evaluates memory usage, fragmentation, and availability for each scenario.

## Project Structure

- **Scheduler**: Contains the source code, input files, and scripts to run the scheduling simulation. This includes the implementation of FCFS, Round Robin, and Priority Scheduling algorithms.
  
- **Memory Management**: Contains the source code, input files, and scripts for the memory management simulation. This includes memory allocation strategies, partition tracking, and fragmentation analysis.

Both simulators are written in C and come with test scripts to automate the simulation process.

## How to Use

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/SajaFawagreh/Scheduler-Simulator.git
   ```

2. **Navigate to the desired module**:
   - For process scheduling, navigate to the `Scheduler` folder.
   - For memory management, navigate to the `MemoryManagement` folder.

3. **Compile the code**:
   Each folder has specific instructions in its README for compiling and running the respective simulations.

## License
This project is for educational purposes and part of the SYSC 4001 course. No formal license is provided.