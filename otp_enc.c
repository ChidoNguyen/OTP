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

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

int main(int argc, char* argv[]){
	
	int socketFD, portNumber, charsWritten,charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[500000];
	
	//add arg check later//
	
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // clears
	portNumber = atoi(argv[1]); // CHANGE THIS LATER !!!!!!!
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
	
	// Send stuff to server //
	printf("Client: Test a phrase: ");
	memset(buffer, '\0', sizeof(buffer));
	fgets(buffer, sizeof(buffer)-1, stdin); // shorten buffer by 1 to have null term
	buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds
	
	// actual SEND //
	charsWritten = send(socketFD, buffer, strlen(buffer), 0);
	if(charsWritten < 0 ) error("Client: Send failed");
	if( charsWritten < strlen(buffer)) printf("Warning dropped packets\n");
	
	//server response //
	memset(buffer, '\0', sizeof(buffer));
	charsRead = recv(socketFD, buffer, sizeof(buffer) -1, 0);
	if(charsRead < 0 ) error("Client: crickets in the socket");
	printf("Client: Server sent - %s\n", buffer);
	close(socketFD);
	return 0;
}
