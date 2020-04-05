/**
 * @copyright Copyright [2019]
 * @author pusidun@hotmail.com
 * @file log.h
 * @brief log class
 * @date 2019-11-08
 */
#ifndef SWALLOW_BASE_LOG_H_
#define SWALLOW_BASE_LOG_H_

#include <pthread.h>
#include <time.h>
#include <sstream>
#include <cstdarg>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <vector>
#include <string>
#include "lock.h"
#include "singleton.h"

/**
 * @brief log stream macros
 **/
#define SWALLOW_LOG_LEVEL(logger, level)                \
    if (logger->getLevel() <= level)                    \
    swallow::LogEventWrapper(                           \
        swallow::LogEvent::ptr(new swallow::LogEvent(   \
            logger, level, __FILE__, __LINE__, 0, 456,  \
            123, 0, "thread 0")))                       \
        .getSS()

#define SWALLOW_LOG_DEBUG(logger) SWALLOW_LOG_LEVEL(logger, swallow::LogLevel::DEBUG)
#define SWALLOW_LOG_INFO(logger) SWALLOW_LOG_LEVEL(logger, swallow::LogLevel::INFO)
#define SWALLOW_LOG_WARN(logger) SWALLOW_LOG_LEVEL(logger, swallow::LogLevel::WARN)
#define SWALLOW_LOG_ERROR(logger) SWALLOW_LOG_LEVEL(logger, swallow::LogLevel::ERROR)
#define SWALLOW_LOG_FATAL(logger) SWALLOW_LOG_LEVEL(logger, swallow::LogLevel::FATAL)

/**
 * @brief log C format macros
 **/
#define SWALLOW_LOG_FMT_LEVEL(logger, level, fmt, ...)                              \
    if (logger->getLevel() <= level)                                                \
    swallow::LogEventWrapper(                                                       \
        swallow::LogEvent::ptr(new swallow::LogEvent(                               \
            logger, level, __FILE__, __LINE__, 0, 456,                              \
            123, 0, "thread 0"))).getEvent()->format(fmt, __VA_ARGS__)

#define SWALLOW_LOG_FMT_DEBUG(logger, fmt, ...) \
        SWALLOW_LOG_FMT_LEVEL(logger, swallow::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define SWALLOW_LOG_FMT_INFO(logger, fmt, ...) \
        SWALLOW_LOG_FMT_LEVEL(logger, swallow::LogLevel::INFO, fmt, __VA_ARGS__)
#define SWALLOW_LOG_FMT_WARN(logger, fmt, ...) \
        SWALLOW_LOG_FMT_LEVEL(logger, swallow::LogLevel::WARN, fmt, __VA_ARGS__)
#define SWALLOW_LOG_FMT_ERROR(logger, fmt, ...) \
        SWALLOW_LOG_FMT_LEVEL(logger, swallow::LogLevel::ERROR, fmt, __VA_ARGS__)
#define SWALLOW_LOG_FMT_FATAL(logger, fmt, ...) \
        SWALLOW_LOG_FMT_LEVEL(logger, swallow::LogLevel::FATAL, fmt, __VA_ARGS__)

#define SWALLOW_LOG_ROOT() swallow::LoggerMgr::getInstance()->getRoot()
#define SWALLOW_LOG_GET(logname) swallow::LoggerMgr::getInstance()->getLogger(logname)

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
     * @param[in] time 日志时间 现在换为gettimeofday方式已无用处
     **/
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
             , const char* file, int32_t line, uint32_t elapse
             , uint32_t thread_id, uint32_t coroutine_id, uint64_t time
             , const std::string thread_name);

    const char* getFile() const {
        return m_file;
    }
    int32_t getLine() const {
        return m_line;
    }
    uint32_t getElapse() const {
        return m_elapse;
    }
    uint32_t getThreadId() const {
        return m_threadId;
    }
    uint32_t getCoroutineId() const {
        return m_coroutineId;
    }
    uint64_t getTime() const {
        return m_time;
    }
    const std::string& getThreadName() const {
        return m_threadName;
    }
    std::string getContent() const {
        return m_ss.str();
    }
    std::shared_ptr<Logger> getLogger() const {
        return m_logger;
    }
    LogLevel::Level getLevel() const {
        return m_level;
    }
    std::stringstream& getSS() {
        return m_ss;
    }
    void format(const char* fmt, ...);
    void format(const char* fmt, va_list al);

 private:
    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level = swallow::LogLevel::UNKNOW;
    const char* m_file = nullptr;
    int32_t m_line = 0;
    uint32_t m_elapse = 0;
    uint32_t m_threadId = 0;
    uint32_t m_coroutineId = 0;
    uint64_t m_time = 0;  // not used now
    std::string m_threadName;
    std::stringstream m_ss;
};

class LogEventWrapper {
 public:
    explicit LogEventWrapper(LogEvent::ptr e);
    ~LogEventWrapper();
    LogEvent::ptr getEvent() const {
        return m_event;
    }
    std::stringstream& getSS();

 private:
    LogEvent::ptr m_event;
};

class LogFormatter {
 public:
    typedef std::shared_ptr<LogFormatter> ptr;

    /**
     * @brief pattern
     * m:消息
     * p:日志级别
     * r:累计毫秒数
     * c:日志名称
     * t:线程id
     * n:换行
     * d:时间
     * f:文件名
     * l:行号
     * T:Tab
     * F:协程id
     **/
    explicit LogFormatter(const std::string& pattern);
    std::string format(std::shared_ptr<Logger> logger, \
                        LogLevel::Level level, LogEvent::ptr event);
    std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, \
                        LogLevel::Level level, LogEvent::ptr event);

 public:
    class FormatItem {
     public:
        typedef std::shared_ptr<FormatItem> ptr;

        virtual ~FormatItem() {}
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };

    void init();
    bool isError() const {
        return m_error;
    }
    const std::string getPattern() const {
        return m_pattern;
    }

 private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
    bool m_error = false;  // TODO(pusidun): format error.
};

class LogAppender {
 public:
    typedef std::shared_ptr<LogAppender> ptr;
    friend class Logger;

    virtual ~LogAppender() {}
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    void setFormatter(LogFormatter::ptr val);
    LogFormatter::ptr getFormatter();
    LogLevel::Level getLevel() const {
        return m_level;
    }
    void setLevel(LogLevel::Level level) {
        m_level = level;
    }

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
    explicit FileLogAppender(const std::string &filename);
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
    explicit Logger(const std::string& name = "root");

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
    // void init();
    Logger::ptr getRoot() const {
        return m_root;
    }

 private:
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root;
    MutexLock m_mutex;
};

typedef Singleton<LoggerManager> LoggerMgr;

}  // namespace swallow

#endif  // SWALLOW_BASE_LOG_H_
