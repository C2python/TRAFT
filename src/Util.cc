#include "Util.hpp"

namespace TRAFT{

int _get_bits_of(int v) {
    int n = 0;
    while (v) {
      n++;
      v = v >> 1;
    }
    return n;
}

up_time getCurrentTimeSeconds(){
  return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

char* check_err(int result,char* buf,int err){
    if ( result ){
        sprintf(buf,"unknown error: %d",err);
    }
    return buf;
}

char* check_err(char* result, char*, int){
    return result;
}

std::string cpp_strerror(int err){
    char buf[512];
    char* errmsg;
    if ( err < 0 ){
        err = -err;
    }
    std::ostringstream oss;
    buf[0] = '\0';
    /*
    *   Two Version Strerror_r:
    *   XSI-Compliant Version
    *       Note: strerror_r return -1 and errno = ERANGE if buf space not enough.
    *       We should realloc buf when fails.
    *       To Fix it.
    * 
    *   GNU Version: Message is Returned. Buf not Used.
    */
    errmsg = check_err(strerror_r(err,buf,sizeof(buf)),buf,err);
    oss<<"("<<err<<")"<<errmsg;
    return oss.str();
}

ssize_t safe_read(int fd,void* buf,size_t count){
    size_t cnt = 0;
    while ( cnt < count ){
        ssize_t r = read(fd,buf,count);
        if ( r <= 0 ){
        if ( r == 0 )
            return cnt;
        if ( errno == EINTR )
            continue;
        return -errno;
        }
        cnt += r;
        count -= r;
    }
        return cnt;
}

ssize_t safe_read_exact(int fd,void* buf,size_t count){
    ssize_t r = read(fd,buf,count);
    if ( r < 0 )
        return r;
    if ( r != count )
        return EDOM;
    return r;

}

ssize_t safe_write(int fd,const void* buf,size_t count){
    while(count > 0){
        ssize_t r = write(fd,buf,count);
        if ( r < 0 ){
            if ( errno == EINTR )
                continue;
            return -errno;
        }
        count -= r;
        buf = (char*)buf + r;
    }
    return 0;
}

}