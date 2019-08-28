#ifndef TRAFT_LOG_H
#define TRAFT_LOG_H

#include<vector>
#include<queue>
#include<mutex>
#include<thread>
#include<condition_variable>
#include<atomic>
#include<cassert>
#include<string>
#include<iostream>

#include "on_exit.hpp"
#include "util.hpp"
#include "entry.hpp"

namespace TRAFT{

class Log{
    
private:

    int m_fd; //Log file descriptor

    Log** m_exit;

    std::vector<std::shared_ptr<Entry>> m_new;
    std::queue<std::shared_ptr<Entry>> m_recent;
    int max_new;
    int max_recent;

    std::string m_log_file;
    uid_t m_uid;
    gid_t m_gid;

    std::atomic<bool> m_stop{true};

    std::thread logHandle;
    std::mutex lock_queue;
    std::mutex lock_flush;
    std::condition_variable cond_queue;
    std::condition_variable cond_flush;

    pthread_t m_queue_mutex_holder;
    pthread_t m_flush_mutex_holder;

    void _flush(std::vector<std::shared_ptr<Entry>>&,bool crash=false);

public:

    Log(const std::string&);
    ~Log();
    void set_flush_on_exit();
    void flush();
    void start();
    void stop();
    bool is_started();
    int open_log_file(const std::string&);
    void thread_handle(const std::string name);
    void join();
    void detach();
    std::shared_ptr<Entry> create_entry(short level,pthread_t,const std::string&);
    std::shared_ptr<Entry> create_entry(short level,pthread_t p_id = pthread_self());
    void submit_entry(std::shared_ptr<Entry>);
    int set_log_file(const std::string& log_file);
    std::string get_log_file(){
        return m_log_file;
    }

};

}

#endif