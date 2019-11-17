/**
 * @copyright Copyright [2019]
 * @author pusidun@hotmail.com
 * @file singleton.h
 * @brief
 * @date 2019-11-17
 */
#ifndef SWALLOW_BASE_SINGLETON_H_
#define SWALLOW_BASE_SINGLETON_H_

namespace swallow
{

template<typename T>
class Singleton
{
 public:
    static T* getInstance() {
        static T v;
        return &v;
    }
};

}  // namespace swallow

#endif  // SWALLOW_BASE_SINGLETON_H_
