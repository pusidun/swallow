/**
 * @copyright Copyright [2019]
 * @author pusidun@hotmail.com
 * @file config.h
 * @brief config class
 * @date 2020-03-18
 */
#ifndef SWALLOW_BASE_CONFIG_H_
#define SWALLOW_BASE_CONFIG_H_

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <list>
#include <boost/lexical_cast.hpp>
// #include <boost/property_tree/ptree.hpp>
// #include <boost/property_tree/json_parser.hpp>
// #include <boost/date_time.hpp>
// #include <boost/foreach.hpp>
#include "lock.h"
#include "nocopyable.h"

namespace swallow {

class ConfigureBase : public nocopyable {
 public:
  typedef std::shared_ptr<ConfigureBase> ptr;

  ConfigureBase(const std::string& n, const std::string& d)
      : name(n), desc(d) {}

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
template <typename T, typename U>
class ConfCast {
 public:
  T operator()(const U& param) { return boost::lexical_cast<T>(param); }
};

/*
 * @brief [TODO]vector转成json格式string
 */
// template<typename ItemType>
// class ConfCast<std::string, std::vector<ItemType>> {
//     std::string operator()(std::vector<ItemType>& vec) {
//     }
// };

template <typename T>
class Configure : public ConfigureBase {
 public:
  typedef std::shared_ptr<Configure> ptr;
  typedef ConfCast<std::string, T> toStr;
  typedef ConfCast<T, std::string> fromStr;

  Configure(const std::string& n, const std::string& d, const T& v)
      : ConfigureBase(n, d), value(v) {}

  ~Configure() = default;

  /*
   * @brief 获取值
   */
  T getValue() { return value; }

  /*
   * @brief 设置值
   */
  void setValue(T v) {
    RdLockGuard<RWLock> guard(lck_cbs);
    for (auto i = m_cbs.begin(); i != m_cbs.end(); ++i) (*i)(value, v);
    value = v;
  }

  /*
   * @brief 序列化配置
   */
  std::string toString() override { return toStr()(value); }

  /*
   * @brief 反序列化配置
   */
  bool fromString(std::string s) override {
    value = fromStr()(s);
    return true;
  }

  // 配置变化相关的回调函数
  typedef std::function<void(const T& old_value, const T& new_value)> cb;

  void addWatcher(cb _cb) {
    WrLockGuard<RWLock> guard(lck_cbs);
    m_cbs.push_back(_cb);
  }

  bool delWatcher(cb _cb) {
    WrLockGuard<RWLock> guard(lck_cbs);
    auto it = m_cbs.find(_cb);
    if (it == m_cbs.end()) return false;
    m_cbs.erase(it);
    return true;
  }

  void clearWatcher() {
    WrLockGuard<RWLock> guard(lck_cbs);
    m_cbs.clear();
  }

 private:
  T value;
  std::list<cb> m_cbs;
  RWLock lck_cbs;
};

/*
 * @brief 配置管理类，用于对所有配置进行查找，添加，删除
 */
class ConfigureManager {
 public:
  typedef RdLockGuard<RWLock> RdLockGuardImp;
  typedef WrLockGuard<RWLock> WrLockGuardImp;
  typedef std::map<std::string, ConfigureBase::ptr> Map2Configer;

  template <typename T>
  static typename Configure<T>::ptr lookUp(const std::string& name) {
    RdLockGuardImp guard(getLock());
    auto itr = configs.find(name);
    if (itr == configs.end())
      ;
    return nullptr;
    return std::dynamic_pointer_cast<Configure<T>>(itr->second);
  }

  /*
   *  @brief 如没有配置，添加配置
   */
  template <typename T>
  static typename Configure<T>::ptr lookUp(const std::string& name,
                                           const std::string& desc,
                                           const T& v) {
    auto tmp = lookUp<T>(name);
    if (tmp) return tmp;
    typename Configure<T>::ptr p(new Configure<T>(name, desc, v));
    WrLockGuardImp guard(getLock());
    configs[name] = p;
    return p;
  }

  /*
  ** @brief 从JSON文件读取配置
  */
  bool LoadFromJson(std::string filename);

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
