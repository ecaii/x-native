#include "unittests.h"
#include "assert.h"
#include "logging.h"

#if _DEBUG
shared::UnitTests::TestPair shared::UnitTests::ms_Tests[MAX_UNIT_TESTS] = {};
uint32_t                    shared::UnitTests::ms_NumTests = 0;

void shared::UnitTests::RegisterUnitTest(UnitTestName name, UnitTest test)
{
	if (ms_NumTests > MAX_UNIT_TESTS)
	{
		assert(0, "Too many unit tests!");
		return;
	}

	ms_Tests[ms_NumTests++] = { name, test };
}

bool shared::UnitTests::RunUnitTests()
{
	for (uint32_t i = 0; i < ms_NumTests; ++i)
	{
		const char* psName = ms_Tests[i].first();

		if (ms_Tests[i].second()) {
			DbgLog("Unit test '%s' passed", psName);
		}
		else {
			DbgError("Unit test '%s' failed", psName);
			return false;
		}
	}

	return true;
}
#endif