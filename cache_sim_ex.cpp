/*
* CS3339 HW3 Extra Credit Version: cache_sim_ex.cpp
* Vanny Bundick - YDE18
*/

#include<iostream>
#include<fstream>
#include <cstdlib>
#include <unordered_set>

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
* Cache organization = num_sets x associativaty
* - Extra credit implementations:
*   -l1/L2 Hierarchy and MISS classification.
*/
class Cache {
public:
  Cache(int entries, int assoc, int block_size = 1, bool is_L1 = true){
    this->assoc = assoc;
    this->num_entries = entries;
    this->num_sets = entries / assoc;
    this->counter = 0;                              // Fix: Initialize LRU counter

    this->block_size = block_size;
    this->is_L1 = is_L1;


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
  * Public Member Function: set_L2(Cache*)
  * Connects L1 -> L2 cache
  */
  void set_L2(Cache* l2_cache){
    this->L2 = l2_cache;
  }

  /*
  * Public Member Function: probe_L2(unsigned long)
  * Looks up L2 check from L1 without insertion
  */
  bool probe_L2(unsigned long addr){
    unsigned long block = addr / block_size;
    int index = get_index(block);
    unsigned tag = get_tag(block);

    for(int i = 0; i < assoc; i++){
        if(entries[index][i].get_valid() &&
           entries[index][i].get_tag() == tag)
           return true;
        }
        return false;
    }

  /*
  * Public Member Function: access(unsigned long, ofstream)
  * Simulates a single memory reference
  *   -sets index and tag
  *   -checks for HIT/MISS, and produces output
  *   -if MISS, replaces using LRU counter
  *   - Added Extra Credit Implementation:
  *     -L1 HIT, L1 MISS -> L2 Check
  *     -L2 MISS -> memory access
  */
  void access(unsigned long addr, ofstream &out){
    int index = get_index(addr);
    unsigned tag = get_tag(addr);
    unsigned long block = addr / block_size;

    // HIT Check - FOR L1
    for(int i = 0; i < assoc; i++){
      if(entries[index][i].get_valid()&&
         entries[index][i].get_tag() == tag){

          entries[index][i].set_ref(counter++);     // Updates LRU Counter

          out << addr << " : HIT (L1)" << endl;
          return;
         }
    }

    // L1 MISS -> check L2 -> L2 MISS
    if(is_L1 && L2){
        if(L2->probe_L2(addr)){
            out << addr << " : MISS (L1), HIT (L2)" << endl;
            insert(addr);                           // Promotes to L1
            return;
        }

        out << addr << " : MISS (L1), MISS (L2)" << endl;
        insert(addr);
        L2->insert(addr);
        return;
    }

    // MISS Check Classification
    string type = classify(block);
    out << addr << " : MISS (" << type << ")" << endl;
    insert(addr);
  }
  
private:
  int assoc;
  unsigned num_entries;
  int num_sets;
  Entry **entries;
  int counter = 0;

  int block_size;
  bool is_L1;
  Cache* L2 = nullptr;
  unordered_set<unsigned long> seen_blocks;        // Tracks block access, used for MISS detection

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
* Private Member Function: insert(unsigned long):
* - Inserts block using the LRU replacement and counter
*/
  void insert(unsigned long addr){
    unsigned long block = addr / block_size;
    int index = get_index(block);
    unsigned tag = get_tag(block);
    int replace = 0;

    for(int i = 0; i < assoc; i++){
        if(!entries[index][i].get_valid()){
            replace = i;
            break;
        }

        if(entries[index][i].get_ref() <
           entries[index][replace].get_ref())
           replace = i;
    }

    entries[index][replace].set_valid(true);
    entries[index][replace].set_tag(tag);
    entries[index][replace].set_ref(counter++);
  }

/*
* Private Member Function: classify(unsigned long):
* Classifies MISS as compulsory, capacity/conflict:
*  - first time block is seen -> compulsory
*  - otherwise -> capacity or conflict
*  - NOTE: would need fully associative cache to compare to
*    in order to differentiate capacity vs conflict MISS,
*    but I don't have time to do parallel simulation codes.
*/
  string classify(unsigned long block){
    if(seen_blocks.find(block) == seen_blocks.end()){
        seen_blocks.insert(block);
        return "COMPULSORY";
    }
    return "CAPACITY/CONFLICT";
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
    cout << "Usage: ./cache_sim_ex num_entries associativity input_file" << endl;
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

  // Create L1 and L2 caches and connect L1->L2
  Cache L1(num_entries, assoc, 1, true);
  Cache L2(num_entries * 4, assoc * 2, 1, false);
  L1.set_L2(&L2);
  unsigned long addr;

  // Process each memory reference through public access function
  while(input >> addr){
    L1.access(addr,output);
  }

  input.close();
  output.close();

  return 0;
}