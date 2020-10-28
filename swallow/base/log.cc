/**
 * @copyright Copyright [2019]
 * @author pusidun@hotmail.com
 * @file log.cc
 * @brief log class
 * @date 2019-11-08
 */
#include "swallow/base/log.h"

#include <sys/time.h>

#include <cstdio>
#include <ctime>
#include <functional>
#include <iostream>
#include <map>
#include <utility>
#include <vector>

namespace swallow {

const char* LogLevel::ToString(LogLevel::Level level) {
  switch (level) {
    case LogLevel::DEBUG:
      return "DEBUG";
      break;
    case LogLevel::INFO:
      return "INFO";
      break;
    case LogLevel::WARN:
      return "WARN";
      break;
    case LogLevel::ERROR:
      return "ERROR";
      break;
    case LogLevel::FATAL:
      return "FATAL";
      break;
    default:
      return "UNKNOWN";
  }

  return "UNKNOWN";
}

/**
 * @brief LogEvent Impl
 */
LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,
                   const char* file, int32_t line, uint32_t elapse,
                   uint32_t thread_id, uint32_t coroutine_id, uint64_t time,
                   const std::string thread_name)
    : m_logger(logger),
      m_level(level),
      m_file(file),
      m_line(line),
      m_elapse(elapse),
      m_threadId(thread_id),
      m_coroutineId(coroutine_id),
      m_time(time),
      m_threadName(thread_name) {}

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
LogEventWrapper::LogEventWrapper(LogEvent::ptr e) : m_event(e) {}

LogEventWrapper::~LogEventWrapper() {
  m_event->getLogger()->log(m_event->getLevel(), m_event);
}

std::stringstream& LogEventWrapper::getSS() { return m_event->getSS(); }

/**
 * @brief: LogFormatter Impl
 */
LogFormatter::LogFormatter(const std::string& pattern) : m_pattern(pattern) {
  init();
}

class MessageFormatItem : public LogFormatter::FormatItem {
 public:
  explicit MessageFormatItem(const std::string& str = "") {}
  void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
              LogEvent::ptr event) override {
    os << event->getContent();
  }
};

class ElapseFormatItem : public LogFormatter::FormatItem {
 public:
  explicit ElapseFormatItem(const std::string& str = "") {}
  void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
              LogEvent::ptr event) override {
    os << event->getElapse();
  }
};

class LevelFormatItem : public LogFormatter::FormatItem {
 public:
  explicit LevelFormatItem(const std::string& str = "") {}
  void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
              LogEvent::ptr event) override {
    os << LogLevel::ToString(level);
  }
};

class NameFormatItem : public LogFormatter::FormatItem {
 public:
  explicit NameFormatItem(const std::string& str = "") {}
  void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
              LogEvent::ptr event) override {
    os << event->getLogger()->getName();
  }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem {
 public:
  explicit ThreadIdFormatItem(const std::string& str = "") {}
  void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
              LogEvent::ptr event) override {
    os << event->getThreadId();
  }
};

class CoroutineIdItem : public LogFormatter::FormatItem {
 public:
  explicit CoroutineIdItem(const std::string& str = "") {}
  void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
              LogEvent::ptr event) override {
    os << event->getCoroutineId();
  }
};

class DataTimeFormatItem : public LogFormatter::FormatItem {
 public:
  explicit DataTimeFormatItem(const std::string& str = "") {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    _sys_cur_sec = tv.tv_sec;
    _sys_cur_min = _sys_cur_sec / 60;
    struct tm cur_tm;
    localtime_r(reinterpret_cast<time_t*>(&_sys_cur_sec), &cur_tm);
    year = cur_tm.tm_year + 1900;
    month = cur_tm.tm_mon + 1;
    day = cur_tm.tm_mday;
    hour = cur_tm.tm_hour;
    min = cur_tm.tm_min;
    sec = cur_tm.tm_sec;
    timeReset();
  }

  void timeReset() {
    snprintf(buff, sizeof(buff), "%d-%d-%d %02d:%02d:%02d", year, month, day,
             hour, min, sec);
  }

