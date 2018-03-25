#include "cSteeringManager.h"
#include "cDebugRenderer.h"

#include <iostream>

#include "Thread\Opponent_Threaded.h"
//#include "Thread\cOpponent.h"
extern iOpponentManager* g_pOpponentManager;

extern float generateRandomNumber( float min, float max );

extern cDebugRenderer* g_pDebugRenderer;

static const float MAX_FORCE = 40.0f; // 5.4f; //10.0f;

float calculateAngle( glm::vec3 vel )
{
	// Calculate the moving angle using the velocity
	float movAngle = glm::atan( vel.x, vel.z );

	// Convert it from radians to degrees
	movAngle = glm::degrees( movAngle );

	return movAngle;
}

void setAngle( glm::vec3 &vector, float value )
{
	float lenght = glm::length( vector );
	vector.x = glm::cos( value ) * lenght;
	vector.z = glm::sin( value ) * lenght;
}

cSteeringManager::cSteeringManager()
{
	currentTimeStep = 0.0;
}


cSteeringManager::~cSteeringManager()
{
}

void cSteeringManager::updateAll( double deltaTime )
{
	this->currentTimeStep = deltaTime;

	cGameObject* pTargetGO = findTarget( nullptr );

	::g_pOpponentManager->getOpponentPositionsAtIndexRange( vecDalekPositions );

	for( int i = 0; i != ::g_vecOpponentsGO.size(); i++ )
	{
		cGameObject* pTheGO = ::g_vecOpponentsGO[i];

		//setBehaviour( pTheGO, pTargetGO );
		//solveSteering( pTheGO, pTargetGO );



		update( pTheGO, deltaTime );
	}
}

void cSteeringManager::reset()
{
}

glm::vec3 limitVector( glm::vec3 currentVector, float maximum )
{
	float velTotal = abs(currentVector.x) + abs( currentVector.y ) + abs( currentVector.z );
	if( velTotal > maximum )
	{
		glm::vec3 newVector = glm::vec3( ( currentVector.x / velTotal ) * maximum,
										 ( currentVector.y / velTotal ) * maximum,
										 ( currentVector.z / velTotal ) * maximum );

		return newVector;
	}
	else
		return currentVector;
}

glm::vec3 scaleVector( glm::vec3 currentVector, float maximum )
{
	glm::vec3 newVector = glm::normalize( currentVector ) * maximum;
	return newVector;
}


void cSteeringManager::update( cGameObject* pTheGO, double deltaTime )
{
	glm::vec3 velocity = pTheGO->vel;
	glm::vec3 position = pTheGO->position;
	glm::vec3 accel = pTheGO->accel;

	accel = limitVector( accel, MAX_FORCE );

	if( !isnan( accel.x ) && !isnan( accel.y ) && !isnan( accel.z ) )
	{
		velocity += accel;		
		velocity = limitVector( velocity, pTheGO->maxVel );
	}

	if( !isnan( velocity.x ) && !isnan( velocity.y ) && !isnan( velocity.z ) )
	{
		position += ( velocity * (float) deltaTime );
	}

	pTheGO->vel = velocity;
	pTheGO->position = position;
	pTheGO->accel = glm::vec3( 0.0f );
	
	if( !isnan( velocity.x ) && !isnan( velocity.y ) && !isnan( velocity.z ) && velocity != glm::vec3( 0.0f ) )
	{
		glm::vec3 tempVel = velocity * 1.0f; // scaleVector( ( velocity + position ), 1.0f );
		tempVel += position;
		::g_pDebugRenderer->addLine( position, tempVel, glm::vec3( 1.0f, 0.0f, 0.0f ), false );
	}

	if( !isnan( velocity.x ) && !isnan( velocity.y ) && !isnan( velocity.z ) )
	{
		float rotation = calculateAngle( velocity );
		rotation = glm::radians( rotation );
		glm::vec3 rotateAngle = glm::vec3( 0.0f, rotation, 0.0f );
		pTheGO->overwrtiteQOrientationFormEuler( rotateAngle );
	}

	return;
}

