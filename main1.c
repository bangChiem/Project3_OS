 /*
Bang and Patrick
Project 3 OS
main1
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//Definitions
#define PAGE_SIZE 256
#define NUM_FRAMES 256
#define TLB_SIZE 16

char physical_memory[NUM_FRAMES][PAGE_SIZE];
unsigned int page_table[NUM_FRAMES];
unsigned int frame_next = 0;
bool pages_loop = false;

typedef struct TLB_ENTRY_{
	int page_number;
	int frame_number;
}tlb_entry;
tlb_entry tlb_table[TLB_SIZE] = {0}; //TODO make functions for editing tlb
int tlb_next = 0;
bool tlb_loop = false;

//Function Declarations
//takes an address then septerates its pnumber and offset
void extract(unsigned int address, unsigned int *page_number, unsigned int *offset);
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

//TLB functions
//returns frame number or -1
int tlbSearch(int page_number);
//adds page
void tlbAdd(int page_number, int frame_number);

//Page table functions
//looks for page in table
int pageTableSearch(int page_number);
//adds page to first open frame
int pageTableAdd(int page_number);


int main(int argc, char** argv) {
	//open all files and check for error
	FILE * faddy = fopen("addresses.txt","r");
	if(faddy == NULL){fprintf(stderr,"file open error"); exit(1);}
    FILE * out1 = fopen("out1.txt","w+");
	if(out1 == NULL){fprintf(stderr,"file open error"); exit(1);}
    FILE * out2 = fopen("out2.txt","w+");
	if(out2 == NULL){fprintf(stderr,"file open error"); exit(1);}
    FILE * out3 = fopen("out3.txt","w+");
	if(out3 == NULL){fprintf(stderr,"file open error"); exit(1);}


	//while addresses are being read
	unsigned int logical_address;
	while(fscanf(faddy, "%u\n", &logical_address)!=EOF){
    	//print logcal address
		fprintf(out1, "%d\n", logical_address);

		//determine physical address
		unsigned int physical_address;
		unsigned int page_number;
		unsigned int offset;
		int frame_number;
		extract(logical_address, &page_number, &offset);
			//if not in tlb
			frame_number = tlbSearch(page_number);
			if(frame_number == -1){
				//check page table
				frame_number = pageTableSearch(page_number);
				//if not in page table
				if(frame_number == -1){
					//add to page table
					frame_number = pageTableAdd(page_number);
				}
				//add to tlb
				tlbAdd(page_number, frame_number);
			}
		//printf physical address
		fprintf(out2, "%u\n",(frame_number << 8)+offset);
		
		//determine value at physical address
		int value;
		FILE * bin = fopen("BACKING_STORE.bin", "rb");
		if(bin == NULL){
			fprintf(stderr,"BACKING_STORE.bin open error");
			exit(1);
		}
		fseek(bin, offset, 256*frame_number);;
		fread(&value, sizeof(int), 1, bin);
		fclose(bin);
		bin = NULL;
		//print value
		fprintf(out3, "%d\n", value);
		
	}
	
	//close files
	fclose(faddy);
	faddy = NULL;
	fclose(out1);
	out1 = NULL;
	fclose(out2);
	out2 = NULL;
	fclose(out3);
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

// clear out all text in out1 and out2
void resetout1and2(){
    FILE *file = fopen("out2.txt", "w");
    fclose(file);
    fopen("out3.txt", "w");
    fclose(file);
}

//returns frame number else -1
int tlbSearch(int page_number){
	int bound = tlb_next;
	if(pages_loop){
		bound = TLB_SIZE;
	}
	for(int i = 0; i<bound; i++){
		if(tlb_table[i].page_number == page_number)
			return tlb_table[i].frame_number;
	}
	return -1;
}

void tlbAdd(int page_number, int frame_number){
	tlb_table[tlb_next].page_number = page_number;
	tlb_table[tlb_next].frame_number = frame_number;
	tlb_next++;
	tlb_next%=TLB_SIZE;
	if(tlb_next == 0){
		tlb_loop = true;
	}
}

//looks for page in table
int pageTableSearch(int page_number){
	int bound = frame_next;
	if(pages_loop){
		bound = NUM_FRAMES;
	}
	for(int i = 0; i<bound; i++)
		if(page_table[i] == page_number)
			return i;
	return -1;
}
//adds page to first open frame returns frame
//incs frame_next
int pageTableAdd(int page_number){
	page_table[frame_next] = page_number;
	int val = frame_next;
	frame_next++;
	frame_next%=NUM_FRAMES;
	if(frame_next == 0){
		pages_loop = true;
	}
	return val;
}
