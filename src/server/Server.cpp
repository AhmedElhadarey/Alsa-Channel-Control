// alsa-control-server
// shane tully (shane@shanetully.com)
// shanetully.com
// https://github.com/shanet/Alsa-Channel-Control

#include "Server.h"

Server::Server(int port, int backlog) {
   // Check that the port is within the valid range
   this->port = (port > 0 && port <= 65535) ? port : -1;

   // Default backlog if not valid
   this->backlog = (backlog > 0) ? backlog : DEFAULT_BACKLOG;

   numClients = 0;
}

Server::Server() {
   numClients = 0;
   port = -1;
   backlog = DEFAULT_BACKLOG;
   listSock = -1;
   serverInfo = NULL;
}

Server::~Server() {
   freeaddrinfo(serverInfo);
   close(listSock);
}


int Server::start(int aiFamily, int aiFlags) {
   // Get server info
   if(getAddressInfo(aiFamily, aiFlags) == FAILURE) {
     return FAILED_TO_GET_ADDR_INFO;
   }

   // Bind to socket
   if(bind() == FAILURE) {
      return FAILED_TO_BIND;
   }

   // Listen on given socket
   if(listen(listSock, backlog) == FAILURE) {
      return FAILED_TO_LISTEN;
   }

   // Yay!
   return SUCCESS;
}


int Server::getAddressInfo(int aiFamily, int aiFlags) {
   stringstream ss;
   struct addrinfo hints;

   // Convert port to a string so it can be passed as a char* below
   ss << port;

   // Make sure hints memory is clear
   memset(&hints, 0, sizeof hints);

   hints.ai_family = aiFamily;      // IPv4 or IPv6?
   hints.ai_flags = aiFlags;        // IP to use?
   hints.ai_socktype = SOCK_STREAM; // Always use sock stream

   // Get the list of results (res)
   return (getaddrinfo(NULL, ss.str().c_str(), &hints, &serverInfo) == SUCCESS) ? SUCCESS : FAILURE;
}


int Server::bind() {
   int sockOpt = 1;
   addrinfo *tmpServerInfo;

   // Traverse list of results and bind to first socket possible
   for(tmpServerInfo = serverInfo; tmpServerInfo != NULL; tmpServerInfo = tmpServerInfo->ai_next) {
      // Try to get socket
      if((listSock = socket(tmpServerInfo->ai_family, tmpServerInfo->ai_socktype, tmpServerInfo->ai_protocol)) == FAILURE) {
         continue;
      }

      // Allow reuse of port
      if(setsockopt(listSock, SOL_SOCKET, SO_REUSEADDR, &sockOpt, sizeof(int)) == FAILURE) {
         continue;
      }

      // Try to bind to socket
      if(::bind(listSock, tmpServerInfo->ai_addr, tmpServerInfo->ai_addrlen) == FAILURE) {
         // Close the opened socket
         close(listSock);
         continue;
      }

      // No problems above? We're all set up. Move on!
      break;
   }

   // If tmpServerInfo is null, we failed to bind
   if(tmpServerInfo == NULL) {
      listSock = FAILURE;
      return FAILURE;
   } else {
      return SUCCESS;
   }
}


Client Server::acceptConnection() {
   sockaddr_storage clientInfo;
   socklen_t clientInfoSize = sizeof clientInfo;

   int connSock = accept(listSock, (sockaddr *)&clientInfo, &clientInfoSize);

   if(connSock != FAILURE) {
      numClients++;
   }
   
   Client newClient(connSock, clientInfo, numClients);

   return newClient;
}


void Server::stop() {
   close(listSock);
}


/*int Server::send(int connSock, string data) {
   return ::send(connSock, data.c_str(), data.length(), 0);
}


int Server::receive(int connSock, string *reply) {
   char *tmpReply = new char[BUFFER];

   int recvLen = recv(connSock, tmpReply, BUFFER, 0);

   // Add null terminator only if recvLen is >= 0 to avoid going out of bounds
   if(recvLen >= 0) {
      tmpReply[recvLen] = '\0';
      *reply = tmpReply;
   }

   delete tmpReply;

   return recvLen;
}

  
string Server::getIPAddress(sockaddr_storage *connAddr) {
	// Make the IP long enough for IPv6 addresses, even though we currently only support IPv4
   char ip[INET6_ADDRSTRLEN];

   inet_ntop(connAddr->ss_family, &((sockaddr_in*)connAddr)->sin_addr, ip, sizeof ip);

   return string(ip);
}*/

void Server::setPort(int port) {
   this->port = port;
}

int Server::getListSock() const {
   return listSock;
}


int Server::getPort() const {
	return port;
}