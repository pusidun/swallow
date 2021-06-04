/**
 * @copyright Copyright [2021]
 * @author pusidun@hotmail.com
 * @file exception.h
 * @brief exception and backtrace
 * @date 2021-06-04
 */
#ifndef SWALLOW_BASE_EXCEPTION_H_
#define SWALLOW_BASE_EXCEPTION_H_

#include <exception>
#include <string>
#include "swallow/base/thread.h"

namespace swallow {
class Exception: public std::exception {
public:
    Exception(std::string&& cause):_cause(std::move(cause)),stack_trace(Thread::GetThis()->GetBacktrace()){}
    const char* what() const noexcept override {
        return _cause.c_str();
    };
    const char* printStack() const noexcept {
        return stack_trace.c_str();
    }
private:
    std::string _cause;
    std::string stack_trace;
};

}  // namespace swallow

#endif  // SWALLOW_BASE_EXCEPTION_H_
