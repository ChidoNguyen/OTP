/*Chido Nguyen
931506965
Program 4: OTP
OTP_ENC:
our "client" to talk to our server daemon. Program takes 2 arguments our plaintext, and key files. Transfer both to our encryption server, then receives the cipher text back.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

char* pass = "shouldbeasecret";

int KEY[27] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',' '};


void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

void bad_char(char letter){
	int x;
	for(x = 0; x < 27; x++){
		if(KEY[x] == letter)
			return;
	}
	error("BAD CHARACTERS BRUH");
}

int main(int argc, char* argv[]){
	
	int socketFD, portNumber, charsWritten,charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[500000];
	char buffer2[500000];
	char buffer3[500000];
	char hand[20];
	FILE* fp;
	
	if(argc != 4){
		error("Not enough args: USAGE ./[name] [text] [key] [port]");
	}
	
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // clears
	portNumber = atoi(argv[3]); // CHANGE THIS LATER !!!!!!!
	serverAddress.sin_family = AF_INET; // network capable socketFD
	serverAddress.sin_port = htons(portNumber);// store port #
	char *tmp = "localhost";
	serverHostInfo = gethostbyname(tmp);
	if(serverHostInfo == NULL) {
		fprintf(stderr, "CLIENT: Error, no such host\n");
		exit(0);
	}
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char *)serverHostInfo->h_addr, serverHostInfo->h_length); // copy in address fro dns look up
	
	//Socket setup//
	socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if(socketFD < 0) error("Socket setup failed");
	
	//connect to server//
	if( connect ( socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) error("Client: Error Connecting");

	// actual SEND //
	//charsWritten = send(socketFD, buffer, strlen(buffer), 0);
	charsWritten = send(socketFD, pass, strlen(pass), 0);
	memset(hand, '\0', sizeof(hand));
	charsRead = recv(socketFD, hand, 15, 0);
	if(strcmp(hand,pass) != 0){
		error("Bad Handshake");
	}
	
	//////////
	if(charsWritten < 0 ) error("Client: Send failed");
	if( charsWritten < strlen(pass)) printf("Warning dropped packets\n");
	/////////////////
	memset(buffer, '\0', sizeof(buffer));
	memset(buffer2, '\0', sizeof(buffer));
	fp= fopen(argv[1], "r");
	int wordcount;
	wordcount = 0;
	char c;
	c=getc(fp);
	while(c != '\n'){
		bad_char(c);
		buffer[wordcount] = c;
		wordcount++;
		c = getc(fp);
	}
	fclose(fp);
	
	fp= fopen(argv[2], "r");
	int wordcount2;
	wordcount2 = 0;
	char d;
	d=getc(fp);
	while(d != '\n'){
		bad_char(d);
		buffer2[wordcount2] = d;
		wordcount2++;
		d= getc(fp);
	}
	fclose(fp);
	
	if(wordcount > wordcount2){
		error("Key length too short");
	}
	
	buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds
	buffer2[strcspn(buffer2, "\n")] = '\0'; // Remove the trailing \n that fgets adds
	char tmp2[10];
	memset(tmp2, '\0', sizeof(tmp2));
	//itoa(wordcount, tmp2, 10);
	snprintf(tmp2, 10, "%i", wordcount);

	char * pointer;
	pointer = buffer;
	int tmpholder;
	tmpholder =  wordcount;
	charsWritten = send(socketFD, tmp2, 10, 0);

	//send plaintxt
	charsWritten = 0;
	while ( tmpholder > 0){
		charsWritten = send(socketFD, pointer, tmpholder , 0);
		pointer = pointer + charsWritten;
		tmpholder = tmpholder - charsWritten;

	}
	//send key
	charsWritten = 0;
	pointer = buffer2;
	tmpholder = wordcount;
	while (tmpholder > 0){
		charsWritten = send(socketFD, pointer, tmpholder, 0);
		pointer = pointer + charsWritten;
		tmpholder = tmpholder - charsWritten;
	}

	//get cipher back
	memset(buffer3, '\0', sizeof(buffer3));
	pointer = buffer3;
	tmpholder = wordcount;
	while( tmpholder > 0){
		charsRead = recv(socketFD, pointer, wordcount, 0);
		pointer = pointer + charsRead;
		tmpholder = tmpholder - charsRead;
	}
	
	fprintf(stdout, "%s\n", buffer3);
	fflush(stdout);
	
	close(socketFD);
	return 0;
}

