#ifndef TRAFT_THREAD_H
#define TRAFT_THREAD_H

#include <pthread.h>
#include "dout.hpp"
#include "compat.hpp"

class UPThread{
 private:
  pthread_t thread_id;
  pid_t pid;
  int cpuid;
  const char *thread_name;

  void *entry_wrapper();

 public:
  UPThread(const UPThread&) = delete;
  UPThread& operator=(const UPThread&) = delete;

  UPThread();
  virtual ~UPThread();

 protected:
  virtual void *entry() = 0;

 private:
  static void *_entry_func(void *arg);

 public:
  const pthread_t &get_thread_id() const;
  pid_t get_pid() const { return pid; }
  bool is_started() const;
  bool am_self() const;
  int kill(int signal);
  int try_create(size_t stacksize);
  void create(const char *name, size_t stacksize = 0);
  int join(void **prval = 0);
  int detach();
  int set_affinity(int cpuid);
};

#endif