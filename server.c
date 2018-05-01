// David Greenberg
// dxg347
// Usage: server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <math.h>

#define TRUE 1
#define BUFFER_SIZE 512
#define port 9999

// Helper function to conveniently print to stderr AND exit (terminate)
void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) 
{

    // Setup phase
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");
    struct sockaddr_in serv_addr; // Server address struct
    bzero((char *) &serv_addr, sizeof(serv_addr));
    int portno = (port); // Port number is commandline argument
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0) 
         error("ERROR on binding");  //Getting permission denied
    listen(sockfd, 5);
	 

    // Service phase
    struct sockaddr_in cli_addr;
    printf("Waiting for client...\n");
    socklen_t clilen = sizeof(cli_addr); // Address struct length
    int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	printf("Client Connected\n");
    if (newsockfd < 0) 
          error("ERROR on accept");
	  
	//Set up pid for forking
	pid_t pid;
	char buffer[BUFFER_SIZE];	//This is how we send our messages
	bzero(buffer, sizeof(buffer));
	int n;  //Used to error catch our sends/receives
	
	pid = fork();
	
	if (pid > 0) //Parent: handle incoming messages
	{
	  while(TRUE)
	  {
		  
	    recv(newsockfd, buffer, BUFFER_SIZE, 0);  //Receive from client
		
		if (sizeof(buffer) != 0)  //Read buffer as normal
		{
		  if (strcmp(buffer, "exit\n") == 0)  //Client wants to exit chat
		  {
		    bzero(buffer, sizeof(buffer));
		    printf("Disconnected from Client \n");
		    fflush(stdout);
			sprintf(buffer, "exit\n");  //Send 'exit' so client knows to quit
			n = write(newsockfd, buffer, sizeof(buffer));
		    close(newsockfd);
		    close(sockfd);
		    return 0;
		  }
		  else
		  {
		    printf("Client: %s\n", buffer);
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
		//Prepare a message to send to client
	    bzero(buffer, strlen(buffer));
		fgets(buffer, sizeof(buffer), stdin);
		
        n = write(newsockfd, buffer, sizeof(buffer));
		if (n < 0) 
          error("ERROR writing to socket");
	    if (strcmp(buffer, "exit\n") == 0)  //Server wants to exit chat
		{
		  bzero(buffer, sizeof(buffer));
		  close(newsockfd);
		  close(sockfd);
		  return 0;
		}
		bzero(buffer, strlen(buffer));
	  }
	}
	
	
    //close(newsockfd);
    //close(sockfd);
    return 0; 
}
