#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h> 
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "Thread.hpp"
#include "signal.hpp"

#ifdef HAVE_SCHED
#include <sched.h>
#endif


pid_t traft_gettid(void)
{
  return syscall(SYS_gettid);
}

static int _set_affinity(int id)
{
#ifdef HAVE_SCHED
  if (id >= 0 && id < CPU_SETSIZE) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);

    CPU_SET(id, &cpuset);

    if (sched_setaffinity(0, sizeof(cpuset), &cpuset) < 0)
      return -1;
    sched_yield();
  }
#endif
  return 0;
}


UPThread::UPThread()
   :thread_id(0),
    pid(0),
    cpuid(-1),
    thread_name(NULL)
{
}

UPThread::~UPThread()
{
}

void* UPThread::_entry_func(void *arg){
    void *r = ((UPThread*)arg)->entry_wrapper();
    return r;
}

void* UPThread::entry_wrapper(){
    int tid = traft_gettid();
    if (tid > 0)
        pid = tid;
  if (pid && cpuid >= 0)
    _set_affinity(cpuid);

  up_pthread_setname(thread_id,thread_name);
  return entry();
}

const pthread_t &UPThread::get_thread_id() const
{
  return thread_id;
}

bool UPThread::is_started() const
{
  return thread_id != 0;
}

bool UPThread::am_self() const
{
  return (pthread_self() == thread_id);
}

int UPThread::kill(int signal)
{
  if (thread_id)
    return pthread_kill(thread_id, signal);
  else
    return -1;
}

int UPThread::try_create(size_t stacksize)
{
    pthread_attr_t *thread_attr = NULL;
    pthread_attr_t thread_attr_loc;
    
    stacksize &= TR_PAGE_SIZE;  // must be multiple of page
    if (stacksize) {
      thread_attr = &thread_attr_loc;
      pthread_attr_init(thread_attr);
      pthread_attr_setstacksize(thread_attr, stacksize);
    }

    int r;
    sigset_t old_sigset;
    int to_block[] = { SIGPIPE , 0 };
    block_signals(to_block, &old_sigset);
    r = pthread_create(&thread_id, thread_attr, _entry_func, (void*)this);
    restore_sigset(&old_sigset);

    if (thread_attr) {
      pthread_attr_destroy(thread_attr);	
    }

    return r;
}

void UPThread::create(const char *name, size_t stacksize)
{
  assert(strlen(name) < 16);
  thread_name = name;

  int ret = try_create(stacksize);
  if (ret != 0) {
    char buf[256];
    snprintf(buf, sizeof(buf), "Thread::try_create(): pthread_create "
	     "failed with error %d", ret);
    lderr<<buf<<dendl;
    assert(ret == 0);
  }
}

int UPThread::join(void **prval)
{
  if (thread_id == 0) {
    return -1;
  }

  int status = pthread_join(thread_id, prval);

  if (status != 0) {
    char buf[256];
    snprintf(buf, sizeof(buf), "Thread::join(): pthread_join "
             "failed with error %d\n", status);
    lderr<<buf<<dendl;
    assert(status == 0);
  }

  thread_id = 0;
  return status;
}

int UPThread::detach()
{
  return pthread_detach(thread_id);
}

int UPThread::set_affinity(int id)
{
  int r = 0;
  cpuid = id;
  if (pid && traft_gettid() == pid)
    r = _set_affinity(id);
  return r;
}