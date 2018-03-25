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
	if ( this->m_nextOpponentID >= this->m_vecOpponentAccel.size() )
	{	// Too many Opponents
		return false;
	}

	pOpponent = new cOpponent(this->m_nextOpponentID);
	pOpponent->setOpponentManager( this );
	pOpponent->setRandThreaded( g_pThreadedRandom );	

	//pOpponent->maxVelocity = 1.5f;

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
		this->m_vecOpponentAccel.push_back(glm::vec3(0.0f));
		this->m_vecOpponentBehaviour.push_back( eEnemyBehaviour::IDLE );
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
	if ( index < (unsigned int)this->m_vecOpponentAccel.size() )
	{
		return true;
	}
	return false;
}

bool cOpponentManager::setOpponentPositionAtIndex( unsigned int index, glm::vec3 position )
{
	if( !this->m_IsIndexInRange( index ) )
	{
		// Index is out of range
		return false;
	}

	this->m_LockOpponentData();
	this->m_vec_pOpponents[index]->position = position;
	this->m_UnlockOpponentData();

	return true;
}

// From iOpponentManager
bool cOpponentManager::getOpponentAccelAtIndex( unsigned int index, glm::vec3 &accel )
{
	if ( ! this->m_IsIndexInRange(index) )
	{
		// Index is out of range
		return false;
	}

	this->m_LockOpponentData();
	accel = this->m_vecOpponentAccel[index];
	this->m_UnlockOpponentData();

	return true;
}

// From iOpponentManager
// Is only called by the Opponents... 
bool cOpponentManager::setOpponentAccelAtIndex( unsigned int index, glm::vec3 accel )
{
	if (!this->m_IsIndexInRange(index))
	{
		// Index is out of range
		return false;
	}

	this->m_LockOpponentData();
	this->m_vecOpponentAccel[index] = accel;
	this->m_UnlockOpponentData();

	return true;
}

// From iOpponentManger
bool cOpponentManager::getOpponentAccelAtIndexRange(std::vector<glm::vec3> &vecOpponentAccel)
{
	this->m_LockOpponentData();
	for ( unsigned int index = 0; index != this->m_NumberOfOpponents; index++ )
	{
		vecOpponentAccel[index] = this->m_vecOpponentAccel[index];
	}
	// Or...
	//std::copy( this->m_vecOpponentAccel.begin(), this->m_vecOpponentAccel.end(), vecOpponentAccel );
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
void cOpponentManager::UpdateTargetParam( glm::vec3 targetPosition, glm::vec3 targetDirection, float targetHealth )
{
	this->m_LockOpponentData();
	for( unsigned int index = 0; index != this->m_NumberOfOpponents; index++ )
	{
		this->m_vec_pOpponents[index]->target = targetPosition;
		this->m_vec_pOpponents[index]->targetDirection = targetDirection;
		this->m_vec_pOpponents[index]->targetHealth = targetHealth;
	}
	this->m_UnlockOpponentData();

	return;
}


// From iOpponentManager
bool cOpponentManager::getOpponentBehaviourAtIndex( unsigned int index, eEnemyBehaviour &Behaviour )
{
	if( !this->m_IsIndexInRange( index ) )
	{
		// Index is out of range
		return false;
	}

	this->m_LockOpponentData();
	Behaviour = this->m_vecOpponentBehaviour[index];
	this->m_UnlockOpponentData();

	return true;
}

// From iOpponentManager
// Is only called by the Opponents... 
bool cOpponentManager::setOpponentBehaviourlAtIndex( unsigned int index, eEnemyBehaviour newBehaviour )
{
	if( !this->m_IsIndexInRange( index ) )
	{
		// Index is out of range
		return false;
	}

	this->m_LockOpponentData();
	this->m_vecOpponentBehaviour[index] = newBehaviour;
	this->m_UnlockOpponentData();

	return true;
}

// From iOpponentManger
bool cOpponentManager::getOpponentBehaviourAtIndexRange( std::vector<eEnemyBehaviour> &vecOpponentBehaviour )
{
	this->m_LockOpponentData();
	for( unsigned int index = 0; index != this->m_NumberOfOpponents; index++ )
	{
		vecOpponentBehaviour[index] = this->m_vecOpponentBehaviour[index];
	}
	this->m_UnlockOpponentData();

	return true;
}