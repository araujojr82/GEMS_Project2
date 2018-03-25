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
//   (this is thhe "std::vector<glm::vec3> m_vecOpponentAccel" vector
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


	virtual bool setOpponentPositionAtIndex( unsigned int index, glm::vec3 position );

	// All of these call lock and unlock
	virtual bool getOpponentAccelAtIndex(unsigned int index, glm::vec3 &accel);

	// Called by each Opponent...
	virtual bool setOpponentAccelAtIndex(unsigned int index, glm::vec3 accel);
	// Called by Opponents and the main render loop
	// NOTE: Passed vector MUST be allocated BEFORE call
	virtual bool getOpponentAccelAtIndexRange( std::vector<glm::vec3> &vecOpponentAccel );


	// Gets a single Opponent. Not actually called by anything
	virtual bool getOpponentBehaviourAtIndex( unsigned int index, eEnemyBehaviour &Behaviour );
	// Sets the position of a single Opponent. 
	// Called by each Opponent
	virtual bool setOpponentBehaviourlAtIndex( unsigned int index, eEnemyBehaviour newBehaviour );
	// Called by the game loop
	virtual bool getOpponentBehaviourAtIndexRange( std::vector<eEnemyBehaviour> &vecOpponentBehaviour );



	// Sets the bIsAlive to false, exiting the thread
	virtual void KillAllOpponents(void);

	// Sets bIsUpdating on all Opponents
	virtual void SetIsUpdatingOnAllOpponents( bool bIsUpdating );

	// Update target Position on all Opponents
	virtual void UpdateTargetParam( glm::vec3 targetPosition, glm::vec3 targetDirection, float targetHealth );

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

	//std::vector<glm::vec3> m_vecOpponentPosition;
	std::vector<glm::vec3> m_vecOpponentAccel;
	std::vector<eEnemyBehaviour> m_vecOpponentBehaviour;

	// In order to kill or make active
	std::vector<cOpponent*> m_vec_pOpponents;

	// Saves the thread handle, etc.
	std::vector<sOpponentThreadInfo> m_vecOpponentThreadInfo;

	inline bool m_IsIndexInRange(unsigned int index);

	// When creating a Opponent, this is the next index in the array (vector)
	unsigned int m_nextOpponentID;// = 0;
};

#endif
