# CS3339_Homework_3

Overview: This assignment implements a cache simulator in C++. It models a set-associative cache and determines whether each memory reference results in a cache hit or miss. The cache stores valid bits and tags.

To compile: g++ cache_sim.cpp -o cache_sim
To execute: ./cache_sim (num_entries)(associativity)(input_memory_reference_file)
    - num_entries: total number of entries,
    - associativity: associativity of the cache to be simulated
    - input_memory_reference_file: exact file name of file that contains list of memory address references separated by spaces.