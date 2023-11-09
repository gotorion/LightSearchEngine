#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include "InetAddr.h"
#include "Socket.h"

class Connection {
   public:
    Connection(const string &ip, unsigned short port);
    ~Connection();

    int fd();

   private:
    Socket _sock;
    InetAddr _addr;
};

#endif