  void getCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    // not in same second
    if ((uint32_t)tv.tv_sec != _sys_cur_sec) {
      sec = tv.tv_sec % 60;
      _sys_cur_sec = tv.tv_sec;
      // not in same miniutes
      if (_sys_cur_sec / 60 != _sys_cur_min) {
        _sys_cur_min = _sys_cur_sec / 60;
        struct tm cur_tm;
        localtime_r(reinterpret_cast<time_t*>(&_sys_cur_sec), &cur_tm);
        year = cur_tm.tm_year + 1900;
        month = cur_tm.tm_mon + 1;
        day = cur_tm.tm_mday;
        hour = cur_tm.tm_hour;
        min = cur_tm.tm_min;
        timeReset();
      } else {
        timeReset();
      }
    }
  }

  void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
              LogEvent::ptr event) override {
    // time_t time = event->getTime();
    getCurrentTime();
    os << buff;
  }

 private:
  char buff[64];
  uint64_t _sys_cur_min;
  uint64_t _sys_cur_sec;
  int year, month, day, hour, min, sec;
};

class FilenameFormatItem : public LogFormatter::FormatItem {
 public:
  explicit FilenameFormatItem(const std::string& str = "") {}
  void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
              LogEvent::ptr event) override {
    os << event->getFile();
  }
};

class LineFormatItem : public LogFormatter::FormatItem {
 public:
  explicit LineFormatItem(const std::string& str = "") {}
  void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
              LogEvent::ptr event) override {
    os << event->getLine();
  }
};

class NewLineFormatItem : public LogFormatter::FormatItem {
 public:
  explicit NewLineFormatItem(const std::string& str = "") {}
  void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
              LogEvent::ptr event) override {
    os << std::endl;
  }
};

class StringFormatItem : public LogFormatter::FormatItem {
 public:
  explicit StringFormatItem(const std::string& str = "") : m_str(str) {}
  void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
              LogEvent::ptr event) override {
    os << m_str;
  }

 private:
  std::string m_str;
};

class TabFormatItem : public LogFormatter::FormatItem {
 public:
  explicit TabFormatItem(const std::string& str = "") {}
  void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level,
              LogEvent::ptr event) override {
    os << "\t";
  }
};

void LogFormatter::init() {
  static std::map<std::string,
                  std::function<FormatItem::ptr(const std::string&)>>
      mapFormat_items = {
#define XX(c, _fmt)                                                           \
  {                                                                           \
#c, [](const std::string& fmt) { return FormatItem::ptr(new _fmt(fmt)); } \
  }

          XX(m, MessageFormatItem),  XX(r, ElapseFormatItem),
          XX(p, LevelFormatItem),    XX(c, NameFormatItem),
          XX(t, ThreadIdFormatItem), XX(F, CoroutineIdItem),
          XX(d, DataTimeFormatItem), XX(f, FilenameFormatItem),
          XX(l, LineFormatItem),     XX(n, NewLineFormatItem),
          XX(T, TabFormatItem)
#undef XX
      };

  // %d%T%t%T%F%T%p%T%c%T%f:%l%T%m%n
  std::vector<std::pair<int, char>> vecItem;
  for (size_t idx = 0; idx < m_pattern.size(); idx++) {
    if (m_pattern[idx] == '%') {
      vecItem.push_back(std::make_pair(0, m_pattern[idx + 1]));
      ++idx;
    } else {
      vecItem.push_back(std::make_pair(1, m_pattern[idx]));
    }
  }

  for (auto& i : vecItem) {
    int type = i.first;
    std::string item(1, i.second);
    if (type == 1) {
      m_items.push_back(FormatItem::ptr(new StringFormatItem(item)));
    } else {
      auto it = mapFormat_items.find(item);
      if (it != mapFormat_items.end())
        m_items.push_back(it->second(item));
      else
        m_items.push_back(FormatItem::ptr(new StringFormatItem(item)));
    }
  }
}

std::string LogFormatter::format(std::shared_ptr<Logger> logger,
                                 LogLevel::Level level, LogEvent::ptr event) {
  std::stringstream ss;
  for (auto& i : m_items) {
    i->format(ss, logger, level, event);
  }
  return ss.str();
}

/**
 * @brief LogAppender impl
 */
void LogAppender::setFormatter(LogFormatter::ptr val) {
  std::lock_guard<std::mutex> guard(m_mutex);
  m_formatter = val;

  if (m_formatter)
    m_hasFormatter = true;
  else
    m_hasFormatter = false;
}

LogFormatter::ptr LogAppender::getFormatter() {
  std::lock_guard<std::mutex> guard(m_mutex);
  return m_formatter;
}

void StdoutLogAppender::log(Logger::ptr logger, LogLevel::Level level,
                            LogEvent::ptr event) {
  if (level >= LogAppender::m_level) {
    std::cout << LogAppender::m_formatter->format(logger, level, event);
  }
}

