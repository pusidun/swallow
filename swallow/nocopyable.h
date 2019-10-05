/*
** Copyright [2019] <pusidun@hotmail.com>  [MIT LICENCE]
*/
#ifndef SWALLOW_NOCOPYABLE_H_
#define SWALLOW_NOCOPYABLE_H_

namespace swallow {

class nocopyable {
 public:
    nocopyable(const nocopyable&) = delete;
    const nocopyable& operator=(const nocopyable&) = delete;

 protected:
    nocopyable() = default;
    ~nocopyable() = default;
};

}  // namespace swallow

#endif  // SWALLOW_NOCOPYABLE_H_
