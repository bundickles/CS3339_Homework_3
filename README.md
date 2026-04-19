# CS3339_Homework_3

Author: Vanny Bundick - yde18

Overview: This assignment implements a cache simulator in C++. It models a set-associative cache and determines whether each memory reference results in a cache hit or miss. The cache stores valid bits and tags.
This repository contains two versions of this assignment, one for the original required cache simulator, and one for the extra credit version that includes L1/L2 hierarchy, multi-word blocks, and MISS classification. They are separated for readability and debugging purposes, but are executed similarly. Please see compile and execution instructions below.

Original Version:

To compile: g++ cache_sim.cpp -o cache_sim

To execute: ./cache_sim (num_entries)(associativity)(input_memory_reference_file)
- num_entries: total number of entries,
- associativity: associativity of the cache to be simulated
- input_memory_reference_file: exact file name of file that contains list of memory address references separated by spaces.

Extra Credit Version:

To compile: g++ cache_sim_ex.cpp -o cache_sim_ex

To execute: ./cache_sim_ex (num_entries)(associativity)(input_memory_reference_file)
- num_entries: total number of entries,
- associativity: associativity of the cache to be simulated
- input_memory_reference_file: exact file name of file that contains list of memory address references separated by spaces.
