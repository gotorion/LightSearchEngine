#ifndef __SOCKETIO_H__
#define __SOCKETIO_H__

#include <string>

using std::string;

class SocketIO {
   public:
    explicit SocketIO(int fd);
    ~SocketIO();

    int fd();

    int readN(char* buf, int len);

    int writeN(const char* buf, int len);

    //收消息
    string receive();

    //发消息
    int send(const string& str);

   private:
    int _fd;
};

#endif
