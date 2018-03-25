#ifndef _OPPONENT_THREADED_HG_
#define _OPPONENT_THREADED_HG_

#include "iOpponentManager.h"
#include <process.h>
#include <Windows.h>		// For CRITICAL_SECTION, CreateThread, ResumeThread, etc.
#include "Utilities.h"


// Opponent threading solution-example #1
// 
// Locked version, using a critical section lock.
// 
// - The shared data (opponent position) is placed in central location (in manager class)
//   (this is thhe "std::vector<glm::vec3> m_vecOpponentPosition" vector
// - It is protected by a crtical section lock
// - Dalaks write to a single location (index)
// - All Opponent positions are read by both the Opponents and the game loop
//   - This read is also protected by THE SAME lock


#include "OpponentThreadedCommon.h"



class cOpponentManager : public iOpponentManager
{
public:
	cOpponentManager();
	virtual ~cOpponentManager();

	virtual bool Init(unsigned int numberOfOpponents);

	// Spawns the next Opponent, using the game object position
	// returns false if there aren't enough 'slots' for the Opponents
	virtual bool CreateOpponentThread( cGameObject* pGameObject, cOpponent* &pOpponent );

	// All of these call lock and unlock
	virtual bool getOpponentPositionAtIndex(unsigned int index, glm::vec3 &position);

	// Called by each Opponent...
	virtual bool setOpponentPositionAtIndex(unsigned int index, glm::vec3 position);
	// Called by Opponents and the main render loop
	// NOTE: Passed vector MUST be allocated BEFORE call
	virtual bool getOpponentPositionsAtIndexRange( std::vector<glm::vec3> &vecOpponentPositions );

	// Sets the bIsAlive to false, exiting the thread
	virtual void KillAllOpponents(void);

	// Sets bIsUpdating on all Opponents
	virtual void SetIsUpdatingOnAllOpponents( bool bIsUpdating );

	// Update target Position on all Opponents
	virtual void UpdateTargetPosition( glm::vec3 targetPosition );

	virtual bool IsDataLocked(void);

	// Used in the lock-less or lock-free variant 
	// Does nothing in this variant
	virtual void SwitchBuffers(void)
	{
		return;
	}

private:
	void m_LockOpponentData(void);
	void m_UnlockOpponentData(void);

	unsigned int m_NumberOfOpponents;// = 500;

	bool m_bIsLocked;

	CRITICAL_SECTION m_cs_OpponentDataLock;

	std::vector<glm::vec3> m_vecOpponentPosition;
	// In order to kill or make active
	std::vector<cOpponent*> m_vec_pOpponents;

	// Saves the thread handle, etc.
	std::vector<sOpponentThreadInfo> m_vecOpponentThreadInfo;

	inline bool m_IsIndexInRange(unsigned int index);

	// When creating a Opponent, this is the next index in the array (vector)
	unsigned int m_nextOpponentID;// = 0;
};

#endif
