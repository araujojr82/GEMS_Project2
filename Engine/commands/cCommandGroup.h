#ifndef _cCommandGroup_HG_
#define _cCommandGroup_HG_

#include "iCommand.h"

class cCommandGroup
{
public:
	cCommandGroup( std::string theName );
	~cCommandGroup();
	void update( double deltaTime );
	bool isDone();

	// theCommands is a vector of command pointers
	std::vector< iCommand* > theCommands;

	std::string groupName;
};

#endif