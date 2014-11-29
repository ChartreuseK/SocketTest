/*
 *
 * 
 * 
 * 
 ****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h> 					// Types for system calls
#include <sys/socket.h>                 // Stuff for sockets
#include <netinet/in.h>					// Stuff for domain addresses
#include <netdb.h>                      // Used for hostent
#include <arpa/inet.h>

#define  BUF_SIZE            256        // Our recieve buffer

void syscall_error( const char *msg );

int main(int argc, char **argv)
{
	int sockfd;
	int portnum;

	int bytes_read;
	int bytes_written;

	struct sockaddr_in server_addr; 	// Contains the inet address of the server
	struct hostent   * server;			// Defines a host computer on the internet

	char buffer[BUF_SIZE];
	
	
	// Make sure the program was started with enough arguments
	if( argc < 3 ) 
	{
		fprintf(stderr, "ERROR: Not enough arguments specified, usage is %s <hostname> <port>\n", argv[0]);
		exit( EXIT_FAILURE );
	}
	
	// Get our passed port number
	portnum = atoi( argv[2] );
	
	// And our server ip from the hostname
	server  = gethostbyname( argv[1] );
	if( server == NULL )
	{
		fprintf(stderr, "ERROR: Unknown host specified");
		exit( EXIT_FAILURE );
	}
	
	// Create a new IPv4 socket using TCP
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if( sockfd < 0 ) 
	{
		syscall_error("ERROR: Failed to open socket");
	}
	
	// Clear our struct
	memset(&server_addr, 0, sizeof(server_addr));
	
	// Set up our address
	server_addr.sin_family = AF_INET;		  // Address familly IPv4 (AF_INET6 is IPv6)
	server_addr.sin_port   = htons(portnum);  // Port number, converted to network byte order
	
	// Copy the address string to our address
	memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
	
	
	// Now try and connect to the server
	if( connect( sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr) ) < 0)
	{
		syscall_error("ERROR: Unable to connect to server");
	}
	
	printf("Successfully connected to server, sending HELLO\n");
	strncpy(buffer, "HELLO", BUF_SIZE);
	
	// Send our HELLO message
	bytes_written = write( sockfd, buffer, strlen(buffer) );
	if( bytes_written < 0 ) 
	{
		error("ERROR: Unable to write to socket");
	}
	
	// Clear our buffer and try and get our response, read one less byte to keep buffer null terminated
	memset( buffer, 0, sizeof(buffer) );
	bytes_read = read( sockfd, buffer, BUF_SIZE - 1 );
	
	if( bytes_read < 0 )
	{
		error("ERROR: Unable to read from socket");
	}
	
	printf("Recieved message from server: %s\n", buffer);
	
	if( strncmp(buffer, "SUCCESS", sizeof(buffer)) == 0)
	{
		printf("Message successful!\n");
	}
	else
	{
		printf("Message failure!\n");
	}
	
	return EXIT_SUCCESS;
}

/* syscall_error
 *  Handles an error resulting from a system call failure
 **********************/
void syscall_error( const char *msg ) 
{
	perror(msg);
	exit( EXIT_FAILURE );
}
