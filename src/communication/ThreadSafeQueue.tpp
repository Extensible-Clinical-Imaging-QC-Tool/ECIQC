#include "ThreadSafeQueue.hpp"

template <typename T> void ThreadSafeQueue<T>::push(T &&item) {
  {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(item);
  }
  cond_var.notify_one();
}

template <typename T> T &ThreadSafeQueue<T>::front() {
  std::unique_lock<std::mutex> lock(mutex);
  cond_var.wait(lock, [&] { return !queue.empty(); });
  return queue.front();
}

template <typename T> void ThreadSafeQueue<T>::pop() {
  std::lock_guard<std::mutex> lock(mutex);
  queue.pop();
}


template <typename T> std::size_t ThreadSafeQueue<T>::size() {
  return queue.size();
}
