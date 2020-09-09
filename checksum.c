/**
Author: 
	Nikolo Sperberg
	
Class: 
	CS 352 -- Computer Architecture
	
Description:
	This code takes a file and creates a 32 bit code that is the sum of all the bits of the file.
	After getting this code, it prompts the user for a second file and checks if the codes are the same.
*/



#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

typedef unsigned short ushort;
typedef unsigned int uint32;
typedef unsigned long uint64;


/**
this is a helper method to add the 32 bit chuncks of a file together. it also handles the overflow
returns the current total
*/
int addBits(uint32 temp, uint64 total){
	uint64 overflow = 0;//to store overflow and add it back into the total
	total = total + temp;//add 32 bit chunk to total
	overflow = total;//coppy total to overflow.  this is done here to make sure the while loop is entered

	while(overflow > 0){
		overflow = total;//copy total to overflow
		overflow = overflow >> 32;//shift overflow to add back into total
		total = total << 32;//shift 32 bits to remove overflow
		total = total >> 32;//shift 32 bits to have bits in right part
		total = total + overflow;//add overflow to total
	}
	return total;
}

/**
this method grabs 32 bits of a file to be added together
it return a 32 bit code
/
/* define getCode that returns a 32-bit code (codeA)*/
uint32 getCode(FILE *file) {
	
	/* initialzing variables */
	int count = 0;//counter to know when 32 bits are grabbed
	uint32 buffer; /* put 32-bits of file in a buffer */
	uint32 buffArray [4];//array to hold up to 4 chars of a file
	uint32 temp = 0;//to hold 32 bit chunck of file
	uint32 shift = 0;//to shift chars into correct position of 32 bit chunk
	register uint64 total=0;//total to return
	
	/* for loop to make sure array is empty */
	for(int k = 0; k < 4; k++){
		buffArray[k] = 0;
	}
	
	/* beging grabing bits from file */
	buffer = fgetc(file);//stor 8 bits of a file
	while(buffer != EOF){//while loop to keep grabing bits from file
		buffArray[count] = buffer;//place char in correct spot of array
		buffer = fgetc(file);//grab next char
		if (count == 3){// buffer array full
			for(int i = 0; i < 4; i++){//loop through buffer array
				if(i == 0){
					shift = buffArray[0] << 24;//shift first char over 24 bits
				}
				else if (i == 1){

					shift = buffArray[1] << 16;//shift second char over 16 bits


				}
				else if(i == 2){

					shift = buffArray[2] << 8;//shift 3rd char over 8 bits

				}
				else{

					shift = buffArray[3];//assign last char to shift

				}
				temp = temp + shift;// combind shifted chars to get 32 bits of the file

			}
			count = 0;//reste count
			total = addBits(temp, total);// add 32 bit chunks together
			temp = 0;//zero out temp
		}
		
		/* this else if is to handle special case when the end of file has less than 4 chars */
		else if( buffer == EOF){
			int x = 0;
			for(int j = count; j != 0; j--){
				if(x == 0){
					shift = buffArray[j];// set last char to end of 32 bits
				}
				else if (x == 1){
					shift = buffArray[j] << 8;//shift 8 bits
				}
				else if( x == 2){
					shift = buffArray[j] << 16;//shift 16 bits
				}
				else if (x == 3){
					shift = buffArray[j] << 24;//shift 24 bits
				}
				temp = temp + shift;//comnbind shifts
			}
			total = addBits(temp, total);//add 32 bit chunks together
			temp = 0;//zero out temp

		}
		count++;//incriment count
	}
	total = ~ total;//flip bits in total
	return (uint32)total; /* one’s complement of the total */
}

/**
this test returns true if 32 bit code matches for two files, false if they do not
*/

/* define testValidity that returns true if getCode of the test file == some code sent in as argument */
bool testValidity(FILE *fileB, uint32 codeA){
	uint32 codeB = getCode(fileB);//calculate code
	printf("codeB in hex: %04x\n", codeB);//print code
	return(codeA == codeB);//check if code is the same
}



/**
main method -- driver
*/
int main() {
	
	/* decliration of variables */
	FILE* fp;
	char fname[50], input [50], c;
	int count, i = 1;
	bool flag0 = true, flag1= true, flag2 = true, flag3 = true;
	uint32 codeA;
	
	while(flag0){ //while loop to get valid file from user
		flag1 = true;
		printf("Enter filename: ");
		scanf("%s", &fname);//get user input
		fp = fopen(fname, "r");//open file
		if(fp == NULL){//if file not found
			printf("User Alert! -- File not found.\n");
			while(flag1){//prompt user to try again
				printf("Try again? (Y)es (N)o\n");
				scanf("%s", &input);//get user input
				if(strcmp(input, "Y") == 0 || strcmp(input, "y") == 0){
					flag1 = false;
				}
				else if (strcmp(input, "N") == 0 || strcmp(input, "n") == 0){
					printf("Exiting...\n");
					exit(1);
				}
				else{
					printf("User Alert! -- Invalid response. Please respond with Y for yes or N for no\n");  
				}
			}
		}
		else{//found valid file
			flag0 = false;
			codeA = getCode(fp);//calculate code
			printf("codeA in hex: %04x\n", codeA);//print code in hex
		}
	}
	while(flag2){//wile loop to get second file from user
		flag3 = true; 
		printf("Enter second filename: ");
		scanf("%s", &fname);//grab user input
		fp = fopen(fname, "r");//open file
		if(fp == NULL){//if file not found
			printf("User Alert! -- File not found.\n");
		}
		else{//file found
			if(testValidity(fp, codeA)){//test if same file
				printf("Files match.\nExiting...\n");
				exit(1);//exit on match
			}
			else{//files do not match
				printf("Files do not match.\n");
				while(flag3){//while loop to get yes or no from user
					printf("Try again? (Y)es (N)o\n");
					scanf("%s", &input);//scan user input
					if(strcmp(input, "Y") == 0 || strcmp(input, "y") == 0){
						flag3 = false;
					}
					else if (strcmp(input, "N") == 0 || strcmp(input, "n") == 0){//exit if user says no
						printf("Exiting...\n");
						exit(1);
					}
					else{//if response invalid
						printf("User Alert! -- Invalid response. Please respond with Y for yes or N for no\n"); 
					}
				}
			}
		}
	}
}


