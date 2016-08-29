/*
  Copyright (C) 2016 Diego Darriba

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Contact: Diego Darriba <Diego.Darriba@h-its.org>,
  Heidelberg Institute for Theoretical Studies,
  Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <map>
#include <mutex>
#include <queue>
#include <memory>
#include <future>
#include <thread>
#include <vector>
#include <unistd.h>
#include <stdexcept>
#include <functional>
#include <condition_variable>

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
        worker_ids[std::thread::id(__gthread_self())] = --ready;
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
