#include <cstring>
#include "redis.h"
#include "log.h"

namespace swallow {

static std::shared_ptr<Logger> log = SWALLOW_LOG_GET("database");

/*
* Redis impl
*/
ReplyPtr Redis::cmd(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    ReplyPtr rptr = cmd(fmt, ap);
    va_end(ap);
    return rptr;
}

ReplyPtr Redis::cmd(const char* fmt, va_list ap) {
    auto r = (redisReply*)redisvCommand(m_context.get(), fmt, ap);
    if(!r)
        return nullptr;
    // 结束时调用freeReplyObject析构
    ReplyPtr smptr(r, freeReplyObject);
    if(r->type != REDIS_REPLY_ERROR) {
        return smptr;
    }
    return nullptr;
}

// void *redisCommandArgv(redisContext *c, int argc, const char **argv, const size_t *argvlen)
ReplyPtr Redis::cmd(const std::vector<std::string>& varg) {
    std::vector<const char*> argv;
    std::vector<size_t> argvlen;
    for(auto& arg: varg) {
        argv.push_back(arg.c_str());
        argvlen.push_back(arg.size());
    }
    auto r = (redisReply*)redisCommandArgv(m_context.get(), varg.size(), &argv[0], &argvlen[0]);
    ReplyPtr smptr(r, freeReplyObject);
    if(r->type != REDIS_REPLY_ERROR) {
        return smptr;
    }
    if(m_logEnable) {
        SWALLOW_LOG_ERROR(log) << "redisCommand error.";
    }
    return nullptr;
}

bool Redis::connect(const std::string& ip, int port, uint64_t ms) {
    m_host = ip;
    m_port = port;
    m_timeout = ms;
    if(m_context) return true;
    timeval tv = {(int)ms/1000, (int)ms*1000%1000};
    auto ctx = redisConnectWithTimeout(m_host.c_str(), m_port, tv);
    if(ctx) {
        m_context.reset(ctx, redisFree);
        if(!getPasswd().empty()) {
            auto r = (redisReply*)redisCommand(ctx, "auth %s", getPasswd().c_str());
            if(strcmp(r->str, "OK"))
                return true;
        }
        return true;
    }
    return false;
}

bool Redis::connect() {
    return connect(m_host, m_port, m_timeout);
}

bool Redis::reconnect() {
    return redisReconnect(m_context.get());
}

// pipeline
int Redis::appendCmd(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int rt = appendCmd(fmt, ap);
    va_end(ap);
    return rt;
}

int Redis::appendCmd(const char* fmt, va_list ap) {
    return redisvAppendCommand(m_context.get(), fmt, ap);
}

int Redis::appendCmd(const std::vector<std::string>& varg) {
    std::vector<const char*> argv;
    std::vector<size_t> argvlen;
    for(auto& arg: varg) {
        argv.push_back(arg.c_str());
        argvlen.push_back(arg.size());
    }
    return redisAppendCommandArgv(m_context.get(), varg.size(), &argv[0], &argvlen[0]);
}

ReplyPtr Redis::getReply() {
    redisReply* reply = nullptr;
    if(redisGetReply(m_context.get(), (void**)&reply) == REDIS_OK ) {
        ReplyPtr rsmptr(reply, freeReplyObject);
        return rsmptr;
    }
    if(m_logEnable) {
        SWALLOW_LOG_ERROR(log) << "redisGetReply error! host:" << m_host 
            << " port:" << m_port << " name:" << m_name;
    }
    return nullptr;
}

}  // swallow end
