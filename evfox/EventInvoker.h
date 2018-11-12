/*

*/
#ifndef __EVFOX_EVENT_INVOKER_H__
#define __EVFOX_EVENT_INVOKER_H__

#include <functional>
#include <event.h>
#include <evutil.h>

namespace evfox
{
	class EventInvoker
	{
	public:
		typedef std::function<void()> InvokerFunc;
	public:
		EventInvoker(struct event_base* base, const InvokerFunc& func);
		virtual ~EventInvoker();

	public:
		void Init();

		void UnInit();

		void AddWatch();

		void Invoker();
	private:
		static void OnReadHandle(evutil_socket_t sock, short event, void * context);
	private:
		struct event* event_;

		struct event_base* event_base_;

		evutil_socket_t socket_pair_[2];

		InvokerFunc invoker_func_;
	};
}
#endif //! __EVFOX_EVENT_INVOKER_H__
