/**
 * @copyright Copyright [2019] 
 * @author pusidun@hotmail.com
 * @file log.h
 * @brief log class
 * @date 2019-11-08
 */

#include <map>
#include <vector>
#include <ctime>
#include <functional>
#include <iostream>
#include <cstdio>
#include "log.h"

namespace swallow {

const char* LogLevel::ToString(LogLevel::Level level) {
    switch(level) {
        case LogLevel::DEBUG: return "DEBUG";break;
        case LogLevel::INFO: return "INFO";break;
        case LogLevel::WARN: return "WARN";break;
        case LogLevel::ERROR: return "ERROR";break;
        case LogLevel::FATAL: return "FATAL";break;
        default:
            return "UNKNOWN";
    }

    return "UNKNOWN";
}

/**
 * @brief LogEvent Impl
 */
LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
             , const char* file, int32_t line, uint32_t elapse
             , uint32_t thread_id, uint32_t coroutine_id, uint64_t time
             , const std::string thread_name)
             :m_logger(logger) ,m_level(level),m_file(file), m_line(line), m_elapse(elapse),
             m_threadId(thread_id), m_coroutineId(coroutine_id),
             m_time(time), m_threadName(thread_name) {}

void LogEvent::format(const char* fmt, ...) {
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}

void LogEvent::format(const char* fmt, va_list al) {
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if (len != 1) {
        m_ss << std::string(buf, len);
        free(buf);
    }
}

/** 
 * @brief: LogEventWrapper Impl
 */
LogEventWrapper::LogEventWrapper(LogEvent::ptr e):m_event(e) {}

LogEventWrapper::~LogEventWrapper() {
    m_event->getLogger()->log(m_event->getLevel(), m_event);
}

std::stringstream& LogEventWrapper::getSS() {
    return m_event->getSS();
}

/** 
 * @brief: LogFormatter Impl
 */
LogFormatter::LogFormatter(const std::string& pattern):m_pattern(pattern) {
        init(); 
}

class MessageFormatItem : public LogFormatter::FormatItem {
 public:
    MessageFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getContent();
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem {
 public:
    ElapseFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getElapse();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem {
 public:
    LevelFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << LogLevel::ToString(level);
    }
};

class NameFormatItem: public LogFormatter::FormatItem {
 public:
    NameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getLogger()->getName();
    }
};

class ThreadIdFormatItem: public LogFormatter::FormatItem {
 public:
    ThreadIdFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getThreadId();
    }
};

class CoroutineIdItem: public LogFormatter::FormatItem {
 public:
    CoroutineIdItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getCoroutineId();
    }
};

class DataTimeFormatItem: public LogFormatter::FormatItem {
 public:
    DataTimeFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        struct tm tm;
        std::string format = "%Y-%m-%d %H:%M:%S";
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), format.c_str(), &tm);
        os << buf;
    }
};

class FilenameFormatItem: public LogFormatter::FormatItem {
 public:
    FilenameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getFile();
    }
};

class LineFormatItem: public LogFormatter::FormatItem {
 public:
    LineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getLine();
    }
};

class NewLineFormatItem: public LogFormatter::FormatItem {
 public:
    NewLineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << std::endl;
    }
};

class StringFormatItem: public LogFormatter::FormatItem {
 public:
    StringFormatItem(const std::string& str = ""):m_str(str) {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << m_str;
    }
 private:
    std::string m_str;
};

class TabFormatItem: public LogFormatter::FormatItem {
 public:
    TabFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << "\t";
    }
};

void LogFormatter::init() {
    static std::map<std::string, std::function<FormatItem::ptr(const std::string&)> >
        mapFormat_items = {
            {"m", [](const std::string& fmt){ return FormatItem::ptr(new MessageFormatItem(fmt)); } },
            {"r", [](const std::string& fmt){ return FormatItem::ptr(new ElapseFormatItem(fmt)); } },
            {"p", [](const std::string& fmt){ return FormatItem::ptr(new LevelFormatItem(fmt)); } },
            {"c", [](const std::string& fmt){ return FormatItem::ptr(new NameFormatItem(fmt)); } },
            {"t", [](const std::string& fmt){ return FormatItem::ptr(new ThreadIdFormatItem(fmt)); } },
            {"F", [](const std::string& fmt){ return FormatItem::ptr(new CoroutineIdItem(fmt)); } },
            {"d", [](const std::string& fmt){ return FormatItem::ptr(new DataTimeFormatItem(fmt)); } },
            {"f", [](const std::string& fmt){ return FormatItem::ptr(new FilenameFormatItem(fmt)); } },
            {"l", [](const std::string& fmt){ return FormatItem::ptr(new LineFormatItem(fmt)); } },
            {"n", [](const std::string& fmt){ return FormatItem::ptr(new NewLineFormatItem(fmt)); } },
            {"T", [](const std::string& fmt){ return FormatItem::ptr(new TabFormatItem(fmt)); } }
        };

    //%d%T%t%T%F%T%p%T%c%T%f:%l%T%m%n
    std::vector<std::pair<int, char>> vecItem;
    for(size_t idx=0; idx < m_pattern.size(); idx++) {
        if(m_pattern[idx] == '%') {
            vecItem.push_back(std::make_pair(0, m_pattern[idx+1]));
            ++idx;
        }
        else {
            vecItem.push_back(std::make_pair(1, m_pattern[idx]));
        }
    }

    for(auto &i: vecItem) {
        int type = i.first;
        std::string item(1, i.second);
        if(type == 1) {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(item)));
        }
        else {
            auto it = mapFormat_items.find(item);
            if( it != mapFormat_items.end() )
                m_items.push_back(it->second(item));
            else
                m_items.push_back(FormatItem::ptr(new StringFormatItem(item)));
        }
    }
}

