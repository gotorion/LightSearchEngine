#include "../include/Configuration.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>

using std::ifstream;
using std::cerr;
using std::endl;
using std::istringstream;

Configuration* Configuration::_pConf=nullptr;

Configuration::Configuration(const string &confPath)
{
    ifstream ifs(confPath);
    if(!ifs.good()){
        cerr<<"Configuration: Failed to open file"<<endl;
        exit(1);
    }

    string line;
    while(getline(ifs,line)){
        istringstream iss(line);
        string key;
        string value;
        while(iss>>key>>value){
            _confMap[key]=value;
        }
    }
}

void Configuration::destroy(){
    if(_pConf){
        delete _pConf;
        _pConf=nullptr;
    }
}

map<string,string> Configuration::getConfMap(){
    return _confMap;
}

Configuration* Configuration::getInstance(const string &confPath){
    if(_pConf == nullptr){
        atexit(destroy);
        _pConf = new Configuration(confPath);
    }
    return _pConf;
}
