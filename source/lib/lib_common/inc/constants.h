#pragma once

//
// Welcome to versioning hell brother
//
#define NVMP_VERSION_MAJOR 10         // DONT FUCK with this. We are going to be consistently X from now - minor is the dominant
#define NVMP_VERSION_MINOR 0          // Yo, FUCK with this! Increase incrementally when connections should go incompatible

// There was a PATCH in old NVMP legacy, but it was completely fucking irrelevant and got bumped whenever someone felt like it

#ifdef _DEBUG
	#define NVMP_VERSION_BUILD "debug"
#else
	#define NVMP_VERSION_BUILD "release"
#endif