#include "cOpponent.h"
#include "Utilities.h"	// cRandThreaded

#include <iostream>

float CIRCLE_DISTANCE = 4.0f;
float CIRCLE_RADIUS = 2.0f;
float ANGLE_CHANGE = 0.20f;

// Can't call (is private)
cOpponent::cOpponent()
{
	return;
}

cOpponent::cOpponent(unsigned int OpponentID_or_Index)
{	
	this->position = glm::vec3(0.0f);
	this->range = 6.0f;

	this->wanderAngle = 0.0f;
	this->accel = glm::vec3( 0.0f );
	this->velocity = glm::vec3(0.0f);
	this->target = glm::vec3(0.0f);
	this->targetDirection = glm::vec3( 0.0f );
	this->targetHealth = 100.0f;

	// set this to -ve to force it choosing a new direction
	this->timeToMove = -1.0f;

	this->maxVelocity = 1.5f;

	this->bIsAlive = true;

	this->bIsUpdating = false;
	
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

bool cOpponent::isTargetFacingMe()
{
	bool isItFacing;

	float facing = glm::dot( glm::normalize( this->position - this->target ), this->targetDirection );

	if( facing < 0 )	// It's not facing, looking to opposite direction
	{
		isItFacing = false;
	}
	else				// It's in the 180 degrees direction
	{
		if( facing >= 0.5f ) // It's in a 90 degrees cone
			isItFacing = true;
		else
			isItFacing = false;
	}

	return isItFacing;
}

void cOpponent::setBehaviour()
{
	float distanceFromThePlayer = glm::distance( this->position, this->target );
	float playerHealth = this->targetHealth;

	bool bPlayerIsFacingMe = isTargetFacingMe();
	bool bIsPlayerInRange = false;
	if( distanceFromThePlayer <= this->range ) bIsPlayerInRange = true;

	if( playerHealth < 25.0f )
	{
		this->behaviour = eEnemyBehaviour::SEEK;
	}
	else
	{
		if( bIsPlayerInRange )
		{
			if( bPlayerIsFacingMe )	this->behaviour = eEnemyBehaviour::FLEE;
			else					this->behaviour = eEnemyBehaviour::SEEK;
		}
		else
		{
			this->behaviour = eEnemyBehaviour::IDLE;
		}
	}


	if( this->m_pOpponentManager )
	{
		this->m_pOpponentManager->setOpponentBehaviourlAtIndex( this->m_OpponentID_or_Index, this->behaviour );
	}
	return;
}


void cOpponent::Update(void)
{
	//std::vector<glm::vec3> newAccel;

#ifdef DEBUG_PRINT_STATUS_TO_CONSOLE 
	std::cout << "Opponent " << this->m_OpponentID_or_Index << " Update() called" << std::endl;
	std::cout.flush();
#endif

	float totalTimeSinceReset = this->m_pTimer->get_fLondDurationTotalSeconds();

	if ( totalTimeSinceReset >= this->timeToMove )
	{

		if( this->m_pRand )
		{

			setBehaviour();		

			//pTheGO->accel += wander( pTheGO );

			switch( this->behaviour )
			{
				case eEnemyBehaviour::SEEK:
					this->accel += seek();
					break;

				case eEnemyBehaviour::FLEE:
					this->accel += flee();
					break;

				case eEnemyBehaviour::IDLE:
					this->accel += wander();
					break;
			}




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
			//		this->m_pOpponentManager->setOpponentAccelAtIndex( this->m_OpponentID_or_Index, this->position );
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
			//			this->m_pOpponentManager->setOpponentAccelAtIndex( this->m_OpponentID_or_Index, this->position );
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

void cOpponent::setOpponentManager(iOpponentManager* pDM)
{
	this->m_pOpponentManager = pDM;
	return;
}

void cOpponent::setRandThreaded(cRandThreaded* pRT)
{
	this->m_pRand = pRT;
	return;
}

glm::vec3 cOpponent::scaleVector( glm::vec3 currentVector, float maximum )
{
	glm::vec3 newVector = glm::normalize( currentVector ) * maximum;
	return newVector;
}

glm::vec3 cOpponent::seek()
{
	float slowingRadius = 2.0f;
	glm::vec3 force;

	glm::vec3 targetVec = this->target - this->position;
	float distance = glm::length( targetVec );
	targetVec = glm::normalize( targetVec );

	if( distance <= slowingRadius )
	{
		float limit = this->maxVelocity * distance / slowingRadius;
		force = scaleVector( targetVec, limit );
	}
	else
	{
		force = scaleVector( targetVec, this->maxVelocity );
	}

	force = force - this->velocity;

	return force;
}

glm::vec3 cOpponent::flee()
{
	glm::vec3 force;

	glm::vec3 targetVec = this->position - this->target;
	targetVec = glm::normalize( targetVec );

	force = scaleVector( targetVec, this->maxVelocity );

	force = force - this->velocity;

	return force;
}

glm::vec3 cOpponent::wander( )
{
	glm::vec3 wanderForce;
	glm::vec3 circleCenter;
	glm::vec3 displacement;

	if( this->velocity == glm::vec3( 0.0f ) )
	{
		this->velocity = glm::vec3( getRandInRange<float>( -0.1f, 0.1f, ( float )this->m_pRand->getNextRandDouble() ),
								 0.0f,
								 getRandInRange<float>( -0.1f, 0.1f, ( float )this->m_pRand->getNextRandDouble() ) );		
	}

	circleCenter = this->velocity;
	glm::normalize( circleCenter );

	circleCenter = scaleVector( circleCenter, CIRCLE_DISTANCE );

	displacement = glm::vec3( 0, 0, -1 );
	displacement = scaleVector( displacement, CIRCLE_RADIUS );

	float wanderAngle = this->wanderAngle;
	
	float lenght = glm::length( displacement );
	displacement.x = glm::cos( wanderAngle ) * lenght;
	displacement.z = glm::sin( wanderAngle ) * lenght;

	float random = getRandInRange<float>( 0.0f, 0.99f, ( float )this->m_pRand->getNextRandDouble() );

	float newAngle = random * ANGLE_CHANGE - ANGLE_CHANGE * 0.5f;
	this->wanderAngle = wanderAngle + newAngle;

	wanderForce = circleCenter + displacement;

	wanderForce = scaleVector( wanderForce, this->maxVelocity );

	return wanderForce;
}