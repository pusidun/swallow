/**
 * @copyright Copyright [2019]
 * @author pusidun@hotmail.com
 * @file config.h
 * @brief config class
 * @date 2020-03-18
 */
#ifndef SWALLOW_BASE_CONFIG_H_
#define SWALLOW_BASE_CONFIG_H_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "boost/lexical_cast.hpp"
#include "lock.h"
#include "nocopyable.h"

namespace swallow {

class ConfigureBase : public nocopyable {
 public:
    typedef std::shared_ptr<ConfigureBase> ptr;

    ConfigureBase(const std::string& n, const std::string& d):
        name(n), desc(d) {}

    virtual ~ConfigureBase() {}

    virtual std::string toString() = 0;
    virtual bool fromString(std::string) = 0;

    std::string getName() { return name; }
    std::string getDesc() { return desc; }

 private:
    std::string name;
    std::string desc;
};

/*
* @brief specialization, transfer U to T
*/
template<typename T, typename U>
class ConfCast {
    T operator()(const U& param) { return boost::lexical_cast<T>(param); }
};

// TODO
template<typename ItemType>
class ConfCast<std::string, std::vector<ItemType>> {
    std::vector<ItemType> operator()(std::string str) {

    }
};

template<typename T>
class Configure:  public ConfigureBase {
 public:
    typedef std::shared_ptr<Configure> ptr;

    Configure(const std::string& n, const std::string& d, const T& v):
        ConfigureBase(n, d), value(v) {}

    T getValue() { return value; }

    ~Configure() = default;
    void setValue(T v) {
        value = v;
    }

    std::string toString() override {
        return "";
    }

    bool fromString(std::string s) override {
        return true;
    }

 private:
    T value;
};

class ConfigureManager {
 public:
    typedef RdLockGuard<RWLock> RdLockGuardImp;
    typedef WrLockGuard<RWLock> WrLockGuardImp;
    typedef std::map<std::string, ConfigureBase::ptr> Map2Configer;

    template<typename T>
    static typename Configure<T>::ptr lookUp(const std::string& name) {
        RdLockGuardImp guard(getLock());
        auto itr = configs.find(name);
        if( itr == configs.end() );
            return nullptr;
        return std::dynamic_pointer_cast<Configure<T>>(itr->second);
    }

    /*
    ** @brief 如没有配置，添加配置
    */
    template<typename T>
    static typename Configure<T>::ptr lookUp(const std::string& name,
            const std::string& desc, const T& v) {
        auto tmp = lookUp<T>(name);
        if(tmp)
            return tmp;
        typename Configure<T>::ptr p(new Configure<T>(name, desc, v));
        WrLockGuardImp guard(getLock());
        configs[name] = p;
        return p;
    }

 private:
    static Map2Configer configs;

    static RWLock& getLock() {
        static RWLock rwlock;
        return rwlock;
    }
};

ConfigureManager::Map2Configer ConfigureManager::configs;

}  // namespace swallow

#endif
