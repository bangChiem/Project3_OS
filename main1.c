/*
Bang and Patrick
Project 3 OS
main1
*/

#include <stdio.h>

int main() {
    unsigned int num = 2; // input decimal address number 

    // Extract higher 8 bits / 2 bytes for page number 
    // and lower 8 bits / 2 bytes for offset
    unsigned int page_number = (num >> 8) & 0xFF; // First 8 bits
    unsigned int offset = num & 0xFF;         // Last 8 bits

    // Output results
    printf("Page number: 0x%X\n",page_number);

    printf("Offset: 0x%X\n",offset);

    return 0;
}