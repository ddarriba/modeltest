/*
  Copyright (c) 2012 Jakob Progsch, Václav Zeman

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.

  Modified by 2016 Diego Darriba

  Contact: Diego Darriba <Diego.Darriba@h-its.org>,
  Heidelberg Institute for Theoretical Studies,
  Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <map>
#if(USE_POSIX_THREADS)
#include <mutex>
#include <thread>
#include <future>
#include <condition_variable>
#else
#include "../mingw/mingw.mutex.h"
#include "../mingw/mingw.thread.h"
#include "../mingw/mingw.future.h"
#include "../mingw/mingw.condition_variable.h"
#endif
#include <queue>
#include <memory>
#include <vector>
#include <unistd.h>
#include <stdexcept>
#include <functional>

namespace modeltest {

class ThreadPool {
public:
  ThreadPool(mt_size_t);
  template<class F, class... Args>
  auto enqueue(F&& f, Args&&... args)
  -> std::future<typename std::result_of<F(Args...)>::type>;
  void clean( void )
  {
      while (!this->tasks.empty())
        this->tasks.pop();
  }
  ~ThreadPool();
  std::map< std::thread::id, mt_index_t > worker_ids;
  mt_size_t ready;
private:
  // need to keep track of threads so we can join them
  std::vector< std::thread > workers;
  // the task queue
  std::queue< std::function<void()> > tasks;

  // synchronization
  std::mutex queue_mutex;
  std::condition_variable condition;
  bool stop;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(mt_size_t threads)
  :   ready(threads), stop(false)
{
  for(size_t i = 0;i<threads;++i)
    workers.emplace_back(
    [this]
    {
      {
        std::unique_lock<std::mutex> lock(this->queue_mutex);
        worker_ids[std::this_thread::get_id()] = --ready;
      }
      for(;;)
      {
        std::function<void()> task;

        {
          std::unique_lock<std::mutex> lock(this->queue_mutex);
          this->condition.wait(lock,
            [this]{ return this->stop || !this->tasks.empty(); });
          if(this->stop && this->tasks.empty())
            return;
          task = std::move(this->tasks.front());
          this->tasks.pop();
        }
        task();
      }
    }
  );
  /* wait for initialization */
  while(1)
  {
    std::unique_lock<std::mutex> lock(this->queue_mutex);
    if(!ready) break;
  }

  ;
}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
-> std::future<typename std::result_of<F(Args...)>::type>
{
  using return_type = typename std::result_of<F(Args...)>::type;

  auto task = std::make_shared< std::packaged_task<return_type()> >(
  std::bind(std::forward<F>(f), std::forward<Args>(args)...)
  );

  std::future<return_type> res = task->get_future();
  {
  std::unique_lock<std::mutex> lock(queue_mutex);

  // don't allow enqueueing after stopping the pool
  if(stop)
  throw std::runtime_error("enqueue on stopped ThreadPool");

  tasks.emplace([task](){ (*task)(); });
  }
  condition.notify_one();
  return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
  {
  std::unique_lock<std::mutex> lock(queue_mutex);
  stop = true;
  }
  condition.notify_all();
  for(std::thread &worker: workers)
  worker.join();
}

} /* modeltest */

#endif // THREADPOOL_H
