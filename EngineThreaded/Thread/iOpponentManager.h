#ifndef _iOPPONENT_MANAGER_HG_
#define _iOPPONENT_MANAGER_HG_

#include <glm/glm.hpp>
#include <vector> 

class cGameObject;	// Forward declare to omit include
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

	// Gets a single Opponent. Not actually called by anything
	virtual bool getOpponentPositionAtIndex(unsigned int index, glm::vec3 &position) = 0;
	// Sets the position of a single Opponent. 
	// Called by each Opponent
	virtual bool setOpponentPositionAtIndex(unsigned int index, glm::vec3 position) = 0;
	// Called by both Opponents and the game loop
	virtual bool getOpponentPositionsAtIndexRange(std::vector<glm::vec3> &vecOpponentPositions) = 0;

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
	virtual void UpdateTargetPosition( glm::vec3 targetPosition ) = 0;

};

#endif

