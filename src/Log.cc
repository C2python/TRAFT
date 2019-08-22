#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h> 
#include<cerrno>

#include "Log.hpp"
#include "Config.hpp"

#define DEFAULT_MAX_NEW    100
#define DEFAULT_MAX_RECENT 10000

namespace TRAFT{

    extern Context* cct;

    static OnExitManager exit_callbacks;

    static void log_on_exit(void *p)
    {
        Log *l = *(Log **)p;
        if (l){
            l->flush();
        }
        delete (Log **)p;
    }

    Log::Log(const std::string& log_name):
                max_new(DEFAULT_MAX_NEW),
                max_recent(DEFAULT_MAX_RECENT),
                m_queue_mutex_holder(0),
                m_flush_mutex_holder(0),
                m_stop(true),
                m_fd(-1),
                m_exit(nullptr),
                m_uid(0),
                m_gid(0),
                m_log_file(log_name){
                   open_log_file(log_name);
    }
    Log::~Log(){
        if (m_exit){
            *m_exit = nullptr;
        }

        assert(!is_started());
        if (m_fd >= 0)
            ::close(m_fd);
    }

    void Log::set_flush_on_exit(){

        if (m_exit == nullptr){
            m_exit = new (Log*)(this);
            exit_callbacks.add_callback(log_on_exit,m_exit);
        }

    }

    void Log::submit_entry(std::shared_ptr<Entry> log){
        std::unique_lock<std::mutex> lock(lock_queue);
        m_queue_mutex_holder = pthread_self();

        cond_queue.wait(lock,[&]{return m_new.size() < max_new;});
        
        m_new.push(log);

        cond_flush.notify_one();
        m_queue_mutex_holder = 0;
        lock.unlock();
    }

    bool Log::is_started(){
        return m_stop == false;
    }

    void Log::stop(){
        assert(is_started());
        {
            std::lock_guard<std::mutex> lock(lock_queue);
            m_stop = true;
            cond_flush.notify_one();
            cond_queue.notify_all();
        }
    }

    void Log::start(){
        assert(!is_started());
        {
            std::lock_guard<std::mutex> lock(lock_queue);
            m_stop = false;
        }
        logHandle = std::move(std::thread(&Log::thread_handle,this,"Log"));
    }

    int Log::open_log_file(const std::string& log_file){
        std::unique_lock<std::mutex> lock(lock_flush);
        m_flush_mutex_holder = pthread_self();
        m_log_file = log_file;

        if (m_fd > 0){
            ::close(m_fd);
        }

        if (m_log_file.length() > 0){
            m_fd = ::open(m_log_file.c_str(), O_CREAT|O_WRONLY|O_APPEND, 0644);
            if (m_fd >= 0 && (m_uid || m_gid)) {
                int r = ::fchown(m_fd, m_uid, m_gid);
                if (r < 0) {
                    r = -errno;
                    std::cerr << "failed to chown " << m_log_file << ": " << r << std::endl;
                }   
            }
        }else {
                m_fd = -1;
        }
        m_flush_mutex_holder = 0;
        lock.unlock();
        return m_fd >= 0? 1:-1;
    }

    int Log::set_log_file(const std::string& log_file){
        int ret = open_log_file(log_file);
        return ret;
    }

    std::shared_ptr<Entry> Log::create_entry(int level,std::thread::id p_id,const std::string& msg){
        return std::make_shared<Entry>(getCurrentTimeSeconds(),p_id,level,msg.c_str());
    }

    /*
    单消费者模型。若要支持多消费者模型，需要修改flush()处代码
    */
    void Log::thread_handle(std::string thread_name){
        std::unique_lock<std::mutex> lock(lock_queue);
        m_queue_mutex_holder = pthread_self();
        while ( !m_stop ){
            m_queue_mutex_holder = 0;
            cond_flush.wait(lock,[&]{return !m_new.empty();});
            lock.unlock();
            flush();
            lock.lock();
            m_queue_mutex_holder = pthread_self();
        }
        m_queue_mutex_holder = 0;
        lock.unlock();
        flush();
    }

    void Log::flush(){

        std::unique_lock<std::mutex> queue_lock(lock_queue);
        m_queue_mutex_holder = pthread_self();
        std::unique_lock<std::mutex> flush_lock(lock_flush);
        m_flush_mutex_holder = pthread_self();

        std::queue<std::shared_ptr<Entry>> flush_pr;
        m_new.swap(flush_pr);

        cond_queue.notify_all();
        m_queue_mutex_holder = 0;
        queue_lock.unlock();

        _flush(flush_pr);

        while ( m_recent.size() >= max_recent ){
            m_recent.pop();
        }

        m_flush_mutex_holder = 0;
        flush_lock.unlock();

    }

    void Log::_flush(std::queue<std::shared_ptr<Entry>>& flush_pr,bool crash){
        /*
        crash: To do
        */
       std::shared_ptr<Entry> e;
       while (!flush_pr.empty()){
            e = flush_pr.front();
            flush_pr.pop();
            char* buf;
            char buf0[e->size()+2];
            size_t buflen = 0;
            //Alloc heap buf if need more then 64K buffer.
            bool need_dynamic = (e->size()+2) > 0x10000;
            if ( need_dynamic ){
                buf = new char[e->size()+2];
            }else{
                buf = buf0;
            }
            buflen = e->size()+2;
            e->snprintf(buf,e->size()+1);
            buf[buflen-1] = '\n';
            ssize_t r = safe_write(m_fd,buf,buflen);
            if (r < 0){
                std::cerr<<"Problem Occured in Writing to Log File: "<<m_log_file<<cpp_strerror(r)<<std::endl;
            }
            if ( need_dynamic ){
                delete []buf;
            }
            m_recent.push(e);
       }
    }

    void Log::join(){
        logHandle.join();
    }

    void Log::detach(){
        logHandle.detach();
    }

}