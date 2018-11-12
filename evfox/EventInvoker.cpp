#include "EventInvoker.h"
#include <cstring>
#include <cstdlib>
#include "LogUtils.h"

namespace evfox
{
EventInvoker::EventInvoker(struct event_base* base, const InvokerFunc& func)
{
	event_ = nullptr;
	event_base_ = base;
	invoker_func_ = func;
	memset(socket_pair_, 0, 2 * sizeof(evutil_socket_t));
}

EventInvoker::~EventInvoker()
{
	UnInit();
}

void EventInvoker::Init()
{
	if (evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, socket_pair_) < 0) {
		int err = errno;
		EVFOX_LOG_ERROR << "EventInvoker::Init evutil_socketpair error errno=" << err << " " << strerror(err);
		return;
	}
	if (evutil_make_socket_nonblocking(socket_pair_[0]) < 0 ||
		evutil_make_socket_nonblocking(socket_pair_[1]) < 0) {
		EVFOX_LOG_ERROR << "EventInvoker::Init evutil_make_socket_nonblocking error";
	}

	event_ = ::event_new(event_base_, socket_pair_[1], EV_READ | EV_PERSIST, &EventInvoker::OnReadHandle, this);
}

void EventInvoker::UnInit()
{
	::event_free(event_);
	if (socket_pair_[0] > 0)
	{
		EVUTIL_CLOSESOCKET(socket_pair_[0]);
		EVUTIL_CLOSESOCKET(socket_pair_[1]);
	}
}

void EventInvoker::AddWatch()
{
	::event_add(event_, nullptr);
}

void EventInvoker::Invoker()
{
	char data[1] = { 0 };
	::send(socket_pair_[0], data, sizeof(data), 0);
}
void EventInvoker::OnReadHandle(evutil_socket_t sock, short event, void * context)
{
	EventInvoker* pThis = static_cast<EventInvoker*>(context);
	char data[1] = { 0 };
	if (::recv(pThis->socket_pair_[1], data, sizeof(data), 0) > 0)
	{
		pThis->invoker_func_();
	}
}
}