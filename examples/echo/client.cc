#include "FdEvent.h"
#include "EventLoop.h"

class StdInFd : public swallow::FdEvent {
 public:
    StdInFd():{}

    virtual ~StdInFd();

    virtual void handleRead()

 private:
    EchoCli* pCli;
}

class EchoCli {

 private:
    swallow::TCP
};