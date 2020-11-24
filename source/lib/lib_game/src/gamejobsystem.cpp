#include "GameJobSystem.h"
#include "logging.h"

GameJobSystem& GameJobSystem::GetInstance()
{
	static GameJobSystem s_Instance;
	return s_Instance;
}

void GameJobSystem::Shutdown()
{
	m_ShuttingDown = true;
	Wait();
}

void GameJobSystem::Initialize()
{
	m_NumCores = std::thread::hardware_concurrency();
	m_NumThreads = std::max(1u, m_NumCores);

	m_FinishedLabels.store(0);

	for (uint32_t threadID = 0; threadID < m_NumThreads; ++threadID)
	{
		DbgLog("Creating idle worker %u", threadID);

		std::thread worker([this] {
			std::function<void()> job; // current job for the thread

			while (true)
			{
				if (m_JobPool.pop_front(job))
				{
					job();
					m_FinishedLabels.fetch_add(1);
				}
				else
				{
					std::unique_lock<std::mutex> lock(m_WakeMutex);
					m_WakeCondition.wait(lock);
				}
			}
			});

		worker.detach();
	}
}

void GameJobSystem::Start(const shared::CompilerHashU8& name)
{
	(void)name;
}

void GameJobSystem::Poll()
{
	m_WakeCondition.notify_one();
	std::this_thread::yield();
}

void GameJobSystem::ExecuteOrQueue(const JobClosure& closure)
{
	if (m_ShuttingDown)
	{
		return;
	}

	m_CurrentLabel += 1;

	while (!m_JobPool.push_back(closure))
	{
		Poll();
	}
}

void GameJobSystem::Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(JobDispatchArgs)>& job)
{
	if (jobCount == 0 || groupSize == 0)
	{
		return;
	}

	const uint32_t groupCount = (jobCount + groupSize - 1) / groupSize;
	m_CurrentLabel += groupCount;

	for (uint32_t groupIndex = 0; groupIndex < groupCount; ++groupIndex)
	{
		// For each group, generate one real job:
		auto const& jobGroup = [jobCount, groupSize, job, groupIndex]() {
			// Calculate the current group's offset into the jobs:
			const uint32_t groupJobOffset = groupIndex * groupSize;
			const uint32_t groupJobEnd = std::min(groupJobOffset + groupSize, jobCount);

			JobDispatchArgs args;
			args.groupIndex = groupIndex;

			// Inside the group, loop through all job indices and execute job for each index:
			for (uint32_t i = groupJobOffset; i < groupJobEnd; ++i)
			{
				args.jobIndex = i;
				job(args);
			}
		};

		// Try to push a new job until it is pushed successfully:
		while (!m_JobPool.push_back(jobGroup))
		{
			Poll();
		}

		m_WakeCondition.notify_one(); // wake one thread
	}
}

bool GameJobSystem::IsBusy()
{
	return m_FinishedLabels.load() < m_CurrentLabel;
}

void GameJobSystem::Wait()
{
	while (IsBusy())
	{
		Poll();
	}
}
