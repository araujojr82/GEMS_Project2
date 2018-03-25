#ifndef _iCommand_HG_
#define _iCommand_HG_

#include "../cGameObject.h"
#include "../cDebugRenderer.h"

// Interface for all the commands

class iCommand
{
public:
	//virtual iCommand(); no constructor on the interface
	virtual ~iCommand() {}; // {} to make it virtual

	// Virtual Methods
	virtual void init( glm::vec3 parameter1, float parameter2, glm::vec3 parameter3 ) = 0;
	virtual void update( double deltaTime ) = 0;
	virtual bool isDone() = 0;
	virtual void setMyGO( cGameObject* myGO ) = 0;
	virtual cGameObject* getMyGO() = 0;
};


#endif