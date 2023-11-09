#include "CacheManager.h"
#include "SearchServer.h"
#include "configure.h"
#include "mylogger.h"
int main() {
    auto conf = Configuration::getInstance()->getConfigMap();
    SearchServer server;
    // TODO: SearchServer里进一步封装CacheManager
    CacheManager::getInstance()->initCache(
        atoi(conf["THREAD_POOL_SIZE"].c_str()), conf["LRU_BAK_PATH"]);
    server.start();
}