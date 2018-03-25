#include "cCommandScheduler.h"

cCommandScheduler::cCommandScheduler()
{

}


cCommandScheduler::~cCommandScheduler()
{

}

cCommandGroup* cCommandScheduler::findGroupByName( std::string name )
{
	for( int i = 0; i != this->commandGroups.size(); i++ )
	{
		if( this->commandGroups[i]->groupName == name )
		{
			return this->commandGroups[i];
		}
	}

	return NULL;
}