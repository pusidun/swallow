/**
 * @copyright Copyright [2019]
 * @author pusidun@hotmail.com
 * @file redis.h
 * @brief wrapper hiredis
 * @date 2020-05-28
 */
#ifndef SWALLOW_BASE_REDIS_H_
#define SWALLOW_BASE_REDIS_H_

#include <cstdlib>
#include <ctime>
#include <memory>
#include <vector>

#include "hiredis.h"

namespace swallow {

typedef std::shared_ptr<redisReply> ReplyPtr;

class IRedis {
 public:
  typedef std::shared_ptr<IRedis> ptr;
  IRedis() : m_logEnable(true) {}
  virtual ~IRedis() {}

  virtual ReplyPtr cmd(const char* fmt, ...) = 0;
  virtual ReplyPtr cmd(const char* fmt, va_list ap) = 0;
  virtual ReplyPtr cmd(const std::vector<std::string>& argv) = 0;

  const std::string& getName() const { return m_name; }
  void setName(const std::string& name) { m_name = name; }
  const std::string& getPasswd() const { return m_passwd; }
  void setPasswd(const std::string& passwd) { m_passwd = passwd; }

 protected:
  std::string m_name;
  std::string m_passwd;
  bool m_logEnable;
};

class ISyncRedis : public IRedis {
 public:
  typedef std::shared_ptr<ISyncRedis> ptr;
  virtual ~ISyncRedis() {}

  virtual bool connect(const std::string& ip, int port,
                       uint64_t timeout = 0) = 0;
  virtual bool connect() = 0;
  virtual bool reconnect() = 0;

  // pipeline
  virtual int appendCmd(const char* fmt, ...) = 0;
  virtual int appendCmd(const char* fmt, va_list ap) = 0;
  virtual int appendCmd(const std::vector<std::string>& argv) = 0;
  virtual ReplyPtr getReply() = 0;

  void setLastActiveTime(uint64_t time) { m_lastActiveTime = time; }
  uint16_t getLastActive() const { return m_lastActiveTime; }

 private:
  uint16_t m_lastActiveTime;
};

class Redis : public ISyncRedis {
 public:
  typedef std::shared_ptr<Redis> ptr;

  virtual ~Redis() {}

  virtual bool connect(const std::string& ip, int port, uint64_t timeout = 0);
  virtual bool connect();
  virtual bool reconnect();

  virtual ReplyPtr cmd(const char* fmt, ...);
  virtual ReplyPtr cmd(const char* fmt, va_list ap);
  virtual ReplyPtr cmd(const std::vector<std::string>& varg);

  // pipeline
  virtual int appendCmd(const char* fmt, ...);
  virtual int appendCmd(const char* fmt, va_list ap);
  virtual int appendCmd(const std::vector<std::string>& varg);
  virtual ReplyPtr getReply();

 private:
  std::string m_host;  // ip addr
  uint32_t m_port;     // port number
  uint64_t m_timeout;  // ms
  std::shared_ptr<redisContext> m_context;
};

}  // namespace swallow

#endif
