/*
* CS3339 HW3: cache_sim.cpp
* Vanny Bundick - YDE18
*/


#include<iostream>
#include<fstream>
#include <cstdlib>

using namespace std;

/*
* Entry Class: Represents one cache line
* Stores: valid bit, tag for memory block,
* and ref for LRU replacement
*/
class Entry {
public:
  Entry(){
    valid = false;
    tag = 0;
    ref = 0;
  }
  
  ~Entry(){}

  void set_tag(int _tag) { tag = _tag; }
  int get_tag() { return tag; }

  void set_valid(bool _valid) { valid = _valid; }
  bool get_valid() { return valid; }

  void set_ref(int _ref) { ref = _ref; }
  int get_ref() { return ref; }

private:  
  bool valid;
  unsigned tag;
  int ref;
};

/*
* Cache Class: Represents entire cache
* num_sets x associativoty
*/
class Cache {
public:
  Cache(int entries, int assoc){
    this->assoc = assoc;
    this->num_entries = entries;
    this->num_sets = entries / assoc;
    this->counter = 0;                              // Fix: Initialize LRU counter

    // Allocate 2D cache structure
    this->entries = new Entry*[num_sets];
    for(int i = 0; i < num_sets; i++){
      this->entries[i] = new Entry[assoc];
    }
  }
  ~Cache(){
    for(int i = 0; i < num_sets; i++){
      delete[] entries[i];
    }
    delete[] entries;
  }

  /*
  * Public Member Function: access(unsigned long, ofstream)
  * Simulates a single memory reference
  *   -sets index and tag
  *   -checks for HIT/MISS, and produces output
  *   -if MISS, replaces using LRU counter
  */
  void access(unsigned long addr, ofstream &out){
    int index = get_index(addr);
    unsigned tag = get_tag(addr);

    // HIT Check
    for(int i = 0; i < assoc; i++){
      if(entries[index][i].get_valid()&&
         entries[index][i].get_tag() == tag){

          entries[index][i].set_ref(counter++);     // Updates LRU Counter

          out << addr << " : HIT" << endl;
          return;
         }
    }

    // MISS Check
    int replace = find_replace(index);

    entries[index][replace].set_valid(true);
    entries[index][replace].set_tag(tag);

    entries[index][replace].set_ref(counter++);     // Updates LRU Counter

    out << addr << " : MISS" << endl;
  }
  
private:
  int assoc;
  unsigned num_entries;
  int num_sets;
  Entry **entries;
  int counter = 0;

/*
* Private Member Function: get_index(unsigned long):
* -select index of cache set
*/
  int get_index(unsigned long addr){
    return addr % num_sets;
  }

/*
* Private Member Function: get_tag(unsigned long):
* -identify memory block tag
*/
  unsigned get_tag(unsigned long addr){
    return addr / num_sets;
  }

/*
* Private Member Function: find_replace(int):
* - first invalid entry, otherwise replace least recently used
*/
  int find_replace(int index){
    // Prefer invalid entry
    for(int i = 0; i < assoc; i++){
      if(!entries[index][i].get_valid())
        return i;
      }

      // Find least recently used
      int lru = 0;
      for(int i = 1; i < assoc; i++){
        if(entries[index][i].get_ref() < entries[index][lru].get_ref())
          lru = i;
      }
      return lru;
    }
};

/*
* Main Driver - main(command line arguments)
* -read input from command line arguments
* -run cache simulation and write results to output file
*/
int main(int argc, char* argv[]){

  // Check for valid input, show how to execute file
  if(argc < 4){
    cout << "Usage: ./cache_sim num_entries associativity input_file" << endl;
    return 0;
  }

  // Set command-line arguments
  int num_entries = atoi(argv[1]);
  int assoc = atoi(argv[2]);
  string input_file = argv[3];

  ifstream input(input_file);
  ofstream output("cache_sim_output");

  if(!input.is_open()){
    cerr << "Error: cannot open input file" << endl;
    return 1;
  }

  Cache cache(num_entries, assoc);
  unsigned long addr;

  // Process each memory reference through public access function
  while(input >> addr){
    cache.access(addr,output);
  }

  input.close();
  output.close();

  return 0;
}