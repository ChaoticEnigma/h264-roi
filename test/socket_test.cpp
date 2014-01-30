#include "test.h"

#include "zsocket.h"

int socket_test(){

    ZSocket socket;
    socket.open(ZSocket::tcp, ZSocket::ipv4);
    socket.listen(8080);

    main()
{
   struct sockaddr_in socketInfo;
   char sysHost[MAXHOSTNAME+1];  // Hostname of this computer we are running on
   struct hostent *hPtr;
   int socketHandle;
   int portNumber = 8080;

   signal(SIGCHLD, handleSig);

   bzero(&socketInfo, sizeof(sockaddr_in));  // Clear structure memory

   // Get system information

   gethostname(sysHost, MAXHOSTNAME);  // Get the name of this computer we are running on
   if((hPtr = gethostbyname(sysHost)) == NULL)
   {
      cerr << "System hostname misconfigured." << endl;
      exit(EXIT_FAILURE);
   }

   // create socket

   if((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      close(socketHandle);
      exit(EXIT_FAILURE);
   }

       // Load system information into socket data structures

       socketInfo.sin_family = AF_INET;
       socketInfo.sin_addr.s_addr = htonl(INADDR_ANY); // Use any address available to the system
       socketInfo.sin_port = htons(portNumber);      // Set port number

       // Bind the socket to a local socket address

       if( bind(socketHandle, (struct sockaddr *) &socketInfo, sizeof(struct sockaddr_in)) < 0)
       {
          close(socketHandle);
          perror("bind");
          exit(EXIT_FAILURE);
       }

       listen(socketHandle, 1);

       int socketConnection;
       for(;;)  // infinite loop to handle remote connections. This should be limited.
       {
          if( (socketConnection = accept(socketHandle, NULL, NULL)) < 0)
          {
             close(socketHandle);
             if(errno == EINTR) continue;
             perror("accept");
             exit(EXIT_FAILURE);
          }
          switch(fork())
          {
             case -1:
                perror("fork");
                close(socketHandle);
                close(socketConnection);
                exit(EXIT_FAILURE);
             case 0:   // Child process - do stuff
                close(socketHandle);
                // Do your server stuff like read/write messages to the socket here!
                exit(0);
             default:    // Parent process, look for another connection
                close(socketConnection);
                continue;
          }
       }

    }

}
