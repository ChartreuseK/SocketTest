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


#define  BUF_SIZE            256        // Our recieve buffer
#define  MAX_BACKLOG_SIZE	   1		// Have the system queue 1 connection

void syscall_error( const char *msg );

int main(int argc, char **argv)
{
	int sockfd;
	int client_sockfd;
	int portnum;
	int clientlen;
	int bytes_read;
	int bytes_written;
	
	char buffer[BUF_SIZE];
	
	struct sockaddr_in server_addr;		// Contains the inet address of the server
	struct sockaddr_in client_addr;     // Contains the inet address of the client
	
	// Make sure the program was started with enough arguments
	if( argc < 2 ) 
	{
		fprintf(stderr, "ERROR: No port number provided, usage is %s <port>", argv[0]);
		exit( EXIT_FAILURE );
	}
	
	// Get our passed port number
	portnum = atoi( argv[1] );
	
	clientlen = sizeof(client_addr);
	
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
	
	server_addr.sin_addr.s_addr = INADDR_ANY; // Since we're a server bind to our own IP
	
	
	// Try and bind our address to our existing socket
	if( bind( sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0 )
	{
		syscall_error("ERROR: Failed to bind address to socket");
	}
	
	// Allow us to listen to the socket for connections
	listen( sockfd, MAX_BACKLOG_SIZE );
	
	// Block until we get a connection
	if( (client_sockfd = accept( sockfd, (struct sockaddr*) &client_addr, &clientlen )) < 0 )
	{
		syscall_error("ERROR: Failed to accept client connection");
	}
	
	// We now have a client!
	while( client_sockfd ) 
	{
		printf("Incoming connection from %s - Waiting for hello\n", inet_ntoa(client_addr.sin_addr));
		
		// Clear our buffer and read one less byte than our size so we are null terminated
		memset(buffer, 0, sizeof(buffer));
		bytes_read = read( client_sockfd, buffer, BUF_SIZE - 1);
		
		if( bytes_read < 0 ) 
		{
			syscall_error("ERROR: Unable to read from socket");
		}
		
		// We got a message from the client
		printf("Recieved message from client: %s\n", buffer);
		
		if( strncmp(buffer, "HELLO", BUF_SIZE) ) 
		{
			printf("Recieved HELLO from client, responding SUCCESS\n");
			bytes_written = write( client_sockfd, "SUCCESS", strlen("SUCCESS") );
		}
		else
		{
			printf("Unknown response from client, responding FAILURE\n");
			bytes_written = write( client_sockfd, "FAILURE", strlen("FAILURE") );
		}
		
		if( bytes_written < 0 ) 
		{
			error("ERROR: Unable to write to socket");
		}
	
	
		// We're done with this client, close the connection and get a new client
		close(client_sockfd);
		
		if( (client_sockfd = accept( sockfd, (struct sockaddr*) &client_addr, &clientlen )) < 0 )
		{
			syscall_error("ERROR: Failed to accept client connection");
		}
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
