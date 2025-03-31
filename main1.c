/*
Bang and Patrick
Project 3 OS
main1
*/

#include <stdio.h>
#include <stdlib.h>

//Definitions

//TODO number of addresses should be discovered at runtime
//I think we could change it from an array to a linked list
#define NUM_ADDRESSES 1000

typedef struct LOGICAL_ADDRESS_{
	unsigned int page_number;
	unsigned int offset;
}logical_addy;

//TODO finish implementing TLB
typedef struct TLB_ENTRY_{
	unsigned int page_number;
	unsigned int frame_number;
}tlb_entry;
tlb_entry tlb[15];

//Function Declarations
//takes an address then septerates its pnumber and offset
void extract(unsigned int address, unsigned int *page_number, unsigned int *offset);
//Reads and stores from addresses.txt
void loadaddys(logical_addy * addys);
//prints logical addresses
void out1(logical_addy * addys);


int main() {
    logical_addy logical_addys[NUM_ADDRESSES];
	//TODO the size needs to be arbitrary not hardset to 1000
	loadaddys(logical_addys);

    // Output results
	out1(logical_addys);
    return 0;
}

//Function Definitions

//takes an address then septerates its pnumber and offset
void extract(unsigned int address, unsigned int *page_number, unsigned int *offset){
    // Extract higher 8 bits / 2 bytes for page number 
    // and lower 8 bits / 2 bytes for offset
    *page_number = (address >> 8) & 0xFF; // First 8 bits
    *offset = address & 0xFF;         // Last 8 bits
}

//Reads and stores from addresses.txt
void loadaddys(logical_addy * addys){
	FILE * addys_file = fopen("addresses.txt", "r");
	if(addys_file == NULL){fprintf(stderr, "Error opening addresses.txt\n");exit(1);}
	for(int i = 0; i<NUM_ADDRESSES; i++){
		unsigned int curr_address;
		fscanf(addys_file, "%u\n", &curr_address);
		extract(curr_address, &addys[i].page_number, &addys[i].offset);
	}
	fclose(addys_file);
}

//prints logical addresses
void out1(logical_addy * addys){	
	FILE * out1_file = fopen("out1.txt", "w+");
	if(out1_file == NULL){fprintf(stderr, "Error opening out1.txt\n");exit(1);}
	for(int i = 0; i<NUM_ADDRESSES; i++){
		fprintf(out1_file, "%u%u\n", addys[i].page_number, addys[i].offset);
	}
	fclose(out1_file);
}
