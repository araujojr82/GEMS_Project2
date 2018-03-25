#include "cComMoveTo.h"

#include <iostream>
#include <GLFW/glfw3.h>

void cComMoveTo::setMyID( int myID )
{
	this->m_uniqueID = myID;
	return;
}

cComMoveTo::cComMoveTo()
{
	this->initPosition = glm::vec3( 0.0f, 0.0f, 0.0f );
	this->finalPosition = glm::vec3( 0.0f, 0.0f, 0.0f );
	this->direction = glm::vec3( 0.0f, 0.0f, 0.0f );
	this->distanceToTarget = 0.0f;
	this->velocity = 0.0f;
	this->initialTime = 0;
	this->elapsedTime = 0;

	this->smoothRange1 = 0.1f;
	this->smoothRange2 = 0.99f;

	this->hasStarted = false;
	this->isCommandDone = false;

	return;
}

cComMoveTo::~cComMoveTo()
{
	return;
}

void cComMoveTo::init( glm::vec3 destination, float time, glm::vec3 theSmoothStepParam )
{
	this->finalPosition = destination;
	this->duration = time;

	// This way we make sure they are not 0, and mantain their init values
	if( theSmoothStepParam.x != 0 ) this->smoothRange1 = theSmoothStepParam.x;
	if( theSmoothStepParam.y != 0 ) this->smoothRange2 = theSmoothStepParam.y;
	//theSmoothStepParam.z? //Not used right now

	return;
}

void cComMoveTo::update( double deltaTime )
{

	if( this->hasStarted == false )
	{

		//std::cout << "ID: " << this->theGO->getID()
		//	<< " - MoveTo: " << this->finalPosition.x << ", "
		//	<< this->finalPosition.y << ", "
		//	<< this->finalPosition.z
		//	<< std::endl;

		if( this->targetGO != nullptr ) // Get the position from the target GO
			this->finalPosition = this->targetGO->position;

		this->initialTime = glfwGetTime();
		this->hasStarted = true;
		this->initPosition = this->theGO->position;
		this->direction = glm::normalize( finalPosition - initPosition );
		this->distanceToTarget = glm::distance( finalPosition, initPosition );
		// This is the average velocity it would take to reach the destination
		this->velocity = (float) this->distanceToTarget / this->duration;

	}
	
	// Calculate remaining distance
	float remainingDistance = glm::distance( this->finalPosition, this->theGO->position );

	this->elapsedTime = glfwGetTime() - this->initialTime;

	//float range1 = this->smoothRange1 * this->distanceToTarget;
	//float range2 = this->smoothRange2 * this->distanceToTarget;
	float range1 = this->smoothRange1 * this->duration;
	float range2 = this->smoothRange2 * this->duration;

	float traveledDistance = glm::distance( this->initPosition, this->theGO->position );

	// Using the smooth step 2x to calculate aceleration and deceleration
	//float factor = glm::smoothstep( -0.1f, range1, traveledDistance ) * ( 1 - glm::smoothstep( range2, this->distanceToTarget + 0.1f , traveledDistance ) );
	float factor = glm::smoothstep( -0.1f, range1, ( float )this->elapsedTime ) * ( 1 - glm::smoothstep( range2, this->duration, (float)this->elapsedTime ) );

	factor = factor * this->velocity;

	//this->direction = glm::normalize( this->finalPosition - this->theGO->position );

	this->theGO->vel = this->direction * factor;

	// Calculate delta position according to the velocity based on time elapsed
	glm::vec3 deltaPosition = ( float )deltaTime * this->theGO->vel;
	
	// Where the object will be
	glm::vec3 nextPosition = this->theGO->position += deltaPosition;
	float nextDistance = glm::distance( finalPosition, nextPosition );

	//if( nextDirection != this->direction )
	if( nextDistance > remainingDistance )
	{ // It means it has inverted the direction, or that it has passed the final position
		this->theGO->position = this->finalPosition;
		// Set the Velocity to 0
		this->theGO->vel = glm::vec3( 0.0f, 0.0f, 0.0f );
	}
	else
	{
		this->theGO->position += deltaPosition;
	}


	//std::cout
	//	<< "ID: " << this->theGO->getID()
	//	<< " | Max.vel: " << this->velocity
	//	<< " | Elapsed Time: " << this->elapsedTime
	//	<< " | Vel.: " << this->theGO->vel.x
	//	<< ", " << this->theGO->vel.y << ", " << this->theGO->vel.z
	//	<< " - Traveled distancet: " << traveledDistance
	//	//<< " | Distance: " << remainingDistance
	//	<< std::endl;
	return;
}


bool cComMoveTo::isDone()
{
	if( this->isCommandDone ) return true;

	// If the GO is on destination, clear the velocity 
	if( this->theGO->position == this->finalPosition ||
		this->elapsedTime >= this->duration )
	{
		this->isCommandDone = true;
		// Set the Velocity

		//std::cout << "ID: " << this->theGO->getID()
		//	<< " final position: " << this->theGO->position.x << ", "
		//	<< this->theGO->position.y << ", "
		//	<< this->theGO->position.z
		//	<< std::endl;

		this->theGO->vel = glm::vec3( 0.0f, 0.0f, 0.0f );
		return true;
	}
	else
		return false;
}

void cComMoveTo::setMyGO( cGameObject* myGO )
{
	this->theGO = myGO;
	return;
}

cGameObject* cComMoveTo::getMyGO()
{
	return this->theGO;
}

void cComMoveTo::setTargetGO( cGameObject* target )
{
	this->targetGO = target;
	return;
}

cGameObject* cComMoveTo::getTargetGO()
{
	return this->targetGO;
}
