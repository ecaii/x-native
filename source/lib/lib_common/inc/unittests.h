#pragma once
#include <inttypes.h>
#include <utility>

#if NETWORK_RUN_UNIT_TESTS
#define TESTABLE_OBJECT(Type)\
		static bool UnitTest(); \
		static const char* UnitTestName() { return #Type; } \

#define TESTABLE_DEFINE(t, f) bool t ##::UnitTest() ##f
#define TESTABLE_REGISTER(t) UnitTests::RegisterUnitTest(t::UnitTestName, t::UnitTest)
#define TESTABLE_RUN_ALL() UnitTests::RunUnitTests()
#else
#define TESTABLE_OBJECT(t)
#define TESTABLE_DEFINE(t, f)
#define TESTABLE_REGISTER(t)
#define TESTABLE_RUN_ALL()
#endif

namespace shared
{

#if _DEBUG
class UnitTestable
{
public:
	void SetIsInUnitTest(bool v) { m_bIsInUnitTest = v; }
	bool IsInUnitTest() const { return m_bIsInUnitTest; }

private:
	bool m_bIsInUnitTest = false;
};

class UnitTests
{
public:
#define MAX_UNIT_TESTS 32
	typedef        bool(*UnitTest)(void);
	typedef const char*(*UnitTestName)(void);
	typedef std::pair<UnitTestName, UnitTest> TestPair;

	static void RegisterUnitTest(UnitTestName name, UnitTest test);
	static bool RunUnitTests();

protected:
	static uint32_t ms_NumTests;
	static TestPair ms_Tests[MAX_UNIT_TESTS];
};

#else
class UnitTestable
{
public:
	bool IsInUnitTest() const { return false; }
};
#endif

}