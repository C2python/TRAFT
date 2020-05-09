#include "Timer.hpp"

SafeTimer::SafeTimer(bool safe_callbacks)
	:safe_callbacks(safe_callbacks),
	stop(false)
{}

SafeTimer::~SafeTimer(){
	assert(stop == true);
}

void* SafeTimer::entry(){
	time_thread();
	return nullptr;
}

void SafeTimer::time_thread(){
    std::unique_lock<std::mutex> lock{mtx};
	while(!stop){
		auto now = std::chrono::system_clock::now();
		if (schedule.empty()){
			cond.wait(lock);
		}else{
			cond.wait_until(lock,schedule.begin()->first);
		}
		while(!schedule.empty()){
			auto p = schedule.begin();
			if (p->first > now)
				break;
			auto* ctx = p->second;
			ctx_map.erase(ctx);
			schedule.erase(p);
			if (!safe_callbacks){
				lock.unlock();
				ctx->complete(0);
				lock.lock();
			}else{
				ctx->complete(0);
			}
			if (!safe_callbacks && !stop){
				break;
			}
		}
	}
}

void SafeTimer::init(){
	create("SafeTimer");
}

void SafeTimer::shutdown(){
	{
		cancel_all_event();
        std::lock_guard<std::mutex> lock(mtx);
		stop = true;
		cond.notify_all();
	}
	join();
}

bool SafeTimer::cancel_event(UPContext* ctx){
	std::lock_guard<std::mutex> lock(mtx);
	auto iter = ctx_map.find(ctx);
	if (iter == ctx_map.end()){
		return false;
	}
	delete iter->first;
	schedule.erase(iter->second);
	ctx_map.erase(iter);
	cond.notify_all();
	return true;
}

void SafeTimer::cancel_all_event(){
	std::lock_guard<std::mutex> lock(mtx);
	while(!ctx_map.empty()){
        auto ctx = ctx_map.begin();
		delete ctx->first;
		schedule.erase(ctx->second);
		ctx_map.erase(ctx);
	}
	cond.notify_all();
}

void SafeTimer::dump(const char* caller) const {
    if (!caller){
        caller = "";
    }
    dout(10)<<"Dump "<<caller<<dendl;
    for (schedule_map::const_iterator iter = schedule.begin();iter!=schedule.end();++iter){
        dout(10)<<std::fixed<<std::chrono::duration<double>(iter->first.time_since_epoch()).count()<<"s->"<<iter->second<<dendl;
    }
}

UPContext* SafeTimer::add_event_after(int sec,UPContext* ctx){
	std::chrono::seconds t(sec);
	auto tp = std::chrono::system_clock::now() + t;
	return add_event_at(tp,ctx);
}

UPContext* SafeTimer::add_event_at(uptime_point when,UPContext* ctx){
	if (stop){
		delete ctx;
		return nullptr;
	}
	
	schedule_map::iterator iter = schedule.insert(std::pair<uptime_point,UPContext*>(when,ctx));
    std::pair<context_map::iterator, bool> citer = ctx_map.insert(std::pair<UPContext*,schedule_map::iterator>(ctx,iter));
	
	if (citer.second){
		return ctx;
	}
	
	if (iter == schedule.begin()){
		cond.notify_all();
	}
	
	return ctx;
}


