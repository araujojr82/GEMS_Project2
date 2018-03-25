#pragma once
#include "globalGameStuff.h"

class cSteeringManager
{
public:
	cSteeringManager();
	~cSteeringManager();

	void updateAll( double deltaTime );
	void reset();

	float CIRCLE_DISTANCE = 4.0f;
	float CIRCLE_RADIUS = 2.0f;
	float ANGLE_CHANGE = 0.20f;

private:
	void update( cGameObject* pTheGO, double deltaTime );

	cGameObject* findTarget( cGameObject* pTheGO );

	void setBehaviour( cGameObject* pTheGO, cGameObject* pTargetGO );
	void solveSteering( cGameObject* pTheGO, cGameObject* pTargetGO );

	glm::vec3 seek( cGameObject* pTheGO, glm::vec3 targetDestination, float slowingRadius );
	glm::vec3 flee( cGameObject* pTheGO, glm::vec3 targetDestination );
	glm::vec3 wander( cGameObject* pTheGO );
	glm::vec3 evade( cGameObject* pTheGO, cGameObject* pTargetGO );
	glm::vec3 pursuit( cGameObject* pTheGO, cGameObject* pTargetGO, float slowingRadius );
	glm::vec3 approach( cGameObject* pTheGO, glm::vec3 targetDestination, float slowingRadius );

	double currentTimeStep;
};

