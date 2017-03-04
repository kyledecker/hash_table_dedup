#include <cstdlib>
#include <iostream>
#include <vector>
#include <string.h>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <list>

bool content_compare(std::string& file1, std::string& file2);
void HashRemove(const std::string& path, std::vector<std::list<std::pair<size_t,std::string> > > &table);

void HashRemove(const std::string& path, std::vector<std::list<std::pair<size_t,std::string> > > &table){
  
  DIR * dir;
  struct dirent *ent;
  char dot[] = ".";
  char dotdot[] = "..";
  if((dir = opendir(path.c_str())) != NULL){
    while ((ent = readdir(dir)) != NULL){
      // If regular file 
      if((ent->d_type & DT_REG ) != 0){
	std::string fileName = path;
	fileName += "/";
	fileName += std::string(ent->d_name);
	//std::cout << fileName << std::endl;
	std::ifstream f;
	f.open(fileName);
	if (!f.good()) {
	  //std::cout<< "Could not open file";
	  //exit(EXIT_FAILURE);
	  break;
	}
	// Read all lines of data
	std::string lines;
	std::string curr;
	while (std::getline(f,curr) && f.good()){
	  //lines += '\n';
	  lines += curr;
	}
	f.close();
	// Get Hash Value for file based on contents
	size_t hash_val = std::hash<std::string>{}(lines);
	int h = hash_val % table.size();
	// Add file to hash table
	(table)[h].push_front(std::pair<size_t,std::string>(hash_val,fileName));
	//std::cout << h << " "<< hash_val << "\n";
      }
      // If directory
      else if(((ent->d_type & DT_DIR ) != 0) && (strcmp(ent->d_name,dot) != 0)  && (strcmp(ent->d_name,dotdot) != 0)  ){
      	std::string nextDir = path;
	nextDir += "/";
	nextDir += std::string(ent->d_name);
	HashRemove(nextDir,table);
      }
    }
  }
  closedir (dir);

  // Go through the hash table and find duplicate files
  for (size_t i = 0; i < table.size(); i++){
    size_t first_hash_val;
    bool same_content = false;
    std::string first_fileName;
    std::string compare_fileName;
    while(table[i].size() > 0){
      first_hash_val = table[i].front().first;
      first_fileName = table[i].front().second;
      std::list<std::pair<size_t,std::string> >::iterator iterator;
      iterator = table[i].begin();
      for (std::advance(iterator,1); iterator != table[i].end(); ++iterator){
	//for (iterator = table[i].begin(); iterator != table[i].end(); ++iterator){
	if (iterator->first == first_hash_val){
	  compare_fileName = iterator->second;
	  same_content = content_compare(first_fileName,compare_fileName);
	  if (same_content == true){
	    std::cout << "#Removing " << compare_fileName;
	    std::cout << " (duplicate of " << first_fileName << "). \n";
	    std::cout << "rm " << compare_fileName << "\n";
	    //table[i].erase(iterator++);
	  }
	} 
      }
      table[i].pop_front();
    }
  }
}


bool content_compare(std::string& file1, std::string& file2){
  // Get contents of file 1
  std::ifstream f1;
  f1.open(file1);
  // Read all lines of data
  std::string lines1;
  std::string curr1;
  while (std::getline(f1,curr1) && f1.good()){
    lines1 += curr1;
  }
  f1.close();

  // Get contents of file 2
  std::ifstream f2;
  f2.open(file2);
  // Read all lines of data
  std::string lines2;
  std::string curr2;
  while (std::getline(f2,curr2) && f2.good()){
    lines2 += curr2;
  }
  f2.close();

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
      HashRemove(path,table);
    }
  }

  return EXIT_SUCCESS;

}
