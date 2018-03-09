/*
Chido Nguyen
931506965
Program 4 OTP: Keygen file
The program/app will take 1 argument that denotes the length of they key to be generated. The allowed characters are 26 letters of the alphabet and space (27 total).
*/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

int KEY[27] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',' '};

int main(int argc, char *argv[]){
	srand(time(NULL));
	int length;
	length = atoi(argv[1]);
	if( argc == 2){
		int i, index;
		for(i = 0; i < length ; i++){
			index = rand() % 27;
			printf("%c", KEY[index]);
		}
		printf("\n");
	}
	else // https://stackoverflow.com/questions/39002052/how-i-can-print-to-stderr-in-c
		fprintf(stderr, "%s","Wrong number or arguments. ./keygen [keylength]\n");
	return 0;
}