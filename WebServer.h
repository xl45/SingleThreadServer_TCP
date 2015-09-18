#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <stdio.h>
#include <sys/time.h>
#include <arpa/inet.h>


#define RECV_BUF_MAX_SIZE 1024*10
// set timeout for persistent connection to 30s
#define TIMEOUT 30

typedef unsigned char BYTE;

class WebServer
{
    public:
        // initialize structs, create fd, bind and listen to the port
        WebServer(const char* port);
        virtual ~WebServer();
        // accept incoming connections if there is any
        void xl45_accept();
        // send and receive data if there is any
        void xl45_send_recv();

    private:
        struct addrinfo hints;
        struct addrinfo *serverinfo;
        struct sockaddr_storage client_addr;
        socklen_t addr_size;
        int serverFD;
        int acceptFD;
        BYTE recv_buffer[RECV_BUF_MAX_SIZE];
        // for timeout set
        struct timeval tv;

        // get the requested file name
        std::string get_req_filename();
        // send bad request
        void bad_req();
        // check connection field (if keep alive)
        bool check_conn_field();
        // close connection
        void serverShutDown();
};

#endif // WEBSERVER_H
