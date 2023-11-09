#include "../include/TcpClient.h"
#include "../include/Configuration.h"

int main(void){
    Configuration* pConn=Configuration::getInstance("../conf/client.conf");
    TcpClient client(*pConn);
    client.start();

    return 0;
}
