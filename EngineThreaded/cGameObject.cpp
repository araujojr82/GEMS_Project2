#include "cGameObject.h"

#include <iostream>

// Start the unique IDs at 1. Why not?
/*static*/ unsigned int cGameObject::m_nextUniqueID = 1;

cGameObject::cGameObject()
{
	this->scale = 1.0f;	// (not zero)
	this->position = glm::vec3(0.0f);
	this->prevPosition = glm::vec3( NULL );
	this->overwrtiteQOrientationFormEuler( glm::vec3( 0.0f, 0.0f, 0.0f ) );

	this->vel = glm::vec3( 0.0f );
	this->accel = glm::vec3( 0.0f );

	// If you aren't sure what the 4th value should be,
	// make it a 1.0f ("alpha" or transparecy)
	this->diffuseColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	//Assume everything is simulated 
	this->bIsUpdatedInPhysics = true; //??? 
	this->radius = 0.0f;	// Is this the best value??? Who knows?
	this->mass = 0.0f;
	this->inverseMass = 0.0f;
	this->typeOfObject = UNKNOWN;
	
	this->bIsLight = false;
	this->bIsWireFrame = false;
	
	this->myLight = NULL;
	
	// Set all texture blend values to 0.0f (meaning NO texture)
	for( int index = 0; index != NUMTEXTURES; index++ )
	{
		this->textureBlend[index] = 0.0f;
	}

	// Assign unque ID, the increment for next created object
	// (Note: if you write your own copy constructor, be sure to COPY this
	//	value, rather than generate a new one - i.e. call the c'tor again)
	this->m_UniqueID = cGameObject::m_nextUniqueID++;

	//this->pDebugRenderer = NULL;

	//this->bIsSkyBoxObject = false;

	this->type = eTypeOfGO::OTHER;
	this->team = eTeam::NONE;
	this->enemyType = eEnemyType::UNAVAIABLE;
	this->behaviour = eEnemyBehaviour::NA;
	this->range = NULL;
	this->health = NULL;
	this->maxVel = NULL;
	this->wanderAngle = 0.0f;

	return;
}

cGameObject::~cGameObject()
{
	return;
}

void cGameObject::overwrtiteQOrientationFormEuler( glm::vec3 eulerAxisOrientation )
{
	this->prevOrientation = this->qOrientation;
	// Calcualte the quaternion represnetaiton of this Euler axis
	// NOTE: We are OVERWRITING this..
	this->qOrientation = glm::quat( eulerAxisOrientation );

	return;
}

void cGameObject::adjustQOrientationFormDeltaEuler( glm::vec3 eulerAxisOrientChange )
{
	// How do we combine two matrices?
	// That's also how we combine quaternions...

	// So we want to "add" this change in oriention
	glm::quat qRotationChange = glm::quat( eulerAxisOrientChange );

	// Mulitply it by the current orientation;
	this->qOrientation = this->qOrientation * qRotationChange;

	return;
}

glm::vec3 cGameObject::getDirectionVector()
{
	glm::vec3 eulerAngles;

	eulerAngles = glm::eulerAngles( this->qOrientation );
	//eulerAngles = glm::normalize( eulerAngles );
	//}

	//return directionVector;

	float movAngleDegrees;

	float zDegrees = glm::degrees( eulerAngles.z );
	float yDegrees = glm::degrees( eulerAngles.y );

	if( zDegrees != 0.0f )
	{
		movAngleDegrees = ( zDegrees / 2 ) + ( ( zDegrees / 2 ) - abs( yDegrees ) );
		if( yDegrees < 0 )
		{
			movAngleDegrees *= -1;
		}
	}
	else
	{
		movAngleDegrees = yDegrees;
	}

	glm::vec3 directionVector = glm::vec3( 0.0f );
	directionVector.z = glm::cos( glm::radians( movAngleDegrees ) );
	directionVector.x = glm::sin( glm::radians( movAngleDegrees ) );

	directionVector = glm::normalize( directionVector );
	
	//float temp = glm::atan( directionVector.x, directionVector.z );
	//// Convert it from radians to degrees
	//temp = glm::degrees( temp );

	//std::cout << "Angle in Degrees: " << movAngleDegrees
	//	<< " New angle in Degrees: " << temp << std::endl;
	//	//<< " dir. vector: ( " << directionVector.x << ", "
	//	//<< directionVector.y << ", "
	//	//<< directionVector.z << ")"
	//	//<< std::endl;

	return directionVector;
}

bool cGameObject::isFacingMe( glm::vec3 targetDirection, glm::vec3 targetPosition )
{
	bool isItFacing;

	// dotProduct( normalize( B - A ), normalize( directionFacingOfA ) )
	float facing = glm::dot( glm::normalize( this->position - targetPosition ), targetDirection );

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

	std::string facingText;
	if( isItFacing ) facingText = "True";
	else facingText = "False ";

	//std::cout << "Facing: " << isItFacing << " " << facingText << " scale: " << facing << std::endl;
	return isItFacing;
}