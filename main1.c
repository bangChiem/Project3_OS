/*
Bang and Patrick
Project 3 OS
main1
*/

#include <stdio.h>
#include <stdlib.h>

//Definitions
char physical_memory[256][256];
int page_table[256];
int open_frame_num = 0;

//TODO number of addresses should be discovered at runtime
//I think we could change it from an array to a linked list
#define NUM_ADDRESSES 1000
#define PAGE_SIZE 256

// I removed the addresses struct and converted it to an array of logical addresses

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
void loadaddys(unsigned int addresses[]);
//prints logical addresses
void out1(unsigned int addresses[]);
// initialize page table
void initPageTable();
// write physical address to out2.txt
void out2(int physical_address);
// write signed byte value stored in physical memory to out3.txt
void out3(char value);
// read frame from backing store into physical_memory
void read_from_back_store(unsigned int page_number);
// clear out all text in out1 and out2
void resetout1and2();

int main(int argc, char** argv) {

    // reset out2 and out3
    resetout1and2();
    unsigned int addresses[NUM_ADDRESSES];
	//TODO the size needs to be arbitrary not hardset to 1000
	loadaddys(addresses);
    initPageTable();
    // process every logical address
    for (int i; i < NUM_ADDRESSES; i++){
        unsigned int page_number;
        unsigned int offset;
        // extract logical addresses page number and offset
        extract(addresses[i], &page_number, &offset);
        // If page_table entry is not -1(page hit) write to out2 physical address
        // and value in physical address to out3
        if (page_table[page_number] != -1){
            out2( (page_table[page_number] * 256) + offset);
            out3(physical_memory[page_table[page_number]][offset]);
        }
        // if page_table entry is -1 (page miss)
        else{
            read_from_back_store(page_number);
            out2( (page_table[page_number] * 256) + offset);
            out3(physical_memory[page_table[page_number]][offset]);
        }
    }

    printf("%d", physical_memory[1][151]);


	out1(addresses); // print virtual addresses
    return 0;
}

//Function Definitions

// initialize page table with all -1 to mark empty table
void initPageTable(){
    for (int i = 0; i < PAGE_SIZE; i++){
        page_table[i] = -1;
    }
}


//takes an address then septerates its pnumber and offset
void extract(unsigned int address, unsigned int *page_number, unsigned int *offset){
    // Extract higher 8 bits
    // and lower 8 bits
    *page_number = (address / 256); // First 8 bits
    *offset = address % 256;         // Last 8 bits
}

//Reads and stores from addresses.txt
void loadaddys(unsigned int addresses[]) {
    // Open the file "addresses.txt" in read mode
    FILE * addys_file = fopen("addresses.txt", "r");
    
    // Check if the file was opened successfully
    if (addys_file == NULL) {
        fprintf(stderr, "Error opening addresses.txt\n");
        exit(1);  // Exit the program if the file cannot be opened
    }

    // Loop through and read NUM_ADDRESSES addresses into the array
    for (int i = 0; i < NUM_ADDRESSES; i++) {
        fscanf(addys_file, "%u\n", &addresses[i]);  // Read an unsigned integer from the file
    }

    // Close the file after reading all addresses
    fclose(addys_file);
}

//prints logical addresses
void out1(unsigned int addresses[]) {	
    // Open the file "out1.txt" in write mode
    FILE * out1_file = fopen("out1.txt", "w+");
    
    // Check if the file was opened successfully
    if (out1_file == NULL) {
        fprintf(stderr, "Error opening out1.txt\n");
        exit(1);  // Exit the program if the file cannot be opened
    }

    // Loop through the addresses array and write each address to the file
    for (int i = 0; i < NUM_ADDRESSES; i++) {
        fprintf(out1_file, "%u\n", addresses[i]); // Write the address directly
    }

    // Close the file after writing all addresses
    fclose(out1_file);
}

// write physical address to out2.txt
void out2(int physical_address){
    // Open the file "out2.txt" in append mode
    FILE * out2_file = fopen("out2.txt", "a+");

    // Check if the file was opened successfully
    if (out2_file == NULL) {
        fprintf(stderr, "Error opening out2.txt\n");
        exit(1);  // Exit the program if the file cannot be opened
    }

    // write physical address to out2.txt
    fprintf(out2_file, "%d\n", physical_address); 
}

// write signed byte value stored in physical memory to out3.txt
void out3(char value){
    // Open the file "out3.txt" in append mode
    FILE * out3_file = fopen("out3.txt", "a+");

    // Check if the file was opened successfully
    if (out3_file == NULL) {
        fprintf(stderr, "Error opening out3.txt\n");
        exit(1);  // Exit the program if the file cannot be opened
    }

    // write physical address to out2.txt
    fprintf(out3_file, "%d\n", value); 
}

void read_from_back_store(unsigned int page_number){
    // in case of page fault open of backingstore
    FILE *BACKING_STORE = fopen("BACKING_STORE.bin", "rb"); 
    if (BACKING_STORE == NULL) {
        perror("Error opening file");
        return;
    }

    // move reader to first item in page
    fseek(BACKING_STORE, page_number * 256, SEEK_SET); 
    // read 256 bytes from backing store into buffer frame
    fread(physical_memory[open_frame_num], 1, PAGE_SIZE, BACKING_STORE);
    //update page table to point to new frame that was just loaded
    page_table[page_number] = open_frame_num;
    // increment open frame# by 1
    open_frame_num++;
    // printf("\n\nValues at page %d\n", page_number);
    // for(int i = 1; i <= PAGE_SIZE; i++){
    //     printf("%3d  ", physical_memory[open_frame_num][i]);
    //     if(i % 16 == 0){printf("\n");}
    // }

    fclose(BACKING_STORE); // Close the file
}

// clear out all text in out1 and out2
void resetout1and2(){
    FILE *file = fopen("out2.txt", "w");
    fclose(file);
    fopen("out3.txt", "w");
    fclose(file);
}
