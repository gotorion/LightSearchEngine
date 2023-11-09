cmake_minimum_required(VERSION 3.10)
project(SearchEngine)

# 设置编译选项
# 必须指定C++17，否则无法连接redis++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#添加可执行文件 server
add_executable(SearchServer
    src/server/Acceptor.cc
    src/server/Condition.cc
    src/server/EventLoop.cc
    src/server/InetAddress.cc
    src/server/main.cc
    src/server/MutexLock.cc
    src/server/SearchServer.cc
    src/server/Socket.cc
    src/server/SocketIO.cc
    src/server/TaskQueue.cc
    src/server/TcpConnection.cc
    src/server/TcpServer.cc
    src/server/Thread.cc
    src/server/ThreadPool.cc
    src/server/SearchServer.cc
    src/server/ProtocolParser.cc
    src/server/Timerfd.cc
    src/dict/EditDistance.cc
    src/dict/EnRecommender.cc
    # src/dict/ChDictionary.cc
    src/dict/ChRecommender.cc
    # src/dict/MixedDictionary.cc
    src/dict/MixedRecommender.cc
    src/utility/CppJieba.cc
    src/utility/Mylogger.cc
    src/utility/Configure.cc
    src/utility/WebPageQuery.cc
    src/utility/CacheManager.cc
    src/utility/LRUCache.cc
    src/utility/Time.cc
    src/utility/RedisServer.cc
    src/utility/Query.cc
)

# 添加头文件搜索路径
target_include_directories(SearchServer PRIVATE
    include
    include/server
    include/dict
    include/utility
)

# 链接 pthread 库
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(SearchServer PRIVATE Threads::Threads)
#链接redis—plus-plus
#find_package(hiredis REQUIRED)
#find_package(redis++ REQUIRED)
target_link_libraries(SearchServer PRIVATE hiredis)
target_link_libraries(SearchServer PRIVATE redis++)
target_link_libraries(SearchServer PRIVATE mysqlclient)

# 链接log4cpp库
target_link_libraries(SearchServer PRIVATE log4cpp)

# 指定安装规则
# install(TARGETS Sever DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)
install(TARGETS SearchServer DESTINATION ${PROJECT_SOURCE_DIR}/bin)


 #添加可执行文件
 add_executable(mainPageLib
    src/PageLib/DirScanner.cc
    src/PageLib/PageLibPreprocessor.cc
    src/PageLib/SplitTool.cc
    src/PageLib/WebPage.cc
    src/PageLib/main.cc
    src/PageLib/RssReader.cc
    src/PageLib/tinyxml2.cc
    src/utility/Configure.cc
    src/utility/Time.cc
    src/utility/Query.cc
 )

 # 添加头文件搜索路径
 target_include_directories(mainPageLib PRIVATE
    include/PageLib
    include
    include/simhash
    include/utility
 )

# # 链接log4cpp库
# target_link_libraries(mainPageLib PRIVATE log4cpp)

# # 链接 tinyxml2 库
target_link_libraries(mainPageLib PRIVATE tinyxml2)
target_link_libraries(mainPageLib PRIVATE mysqlclient)
# # 指定安装规则
install(TARGETS mainPageLib DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# 添加可执行文件
add_executable(indexgen
    src/indexgen/dictionary.cc
    src/indexgen/DirScanner.cc
    src/indexgen/main.cc
    src/utility/CppJieba.cc
    src/utility/Configure.cc
    src/utility/Time.cc
    src/utility/Query.cc
)
target_link_libraries(indexgen PRIVATE mysqlclient)
#添加头文件搜索路径
target_include_directories(indexgen PRIVATE
    include/indexgen
    include/utility
    include
)

#指定安装规则
install(TARGETS indexgen DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_executable(client
        client/src/Configuration.cpp
        client/src/Connection.cpp
        client/src/InetAddr.cpp
        client/src/main.cpp
        client/src/ProtocolParser.cpp
        client/src/Socket.cpp
        client/src/SocketIO.cpp
        client/src/TcpClient.cpp
        client/src/TcpConnection.cpp)

target_include_directories(client PRIVATE
    client/include)

install(TARGETS client DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/client/bin)
# # 添加可执行文件
# add_executable(EnDictionary
#     src/Dict/dictionary.cpp
# )

# #添加头文件搜索路径
# target_include_directories(ChDictionary PRIVATE
#     include/Dict
#     include
# )

# #指定安装规则
# install(TARGETS EnDictionary DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)
