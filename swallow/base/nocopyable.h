/**
 * @copyright Copyright [2019]
 * @author pusidun@hotmail.com
 * @file nocopyable.h
 * @brief class which override this class is nocopyable
 * @date 2019-11-08
 */
#ifndef SWALLOW_BASE_NOCOPYABLE_H_
#define SWALLOW_BASE_NOCOPYABLE_H_

namespace swallow {

class nocopyable {
 protected:
    nocopyable() = default;
    ~nocopyable() = default;

 public:
    nocopyable(const nocopyable&) = delete;
    nocopyable& operator=(const nocopyable&) = delete;
};

}  // namespace swallow

#endif  // SWALLOW_BASE_NOCOPYABLE_H_
