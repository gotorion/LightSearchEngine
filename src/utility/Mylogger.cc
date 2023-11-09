#include <iostream>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>
using std::cout;
using std::endl;
#include "configure.h"
#include "mylogger.h"

Mylogger *Mylogger::_pInstance = nullptr;
Mylogger::Recycle Mylogger::_recycle;
Mylogger *Mylogger::getInstance() {
    if (nullptr == _pInstance) {
        _pInstance = new Mylogger();
    }

    return _pInstance;
}

void Mylogger::destroy() {
    if (_pInstance) {
        delete _pInstance;
        _pInstance = nullptr;
    }
}

Mylogger::Mylogger()
    : _logger(log4cpp::Category::getRoot().getInstance("searchEngine")) {
    using namespace log4cpp;

    // cout << "Mylogger()" << endl;

    //日志的格式
    PatternLayout *ppl = new PatternLayout();
    ppl->setConversionPattern("%d %c [%p] %m%n");

    //日志的目的地

    FileAppender *pfa = new FileAppender(
        "FileAppender",
        Configuration::getInstance()->getConfigMap()["LOG_PATH"]);
    pfa->setLayout(ppl);

    //添加日志目的地到Category
    _logger.addAppender(pfa);

    //日志的优先级
    _logger.setPriority(log4cpp::Priority::DEBUG);
}

Mylogger::~Mylogger() {
    cout << "~Mylogger()" << endl;
    log4cpp::Category::shutdown();
}

void Mylogger::warn(const char *msg) { _logger.warn(msg); }

void Mylogger::error(const char *msg) { _logger.error(msg); }

void Mylogger::info(const char *msg) { _logger.info(msg); }

void Mylogger::debug(const char *msg) { _logger.debug(msg); }

void Mylogger::setPriority(Priority p) {
    switch (p) {
        case WARN:
            _logger.setPriority(log4cpp::Priority::WARN);
            break;
        case ERROR:
            _logger.setPriority(log4cpp::Priority::ERROR);
            break;
        case INFO:
            _logger.setPriority(log4cpp::Priority::INFO);
            break;
        case DEBUG:
            _logger.setPriority(log4cpp::Priority::DEBUG);
            break;
    }
}