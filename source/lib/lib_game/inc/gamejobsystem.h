#pragma once
#include <functional>
#include <mutex>
#include "hash.h"

class GameJobSystem
{
public:
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
			uint32_t next = (m_head + 1) % m_capacity;
			if (next != m_tail)
			{
				m_data[m_head] = item;
				head = next;
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
				item = m_data[tail];
				m_tail = (m_tail + 1) % m_capacity;
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

	void   Start(const shared::CompilerHashU8& name);
	void   Execute(const JobClosure& closure);
	void   Wait();
	bool   IsBusy();

	static GameJobSystem& GetInstance();

protected:
	ThreadSafeRingBuffer<JobClosure, 3> m_JobPool;
};
