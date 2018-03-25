#include "cOpponent.h"
#include "Utilities.h"	// cRandThreaded

#include <iostream>

//#define DEBUG_PRINT_STATUS_TO_CONSOLE

// Can't call (is private)
cOpponent::cOpponent()
{
	return;
}

// These are all static
const float cOpponent::DEFAULT_MIN_DISTANCE_TO_MOVE = 1.0;
const float cOpponent::DEFAULT_MAX_DISTANCE_TO_MOVE = 25.0;


cOpponent::cOpponent(unsigned int OpponentID_or_Index)
{
#ifdef DEBUG_PRINT_STATUS_TO_CONSOLE 
	std::cout << "Opponent " << OpponentID_or_Index << " is created" << std::endl;
	std::cout.flush();
#endif

	this->position = glm::vec3(0.0f);
	this->velocity = glm::vec3(0.0f);
	this->target = glm::vec3(0.0f);

	// set this to -ve to force it choosing a new direction
	this->timeToMove = -1.0f;

	this->maxVelocity = 1.0f;

	this->bIsAlive = true;

	this->bIsUpdating = false;

	float minDistanceToMove = cOpponent::DEFAULT_MIN_DISTANCE_TO_MOVE;
	float maxDistanceToMove = cOpponent::DEFAULT_MAX_DISTANCE_TO_MOVE;


	this->m_OpponentID_or_Index = OpponentID_or_Index;

	// Each Opponent has it's own timer, since each thread could execute at different rates
	this->m_pTimer = new CHRTimer();

	this->m_pTimer->ResetLongDuration();

	this->m_pOpponentManager = NULL;
	this->m_pRand = NULL;

	// Thread information (in case we need it)
	HANDLE threadHandle = 0;
	DWORD threadAddress = 0;


	return;
}

unsigned int cOpponent::getOpponentID(void)
{
	return this->m_OpponentID_or_Index;
}


void cOpponent::Update(void)
{
	std::vector<glm::vec3> newPosition;

#ifdef DEBUG_PRINT_STATUS_TO_CONSOLE 
	std::cout << "Opponent " << this->m_OpponentID_or_Index << " Update() called" << std::endl;
	std::cout.flush();
#endif

	float totalTimeSinceReset = this->m_pTimer->get_fLondDurationTotalSeconds();

	if ( totalTimeSinceReset >= this->timeToMove )
	{	
		
		if( this->m_pRand )
		{
			//if( !this->bIsScanning )
			//{
			//	int hDist = ( int )this->target.x - this->position.x;
			//	int vDist = ( int )this->target.z - this->position.z;

			//	int posX = ( int )this->position.x;
			//	int posZ = ( int )this->position.z;
			//	int newX = posX;
			//	int newZ = posZ;

			//	if( hDist > 0 ) newX = posX + 1;
			//	else if( hDist < 0 ) newX = posX - 1;
			//	if( vDist > 0 ) newZ = posZ + 1;
			//	else if( vDist < 0 ) newZ = posZ - 1;

			//	if( newX != posX ) // Can move Horizontally
			//	{
			//		// Test if it's a valid position, false means it's a space
			//		if( !this->m_pOpponentManager->CheckWorldPosition( newX, posZ ) )
			//		{
			//			newPosition.push_back( glm::vec3( ( float )newX, this->position.y, this->position.z ) );
			//		}
			//	}
			//	if( newZ != posZ ) // Can move Vertically
			//	{
			//		// Test if it's a valid position, false means it's a space
			//		if( !this->m_pOpponentManager->CheckWorldPosition( posX, newZ ) )
			//		{
			//			newPosition.push_back( glm::vec3( this->position.x, this->position.y, ( float )newZ ) );
			//		}
			//	}

			//	if( newPosition.size() == 1 )
			//	{
			//		this->position = newPosition[0];
			//	}
			//	else if( newPosition.size() > 1 )
			//	{
			//		if( hDist > vDist ) this->position = newPosition[0];
			//		else this->position = newPosition[1];
			//	}

			//	if( this->m_pOpponentManager )
			//	{
			//		this->m_pOpponentManager->setOpponentPositionAtIndex( this->m_OpponentID_or_Index, this->position );
			//	}

			//	Sleep( 100 );
			//}
			//else
			//{	// This is a scanning protocol
			//	int hDist = ( int )this->target.x - this->position.x;
			//	int vDist = ( int )this->target.z - this->position.z;

			//	int maxX = this->m_pOpponentManager->GetWorldMaxX() -1;
			//	int maxZ = this->m_pOpponentManager->GetWorldMaxZ() -1;

			//	if( ( abs( hDist ) == 0 && abs( vDist ) == 1 ) ||
			//		( abs( hDist ) == 1 && abs( vDist ) == 0 ) )
			//	{
			//		// The Opponent Scan is too close
			//	}
			//	else
			//	{
			//		int posX = ( int )this->position.x;
			//		int posZ = ( int )this->position.z;

			//		int newX;
			//		int newZ;
			//		
			//		if( posZ > 0 )
			//		{
			//			newX = posX;
			//			newZ = posZ - 1;
			//			
			//			// Test if it's a valid position, false means it's a space
			//			if( !this->m_pOpponentManager->CheckWorldPosition( newX, newZ ) )						
			//				newPosition.push_back( glm::vec3( ( float )newX, this->position.y, ( float )newZ ) );
			//		}
			//		
			//		if( posX > 0 )
			//		{
			//			newX = posX - 1;
			//			newZ = posZ;

			//			// Test if it's a valid position, false means it's a space
			//			if( !this->m_pOpponentManager->CheckWorldPosition( newX, newZ ) )
			//				newPosition.push_back( glm::vec3( ( float )newX, this->position.y, ( float )newZ ) );
			//		}

			//		if( posX < maxX )
			//		{
			//			newX = posX + 1;
			//			newZ = posZ;

			//			// Test if it's a valid position, false means it's a space
			//			if( !this->m_pOpponentManager->CheckWorldPosition( newX, newZ ) )
			//				newPosition.push_back( glm::vec3( ( float )newX, this->position.y, ( float )newZ ) );
			//		}

			//		if( posZ < maxZ )
			//		{
			//			newX = posX;
			//			newZ = posZ + 1;

			//			// Test if it's a valid position, false means it's a space
			//			if( !this->m_pOpponentManager->CheckWorldPosition( newX, newZ ) )
			//				newPosition.push_back( glm::vec3( ( float )newX, this->position.y, ( float )newZ ) );
			//		}

			//		if( newPosition.size() > 0 )
			//		{
			//			int origSize = newPosition.size();

			//			for( int times = 0; times < 5; times++ )
			//			{
			//				for( int i = 0; i != origSize; i++ )
			//				{
			//					newPosition.push_back( newPosition[i] );
			//				}
			//			}						

			//			int index = ( int )getRandInRange<float>( 0, newPosition.size()-1, ( float )this->m_pRand->getNextRandDouble() );
			//			this->position = newPosition[index];
			//		}

			//		if( this->m_pOpponentManager )
			//		{
			//			this->m_pOpponentManager->setOpponentPositionAtIndex( this->m_OpponentID_or_Index, this->position );
			//		}					
			//	}
			//	Sleep( 250 );
			//}
		}//if ( this->m_pRand )

		this->m_pTimer->ResetLongDuration();
		this->m_pTimer->ResetAndStart();
	}

	return;
}

void cOpponent::setDalkeManager(iOpponentManager* pDM)
{
	this->m_pOpponentManager = pDM;
	return;
}

void cOpponent::setRandThreaded(cRandThreaded* pRT)
{
	this->m_pRand = pRT;
	return;
}
