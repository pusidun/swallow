/**
 * @copyright Copyright [2019] 
 * @author pusidun@hotmail.com
 * @file log.h
 * @brief log class
 * @date 2019-11-08
 */
#ifndef SWALLOW_BASE_LOG_H_
#define SWALLOW_BASE_LOG_H_

#include <cstdarg>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <vector>
#include <string>
#include <pthread.h>
#include <sstream>
#include "lock.h"

namespace swallow {

class Logger;

class LogLevel {
 public:
    /**
    * @brief loglevel
    */
    enum Level {
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    static const char* ToString(LogLevel::Level level);
};

class LogEvent {
 public:
    typedef std::shared_ptr<LogEvent> ptr;

    /**
     * @brief ctor
     **/
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
             ,const char* file, int32_t line, uint32_t elapse
             ,uint32_t thread_id, uint32_t coroutine_id, uint64_t time
             ,const std::string thread_name);

    const char* getFile() const { return m_file;}
    int32_t getLine() const { return m_line;}
    uint32_t getElapse() const { return m_elapse;}
    uint32_t getThreadId() const { return m_threadId;}
    uint32_t getCoroutineId() const { return m_coroutineId;}
    uint64_t getTime() const { return m_time;}
    const std::string& getThreadName() const { return m_threadName;}
    std::string getContent() const { return m_ss.str(); }
    std::shared_ptr<Logger> getLogger() const { return m_logger;}
    LogLevel::Level getLevel() const { return m_level;}
    std::stringstream& getSS() { return m_ss;}
    void format(const char* fmt, ...);
    void format(const char* fmt, va_list al);
 private:
    const char* m_file = nullptr;
    int32_t m_line = 0;
    uint32_t m_elapse = 0;
    uint32_t m_threadId = 0;
    uint32_t m_coroutineId = 0;
    uint64_t m_time = 0;
    std::string m_threadName;
    std::stringstream m_ss;
    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level;
};

class LogEventWrapper {
 public:
    LogEventWrapper(LogEvent::ptr e);
    ~LogEventWrapper();
    LogEvent::ptr getEvent() const { return m_event; }
    std::stringstream& getSS();

 private:
    LogEvent::ptr m_event;
};

class LogFormatter {
 public:
    typedef std::shared_ptr<LogFormatter> ptr;

    LogFormatter(const std::string& pattern);
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

 public:
    class FormatItem {
     public:
        typedef std::shared_ptr<FormatItem> ptr;

        virtual ~FormatItem() {}
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };

    void init();
    bool isError() const { return m_error; }
    const std::string getPattern() const { return m_pattern; }
 private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
    bool m_error = false;  // TODO
};

class LogAppender {
 public:
    typedef std::shared_ptr<LogAppender> ptr;
    friend class Logger;

    virtual ~LogAppender() {}
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    void setFormatter(LogFormatter::ptr val);
    LogFormatter::ptr getFormatter();
    LogLevel::Level getLevel() const { return m_level; }
    void setLevel(LogLevel::Level level) { m_level = level; }

 protected:
    LogLevel::Level m_level = LogLevel::DEBUG;
    bool m_hasFormatter = false;
    MutexLock m_mutex;
    LogFormatter::ptr m_formatter;
};

class StdoutLogAppender: public LogAppender {
 public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

class FileLogAppender: public LogAppender {
 public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string &filename);
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
    bool reopen();
 private:
    std::string m_filename;
    std::ofstream m_filestream;
    // last open time
    uint64_t m_lastTime = 0;
};

class Logger: public std::enable_shared_from_this<Logger> {
 friend class LoggerManager;
 public:
    typedef std::shared_ptr<Logger> ptr;

    /**
    * @brief ctor
    * @param[in] name logger name
    */
    Logger(const std::string& name = "root");

    /**
    * @brief write log
    * @param[in] level loglevel
    * @param[in] event logevent
    */
    void log(LogLevel::Level level, LogEvent::ptr event);
    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    void clearAppender();
    void setFormatter(LogFormatter::ptr val);
    void setFormatter(const std::string& val);
    LogFormatter::ptr getFormatter();
    const std::string getName() const;
    void setLevel(LogLevel::Level);
    LogLevel::Level getLevel() const;

 private:
    std::string m_name;
    LogLevel::Level m_level = LogLevel::DEBUG;
    MutexLock m_mutex;
    std::list<LogAppender::ptr> m_appenders;
    LogFormatter::ptr m_formatter;
    Logger::ptr m_root;
};

class LoggerManager {
 public:
    LoggerManager() {
        m_root.reset(new Logger);
        m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
        m_loggers[m_root->m_name] = m_root;
    }
    Logger::ptr getLogger(const std::string& name);
    //void init();
    Logger::ptr getRoot() const { return m_root; }

 private:
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root;
    MutexLock m_mutex;
};

}  // namespace swallow

 #endif  // SWALLOW_BASE_LOG_H_
