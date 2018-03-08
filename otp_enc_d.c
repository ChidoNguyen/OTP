/*
Chido Nguyen
931506965
Program 4: OTP
OTP_ENC_D:
This is our daemon or "server" that listens for connection requests from OTP_ENC. It will 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
// "secret pass" to verify ID of which client is sending to it //
char* pass = "shouldbeasecret";

char* KEY[27] = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"," "};

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

struct history{
	int arr[5];
};
struct history SPID;

/*
REAP_THE_SOULS
Purpose: To clean up child forked processes
Simple for loop iteration through global struct array, and does a 
waitpid with WNOHANG
*/
void REAP_THE_SOULS(){
	int z, childstatus;
	waitpid(-1,0,WNOHANG);
	 for(z = 0 ; z < 5; z++){
		if(SPID.arr[z] != -1){
			if(waitpid(SPID.arr[z], &childstatus, WNOHANG)){
				if(WIFEXITED(childstatus)){
					SPID.arr[z] = -1;
				}
			}
		}
	} 
}

/*
handshake:
checks to see if our passphrases match up prevents DEC from talking to our daemon
Input: takes a string
Output: 1-> for matches , 0 for all other cases
*/
int handshake(char *arr, int establishedConnectionFD){
	int charsRead;
	memset(arr, '\0', 20);
	charsRead = recv(establishedConnectionFD,arr, 15, 0);
	if(charsRead < 0)
		fprintf(stderr, "Handshake empty");
	if(strcmp( arr, pass) == 0)
		return 1;
	else
		return 0;
	
}
/*
OTP_encrypt:
This is our encryption function , following OTP mod 27 procedure.
input: plaintext array , and key array of characters
output: sends back a cipher text.
*/
int main( int argc, char * argv[]){
	//
	int z;
	for(z = 0; z < 5; z++){
		SPID.arr[z] = -1;
	}
	//
	int listenSocketFD;
	int establishedConnectionFD;
	int portNumber;
	int charsRead;
	int gatekeeper;
	socklen_t sizeOfClientInfo;
	char buffer[500000];
	char buffer2[500000];
	char hand[20];
	struct sockaddr_in serverAddress, clientAddress;
	
	// Check to make sure only 2 arguments are used total
	if(argc != 2)
		fprintf(stderr, "%s","Not enough arguments.\n USAGE: ./otp_enc_d [portnumber] \n");
	
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clears everything out.
	portNumber = atoi(argv[1]); // convert string to int from our user directed port#
	serverAddress.sin_family = AF_INET; // generate socket with network capability 
	serverAddress.sin_port = htons(portNumber); // store port #
	serverAddress.sin_addr.s_addr = INADDR_ANY; // allow for any address to connect
	
	listenSocketFD = socket(AF_INET,SOCK_STREAM, 0); // init socket
	if(listenSocketFD < 0) error("Error setting up socket");
	
	//Bind the socket to the port so we can receive and send data 
	if(bind(listenSocketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
		error("Binding of socket and port failed");
	listen(listenSocketFD, 5); // turns socket on , 5 connections max
	while(1){
		REAP_THE_SOULS();
		//Accepting connection requests , blocking if more than 5
		sizeOfClientInfo = sizeof(clientAddress);
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
		if(establishedConnectionFD < 0 ) error("Could not accept connection appropriately");
		printf("SERVER: Connected Client at port %d\n", ntohs(clientAddress.sin_port));
		////////////////////TESTING FORK STUFF///////////////////////////
		int spawnPID = fork();
		for(z = 0; z < 5; z++){
			if(SPID.arr[z] == -1){
				SPID.arr[z] = spawnPID;
				break;
			}
		}
		if(spawnPID == -1){
			printf("BROKEN\n");
		}
		else if(spawnPID == 0){
			/*To-do:
			"link" enc_d with enc only dont allow DEC to send to itoa
			-after verifying we need to grab the sent files
			-encrypt
			-send back cipher text
			*/
			gatekeeper = handshake(hand,establishedConnectionFD);
			
			if(gatekeeper == 1){
				memset(buffer, '\0', 500000);
				memset(buffer2, '\0', 500000);
				
				charsRead = recv(establishedConnectionFD, buffer, 500000, 0);
				if(charsRead < 0 ) error("Empty coms");
				printf("# of chars: %i \nMessage: %s\n", charsRead, buffer);
				// Replies ( send) to client
				charsRead = send(establishedConnectionFD,"I am the server, and I got your message", 39, 0);
				if(charsRead < 0 ) error("Send to sock failed");
				
				charsRead = recv(establishedConnectionFD, buffer2, 500000, 0);
				if(charsRead < 0 ) error("Empty coms");
				printf("# of chars: %i \nMessage: %s\n", charsRead, buffer2);
				// Replies ( send) to client
				charsRead = send(establishedConnectionFD,"I am the server, and I got your message", 39, 0);
				if(charsRead < 0 ) error("Send to sock failed");
			}
			else
				fprintf(stderr, "Unauthorized access");
			close(establishedConnectionFD);
			exit(0);
		}
	}
	close(listenSocketFD);
	return 0;
}