#ifndef __CONFIGURE__
#define __CONFIGURE__
#include <iostream>
using std::string;
#include <set>
using std::set;
#include <map>
using std::map;

class Configuration {
   private:
    string _filepath;
    map<string, string> _configMap;
    set<string> _stopWordList;
    static Configuration* _confPtr;

    Configuration(const string& filepath);
    Configuration(const Configuration& configuration) = delete;
    Configuration& operator=(const Configuration& configuration) = delete;

   public:
    map<string, string>& getConfigMap();
    set<string>& getStopWordList();
    static Configuration* getInstance();
    string getItem(const string& key);
};

#endif