#ifndef _OPPONENT_HG_
#define _OPPONENT_HG_

#include "CHRTimer.h"
#include <glm/glm.hpp>

#include "iOpponentManager.h"

class cRandThreaded;		// In Utilities.h

// The Opponents look after themselves in terms of calculation,
// moving, etc. 
// BUT, they update the updated position to the Opponent Manager
// Also, they get all the positions of every other Opponent
// from the Opponent Manager

#include "OpponentThreadedCommon.h"

class cOpponent
{
public:
	cOpponent(unsigned int m_OpponentID_or_Index);

	// This object is used to access all the other Opponents
	//	and to update +this+ Opponent's location
	void setDalkeManager( iOpponentManager* pDM );
	void setRandThreaded( cRandThreaded* pRT );

	CRITICAL_SECTION CS_POSITION;
	
	glm::vec3 position;

	glm::vec3 velocity;
	float maxVelocity;
	glm::vec3 target;
	glm::vec3 targetDirection;
	float timeToMove;

	float minDistanceToMove;
	float maxDistanceToMove;
	static const float DEFAULT_MIN_DISTANCE_TO_MOVE;// = 1.0;
	static const float DEFAULT_MAX_DISTANCE_TO_MOVE;// = 25.0;

	// If false, thread exits
	bool bIsAlive;

	// If true, thread is alive, but not updating
	// Starts as false
	bool bIsUpdating;

	void Update(void);

	// Thread information  (in case we need it)
	sOpponentThreadInfo threadInfo;

	unsigned int getOpponentID(void);

private:
	cOpponent();		// Can't call - is private

	CHRTimer* m_pTimer;

	iOpponentManager* m_pOpponentManager;
	cRandThreaded* m_pRand;

	// This is the unique or, which is used as the index into the 
	//	vector in the OpponentManager. 
	unsigned int m_OpponentID_or_Index;

};

#endif
