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

  void access(unsigned long addr, ofstream &out){
    int index = get_index(addr);
    unsigned tag = get_tag(addr);

    for(int i = 0; i < assoc; i++){
      if(entries[index][i].get_valid()&&
         entries[index][i].get_tag() == tag){

          entries[index][i].set_ref(counter++);

          out << addr << " : HIT" << endl;
          return;
         }
    }

    int replace = find_replace(index);

    entries[index][replace].set_valid(true);
    entries[index][replace].set_tag(tag);

    entries[index][replace].set_ref(counter++);

    out << addr << " : MISS" << endl;
  }
  
private:
  int assoc;
  unsigned num_entries;
  int num_sets;
  Entry **entries;
  int counter;

  int get_index(unsigned long addr){
    return addr % num_sets;
  }

  unsigned get_tag(unsigned long addr){
    return addr / num_sets;
  }

  int find_replace(int index){
    for(int i = 0; i < assoc; i++){
      if(!entries[index][i].get_valid())
        return i;
      }

      int lru = 0;
      
      for(int i = 1; i < assoc; i++){
        if(entries[index][i].get_ref() < entries[index][lru].get_ref())
          lru = i;
      }
      return lru;
    }
};

int main(int argc, char* argv[]){

  if(argc < 4){
    cout << "Usage: ./cache_sim num_entries associativity input_file" << endl;
    return 0;
  }

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
  while(input >> addr){
    cache.access(addr,output);
  }

  input.close();
  output.close();

  return 0;
}