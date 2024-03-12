#include "parallel_context.h"
#include "../plldefs.h"

#include <string.h>
#include <iostream>

using namespace std;

// This is just a default size; the buffer will be resized later according to #part and #threads
#define PARALLEL_BUF_SIZE (128 * 1024)

/* distributed memory level*/
size_t ParallelContext::_num_ranks = 1;
size_t ParallelContext::_rank_id = 0;

/* shared high-level*/
size_t ParallelContext::_num_threadgroups = 0;
thread_local ThreadGroup * ParallelContext::_threadgroup = nullptr;

/* shared low-level*/
thread_local size_t ParallelContext::_thread_id = 0;
thread_local size_t ParallelContext::_threadgroup_id = 0;

std::unordered_map<ThreadIDType, ParallelContext> ParallelContext::_thread_ctx_map;
MutexType ParallelContext::mtx;

#if(MPI_ENABLED)
MPI_Comm ParallelContext::_comm = MPI_COMM_WORLD;
bool ParallelContext::_owns_comm = true;
std::vector<char> ParallelContext::_parallel_buf;
#endif

void ParallelContext::init_mpi(int argc, char * argv[], void * comm)
{
#if(MPI_ENABLED)
  {
    int tmp;

    if (comm)
    {
      // TODO we should think how to get rid of this ugly cast!
      _comm = *((MPI_Comm*) comm);
      _owns_comm = false;
    }
    else
    {
      _comm = MPI_COMM_WORLD;
      _owns_comm = true;
      MPI_Init(&argc, &argv);
    }

    MPI_Comm_rank(_comm, &tmp);
    _rank_id = (size_t) tmp;
    MPI_Comm_size(_comm, &tmp);
    _num_ranks = (size_t) tmp;
  }
#else
  UNUSED(argc);
  UNUSED(argv);
  UNUSED(comm);
#endif
}

void ParallelContext::init_threadgroups(mt_size_t num_threadgroups)
{
  ParallelContext::_num_threadgroups = num_threadgroups;
}

void ParallelContext::init_threadgroup(mt_index_t id, mt_size_t  n_threads_per_group)
{
  if (_threadgroup != nullptr)
  {
    assert(id == _threadgroup_id);
    return;
  }

  size_t mt_bufsize = max(1024lu, sizeof(double) * 2 * n_threads_per_group);
  _threadgroup = new ThreadGroup(id, n_threads_per_group, mt_bufsize);
  _threadgroup_id = id;
}

void ParallelContext::start_thread(size_t thread_id,
                                   ThreadGroup& threadgroup,
                                   const std::function<void()>& thread_main)
{
  ParallelContext::_thread_id      = thread_id;
  ParallelContext::_threadgroup    = &threadgroup;

  thread_main();
}

void ParallelContext::init_threads(mt_size_t n_threads, const std::function<void()>& thread_main)
{
  /* check if threadgroups have been initialized*/
  assert(_threadgroup != nullptr);

#ifdef MULTITHREAD
  mt_size_t threads_to_create = n_threads;
  if (!n_threads)
  {
    threads_to_create = _threadgroup->num_threads;
  }
  assert(threads_to_create > 0);

  /* Launch threads */
  for (size_t i = 1; i < threads_to_create; ++i)
  {
    _threadgroup->threads.emplace_back(ParallelContext::start_thread, i, std::ref(*_threadgroup), thread_main);
  }
#endif
}

void ParallelContext::resize_buffer(size_t size)
{
  _threadgroup->reduction_buf.reserve(size);
}

void ParallelContext::finalize(bool force)
{
  finalize_threads(force);

#if(MPI_ENABLED)
  if (_owns_comm)
  {
    if (force)
      MPI_Abort(_comm, -1);
    else
      MPI_Barrier(_comm);

    MPI_Finalize();
  }
  else
    MPI_Barrier(_comm);
#endif
}

void ParallelContext::finalize_threads(bool force)
{
  if (_threadgroup == nullptr)
    return;

#ifdef MULTITHREAD
  for (thread& t: _threadgroup->threads)
  {
    if (force)
      t.detach();
    else
      t.join();
  }
  _threadgroup->threads.clear();
#endif
}

void ParallelContext::barrier()
{
#if(MPI_ENABLED)
  mpi_barrier();
#endif

#ifdef MULTITHREAD
  thread_barrier();
#endif
}

void ParallelContext::mpi_barrier()
{
#if(MPI_ENABLED)
  if (_thread_id == 0 && _num_ranks > 1)
    MPI_Barrier(_comm);
#endif
}

