#include "Opponent_Threaded.h"

#include "..\cGameObject.h"
#include "cOpponent.h"

cOpponentManager::cOpponentManager()
{
	InitializeCriticalSection( &(this->m_cs_OpponentDataLock) );

	if ( ! ::g_pThreadedRandom )
	{
		::g_pThreadedRandom = new cRandThreaded();
	}

	this->m_nextOpponentID = 0;

	return;
}

bool cOpponentManager::CreateOpponentThread( cGameObject* pGameObject, cOpponent* &pOpponent )
{
	if ( this->m_nextOpponentID >= this->m_vecOpponentPosition.size() )
	{	// Too many Opponents
		return false;
	}

	pOpponent = new cOpponent(this->m_nextOpponentID);
	pOpponent->setDalkeManager( this );
	pOpponent->setRandThreaded( g_pThreadedRandom );	

	pOpponent->minDistanceToMove = 50.0f;
	pOpponent->maxDistanceToMove = 250.0f;
	pOpponent->maxVelocity = 25.0f;

	pOpponent->position = pGameObject->position;
	
	this->m_vec_pOpponents[pOpponent->getOpponentID()] = pOpponent;


	pOpponent->threadInfo.handle = CreateThread( 
	                NULL, // Attributes
		            0,		// 0 = default stack size,
//	                OpponentBrainThread_01, 
	                OpponentBrainThread, 
	                (void*)pOpponent,
	                0,			// 0 or CREATE_SUSPENDED, 
	                &(pOpponent->threadInfo.address) );
//	OpponentBrainThread_01
	this->m_nextOpponentID++;

	return true;
}



bool cOpponentManager::Init(unsigned int numberOfOpponents)
{
	this->m_LockOpponentData();

	this->m_NumberOfOpponents = numberOfOpponents;

	// Load up positions for 500 Opponents... 
	// The values aren't important as they will be overwritten by the Opponents themselves, later
	for ( int count = 0; count != this->m_NumberOfOpponents; count++ )
	{
		// Make an "empty" Opponent at this index...
		this->m_vecOpponentPosition.push_back(glm::vec3(0.0f));
		// ...and make an "empty" thread info at this index, too
		this->m_vecOpponentThreadInfo.push_back( sOpponentThreadInfo() );

		this->m_vec_pOpponents.push_back( NULL );
	}
	this->m_UnlockOpponentData();

	return true;
}


cOpponentManager::~cOpponentManager()
{
	DeleteCriticalSection( &(this->m_cs_OpponentDataLock) );
	return;
}

//inline 
bool cOpponentManager::m_IsIndexInRange(unsigned int index)
{
	if ( index < (unsigned int)this->m_vecOpponentPosition.size() )
	{
		return true;
	}
	return false;
}


// From iOpponentManager
bool cOpponentManager::getOpponentPositionAtIndex(unsigned int index, glm::vec3 &position)
{
	if ( ! this->m_IsIndexInRange(index) )
	{
		// Index is out of range
		return false;
	}

	this->m_LockOpponentData();
	position = this->m_vecOpponentPosition[index];
	this->m_UnlockOpponentData();

	return true;
}

// From iOpponentManager
// Is only called by the Opponents... 
bool cOpponentManager::setOpponentPositionAtIndex(unsigned int index, glm::vec3 position)
{
	if (!this->m_IsIndexInRange(index))
	{
		// Index is out of range
		return false;
	}

	this->m_LockOpponentData();
	this->m_vecOpponentPosition[index] = position;
	this->m_UnlockOpponentData();

	return true;
}

// From iOpponentManger
bool cOpponentManager::getOpponentPositionsAtIndexRange(std::vector<glm::vec3> &vecOpponentPositions)
{
	this->m_LockOpponentData();
	for ( unsigned int index = 0; index != this->m_NumberOfOpponents; index++ )
	{
		vecOpponentPositions[index] = this->m_vecOpponentPosition[index];
	}
	// Or...
	//std::copy( this->m_vecOpponentPosition.begin(), this->m_vecOpponentPosition.end(), vecOpponentPositions );
	this->m_UnlockOpponentData();

	return true;
}

// From iOpponentManger
bool cOpponentManager::cOpponentManager::IsDataLocked(void)
{
	return this->m_bIsLocked;
}

// ******************************************

void cOpponentManager::m_LockOpponentData(void)
{
	EnterCriticalSection( &(this->m_cs_OpponentDataLock) );
	this->m_bIsLocked = true;
	return;
}

void cOpponentManager::m_UnlockOpponentData(void)
{
	this->m_bIsLocked = false;
	LeaveCriticalSection( &(this->m_cs_OpponentDataLock) );
	return;
}


// Sets the bIsAlive to false, exiting the thread
void cOpponentManager::KillAllOpponents(void)
{
	this->m_LockOpponentData();
	for ( unsigned int index = 0; index != this->m_NumberOfOpponents; index++ )
	{
		this->m_vec_pOpponents[index]->bIsAlive = false;
	}
	this->m_UnlockOpponentData();

	return;
}

// Sets bIsUpdating on all Opponents
void cOpponentManager::SetIsUpdatingOnAllOpponents( bool bIsUpdating )
{
	this->m_LockOpponentData();
	for ( unsigned int index = 0; index != this->m_NumberOfOpponents; index++ )
	{
		this->m_vec_pOpponents[index]->bIsUpdating = bIsUpdating;
	}
	this->m_UnlockOpponentData();

	return;
}

// Update target position on all Opponents
void cOpponentManager::UpdateTargetPosition( glm::vec3 targetPosition )
{
	this->m_LockOpponentData();
	for( unsigned int index = 0; index != this->m_NumberOfOpponents; index++ )
	{
		this->m_vec_pOpponents[index]->target = targetPosition;
	}
	this->m_UnlockOpponentData();

	return;
}