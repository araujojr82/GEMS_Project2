#ifndef _iOPPONENT_MANAGER_HG_
#define _iOPPONENT_MANAGER_HG_

#include <glm/glm.hpp>
#include <vector> 

#include "../cGameObject.h"

//class cGameObject;	// Forward declare to omit include
//enum eEnemyBehaviour;	// Forward declare to omit include

class cOpponent;

class iOpponentManager
{
public:
	virtual ~iOpponentManager() {};

	// Sets up the protected data in memory (the "thread safe" area)
	virtual bool Init( unsigned int numberOfOpponents ) = 0;

	// Actually creates the thread and starts the process. 
	// Passes the OpponentManager and the thread safe rand
	virtual bool CreateOpponentThread( cGameObject* pGameObject, cOpponent* &pOpponent ) = 0;

	virtual bool setOpponentPositionAtIndex( unsigned int index, glm::vec3 position ) = 0;

	// Gets a single Opponent. Not actually called by anything
	virtual bool getOpponentAccelAtIndex(unsigned int index, glm::vec3 &accel) = 0;
	// Sets the Accelaration of a single Opponent. 
	// Called by each Opponent
	virtual bool setOpponentAccelAtIndex(unsigned int index, glm::vec3 accel ) = 0;
	// Called by both Opponents and the game loop
	virtual bool getOpponentAccelAtIndexRange(std::vector<glm::vec3> &vecOpponentAccel) = 0;


	// Gets a single Opponent. Not actually called by anything
	virtual bool getOpponentBehaviourAtIndex( unsigned int index, eEnemyBehaviour &Behaviour ) = 0;
	// Sets the position of a single Opponent. 
	// Called by each Opponent
	virtual bool setOpponentBehaviourlAtIndex( unsigned int index, eEnemyBehaviour newBehaviour ) = 0;
	// Called by the game loop
	virtual bool getOpponentBehaviourAtIndexRange( std::vector<eEnemyBehaviour> &vecOpponentBehaviour ) = 0;

	// Helper function to check for locking. 
	// Isn't used here, but can be used decide if we should attempt data access
	virtual bool IsDataLocked(void) = 0;

	// Used in the lock-less or lock-free variant 
	// Called each "frame" update
	virtual void SwitchBuffers(void) = 0;

	// Sets the bIsAlive to false, exiting the thread
	virtual void KillAllOpponents(void) = 0;

	// Sets bIsUpdating on all Opponents
	virtual void SetIsUpdatingOnAllOpponents( bool bIsUpdating ) = 0;

	// Update target Position on all Opponents
	virtual void UpdateTargetParam( glm::vec3 targetPosition, glm::vec3 targetDirection, float targetHealth ) = 0;

};

#endif