void ParallelContext::thread_barrier(bool reset)
{
  static thread_local volatile int my_cycle = 0;
  auto &g = *_threadgroup;

  if (g.num_threads == 1)
    return;

  __sync_fetch_and_add( &g.barrier_counter, 1);

  if(_thread_id == 0)
  {
    while(g.barrier_counter < g.num_threads);
    g.barrier_counter = 0;
    g.proceed = !g.proceed;
  }
  else
  {
    while(my_cycle == g.proceed);
    my_cycle = !my_cycle;
  }

  if (reset && g.proceed != 0)
  {
    // call again to reset variables
    thread_barrier(false);

    assert(my_cycle == 0);
    assert(g.proceed == 0);
  }
}

void ParallelContext::thread_reduce(double * data, size_t size, int op)
{
  /* synchronize */
  thread_barrier();
  double *double_buf = (double*) _threadgroup->reduction_buf.data();

  /* collect data from threads */
  size_t i, j;
  for (i = 0; i < size; ++i)
    double_buf[_thread_id * size + i] = data[i];

  /* synchronize */
  thread_barrier();

  /* reduce */
  for (i = 0; i < size; ++i)
  {
    switch(op)
    {
      case PLLMOD_COMMON_REDUCE_SUM:
      {
        data[i] = 0.;
        for (j = 0; j < _threadgroup->num_threads; ++j)
          data[i] += double_buf[j * size + i];
      }
      break;
      case PLLMOD_COMMON_REDUCE_MAX:
      {
        data[i] = _threadgroup->reduction_buf[i];
        for (j = 1; j < _threadgroup->num_threads; ++j)
          data[i] = max(data[i], double_buf[j * size + i]);
      }
      break;
      case PLLMOD_COMMON_REDUCE_MIN:
      {
        data[i] = _threadgroup->reduction_buf[i];
        for (j = 1; j < _threadgroup->num_threads; ++j)
          data[i] = min(data[i], double_buf[j * size + i]);
      }
      break;
    }
  }
}

void ParallelContext::parallel_reduce(double * data, size_t size, int op)
{
#ifdef MULTITHREAD
  if (_threadgroup->num_threads > 1)
    thread_reduce(data, size, op);
#endif
}

void ParallelContext::parallel_reduce_cb(void * context, double * data, size_t size, int op)
{
  ParallelContext::parallel_reduce(data, size, op);
  UNUSED(context);
}

void ParallelContext::thread_broadcast(size_t source_id, void * data, size_t size)
{
  /* write to buf */
  if (_thread_id == source_id)
  {
    memcpy((void *) _threadgroup->reduction_buf.data(), data, size);
  }

  /* synchronize */
  thread_barrier();

  __sync_synchronize();

  /* read from buf*/
  if (_thread_id != source_id)
  {
    memcpy(data, (void *) _threadgroup->reduction_buf.data(), size);
  }

  thread_barrier();
}

void ParallelContext::thread_send_master(size_t source_id, void * data, size_t size) const
{
  /* write to buf */
  if (_thread_id == source_id && data && size)
  {
    memcpy((void *) _threadgroup->reduction_buf.data(), data, size);
  }

  /* synchronize */
  barrier();

  __sync_synchronize();

  /* read from buf*/
  if (_thread_id == 0)
  {
    memcpy(data, (void *) _threadgroup->reduction_buf.data(), size);
  }

  barrier();
}

void ParallelContext::mpi_gather_custom(std::function<int(void*,int)> prepare_send_cb,
                                        std::function<void(void*,int)> process_recv_cb)
{
#if(MPI_ENABLED)
  /* we're gonna use _parallel_buf, so make sure other threads don't interfere... */
  UniqueLock lock;

  if (_rank_id == 0)
  {
    for (size_t r = 1; r < _num_ranks; ++r)
    {
      int recv_size;
      MPI_Status status;
      MPI_Probe(r, 0, _comm, &status);
      MPI_Get_count(&status, MPI_BYTE, &recv_size);

//      printf("recv: %lu\n", recv_size);

      _parallel_buf.reserve(recv_size);

      MPI_Recv((void*) _parallel_buf.data(), recv_size, MPI_BYTE,
               r, 0, _comm, MPI_STATUS_IGNORE);

      process_recv_cb(_parallel_buf.data(), recv_size);
    }
  }
  else
  {
    auto send_size = prepare_send_cb(_parallel_buf.data(), _parallel_buf.capacity());
//    printf("sent: %lu\n", send_size);

    MPI_Send(_parallel_buf.data(), send_size, MPI_BYTE, 0, 0, _comm);
  }
#else
  UNUSED(prepare_send_cb);
  UNUSED(process_recv_cb);
#endif
}
