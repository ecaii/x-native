#pragma once
#include <chrono>

namespace shared
{
	namespace Profiler
	{
		//--------------------------------------------------------------
		// A developer timer for finding how long a context takes to
		// execute. The destructor will dump results into Log.
		//--------------------------------------------------------------
		class Timer
		{
		public:
			Timer(const char* psTimerId, bool bProc = false);
			~Timer();

			void proc_finish();

			float GetMS() const { return m_fMS; };

		private:
			const char* m_psTimerId;
			typedef std::chrono::high_resolution_clock clock_;
			typedef std::chrono::duration<double, std::ratio<1> > second_;
			std::chrono::time_point<clock_> beg_;
			float m_fMS = 0.0f;
			bool m_bProcInduced;
		};
	};
}