cGameObject * cSteeringManager::findTarget( cGameObject* pTheGO )
{
	for( int i = 0; i != ::g_vecGameObjects.size(); i++ )
	{
		cGameObject* pTargetGO = ::g_vecGameObjects[i];
		if( pTargetGO->type != CHARACTER ) continue;
		if( pTargetGO->team == PLAYER )
		{
			return pTargetGO;
		}		
	}
	return nullptr;
}

void cSteeringManager::setBehaviour( cGameObject* pTheGO, cGameObject* pTargetGO )
{
	float distanceFromThePlayer = glm::distance( pTheGO->position, pTargetGO->position );
	float playerHealth = pTargetGO->health;

	bool bPlayerIsFacingMe = pTheGO->isFacingMe( pTargetGO->getDirectionVector(), pTargetGO->position );	
	bool bIsPlayerInRange = false;
	if( distanceFromThePlayer <= pTheGO->range ) bIsPlayerInRange = true;

	switch( pTheGO->enemyType )
	{
		case ANGRY :
			if( playerHealth < 25.0f )
			{
				pTheGO->behaviour = eEnemyBehaviour::SEEK;
			}
			else
			{
				if( bIsPlayerInRange )
				{
					if( bPlayerIsFacingMe )	pTheGO->behaviour = eEnemyBehaviour::FLEE;
					else					pTheGO->behaviour = eEnemyBehaviour::SEEK;
				}
				else
				{
					pTheGO->behaviour = eEnemyBehaviour::IDLE;
				}
			}
			break;

		case CURIOUS :
			if( bIsPlayerInRange )
			{
				//if( bPlayerIsFacingMe )	pTheGO->behaviour = eEnemyBehaviour::EVADE;
				//else					pTheGO->behaviour = eEnemyBehaviour::APPROACH;

				if( bPlayerIsFacingMe )	pTheGO->behaviour = eEnemyBehaviour::EVADE;
				else					pTheGO->behaviour = eEnemyBehaviour::APPROACH;

			}
			else
			{
				pTheGO->behaviour = eEnemyBehaviour::IDLE;
			}
			break;

		case FOLLOWER :
			if( bIsPlayerInRange )
			{
				pTheGO->behaviour = eEnemyBehaviour::SEEK;
			}
			else
			{
				pTheGO->behaviour = eEnemyBehaviour::IDLE;
			}
			break;

		default:
			pTheGO->behaviour = eEnemyBehaviour::IDLE;
			break;
	}
	
	return;
}

void cSteeringManager::solveSteering( cGameObject* pTheGO, cGameObject* pTargetGO )
{
	float slowingRadius = 2.0f;

	//pTheGO->accel += wander( pTheGO );

	switch( pTheGO->behaviour )
	{
		case WANDER :
			pTheGO->accel += wander( pTheGO );
			break;

		case SEEK:
			pTheGO->accel += seek( pTheGO, pTargetGO->position, slowingRadius );
			break;

		case FLEE:
			pTheGO->accel += flee( pTheGO, pTargetGO->position );
			break;

		case PURSUE :
			pTheGO->accel += pursuit( pTheGO, pTargetGO, slowingRadius );
			break;

		case ARRIVE :

			break;

		case APPROACH :
			pTheGO->accel += approach( pTheGO, pTargetGO->position, slowingRadius );
			break;

		case EVADE :
			pTheGO->accel += evade( pTheGO, pTargetGO );
			break;

		case IDLE:
			pTheGO->accel += wander( pTheGO );
			//pTheGO->accel -= pTheGO->vel;
			break;

		default:
			break;
	}
}

glm::vec3 cSteeringManager::seek( cGameObject* pTheGO, glm::vec3 targetDestination, float slowingRadius )
{
	glm::vec3 force;

	glm::vec3 targetVec = targetDestination - pTheGO->position;
	float distance = glm::length( targetVec );
	targetVec = glm::normalize( targetVec );

	::g_pDebugRenderer->addCircle( targetDestination, slowingRadius, glm::vec3( 1.0f, 1.0f, 1.0f ) );

	if( distance <= slowingRadius )
	{
		float limit = pTheGO->maxVel * distance / slowingRadius;
		force  = scaleVector( targetVec, limit );
	}
	else
	{
		force = scaleVector( targetVec, pTheGO->maxVel );
	}

	force = force - pTheGO->vel;

	return force;
}

