#ifndef TRAFT_ENTRY_H
#define TRAFT_ENTRY_H

#include "PrebufferedStream.hpp"
#include "util.hpp"
#include<string>
#include<thread>
#include<ostream>

namespace TRAFT{

#define TRAFT_LOG_ENTRY_PREALLOC 80

struct Entry{

    using pthread_id=pthread_t;
    up_time log_time;
    pthread_id m_t;
    PrebufferedStreambuf m_streambuf;
    char m_buf[TRAFT_LOG_ENTRY_PREALLOC];
    size_t m_buf_len;
    short prior;

    Entry(pthread_id p_id,short prior = 5):Entry(prior,getCurrentTimeSeconds(),p_id,nullptr){}
    Entry(short prior,up_time s,pthread_id id,const char *msg=nullptr):
        log_time(s),
        m_t(id),
        prior(prior),
        m_streambuf(m_buf, sizeof(m_buf)),
        m_buf_len(sizeof(m_buf))
    {
        std::ostream os(&m_streambuf);
        char buf[100];
        int r = std::strftime(buf,sizeof(buf),"%c %Z", std::localtime(&log_time));
        os << buf;
        os << " ";
        os << "Level: "<< prior << " ";
        os << m_t << " ";
        if (msg) {
            os << msg;
            //os << "\n";
        }
    }
    const std::string get_str() const {

        return m_streambuf.get_str();
    }
    size_t size() const {
        return m_streambuf.size();
    }
    int snprintf(char* dst, size_t avail) const {
        return m_streambuf.snprintf(dst, avail);
    }

};

}



#endif