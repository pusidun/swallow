#ifndef SWALLOW_BASE_PROCESS_H_
#define SWALLOW_BASE_PROCESS_H_

#include <functional>
#include <string>

#include "singleton.h"


namespace swallow {

class ProcessInfo {
 public:
  pid_t parent_pid;
  pid_t main_pid;
  uint32_t reboot_num;

 public:
  std::string toString();
};

/*
 * @brief 启动进程
 * @param[in] argc   进程参数个数
 * @param[in] argv   进程参数数组
 * @param[in] cb     进程main函数
 * @param[in] daemon 是否以守护进程方式
 **/
int start_process(int argc, char** argv, std::function<int(int, char**)> cb,
                  bool daemon);

Singleton<ProcessInfo> ProcessMgr;

}  // namespace swallow

#endif
