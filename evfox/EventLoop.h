#ifndef __EVFOX_EVENT_LOOP_H__
#define __EVFOX_EVENT_LOOP_H__
#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

#include <event.h>
#include <evutil.h>
#include "EventInvoker.h"

namespace evfox
{
class EventLoop
{
public:
	typedef std::function<void()> WorkFunctor;
public:
	EventLoop();
	~EventLoop();

	void Init();

	void Run();
public:
	bool IsInLoopThread() const {
		return tid_ == std::this_thread::get_id();
	}
private:
	void DoWorkFunctor();
private:
	struct event_base* event_base_;

	std::thread::id tid_;

	std::atomic<bool> notified_;

	std::mutex mutex_;
	std::vector<WorkFunctor> work_functors_;

	EventInvoker* event_invoker_;
};

}
#endif // !__EVFOX_EVENT_LOOP_H__

