/**
 * @copyright Copyright [2019]
 * @author pusidun@hotmail.com
 * @file circular_q.h
 * @brief circular_q class
 * @date 2020-03-18
 */
#ifndef SWALLOW_BASE_CIRCULAR_Q_H_
#define SWALLOW_BASE_CIRCULAR_Q_H_

#include <utility>
#include <vector>

namespace swallow {

template <typename T>
class circular_q {
 public:
  /**
   * @brief ctor
   * @param size[in] vector<T> reverses one item as marker for full _q
   **/
  explicit circular_q(size_t size) : max_size(size + 1), _q(max_size) {}

  void push(T&& item) {
    _q[rear] = std::move(item);
    rear = (rear + 1) % max_size;
    if (rear == front) {
      front = (front + 1) % max_size;
    }
  }

  void pop(T& item) {
    item = std::move(_q[front]);
    front = (front + 1) % max_size;
  }

  bool empty() { return front == rear; }

  bool full() { return (rear + 1) % max_size == front; }

 private:
  size_t max_size;
  std::vector<T> _q;
  typename std::vector<T>::size_type front = 0;  // 队首
  typename std::vector<T>::size_type rear = 0;   // 尾后
};

}  // namespace swallow

#endif  // SWALLOW_BASE_CIRCULAR_Q_H_
