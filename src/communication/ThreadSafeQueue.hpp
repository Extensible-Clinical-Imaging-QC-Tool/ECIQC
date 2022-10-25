#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H
#include <mutex>
#include <queue>

template <typename T> class ThreadSafeQueue {
  std::mutex mutex;
  std::condition_variable cond_var;
  std::queue<T> queue;

public:
  void push(T &&item);
  T &front();
  void pop();
  std::size_t size();
};

#include "ThreadSafeQueue.tpp"

#endif
