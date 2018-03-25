#ifndef _cGameObject_HG_
#define _cGameObject_HG_

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

enum eTypeOfGO
{
	CHARACTER = 0,
	PROP	  = 1,
	ITEM	  = 2,
	TERRAIN   = 3,
	OTHER	  = 99
};

enum eTeam
{
	PLAYER = 0,
	FRIEND = 1,
	ENEMY = 2,
	NONE = 3
};

enum eEnemyType
{
	ANGRY = 0,
	CURIOUS = 1,
	FOLLOWER = 2,
	UNAVAIABLE = 3
};

enum eEnemyBehaviour
{
	WANDER = 0,
	SEEK,
	FLEE,
	PURSUE,
	ARRIVE,
	APPROACH,
	EVADE,
	IDLE, 
	NA
};

enum eTypeOfObject
{	// "Ok people, here's the deal:"
	SPHERE = 0,		// - it's a sphere
	PLANE = 1,		// - it's a plane
	CAPSULE = 2,    // - it's a capsule
	AABB = 3,		// 3- it's an axis-aligned box
	UNKNOWN = 99	// I don't know
};

class cGameObject
{
public:
	cGameObject();		// constructor
	~cGameObject();		// destructor
	glm::vec3 position;
	glm::vec3 prevPosition;

	// AI additions
	eTypeOfGO type;
	eTeam team;
	eEnemyType enemyType;
	eEnemyBehaviour behaviour;
	float range;
	float health;
	float maxVel;
	float wanderAngle;

	// Now orientation 
	void overwrtiteQOrientationFormEuler( glm::vec3 eulerAxisOrientation );
	// NOTE: Use THIS, not just setting the values
	void adjustQOrientationFormDeltaEuler( glm::vec3 eulerAxisOrientChange );
	glm::quat qOrientation;
	glm::quat prevOrientation;

	glm::vec3 getDirectionVector();
	bool isFacingMe( glm::vec3 targetDirection, glm::vec3 targetPosition );

	float scale;

	// **********************************
	// Add some physics things
	glm::vec3 vel;			// Velocity
	glm::vec3 accel;		// Acceleration
	float angle;
	bool bIsUpdatedInPhysics;		// 
	float mass;		// INFINITY				// 
	float inverseMass;	// = 0.0f	// 
	// Refers to the enum above
	eTypeOfObject typeOfObject;		// (really an int)
	float radius;

	// **********************************
	// Lightn
	bool bIsLight;		// is this object a light representation?
	int myLight;		// which light is this?

	bool bIsWireFrame;

	glm::vec4 diffuseColour;	//  The actual color when lights hit it

	std::string meshName;		// mesh I'd  like to draw

	// The texture of this object
	static const unsigned int NUMTEXTURES = 10;
	std::string textureNames[NUMTEXTURES];
	float textureBlend[NUMTEXTURES];			// 0 - 1

	std::string friendlyName;
	inline unsigned int getUniqueID( void ) { return this->m_UniqueID; }

private:
	unsigned int m_UniqueID;
	// Used when creating objects
	static unsigned int m_nextUniqueID;
	
};

#endif
