#ifndef _OPPONENT_THREADED_COMMON_HG_
#define _OPPONENT_THREADED_COMMON_HG_

#include "Utilities.h"
#include <glm/glm.hpp>

struct sOpponentThreadInfo
{
	sOpponentThreadInfo() :
		handle(0), address(0) {};
	HANDLE handle;
	DWORD address;
};


class cGameObject;		// forward declare

// This is the actual threading function
//unsigned int __stdcall OpponentBrainThread(void* pInitialData);	// _beginthreadex() format
DWORD WINAPI OpponentBrainThread(void* pInitialData);				// CreateThread() format


// Makes a game object, sets the location, and returns pointer
cGameObject* MakeOpponentGameObject(glm::vec3 position);

class cOpponent;		// Forward declare

extern cRandThreaded* g_pThreadedRandom;

// Called when critical section is entered
// Uses the win32 InterlockedXXX calls (which, on x86-64 use lock EXCH commands
// NOTE: Should be aligned on 32 bit boundtry (see _aligned_malloc)
extern int unsigned* g_pCS_per_fram_count;


#endif
