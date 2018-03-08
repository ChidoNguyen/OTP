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
void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int main( int argc, char * argv[]){
	int listenSocketFD;
	int establishedConnectionFD;
	int portNumber;
	int charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[500000];
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
		//Accepting connection requests , blocking if more than 5
		sizeOfClientInfo = sizeof(clientAddress);
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
		if(establishedConnectionFD < 0 ) error("Could not accept connection appropriately");
		printf("SERVER: Connected Client at port %d\n", ntohs(clientAddress.sin_port));
		//Recv message and display 
		memset(buffer, '\0', 500000);
		charsRead = recv(establishedConnectionFD, buffer, 500000, 0);
		if(charsRead < 0 ) error("Empty coms");
		printf("# of chars: %i \nMessage: %s\n", charsRead, buffer);
		// Replies ( send) to client
		charsRead = send(establishedConnectionFD,"I am the server, and I got your message", 39, 0);
		if(charsRead < 0 ) error("Send to sock failed");
		close(establishedConnectionFD);
	}
	close(listenSocketFD);
	return 0;
}