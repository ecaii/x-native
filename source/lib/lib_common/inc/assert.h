#pragma once
#include "logging.h"

#define _str(v) #v

#if _DEBUG
	// In debug we need to throw a debug break so that we can pull the program execution up in a debugger to find out what went wrong
	#ifdef _WIN32
		#define _crash __debugbreak
	#else
		#error  _crash undefined
	#endif
#else
	// In release we need to completely kill the program. If there is a check that requires a _crash to happen, we will not strip it out of 
	// release as it means we should be terminating progress due to dragons ahead
	#ifdef _WIN32
		#define _crash abort
	#else
		#error _crash undefined
	#endif
#endif

#if _DEBUG
// assert          for systematic checks
	#define assert(v, s)       if (!(v)) { DbgLogRaw("\n"); DbgError(_str(v) " assert failed! %s,\n                      in %s\n", s, __FUNCTION__); }
// assert_crash    for critical checks that may not throw a natural crash (crash carried to release, message disposed)
	#define assert_crash(v, s) if (!(v)) { do { assert(0, s); _crash(); } while (true); }
// assert_ctx      for additional data for assert only code, so it removes out in release
	#define assert_ctx(v) v
// todo            for features or code paths that need to be developed before release, will static assert and refuse to build until removed
	#define todo(s) 
// todo_warn       for features or code paths that need to be developed before release, will continue to build in release
	#define todo_warn(s)
#else
	#define assert(v, s)
	#define assert_ctx(v)
	#define assert_crash(v, s) if (!(v)) { do { _crash(); } while (true); }
	#define todo(s)  static_assert(0, s)
	#define todo_warn(s)
#endif