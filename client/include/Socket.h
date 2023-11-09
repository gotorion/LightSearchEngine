#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <sys/socket.h>
#include <unistd.h>

#include <string>

class Socket {
   public:
    Socket();
    Socket(int fd);
    ~Socket();

    int fd();

   private:
    int _fd;
};

#endif