glm::vec3 cSteeringManager::flee( cGameObject* pTheGO, glm::vec3 targetDestination )
{
	glm::vec3 force;

	glm::vec3 targetVec = pTheGO->position - targetDestination;
	targetVec = glm::normalize( targetVec );

	force = scaleVector( targetVec, pTheGO->maxVel );

	force = force - pTheGO->vel;

	return force;
}

glm::vec3 cSteeringManager::wander( cGameObject* pTheGO )
{
	glm::vec3 wanderForce;
	glm::vec3 circleCenter;
	glm::vec3 displacement;

	if( pTheGO->vel == glm::vec3( 0.0f ) )
	{
		pTheGO->vel = glm::vec3( generateRandomNumber( -0.1, 0.1 ),
								 0.0f,
								 generateRandomNumber( -0.1, 0.1 ) );
	}

	circleCenter = pTheGO->vel;
	glm::normalize( circleCenter );

	circleCenter = scaleVector( circleCenter, CIRCLE_DISTANCE );

	displacement = glm::vec3( 0, 0, -1 );
	displacement = scaleVector( displacement, CIRCLE_RADIUS );

	float wanderAngle = pTheGO->wanderAngle;
	setAngle( displacement, wanderAngle );

	float newAngle = generateRandomNumber( 0, 0.99f ) * ANGLE_CHANGE - ANGLE_CHANGE * 0.5f;
	pTheGO->wanderAngle = wanderAngle + newAngle;

	wanderForce = circleCenter + displacement;

	wanderForce = scaleVector( wanderForce, pTheGO->maxVel );

	return wanderForce;
}

glm::vec3 cSteeringManager::evade( cGameObject* pTheGO, cGameObject* pTargetGO )
{
	glm::vec3 targetVec = pTargetGO->position - pTheGO->position;
	float distance = glm::length( targetVec );	

	float updatesNeeded = ( distance / ( pTheGO->maxVel * 10 ) ) * ( float )this->currentTimeStep;

	glm::vec3 targetVel = pTargetGO->vel; // ( float )this->currentTimeStep;
	
	if( targetVel != glm::vec3( 0.0f ) )
	{
		targetVel = scaleVector( targetVel, updatesNeeded );	
		glm::vec3 targetFuturePosition = pTargetGO->position + targetVel;
		return flee( pTheGO, targetFuturePosition );
	}
	else
	{
		return flee( pTheGO, pTargetGO->position );
	}
}

glm::vec3 cSteeringManager::pursuit( cGameObject* pTheGO, cGameObject* pTargetGO, float slowingRadius )
{	
	glm::vec3 targetVec = pTargetGO->position - pTheGO->position;
	float distance = glm::length( targetVec );

	float updatesNeeded = distance / pTheGO->maxVel;

	glm::vec3 targetVel = glm::vec3( 0.0f );
	if( pTargetGO->vel != glm::vec3( 0.0f ) )
		targetVel = scaleVector( pTargetGO->vel, updatesNeeded );

	glm::vec3 targetFuturePosition = pTargetGO->position + targetVel;

	return seek( pTheGO, targetFuturePosition, slowingRadius );
}

glm::vec3 cSteeringManager::approach( cGameObject* pTheGO, glm::vec3 targetDestination, float slowingRadius )
{
	glm::vec3 force;

	glm::vec3 targetVec = targetDestination - pTheGO->position;
	float distance = glm::length( targetVec );
	targetVec = glm::normalize( targetVec );

	::g_pDebugRenderer->addCircle( targetDestination, slowingRadius, glm::vec3( 1.0f, 1.0f, 1.0f ) );

	if( distance <= slowingRadius )
	{
		float limit = ( pTheGO->maxVel * distance / slowingRadius ) / 10.0f ;
		force = scaleVector( targetVec, limit );
	}
	else
	{
		force = scaleVector( targetVec, pTheGO->maxVel );
	}

	force = force - pTheGO->vel;

	return force;
}