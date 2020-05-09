#ifndef TRAFT_TIMER_H
#define TRAFT_TIMER_H

#include <iostream>
#include <assert.h>
#include <chrono>  
#include <ratio> 
#include <map>
#include <cstddef>
#include <mutex>
#include  <condition_variable>

#include "Thread.hpp"
#include "Context.hpp"

class SafeTimer: public UPThread{
private:
	std::condition_variable cond;
	std::mutex mtx;

	using uptime_point = std::chrono::system_clock::time_point;
	
	/*
	* 定时任务集合，key为运行时间点,value为定时任务
	*/
	using schedule_map = std::multimap<std::chrono::system_clock::time_point,UPContext*>;
	schedule_map schedule;
	/*
	* 根据UPContext，快速在shedule_map中检索
	*/
	using context_map = std::map<UPContext*,schedule_map::iterator>;
	context_map ctx_map;
	
	/*
	* True: 成功取消的UPContext，保证不会被运行；
	* False：成功取消的UPContext，也有可能会被运行；
	* 实现细节：True：加锁状态下运行，False：运行时会先释放锁
	*/
	bool safe_callbacks;
	
	bool stop;

	/*
	* 线程入口函数
	*/
	void* entry() override;
	
	/*
	* 定时任务轮询线程
	*/
	void time_thread();

    void dump(const char * caller = 0) const;

public:
	SafeTimer(const SafeTimer&)=delete;
	SafeTimer& operator=(const SafeTimer&)=delete;
	
	SafeTimer(bool safe_callbacks=true);
	virtual ~SafeTimer();
	
	void init();
	void shutdown();

	bool empty(){
		std::lock_guard<std::mutex> lock{mtx};
		return ctx_map.empty();
	}
	
	
	/*
	* 添加定时任务：
	* 1. 相对时间之后运行
	* 2. 指定的时间点运行
	*/
	UPContext* add_event_after(int seconds,UPContext*);
	UPContext* add_event_at(uptime_point,UPContext*);
	
	/*
	* 取消定时任务
	*/
	bool cancel_event(UPContext*);
	void cancel_all_event();
};

#endif