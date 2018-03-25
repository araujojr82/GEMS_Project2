#pragma once
#ifndef _globalGameStuff_HG_
#define _globalGameStuff_HG_

// This is all the global 'game' stuff, so:
// - not OpenGL rendering stuff (unless we can't help it)
// - just 'engine' or game play stuff

#include "cGameObject.h"
#include <vector>
#include <glm/vec3.hpp>
#include "cLightManager.h"
//#include "cModelAssetLoader.h"
#include "cMouseCamera.h"
#include "cSteeringManager.h"

// Remember to #include <vector>...
extern std::vector< cGameObject* >  g_vecGameObjects;

extern bool g_bIsWireframe;

// Returns 0 or NULL if not found
cGameObject* findObjectByFriendlyName( std::string friendlyName, std::vector<cGameObject*> &vec_pGameObjects );
cGameObject* findObjectByUniqueID( unsigned int ID, std::vector<cGameObject*> &vec_pGameObjects );

extern cLightManager*	g_pLightManager;	// (theMain.cpp)
											// Super basic physics update function
//void PhysicsStep( double deltaTime );

const std::string PLAYERNAME = "player";		// Now that we have a lookup

extern cGameObject* g_pThePlayerGO;

extern cMouseCamera* g_pTheMouseCamera;

#endif
