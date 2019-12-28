#include "profiletimer.h"
#include "logging.h"

shared::Profiler::Timer::Timer(const char* psTimerId, bool bProc) :
	m_psTimerId(psTimerId),
	beg_(clock_::now()),
	m_bProcInduced(bProc)
{
}

void shared::Profiler::Timer::proc_finish()
{
	m_fMS = static_cast<float>(std::chrono::duration_cast<second_>(clock_::now() - beg_).count());
	beg_ = clock_::now();

	DbgWarn("[PROFILE] [%s] %f", m_psTimerId, m_fMS);
}

shared::Profiler::Timer::~Timer()
{
	if (!m_bProcInduced)
	{
		proc_finish();
	}
}