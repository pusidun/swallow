/**
 * @copyright Copyright [2020]
 * @author pusidun@hotmail.com
 * @file process.cc
 * @brief process class
 * @date 2020-03-18
 */
#include "swallow/base/process.h"

#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <sstream>
#include <string>

#include "swallow/base/log.h"


namespace swallow {

#define MAXFILE 65535  // 最大文件描述符

static swallow::Logger::ptr g_logger = SWALLOW_LOG_GET("system");

std::string ProcessInfo::toString() {
  std::stringstream ss;
  ss << "Parent pid is:" << parent_pid << ",main pid is:" << main_pid
     << ",reboot number is:" << reboot_num;
  return ss.str();
}

static int start_daemon(int argc, char** argv,
                        std::function<int(int, char**)> cb) {
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork daemon failed!");
    return -1;
  } else if (pid > 0) {
    // 父进程退出，让init进程收养子进程
    exit(0);
  } else if (pid == 0) {
    // 此时子进程被收养,parent pid = 1，进行设置后将此进程当做守护进程
    ProcessMgr.getInstance()->parent_pid = getpid();
    setsid();
    if (chdir("/") != 0) { SWALLOW_LOG_INFO(g_logger) << "change dir failed!"; }
    umask(0);
    // for (int i = 0; i < MAXFILE; ++i) close(i);
    signal(SIGHUP, SIG_IGN);

    while (true) {
      pid = fork();
      if (pid < 0) {
        perror("fork failed!");
        return -1;
      } else if (pid == 0) {
        // 子进程
        ProcessMgr.getInstance()->main_pid = getpid();
        return cb(argc, argv);
      } else if (pid > 0) {
        // 守护进程
        int* status = nullptr;

        waitpid(pid, status, 0);
        if (!status) {
          SWALLOW_LOG_INFO(g_logger) << "status is NULL, daemon exit";
          exit(0);
        }
        if (WIFEXITED(status)) {
          SWALLOW_LOG_INFO(g_logger) << "child process finished.";
          break;
        }
        if (WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL) {
          SWALLOW_LOG_INFO(g_logger) << "child process killed.";
          break;
        }
        SWALLOW_LOG_ERROR(g_logger) << "child process abort!";
        SWALLOW_LOG_INFO(g_logger)
            << "daemon will restart childprocess"
            << " reboot_num: " << ProcessMgr.getInstance()->reboot_num++;

        sleep(5);
      }
    }
    // 守护进程退出
    return 0;
  }
  return -1;
}

int start_process(int argc, char** argv, std::function<int(int, char**)> cb,
                  bool daemon) {
  if (daemon) {
    return start_daemon(argc, argv, cb);
  }
  ProcessMgr.getInstance()->parent_pid = 0;
  ProcessMgr.getInstance()->main_pid = getpid();
  return cb(argc, argv);
}

}  // namespace swallow
