#include "EventLoop.h"
#include "LogUtils.h"

namespace evfox
{
EventLoop::EventLoop()
{
	notified_.store(false);
	struct event_config* cfg = event_config_new();
	if (cfg)
	{
		// Does not cache time to get a preciser timer
		event_config_set_flag(cfg, EVENT_BASE_FLAG_NO_CACHE_TIME);
		event_base_ = event_base_new_with_config(cfg);
		event_config_free(cfg);
	}
	event_invoker_ = new EventInvoker(event_base_, std::bind(&EventLoop::DoWorkFunctor, this));
}

EventLoop::~EventLoop()
{
	event_base_free(event_base_);
	delete event_invoker_;
	event_invoker_ = nullptr;
}

void EventLoop::Init()
{
	tid_ = std::this_thread::get_id();
	event_invoker_->Init();
}

void EventLoop::Run()
{
	event_invoker_->AddWatch();

	int result = ::event_base_dispatch(event_base_);
	if (result)
	{
		EVFOX_LOG_ERROR << "EventLoop::Run event_base_dispatch error " << result;
		return;
	}
	if (event_invoker_) {
		delete event_invoker_;
		event_invoker_ = nullptr;
	}
}
void EventLoop::DoWorkFunctor()
{
	notified_.store(false);
	std::vector<WorkFunctor> functors;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		work_functors_.swap(functors);
	}
	
	for (auto func : functors)
	{
		func();
	}
}
}
