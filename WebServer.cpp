#include "WebServer.h"

// important!!
// for function comments please refer .h files

WebServer::WebServer(const char* port){
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status;
    // getaddrinfo
    if( (status = getaddrinfo(NULL, port, &hints, &serverinfo)) != 0 ){
        std::cout << "getaddrinfo() function error:" << gai_strerror(status) << std::endl;
        exit(1);
    }
    // get the fd
    if( (serverFD = socket(serverinfo->ai_family, serverinfo->ai_socktype, serverinfo->ai_protocol)) == -1){
        std::cout << "create server socket and get serverFD error!" << std::endl;
        exit(1);
    }
    // bind to port
    if( bind(serverFD, serverinfo->ai_addr, serverinfo->ai_addrlen) == -1 ){
        std::cout << "bind() error!" << std::endl;
        exit(1);
    }
    // listen to port
    if( listen(serverFD, 10) == -1 ){
        std::cout << "listen() error!" << std::endl;
        exit(1);
    }
}


void WebServer::xl45_accept(){
    addr_size = sizeof(client_addr);

    // only handles one incoming HTTP connection at a time
    if( (acceptFD = accept(serverFD, (struct sockaddr *)&client_addr, &addr_size)) == -1 ){
        std::cout << "server accept() error!" << std::endl;
        exit(1);
    }

    // for print
    struct sockaddr_in *temp = (struct sockaddr_in *)& client_addr;
    char tempStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, temp, tempStr, INET_ADDRSTRLEN);
    std::cout << "****server receive incoming connection: " << tempStr << std::endl;

    // set timeout
    if( (setsockopt(acceptFD, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval))) == -1 ){
        perror("setsockopt: ");
        exit(1);
    }
}


void WebServer::xl45_send_recv(){
    while(1){
        memset(recv_buffer, 0, RECV_BUF_MAX_SIZE);

        if( recv(acceptFD, &recv_buffer, RECV_BUF_MAX_SIZE, 0) == -1 ){
            std::cout << "server recv() error!" << std::endl;
            exit(1);
        }

        std::string req_filename = get_req_filename();
        std::cout << "HTTP request: GET " << req_filename << std::endl;

        FILE * pFile;

        if( (pFile = fopen(req_filename.c_str() ,"r")) == NULL ){
            // 404 not found
            std::string page = "404 not found!";

            if( (send(acceptFD, page.c_str(), page.length(), 0)) == -1 ){
                std::cout << "server send() error!" << std::endl;
                exit(1);
            }
        }else{
            // obtain file size:
            fseek (pFile , 0 , SEEK_END);
            long file_size = ftell(pFile);
            rewind (pFile);
            // read the file into buffer
            BYTE buffer[file_size];
            fread(buffer, 1, file_size, pFile);
            // send the file
            if( (send(acceptFD, buffer, file_size, 0)) == -1 ){
                std::cout << "server send() error!" << std::endl;
                exit(1);
            }
            fclose(pFile);
        }

        if( check_conn_field() ){
            continue;
        }else{
            serverShutDown();
        }
    }
}


std::string WebServer::get_req_filename(){

    std::string whole_msg((const char*)recv_buffer);
    // std::cout << "whole_msg: \n" << whole_msg << std::endl;

    std::string substr1("GET /");
    std::string substr2(" HTTP/1.1");

    std::size_t found1 = whole_msg.find(substr1);
    std::size_t found2 = whole_msg.find(substr2);

    if( (int)found1 == -1 || (int)found2 == -1 ){
        bad_req();
    }

    std::string req_filename = whole_msg.substr(found1+substr1.length(), found2-found1-substr1.length());
    // std::cout << "req_filename:*" << req_filename << "*" << std::endl;

    return req_filename;
}



bool WebServer::check_conn_field(){
    std::string whole_msg((const char*)recv_buffer);
    std::string substr1("keep-alive");

    std::size_t found1 = whole_msg.find(substr1);

    if( (int)found1 == -1 ){
        return false;
    }else{
        return true;
    }
}


void WebServer::bad_req(){
    std::cout << "400 bad request, shut down now!\n";
    exit(1);
}


void WebServer::serverShutDown(){
    close(serverFD);
    close(acceptFD);
    std::cout << "service finish, server shut down now.\n";
    exit(1);
}


WebServer::~WebServer(){

}



