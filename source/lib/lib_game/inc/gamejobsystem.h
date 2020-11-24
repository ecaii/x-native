#pragma once
#include <functional>
#include <mutex>
#include "hash.h"

class GameJobSystem
{
public:
	static const uint32_t MaxJobsPerUpdate = 128;

	// Fixed size very simple thread safe ring buffer
	template <typename T, uint32_t capacity>
	class ThreadSafeRingBuffer
	{
	public:
		// Push an item to the end if there is free space
		//  Returns true if succesful
		//  Returns false if there is not enough space
		__forceinline bool push_back(const T& item)
		{
			bool result = false;
			m_lock.lock();
			uint32_t next = (m_head + 1) % capacity;
			if (next != m_tail)
			{
				m_data[m_head] = item;
				m_head = next;
				result = true;
			}
			m_lock.unlock();
			return result;
		}

		// Get an item if there are any
		//  Returns true if succesful
		//  Returns false if there are no items
		__forceinline bool pop_front(T& item)
		{
			bool result = false;
			m_lock.lock();
			if (m_tail != m_head)
			{
				item = m_data[m_tail];
				m_tail = (m_tail + 1) % capacity;
				result = true;
			}
			m_lock.unlock();
			return result;
		}

	private:
		T          m_data[capacity];
		uint32_t   m_head = 0;
		uint32_t   m_tail = 0;
		std::mutex m_lock; // this just works better than a spinlock here (on windows)
	};

	typedef std::function<void()>    JobClosure;

	void   Initialize();
	void   Shutdown();
	void   Start(const shared::CompilerHashU8& name);
	void   ExecuteOrQueue(const JobClosure& closure);
	void   Wait();
	bool   IsBusy();

	__forceinline void  Poll();

	static GameJobSystem& GetInstance();

protected:
	struct JobDispatchArgs
	{
		uint32_t jobIndex;
		uint32_t groupIndex;
	};

	ThreadSafeRingBuffer<JobClosure, MaxJobsPerUpdate> m_JobPool;
	
	std::condition_variable m_WakeCondition;
	std::mutex m_WakeMutex;
	std::atomic<uint64_t> m_FinishedLabels;
	
	uint32_t  m_CurrentLabel = 0;
	uint32_t  m_NumCores     = 0;
	uint32_t  m_NumThreads   = 0;

	bool      m_ShuttingDown = false;

	void   Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(JobDispatchArgs)>& job);
};