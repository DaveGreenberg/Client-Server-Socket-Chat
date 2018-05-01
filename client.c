// David Greenberg
// dxg347
// Usage: client ipaddress
// Example: client 127.0.0.1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <math.h>

#define TRUE 1
#define BUFFER_SIZE 512
#define port 9999

// Helper function to conveniently print to stderr AND exit (terminate)
void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    // Check for proper number of commandline arguments
    // Expect program name in argv[0], IP address in argv[1],
    if (argc < 2) 
	{
       fprintf(stderr,"usage %s hostname\n", argv[0]);
       exit(0);
    }

    // Get socket
    int portno = (port);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    // Set up for connect()
    struct hostent *server;
	//char sendline[BUFFER_SIZE];
    //char recvline[BUFFER_SIZE];
    server = gethostbyname(argv[1]);
    if (server == NULL) 
	{
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    // Make connection
    if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
	printf("Connected to Server\n");
	
    //Set up pid for forking
	pid_t pid;
	char buffer[BUFFER_SIZE];  //How we send messages
	bzero(buffer, sizeof(buffer));
	int n; //Error catch send/receives
	
	pid = fork();
	
	if (pid > 0) //Parent: handle incoming messages
	{
	  while(TRUE)
	  {

		recv(sockfd, buffer, BUFFER_SIZE, 0);  //Receive from server
		
	    if (sizeof(buffer) != 0) 	//Print what server sent
		{
		  if (strcmp(buffer, "exit\n") == 0)  //Server wants to exit chat
		  {
		    bzero(buffer, sizeof(buffer));
		    printf("Disconnected from Server \n");
		    fflush(stdout);
			sprintf(buffer, "exit\n"); //We need to send 'exit' here so Server knows to quit
			n = write(sockfd, buffer, sizeof(buffer));
		    close(sockfd);
		    return 0;
		  }
		  else
		  {
		    printf("Server: %s\n", buffer);
		    fflush(stdout);
		    bzero(buffer, sizeof(buffer));
		  }
		}
	  }
	}
	else //Child: handle outgoing messages
	{
	  while(TRUE)
	  {
		//Prepare a message to send to server
	    bzero(buffer, strlen(buffer));
		fgets(buffer, sizeof(buffer), stdin);
		
        n = write(sockfd, buffer, sizeof(buffer));
		if (n < 0) 
          error("ERROR writing to socket");
	    if (strcmp(buffer, "exit\n") == 0)  //Client wants to exit chat
		{
		  bzero(buffer, sizeof(buffer));
		  fflush(stdout);
		  return 0;
		}
		bzero(buffer, strlen(buffer));	
	  }
	}
    //close(sockfd);
    return 0;
}
