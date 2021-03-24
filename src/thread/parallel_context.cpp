#include "parallel_context.h"
#include "../plldefs.h"

#include <string.h>

using namespace std;

// This is just a default size; the buffer will be resized later according to #part and #threads
#define PARALLEL_BUF_SIZE (128 * 1024)

size_t ParallelContext::_num_threads = 1;
size_t ParallelContext::_num_ranks = 1;
size_t ParallelContext::_rank_id = 0;
thread_local size_t ParallelContext::_thread_id = 0;
std::vector<ThreadType> ParallelContext::_threads;
std::vector<char> ParallelContext::_parallel_buf;
std::unordered_map<ThreadIDType, ParallelContext> ParallelContext::_thread_ctx_map;
MutexType ParallelContext::mtx;

#if(MPI_ENABLED)
MPI_Comm ParallelContext::_comm = MPI_COMM_WORLD;
bool ParallelContext::_owns_comm = true;
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
//    printf("size: %lu, rank: %lu\n", _num_ranks, _rank_id);
  }
#else
  UNUSED(argc);
  UNUSED(argv);
  UNUSED(comm);
#endif
}

void ParallelContext::start_thread(size_t thread_id, const std::function<void()>& thread_main)
{
  ParallelContext::_thread_id = thread_id;
  thread_main();
}

void ParallelContext::init_pthreads(const mt_options_t & opts, const std::function<void()>& thread_main)
{
  _num_threads = opts.n_threads;
  _parallel_buf.reserve(PARALLEL_BUF_SIZE);

#ifdef PTHREADS
  /* Launch threads */
  for (size_t i = 1; i < _num_threads; ++i)
  {
    _threads.emplace_back(ParallelContext::start_thread, i, thread_main);
  }
#endif
}

void ParallelContext::resize_buffer(size_t size)
{
  _parallel_buf.reserve(size);
}

void ParallelContext::finalize(bool force)
{
#ifdef PTHREADS
  for (thread& t: _threads)
  {
    if (force)
      t.detach();
    else
      t.join();
  }
  _threads.clear();
#endif

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

void ParallelContext::barrier()
{
#if(MPI_ENABLED)
  mpi_barrier();
#endif

#ifdef PTHREADS
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

void ParallelContext::thread_barrier()
{
  static volatile unsigned int barrier_counter = 0;
  static thread_local volatile int myCycle = 0;
  static volatile int proceed = 0;

  __sync_fetch_and_add( &barrier_counter, 1);

  if(_thread_id == 0)
  {
    while(barrier_counter != ParallelContext::_num_threads);
    barrier_counter = 0;
    proceed = !proceed;
  }
  else
  {
    while(myCycle == proceed);
    myCycle = !myCycle;
  }
}

void ParallelContext::thread_reduce(double * data, size_t size, int op)
{
  /* synchronize */
  thread_barrier();

  double *double_buf = (double*) _parallel_buf.data();

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
        for (j = 0; j < ParallelContext::_num_threads; ++j)
          data[i] += double_buf[j * size + i];
      }
      break;
      case PLLMOD_COMMON_REDUCE_MAX:
      {
        data[i] = _parallel_buf[i];
        for (j = 1; j < ParallelContext::_num_threads; ++j)
          data[i] = max(data[i], double_buf[j * size + i]);
      }
      break;
      case PLLMOD_COMMON_REDUCE_MIN:
      {
        data[i] = _parallel_buf[i];
        for (j = 1; j < ParallelContext::_num_threads; ++j)
          data[i] = min(data[i], double_buf[j * size + i]);
      }
      break;
    }
  }

  //needed?
//  parallel_barrier(useropt);
}


void ParallelContext::parallel_reduce(double * data, size_t size, int op)
{
#ifdef PTHREADS
  if (_num_threads > 1)
    thread_reduce(data, size, op);
#endif

#if(MPI_ENABLED)
  if (_num_ranks > 1)
  {
    thread_barrier();

    if (_thread_id == 0)
    {
      MPI_Op reduce_op = MPI_UNDEFINED;
      if (op == PLLMOD_COMMON_REDUCE_SUM)
        reduce_op = MPI_SUM;
      else if (op == PLLMOD_COMMON_REDUCE_MAX)
        reduce_op = MPI_MAX;
      else if (op == PLLMOD_COMMON_REDUCE_MIN)
        reduce_op = MPI_MIN;
      else
        assert(0);

#if 1
      MPI_Allreduce(MPI_IN_PLACE, data, size, MPI_DOUBLE, reduce_op, _comm);
#else
      // not sure if MPI_IN_PLACE will work in all cases...
      MPI_Allreduce(data, _parallel_buf.data(), size, MPI_DOUBLE, reduce_op, _comm);
      memcpy(data, _parallel_buf.data(), size * sizeof(double));
#endif
    }

    if (_num_threads > 1)
      thread_broadcast(0, data, size * sizeof(double));
  }
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
    memcpy((void *) _parallel_buf.data(), data, size);
  }

  /* synchronize */
  thread_barrier();

//  pthread_barrier_wait(&barrier);
  __sync_synchronize();

  /* read from buf*/
  if (_thread_id != source_id)
  {
    memcpy(data, (void *) _parallel_buf.data(), size);
  }

  thread_barrier();
}

void ParallelContext::thread_send_master(size_t source_id, void * data, size_t size) const
{
  /* write to buf */
  if (_thread_id == source_id && data && size)
  {
    memcpy((void *) _parallel_buf.data(), data, size);
  }

  /* synchronize */
  barrier();

//  pthread_barrier_wait(&barrier);
  __sync_synchronize();

  /* read from buf*/
  if (_thread_id == 0)
  {
    memcpy(data, (void *) _parallel_buf.data(), size);
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
