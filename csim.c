/** * vabowen and hskantrowitz */#include "cachelab.h"#include <stdio.h>#include <stdbool.h>#include <string.h>#include <stdlib.h>/** * Sets up variables and calls the simulation */int main(int argc, char* argv[]){    //so far, this is ONLY for direct mapping!  	bool h_flag = false; // determines if print help	bool v_flag = false; // determines if prints specifics	bool done = false; // checks if should stop	int s_index = -1; // the place of the s	int e_index = -1; // the place of the e	int b_index = -1; // the place of the b	int t_index = -1; // the place of the t	int set_index = 0;  // first required argument	int num_lines = 0;  // second required argument	int block_bits = 0; // third required argument	char file_name[100];	FILE* fp;	// checking if help is there	if(argc == 2 && (strcmp(argv[1],"-h") == 0)){		h_flag = true;		done = true;	}	// this is to get all the arguments	if(!done){		// getting the placement of each param		for(int i = 1; i < argc-1; i++){			// if -s then s arg at i+1			if(!(strcmp(argv[i],"-s"))){				s_index = i+1;			}			// same for -E			else if(!(strcmp(argv[i],"-E"))){				e_index = i+1;			}			// same for -b			else if(!(strcmp(argv[i],"-b"))){				b_index = i+1;			}			// same for -t			else if(!(strcmp(argv[i], "-t"))){				t_index = i+1;			}			// if -v then set v flag			else if(!(strcmp(argv[i],"-v"))){				v_flag = true;			}			// if -h then set h flag			else if(!(strcmp(argv[i], "-h"))){				h_flag = true;			}			// if any other option			else if(argv[i][0] == '-'){				done = true;				printf("./csim: invalid option -- '%c'\n",argv[i][1]);			}		}		// check when the v or h flag are last		if(!(strcmp(argv[argc-1],"-h"))){			h_flag = true;		}		if(!(strcmp(argv[argc-1],"-v"))){			v_flag = true;		}		// checking if h flag to stop and print		if(h_flag){			done = true;		}		// checks that all the required arguments are present		else if(argc < 9 && !done){			puts("./csim: Missing required command line argument");			done = true;		}		// checking that all the flags (for required) were found		if((s_index == -1 || b_index == -1 || e_index == -1 || t_index == -1) && !done){			done = true;			puts("./csim: Missing required command line argument");		}		// proceed with getting the arguments after flags		if (!done) {			char* ptr = 0;			// this gets the set index			long set_index_l = strtol(argv[s_index], &ptr, 10);			set_index = (int) set_index_l;			// this gets the number of lines per set (with v)			long num_lines_l = strtol(argv[e_index], &ptr, 10);			num_lines = (int) num_lines_l;			// this gets the number of block bits (with v)			long block_bits_l = strtol(argv[b_index], &ptr, 10);			block_bits = (int) block_bits_l;			// this checks for all valid input (chars will be zero)			if((set_index <= 0  || num_lines <= 0 || block_bits <= 0) && !done){				done = true;				puts("./csim: Missing required command line argument");			}		}	}	// prints the help if the arguments are not satisfied	if(done == true || h_flag == true) {		printHelp();	}	// all other params are set now time to check the file	strcpy(file_name, argv[t_index]); // get file	fp = fopen(file_name, "r");       // try to open	// if cannot open	if(!fp){		done = true;		printf("%s: No such file or directory\n", argv[t_index]);	}	fclose(fp);	// just to print here to see right now	printf("block bits: %d\n", block_bits);	printf("num_lines: %d\n", num_lines);	printf("set_index: %d\n", set_index);	printf("t_index: %d\n", t_index);	printf("v_flag: %d\n", v_flag);	// this is just for testing purposes right now	fp = fopen(file_name,"r");	char* firstLine = readLine(fp, 0);	printf("\nfirstLine: %s\n", firstLine);	// all arguments set before starting	if (!done) {		struct cacheLine {			int tag;	//the tag, the high-order bits of memory			int validBit;	//0 or 1, whether or not the data is in the cache		};		//values needed to simulate cache		int hit_count = 0;	//number of hits, initialized to 0		int miss_count = 0;	//number of misses, initialized to 0		int eviction_count = 0;	//number of evictions, initialized to 0		int num_sets = 2 ^ set_index;		int block_size = 2 ^ block_bits;	        cacheLine* cache = setCache(num_sets, num_lines, block_size);	//assume function that grabs 1 line of file and determines if it is usable	//the output of that function is the address, called address	for (int i = 0; i < linesInFile; i++){	  	  	  output = cacheLookup(address, cache, set_index, block_bits);	   if (output == 0){//it was a miss, no eviction needed	     miss_count++;	   }	   else if (output == 1){//it was a hit	     hit_count++;	   }	   else if (output == 2){//it was a miss with an eviction	     eviction_count++;	     miss_count++;	   }	}	//print verbose code 	printSummary(hit_count, miss_count, eviction_count);	}    return 0;}/** * Gets the index */int getIndex(int address, int index_bits){  //middle bits are the index  int index = address & 0x00007fe0;//bit manipulation to get index  return index;}/** * Gets the tag */int getTag(int address, int tag_bits){  //front bits are tag  int tag = address & 0xFFFF8000;//bit manipulation to get tag  return tag;}/** * Sets the valid bit */int setValidBit(cacheLine* base_index, int index){  cacheLine* location = base_index + index;//set the location to change as the base index plus the given tag plus however many bits are in the tag to get to valid bit  location->validBit = 0;//set the valid bit at that location to the updated value  return 1;}/** * Looks up the given address and determines if it exists in cache */int cacheLookup(int address, cacheLine* cache, int index_bits, int block_bits){  int answer = 0;  int tag_bits = 32 - (index_bits + block_bits);  int tag = getTag(address, tag_bits);  int index = getIndex(address, index_bits);  cacheLine* line = cache + index;  int currentTag = line->tag;  if (tag == currentTag && validBit == 1){    answer = 1;  }  else{    answer = evictionOrMiss(line, currentTag, tag);  }  return answer;}/** * Creates the simulated cache */int setCache(int numSets, int linesPerSet, int blockSize){   cacheLine* cacheBasePtr = malloc(sizeof(cacheLine)*numSets*linesPerSet);//creates array, cacheSize is pointer to 1st element in array   for (int i = 0; i < numSets; i++){     int index = linesPerSet*i;//casts the line currently on to a cacheLine pointer     setValidBit(cacheBasePtr, index);//initializes the valid bit of each line to 0   }       }/** * determines if eviction is needed, performs eviction from cache */int evictionOrMiss(cacheLine* line, int currentTag, int tagNext){  int eviction = 0;  if (validBit == 1){//If there's something in there, kick it out     eviction = 2;  }  //otherwise just populate the space  line->tag = tagNext;  line->validBit = 1;  return eviction;}