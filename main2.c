 /*
Bang and Patrick
Project 3 OS
main2
*/

#include <stdio.h>
#include <stdlib.h>

//Definitions
#define PAGE_SIZE 256
#define FRAME_SIZE PAGE_SIZE
#define NUM_FRAMES 128 //Value must be less than MAX_INT
#define TLB_SIZE 16

char physical_memory[NUM_FRAMES][PAGE_SIZE];
unsigned int page_table[NUM_FRAMES];
int frame_next = 0;

typedef struct TLB_ENTRY_{
	int page_number;
	int frame_number;
}tlb_entry;
tlb_entry tlb_table[TLB_SIZE];
int tlb_next = 0;//FIFO implementation

//Function Declarations
//takes an address then septerates its pnumber and offset
void extract(unsigned int address, unsigned int *page_number, unsigned int *offset);

//TLB functions
//returns frame number or NUM_FRAMES
unsigned int tlbSearch(int page_number);
//adds page
void tlbAdd(int page_number, int frame_number);

//Page table functions
//looks for page in table
unsigned int pageTableSearch(int page_number);
//adds page to first open frame
unsigned int pageTableAdd(int page_number);


int main(int argc, char** argv) {
 	int page_faults = 0;//# of page faults
	int tlb_hits = 0;//# of tlb hits
	int num_adds = 0;//# of logical addreses read

	//Initialize Page Table
	for (int i = 0; i < NUM_FRAMES; i++){
        page_table[i] = -1;
    }
	//Initialize TLB
	for(int i = 0; i<TLB_SIZE; i++){
		tlb_table[i].page_number = -1;
		tlb_table[i].frame_number = -1;
	}

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
    	num_adds++;
		//print logcal address
		fprintf(out1, "%u\n", logical_address);

		//determine physical address
		unsigned int physical_address;
		unsigned int page_number;
		unsigned int offset;
		unsigned int frame_number;
		extract(logical_address, &page_number, &offset);
			frame_number = tlbSearch(page_number);
			//if in tlb inc tlb_hits
			if(frame_number<NUM_FRAMES){
				tlb_hits++;
			}
			//if not in tlb
			else{
				//check page table
				frame_number = pageTableSearch(page_number);
				//if not in page table
				if(frame_number >= NUM_FRAMES){
					page_faults++;
					//add to page table
					frame_number = pageTableAdd(page_number);
				}
				//add to tlb
				tlbAdd(page_number, frame_number);
			}
		//printf physical address
		fprintf(out2, "%u\n",(frame_number << 8)+offset);
		
		//determine value at physical address
		FILE * bin = fopen("BACKING_STORE.bin", "rb");
		if(bin == NULL){
			fprintf(stderr,"BACKING_STORE.bin open error");
			exit(1);
		}
		fseek(bin, PAGE_SIZE*page_number,SEEK_SET);
		fread(&physical_memory[frame_number], PAGE_SIZE, 1, bin);
		fclose(bin);
		bin = NULL;
		//print value
		fprintf(out3, "%d\n", physical_memory[frame_number][offset]);
		
	}
	
	//close files
	fclose(faddy);
	faddy = NULL;
	fclose(out1);
	out1 = NULL;
	fclose(out2);
	out2 = NULL;
	fclose(out3);

	//print statistics
	printf("Page faults = %d/%d, %.2f\n", page_faults, num_adds, (float)page_faults/num_adds);
	printf("TLB hits = %d/%d, %.3f\n", tlb_hits, num_adds, (float)tlb_hits/num_adds);

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

//returns frame number else -1
unsigned int tlbSearch(int page_number){
	for(int i = 0; i<TLB_SIZE; i++){
		if(tlb_table[i].page_number == page_number)
			return tlb_table[i].frame_number;
	}
	return NUM_FRAMES;
}

void tlbAdd(int page_number, int frame_number){
	tlb_table[tlb_next].page_number = page_number;
	tlb_table[tlb_next].frame_number = frame_number;

	//FIFO implementation
	tlb_next++;
	tlb_next%=TLB_SIZE;
}

//looks for page in table
unsigned int pageTableSearch(int page_number){
	for(int i = 0; i<NUM_FRAMES; i++)
		if(page_table[i] == page_number)
			return i;
	return NUM_FRAMES;
}
//adds page to first open frame returns frame
//incs frame_next
unsigned int pageTableAdd(int page_number){
	page_table[frame_next] = page_number;
	int val = frame_next;

	//FIFO implementation iterates through frames
	frame_next++;
	frame_next%=NUM_FRAMES;//Will not be used until main2
	return val;
}