std::string LogFormatter::format(std::shared_ptr<Logger> logger,
                                 LogLevel::Level level, LogEvent::ptr event) {
    std::stringstream ss;
    for (auto &i : m_items) {
        i->format(ss, logger, level, event);
    }
    return ss.str();
}

/**
 * @brief LogAppender impl
 */
void LogAppender::setFormatter(LogFormatter::ptr val){
    MutexLockGuard guard(m_mutex);
    m_formatter = val;

    if(m_formatter)
        m_hasFormatter = true;
    else
        m_hasFormatter = false;
}

LogFormatter::ptr LogAppender::getFormatter() {
    MutexLockGuard guard(m_mutex);
    return m_formatter;
}

void StdoutLogAppender::log(Logger::ptr logger, LogLevel::Level level,
                            LogEvent::ptr event) {
    if (level >= LogAppender::m_level) {
        std::cout << LogAppender::m_formatter->format(logger, level, event);
    }
}

FileLogAppender::FileLogAppender(const std::string &filename)
    : m_filename(filename) {
    reopen();
}

void FileLogAppender::log(Logger::ptr logger, LogLevel::Level level,
                          LogEvent::ptr event) {

    if (level >= LogAppender::m_level) {
        uint64_t now = time(0);
        //每一秒重新打开一次文件，保证在日志文件被删除的情况下可以重新创建文件
        if (now != m_lastTime) {
            reopen();
            m_lastTime = now;
        }
        MutexLockGuard guard(m_mutex);
        m_filestream << LogAppender::m_formatter->format(logger, level, event);
    }
}

bool FileLogAppender::reopen() {
    MutexLockGuard guard(m_mutex);
    if (m_filestream) {
        m_filestream.close();
    }

    m_filestream.open(m_filename, std::ios::app);
    return !!m_filestream;
}

/**
 * @brief Logger Impl
 */
Logger::Logger(const std::string &name)
    : m_name(name)
    , m_level(LogLevel::DEBUG) {
    m_formatter.reset(
        new LogFormatter("%d%T%t%T%F%T%p%T%c%T%f:%l%T%m%n"));
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        auto self = shared_from_this();
        MutexLockGuard guard(m_mutex);
        if (!m_appenders.empty()) {
            for (auto &i: m_appenders)
                i->log(self, level, event);
        } else if (m_root) {
            m_root->log(level, event);
        }
    }
}

void Logger::debug(swallow::LogEvent::ptr event) { log(LogLevel::DEBUG, event); }

void Logger::info(swallow::LogEvent::ptr event) { log(LogLevel::INFO, event); }

void Logger::warn(swallow::LogEvent::ptr event) { log(LogLevel::WARN, event); }

void Logger::error(swallow::LogEvent::ptr event) { log(LogLevel::ERROR, event); }

void Logger::fatal(swallow::LogEvent::ptr event) { log(LogLevel::FATAL, event); }

void Logger::addAppender(LogAppender::ptr appender) {
    MutexLockGuard guard(m_mutex);
    if (!appender->getFormatter()) {
        appender->setFormatter(m_formatter);
    }
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender) {
    MutexLockGuard guard(m_mutex);
    for (auto itr = m_appenders.begin(); itr != m_appenders.end(); ++itr) {
        if (*itr == appender) {
            m_appenders.erase(itr);
        }
    }
}

void Logger::clearAppender() {
    MutexLockGuard guard(m_mutex);
    m_appenders.clear();
}

void Logger::setFormatter(LogFormatter::ptr val) {
    MutexLockGuard guard(m_mutex);
    m_formatter = val;

    for(auto& i: m_appenders) {
        MutexLockGuard iguard(i->m_mutex);
        if(!i->m_hasFormatter)
            i->m_formatter = m_formatter;
    }
}

void Logger::setFormatter(const std::string& val) {
    LogFormatter::ptr fmt(new LogFormatter(val));
    if (fmt->isError()) {
        return;
    }
    setFormatter(fmt);
}

LogFormatter::ptr Logger::getFormatter() {
    MutexLockGuard guard(m_mutex);
    return m_formatter;
}

const std::string Logger::getName() const {
    return m_name;
}

void Logger::setLevel(LogLevel::Level level) {
    m_level = level;
}

LogLevel::Level Logger::getLevel() const {
    return m_level;
}

Logger::ptr LoggerManager::getLogger(const std::string& name) {
    MutexLockGuard guard(m_mutex);
    auto it = m_loggers.find(name);
    if(it != m_loggers.end()) {
        return it->second;
    }

    Logger::ptr logger(new Logger(name));
    logger->m_root = m_root;
    m_loggers[name] = logger;
    return logger;
}

}  // namespace swallow 
