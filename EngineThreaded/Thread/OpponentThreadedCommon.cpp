// OpponentThreadedCommon

#include "..\cGameObject.h"
#include "OpponentThreadedCommon.h"

#include "cOpponent.h"

cRandThreaded* g_pThreadedRandom = NULL;


// NOTE: Should be aligned on 32 bit boundtry (see _aligned_malloc)
extern int unsigned* g_pCS_per_fram_count = NULL;


cGameObject* MakeOpponentGameObject(glm::vec3 initPosition)
{
	cGameObject* pOpponent = new cGameObject();
	pOpponent->friendlyName = "Opponent";
	pOpponent->position = initPosition;
	pOpponent->prevPosition = initPosition;
	pOpponent->scale = 0.75f;

	//sMeshDrawInfo meshInfo;
	//meshInfo.scale = pOpponent->scale;
	//meshInfo.setMeshOrientationEulerAngles( glm::vec3( 0.0f, 0.0f, 0.0f ) );
	//meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	//meshInfo.name = "opponent";
	//meshInfo.vecMehs2DTextures.push_back( sTextureBindBlendInfo( "yellow.bmp", 1.0f ) ); 
	//pOpponent->vecMeshes.push_back(meshInfo);

	return pOpponent;
}

// This is the actual threading function
DWORD WINAPI OpponentBrainThread(void* pInitialData)	// CreateThread() format
{
	cOpponent* pOpponent = (cOpponent*)(pInitialData);

	while ( pOpponent->bIsAlive )
	{
		if ( pOpponent->bIsUpdating )
		{
			// That's it: calls the Update on the object that we passed in...
			pOpponent->Update();

			Sleep(1);
		}
		else
		{	
			// Sleep for a while...
			// This is likely happening during the start up phase
			Sleep(500);
		}
	}//while ( pOpponent->bIsAlive )

	return 0;
}
