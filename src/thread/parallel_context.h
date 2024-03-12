#ifndef RAXML_PARALLELCONTEXT_HPP_
#define RAXML_PARALLELCONTEXT_HPP_

#include "../global_defs.h"

#include <vector>
#include <unordered_map>
#include <memory>

#include <functional>

#if(MPI_ENABLED)
#include <mpi.h>
#endif

#ifdef MULTITHREAD
#include <mutex>
#include <thread>
typedef std::thread ThreadType;
typedef std::thread::id ThreadIDType;
typedef std::mutex MutexType;
typedef std::unique_lock<std::mutex> LockType;
#else
typedef int ThreadType;
typedef size_t ThreadIDType;
typedef int MutexType;
typedef int LockType;
#endif

struct ThreadGroup
{
  size_t group_id;
  size_t num_threads;
  std::vector<char> reduction_buf; 
  std::vector<ThreadType> threads;

  MutexType mtx;
  volatile unsigned int barrier_counter;
  volatile int proceed;

  ThreadGroup(size_t id, size_t size, size_t bufsize = 0) :
    group_id(id), num_threads(size), reduction_buf(bufsize),
    mtx(), barrier_counter(0), proceed(0) {}

  ThreadGroup(ThreadGroup&& other):
    group_id(other.group_id),
    num_threads(other.num_threads), reduction_buf(std::move(other.reduction_buf)),
    mtx(), barrier_counter(other.barrier_counter), proceed(other.proceed) {}
};

class ParallelContext
{
public:

  /* distributed memory level */

  static void init_mpi(int argc, char * argv[], void * comm);

  static size_t num_ranks() { return _num_ranks; }
  static size_t rank_id()   { return _rank_id; }
  static bool master_rank()   { return _rank_id == 0; }

   static void mpi_barrier();

  /* shared high-level */
  static void init_threadgroups(mt_size_t num_threadgroups);
  static void init_threadgroup(mt_index_t id, mt_size_t n_threads = 1);
  static size_t num_threadgroups() { return _num_threadgroups; }
  static size_t threadgroup_id() { return _threadgroup_id; }

  /* shared low-level */
  static void init_threads(mt_size_t n_threads, const std::function<void()>& thread_main);
  static void finalize_threads(bool force = false);

  static size_t num_threads() { return _threadgroup->num_threads; }
  static size_t thread_id() { return _thread_id; }
  static bool master_thread() { return _thread_id == 0; }

  static void thread_barrier(bool reset=false); // must reset thread barriers between forks

  /* hybrid/mixed */
  static void finalize(bool force = false);

  static size_t num_procs() { return _num_ranks * _num_threadgroups; }
  static size_t proc_id()   { return _rank_id + _thread_id; }
  static bool master()        { return proc_id() == 0; }

  static void barrier();
  
  
  static void resize_buffer(size_t size);
  static void parallel_reduce_cb(void * context, double * data, size_t size, int op);
  static void thread_reduce(double * data, size_t size, int op);
  static void thread_broadcast(size_t source_id, void * data, size_t size);
  void thread_send_master(size_t source_id, void * data, size_t size) const;

  static void mpi_gather_custom(std::function<int(void*,int)> prepare_send_cb,
                                std::function<void(void*,int)> process_recv_cb);


  /* static singleton, no instantiation/copying/moving */
  ParallelContext() = delete;
  ParallelContext(const ParallelContext& other) = delete;
  ParallelContext(ParallelContext&& other) = delete;
  ParallelContext& operator=(const ParallelContext& other) = delete;
  ParallelContext& operator=(ParallelContext&& other) = delete;

  class UniqueLock
  {
  public:
    UniqueLock() : _lock(mtx) {}
  private:
    LockType _lock;
  };
private:
  static thread_local ThreadGroup * _threadgroup;
  static size_t _num_threadgroups;
  static size_t _num_ranks;

  static std::unordered_map<ThreadIDType, ParallelContext> _thread_ctx_map;
  static MutexType mtx;

  static size_t _rank_id;
  static thread_local size_t _thread_id;
  static thread_local size_t _threadgroup_id;

#if(MPI_ENABLED)
  static bool _owns_comm;
  static MPI_Comm _comm;
  static std::vector<char> _parallel_buf;
#endif

  static void start_thread(size_t thread_id, ThreadGroup& threadgroup, const std::function<void()>& thread_main);
  static void parallel_reduce(double * data, size_t size, int op);
};

#endif /* RAXML_PARALLELCONTEXT_HPP_ */
