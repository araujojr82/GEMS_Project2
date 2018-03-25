#include "cCommandGroup.h"


cCommandGroup::cCommandGroup( std::string theName )
{
	groupName = theName;
	return;
}


cCommandGroup::~cCommandGroup()
{
	return;
}

// Updates all the commands in this command group in parallel
void cCommandGroup::update( double deltaTime )
{	
	for( int i = 0; i != theCommands.size(); i++ )
	{		
		if( !theCommands[i]->isDone() )
		{
			theCommands[i]->update( deltaTime );
		}
	}
	return;
}

// Check if all commands of this group are done
bool cCommandGroup::isDone()
{
	for( int i = 0; i != theCommands.size(); i++ )
	{
		// If one of the commands isn't done, return false
		if( !theCommands[i]->isDone() )
		{
			return false;
		}
	}

	// all commands are done, return true
	return true;
}