FileLogAppender::FileLogAppender(const std::string& filename)
    : m_filename(filename) {
  reopen();
}

void FileLogAppender::log(Logger::ptr logger, LogLevel::Level level,
                          LogEvent::ptr event) {
  if (level >= LogAppender::m_level) {
    uint64_t now = time(0);
    // 每一秒重新打开一次文件，保证在日志文件被删除的情况下可以重新创建文件
    if (now != m_lastTime) {
      reopen();
      m_lastTime = now;
    }
    std::lock_guard<std::mutex> guard(m_mutex);
    m_filestream << LogAppender::m_formatter->format(logger, level, event);
  }
}

bool FileLogAppender::reopen() {
  std::lock_guard<std::mutex> guard(m_mutex);
  if (m_filestream) {
    m_filestream.close();
  }

  m_filestream.open(m_filename, std::ios::app);
  return !!m_filestream;
}

void AsyncFileLogAppender::log(Logger::ptr logger, LogLevel::Level level,
                               LogEvent::ptr event) {
  if (level >= LogAppender::m_level) {
    std::string msg = LogAppender::m_formatter->format(logger, level, event);
    {
      std::lock_guard<std::mutex> guard(m_mutex);
      m_msg_q.push_back(msg);
      m_cond.notify_one();
    }
  }
}

void AsyncFileLogAppender::threadFunc() {
  while(m_running) {
    std::list<std::string> lst;
    {
      std::unique_lock<std::mutex> locker(m_mutex);
      lst.splice(lst.begin(), m_msg_q);
      if(lst.empty())
        m_cond.wait(locker);
    }
    std::ofstream fs;
    fs.open(m_filename, std::ios::app);
    if(fs) {
      for(auto& str: lst)
        fs << str;
    }
  }
}
/**
 * @brief Logger Impl
 */
Logger::Logger(const std::string& name)
    : m_name(name), m_level(LogLevel::DEBUG) {
  m_formatter.reset(new LogFormatter("%d%T%t%T%F%T%p%T%c%T%f:%l%T%m%n"));
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
  if (level >= m_level) {
    auto self = shared_from_this();
    std::lock_guard<std::mutex> guard(m_mutex);
    if (!m_appenders.empty()) {
      for (auto& i : m_appenders) i->log(self, level, event);
    } else if (m_root) {
      m_root->log(level, event);
    }
  }
}

void Logger::debug(swallow::LogEvent::ptr event) {
  log(LogLevel::DEBUG, event);
}

void Logger::info(swallow::LogEvent::ptr event) { log(LogLevel::INFO, event); }

void Logger::warn(swallow::LogEvent::ptr event) { log(LogLevel::WARN, event); }

void Logger::error(swallow::LogEvent::ptr event) {
  log(LogLevel::ERROR, event);
}

void Logger::fatal(swallow::LogEvent::ptr event) {
  log(LogLevel::FATAL, event);
}

void Logger::addAppender(LogAppender::ptr appender) {
  std::lock_guard<std::mutex> guard(m_mutex);
  if (!appender->getFormatter()) {
    appender->setFormatter(m_formatter);
  }
  m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender) {
  std::lock_guard<std::mutex> guard(m_mutex);
  for (auto itr = m_appenders.begin(); itr != m_appenders.end(); ++itr) {
    if (*itr == appender) {
      m_appenders.erase(itr);
    }
  }
}

void Logger::clearAppender() {
  std::lock_guard<std::mutex> guard(m_mutex);
  m_appenders.clear();
}

void Logger::setFormatter(LogFormatter::ptr val) {
  std::lock_guard<std::mutex> guard(m_mutex);
  m_formatter = val;

  for (auto& i : m_appenders) {
    std::lock_guard<std::mutex> iguard(i->m_mutex);
    if (!i->m_hasFormatter) i->m_formatter = m_formatter;
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
  std::lock_guard<std::mutex> guard(m_mutex);
  return m_formatter;
}

const std::string Logger::getName() const { return m_name; }

void Logger::setLevel(LogLevel::Level level) { m_level = level; }

LogLevel::Level Logger::getLevel() const { return m_level; }

Logger::ptr LoggerManager::getLogger(const std::string& name) {
  std::lock_guard<std::mutex> guard(m_mutex);
  auto it = m_loggers.find(name);
  if (it != m_loggers.end()) {
    return it->second;
  }

  Logger::ptr logger(new Logger(name));
  logger->m_root = m_root;
  m_loggers[name] = logger;
  return logger;
}

}  // namespace swallow
