#include "gamejobsystem.h"
#include "logging.h"

shared::GameJobSystem& shared::GameJobSystem::GetInstance()
{
	static shared::GameJobSystem s_Instance;
	return s_Instance;
}

void shared::GameJobSystem::Start(const CompilerHashU8& name)
{
	DbgWarn("not implemented!");
}

void shared::GameJobSystem::Execute(const JobClosure& closure)
{
	DbgWarn("not implemented!");
}

bool shared::GameJobSystem::IsBusy()
{
	return false;
}

void shared::GameJobSystem::Wait()
{
	DbgWarn("not implemented!");
}
