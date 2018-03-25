#ifndef _cCommandScheduler_HG_
#define _cCommandScheduler_HG_

#include "iCommand.h"
#include "cCommandGroup.h"

class cCommandScheduler
{
public:
	cCommandScheduler();
	~cCommandScheduler();

	cCommandGroup* findGroupByName( std::string name );

	// is commandGroups a vector of vectors pointers of command pointers
	// or it is a vector of vectors of command pointers pointers?
	std::vector< cCommandGroup* > commandGroups;
};

#endif
