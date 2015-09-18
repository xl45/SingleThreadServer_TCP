#include <iostream>
#include "WebServer.h"


int main(){
    WebServer server = WebServer("3480");
    server.xl45_accept();
    server.xl45_send_recv();

    return 0;
}
