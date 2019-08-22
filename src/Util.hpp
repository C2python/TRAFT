//Author: Zhangwen
//Date: 2018.05.15

#ifndef UTIL_H
#define UTIL_H

//#include<sys/syscll.h>
#include<chrono>
#include<unistd.h>
#include<iomanip>
#include<ctime>
#include<sstream>
#include<cstdlib>
#include<string.h>
namespace TRAFT{

using up_time = std::time_t;

#define SetThreadName pthread_setname_np

int _get_bits_of(int v) ;

static unsigned _page_size = sysconf(_SC_PAGESIZE);
static unsigned long _page_mask = ~(unsigned long)(_page_size - 1);
static unsigned _page_shift = _get_bits_of(_page_size - 1);

#define TR_PAGE_SIZE _page_size

/*
pid_t gettid(void)
{
  return syscall(SYS_gettid);
}
*/

up_time getCurrentTimeSeconds();

std::string cpp_strerror(int);

ssize_t safe_read(int fd,void* buf,size_t count);

ssize_t safe_read_exact(int fd,void* buf,size_t count);

ssize_t safe_write(int fd,const void* buf,size_t count);

}

#endif