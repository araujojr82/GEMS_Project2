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
	void setOpponentManager( iOpponentManager* pDM );
	void setRandThreaded( cRandThreaded* pRT );

	CRITICAL_SECTION CS_POSITION;
	
	glm::vec3 position;
	float range;
	float wanderAngle;

	glm::vec3 accel;
	glm::vec3 velocity;
	float maxVelocity;
	glm::vec3 target;
	glm::vec3 targetDirection;
	float targetHealth;
	float timeToMove;

	eEnemyBehaviour behaviour;
	
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

	bool isTargetFacingMe();

	void setBehaviour();

	glm::vec3 scaleVector( glm::vec3 currentVector, float maximum );
	glm::vec3 seek();
	glm::vec3 flee();
	glm::vec3 wander();

};

#endif
