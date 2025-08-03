# Multiprocess Message Queue Program - Generating Ordered Products of Primes

## Overview
This C-based program demonstrates inter-process communication (IPC) using System V message queues. The goal is to generate the smallest `n` numbers of the form `2^a * 3^b * 5^c`, in sorted order, using a central server process and three separate multiplier processes (`mult2`, `mult3`, and `mult5`).

Each multiplier process receives a value from the server, multiplies it by its own factor (2, 3, or 5), and returns the result. The server collects and manages these results, ensuring duplicates are filtered out, and maintains a list of the smallest generated numbers.

## Files
- `server.c`  
  The main coordinating process. Sends a number to all three multiplier processes, receives their products, stores unique results in a list, sorts them, and prints the smallest value at each step.
- `mult.c`  
  A general multiplier process. Compiles to different executables (e.g., `mult2`, `mult3`, `mult5`) by passing the appropriate multiplier value and number of iterations.

## Compilation
```bash
gcc -o server server.c
gcc -o mult mult.c
```

## Usage
Run Instructions
- You must run all processes simultaneously in separate terminals or background processes.
1. Start the multipliers (each in their own terminal or background process):
```bash
./mult 2 15
./mult 3 15
./mult 5 15
```
3. Start the server:
```bash
./server 15
```
The number 15 in this case represents how many values you want the program to compute and print.

## Example Output
```java
1 - smallest value = 1
2 - smallest value = 2
3 - smallest value = 3
4 - smallest value = 4
5 - smallest value = 5
6 - smallest value = 6
7 - smallest value = 8
8 - smallest value = 9
9 - smallest value = 10
10 - smallest value = 12
```

## Key Concepts Demonstrated
- System V message queues
- Inter-process communication
- Insertion sort and de-duplication logic
- Multiprocessing with shared message queues
- Efficient computation of a classically recursive number pattern

## Notes
- Each mult process uses its own unique queue ID (QNAME + multiplier) to avoid collisions.
- The server always communicates via the base queue QNAME (170).
- All queues are removed (deleted) at the end of each process for cleanup.

