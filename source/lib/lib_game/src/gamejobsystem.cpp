#include "gamejobsystem.h"
#include "logging.h"
#include "assert.h"

GameJobSystem& GameJobSystem::GetInstance()
{
	static GameJobSystem s_Instance;
	return s_Instance;
}

void GameJobSystem::Start(const shared::CompilerHashU8& name)
{
	todo("not implemented, no jobbing");
}

void GameJobSystem::Execute(const JobClosure& closure)
{
	todo("not implemented, no jobbing");
}

bool GameJobSystem::IsBusy()
{
	return false;
}

void GameJobSystem::Wait()
{
	todo("not implemented, no jobbing");
}
