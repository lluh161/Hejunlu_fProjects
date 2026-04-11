#include "InetAddress.h"

InetAddress::InetAddress(int port){
    addr_.sin_family=AF_INET;
    addr_.sin_port=htons(port);
    addr_.sin_addr.s_addr=INADDR_ANY;
}

