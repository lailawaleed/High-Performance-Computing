# HPC Assignments - Spring 2025
Faculty of Computers and Artificial Intelligence  
Cairo University  

This repository contains solutions for all three assignments of the **HPC (High Performance Computing)** course using **MPI** and **OpenMP**, as assigned in Spring 2025.

---

## 📁 Contents

- `Assignment1/` – MPI-based solutions (Max Number, Caesar Cipher)
- `Assignment2/` – MPI-based solutions (Counting Primes, Matrix Summation)
- `Assignment3/` – OpenMP + Hybrid (OpenMP + MPI) solutions

Each folder contains:
- C source code for each problem
- Console output screenshots
- Any relevant input/output files

---

## 🧠 Assignment 1 – MPI

### 🔹 Problem 1: Max Number in Array
- **Description**: Find the maximum number in an array using the Master-Slave model with `MPI_Send` and `MPI_Recv`.
- **MPI Functions**: `MPI_Send`, `MPI_Recv`
- **File**: `Assignment1/max_number.c`

### 🔹 Problem 2: Caesar Cipher (Encryption/Decryption)
- **Description**: Encrypt/decrypt a string using Caesar Cipher logic. Input can be from the console or a file.
- **Modes**: Console mode and File mode
- **MPI Functions**: `MPI_Send`, `MPI_Recv`
- **File**: `Assignment1/caesar_cipher.c`

---

## 🧠 Assignment 2 – MPI

### 🔹 Problem 1: Count Prime Numbers in Range
- **Description**: Count prime numbers in a given range using two versions:
  - Version A: Using `MPI_Bcast` and `MPI_Reduce`
  - Version B: Using `MPI_Send` and `MPI_Recv`
- **File A**: `Assignment2/count_primes_bcast_reduce.c`  
- **File B**: `Assignment2/count_primes_send_recv.c`

### 🔹 Problem 2: Matrix Summation (A + B = C)
- **Description**: Perform element-wise matrix addition using MPI.
- **MPI Functions**: `MPI_Scatter`, `MPI_Gather`
- **File**: `Assignment2/matrix_sum.c`

---

## 🧠 Assignment 3 – OpenMP + MPI

### 🔹 Problem 1: Matrix-Vector Multiplication
- **Description**: Multiply a square matrix by a vector using OpenMP.
- **OpenMP**: Parallelize outer loop of multiplication
- **File**: `Assignment3/matrix_vector.c`

### 🔹 Problem 2: Standard Deviation
- **Description**: Compute the standard deviation of an array using OpenMP.
- **OpenMP**: Used for mean, variance, and final standard deviation calculation
- **File**: `Assignment3/std_deviation.c`

### 🔹 Problem 3: Hybrid MPI + OpenMP Array Summation
- **Description**: Compute the sum of a large array using MPI (distributed) + OpenMP (shared-memory threads)
- **File**: `Assignment3/hybrid_array_sum.c`

---

## 🛠️ Compilation & Execution

```bash
# 🔧 MPI Programs
mpicc filename.c -o output_name
mpirun -np <num_processes> ./output_name

# 🔧 OpenMP Programs
gcc -fopenmp filename.c -o output_name
./output_name

# 🔧 Hybrid MPI + OpenMP Programs
mpicc -fopenmp filename.c -o output_name
mpirun -np <num_processes> ./output_name

---

## 👩‍💻 Author

**Laila Waleed**  
