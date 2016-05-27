#include "Threading.h"

static DWORD ToDword() {
	return 0;
}

template<typename T, typename ... Types>
static DWORD ToDword(T t, const Types& ... types) {
	return static_cast<DWORD>(t) | ToDword(types...);
}

Luxko::Threading::SynchronizationAccessRight Luxko::Threading::operator|(SynchronizationAccessRight a, SynchronizationAccessRight b)
{
	return static_cast<SynchronizationAccessRight>(ToDword(a, b));
}

Luxko::Threading::SynchronizationAccessRight Luxko::Threading::operator&(SynchronizationAccessRight a, SynchronizationAccessRight b)
{
	return static_cast<SynchronizationAccessRight>(ToDword(a)&ToDword(b));
}


Luxko::Threading::Event Luxko::Threading::Event::OpenEvent(const std::wstring& eventName, EventAccessRight ear /*= EventAccessRight::ModifyState*/, SynchronizationAccessRight sar /*= SynchronizationAccessRight::Synchronize*/, bool inheritable/* = false*/)
{
	Event e;
	e._hEvent = ::OpenEvent(ToDword(ear, sar), inheritable, eventName.c_str());
	return e;
}

Luxko::Threading::EventNotifyOne Luxko::Threading::EventNotifyOne::Open(std::wstring& eventName, EventAccessRight ear /*= EventAccessRight::ModifyState*/, SynchronizationAccessRight sar /*= SynchronizationAccessRight::Synchronize*/, bool inheritable /*= false*/)
{
	auto e = Event::OpenEvent(eventName, ear, sar, inheritable);
	EventNotifyOne se;
	se._hEvent = e.Handle();
	return se;
}

Luxko::Threading::EventNotifyOne Luxko::Threading::EventNotifyOne::CreateNameLess(bool initiallySet, SECURITY_ATTRIBUTES* sa /*= nullptr*/)
{
	EventNotifyOne se;
	se._hEvent = CreateEvent(sa, FALSE, initiallySet, nullptr);
	return se;
}

Luxko::Threading::EventNotifyOne Luxko::Threading::EventNotifyOne::CreateNamed(bool initiallySet, const std::wstring& name, SECURITY_ATTRIBUTES* sa /*= nullptr*/)
{
	EventNotifyOne se;
	se._hEvent = CreateEvent(sa, FALSE, initiallySet, name.c_str());
	return se;
}

Luxko::Threading::EventNotifyAll Luxko::Threading::EventNotifyAll::Open(std::wstring& eventName, EventAccessRight ear /*= EventAccessRight::ModifyState*/, SynchronizationAccessRight sar /*= SynchronizationAccessRight::Synchronize*/, bool inheritable /*= false*/)
{
	auto e = Event::OpenEvent(eventName, ear, sar, inheritable);
	EventNotifyAll se;
	se._hEvent = e.Handle();
	return se;
}

Luxko::Threading::EventNotifyAll Luxko::Threading::EventNotifyAll::CreateNameLess(bool initiallySet, SECURITY_ATTRIBUTES* sa /*= nullptr*/)
{
	EventNotifyAll ena;
	ena._hEvent = CreateEvent(sa, TRUE, initiallySet, nullptr);
	return ena;
}

Luxko::Threading::EventNotifyAll Luxko::Threading::EventNotifyAll::CreateNamed(bool initiallySet, const std::wstring& name, SECURITY_ATTRIBUTES* sa /*= nullptr*/)
{
	EventNotifyAll ena;
	ena._hEvent = CreateEvent(sa, TRUE, initiallySet, name.c_str());
	return ena;
}

Luxko::Threading::Overlap::Overlap(long long offset, const Event& e)
{
	LARGE_INTEGER li;
	li.QuadPart = offset;
	_ov.Offset = li.LowPart;
	_ov.OffsetHigh = li.HighPart;
	_ov.hEvent = e.Handle();
}

long long Luxko::Threading::Overlap::Offset() const
{
	LARGE_INTEGER li;
	li.LowPart = _ov.Offset;
	li.HighPart = _ov.OffsetHigh;
	return li.QuadPart;
}

void Luxko::Threading::Overlap::SetEvent(const Event& e)
{
	_ov.hEvent = e.Handle();
}

void Luxko::Threading::Overlap::Offset(long long offset)
{
	LARGE_INTEGER li;
	li.QuadPart = offset;
	_ov.Offset = li.LowPart;
	_ov.OffsetHigh = li.HighPart;
}
