#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

using std::map;
using std::string;

class Configuration {
   public:
    static Configuration *getInstance(const string &confPath);
    map<string, string> getConfMap();
    static void destroy();

   private:
    Configuration() = default;
    Configuration(const string &confPath);
    Configuration(const Configuration &rhs) = delete;

   private:
    static Configuration *_pConf;
    map<string, string> _confMap;
};

#endif
