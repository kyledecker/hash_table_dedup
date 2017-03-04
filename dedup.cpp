#include <cstdlib>
#include <iostream>
#include <vector>
#include <string.h>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <list>
#include <streambuf>
#include <sstream>

bool content_compare(std::string& file1, std::string& file2);
void HashMake(const std::string& path, std::vector<std::list<std::pair<size_t,std::string> > > &table);
void HashRemove(std::vector<std::list<std::pair<size_t,std::string> > > &table);

void HashMake(const std::string& path, std::vector<std::list<std::pair<size_t,std::string> > > &table){
  
  DIR * dir;
  struct dirent *ent;
  char dot[] = ".";
  char dotdot[] = "..";
  if((dir = opendir(path.c_str())) != NULL){
    while ((ent = readdir(dir)) != NULL){
      // If regular file 
      if(((ent->d_type == DT_REG ) != 0)){
	std::string fileName = path;
	fileName += "/";
	fileName += std::string(ent->d_name);
	//std::cout << fileName << std::endl;
	
	// Read all lines
	std::ifstream f(fileName);
	std::stringstream buffer;
	buffer << f.rdbuf();
	std::string lines = buffer.str();
	
	// Get Hash Value for file based on contents
	size_t hash_val = std::hash<std::string>{}(lines);
	int h = hash_val % table.size();
	// Add file to hash table
	(table)[h].push_front(std::pair<size_t,std::string>(hash_val,fileName));
	//std::cout << h << " "<< hash_val << "\n";
      }
      // If directory
      else if(((ent->d_type == DT_DIR ) != 0) && (strcmp(ent->d_name,dot) != 0)  && (strcmp(ent->d_name,dotdot) != 0) && ((ent->d_type == DT_LNK) == 0) ){
      	std::string nextDir = path;
	nextDir += "/";
	nextDir += std::string(ent->d_name);
	HashMake(nextDir,table);
      }
    }
  }
  closedir (dir);
}

void HashRemove(std::vector<std::list<std::pair<size_t,std::string> > > &table){

  // Go through the hash table and find duplicate files
  for (size_t i = 0; i < table.size(); i++){
    size_t first_hash_val;
    bool same_content = false;
    std::string first_fileName;
    std::string compare_fileName;
    //size_t col_size = table[i].size();
    while(table[i].size() > 0){
      first_hash_val = table[i].front().first;
      first_fileName = table[i].front().second;
      std::list<std::pair<size_t,std::string> >::iterator iterator;
      //iterator = table[i].begin();
      //for (std::advance(iterator,1); iterator != table[i].end(); ++iterator){
      for (iterator = table[i].begin(); iterator != table[i].end(); ++iterator){
	if ((iterator->first == first_hash_val) && (iterator->second != first_fileName)){
	  compare_fileName = iterator->second;
	  same_content = content_compare(first_fileName,compare_fileName);
	  if (same_content == true){
	    std::cout << "#Removing " << first_fileName;
	    std::cout << " (duplicate of " << compare_fileName << "). \n";
	    std::cout << "rm " << first_fileName << "\n";

	    //table[i].pop_front();
	    //col_size--;
	    break;
	  }
	} 
      }
      //std::cout << col_size << "\n";
      table[i].pop_front();
      //col_size--;
    }
  }
}


bool content_compare(std::string& file1, std::string& file2){
  // Get contents of file 1
  std::ifstream f1(file1);
  std::stringstream buffer1;
  buffer1 << f1.rdbuf();
  std::string lines1 = buffer1.str();

  // Get contents of file 2
  std::ifstream f2(file2);
  std::stringstream buffer2;
  buffer2 << f2.rdbuf();
  std::string lines2 = buffer2.str();

  //std::cout << lines1 << "\n";
  //std::cout << lines2 << "\n";

  if (lines1.compare(lines2) == 0){
    return true;
  }
  else{
    return false;
  }

}

int main(int argc, char ** argv) {
  
  if (argc > 1){
    std::cout << "#!/bin/bash" << "\n";
    int num_dirs = argc-1;
    // Define Hash Table
    std::vector<std::list<std::pair<size_t,std::string> > > table(2099);
    for(int i = 1; i<=num_dirs; i++){
      std::string path = std::string(argv[i]);
      HashMake(path,table);
    }
    HashRemove(table);
  }

  return EXIT_SUCCESS;

}
