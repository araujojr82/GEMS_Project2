#include "globalOpenGL_GLFW.h"
#include "globalGameStuff.h"

#include <iostream>

extern cSteeringManager* g_pSteeringManager;

bool isShiftKeyDown( int mods, bool bByItself = true );
bool isCtrlKeyDown( int mods, bool bByItself = true );
bool isAltKeyDown( int mods, bool bByItself = true );

bool MOVING_FORWARD = false;
bool MOVING_BACKWARD = false;
bool TURNING_LEFT = false;
bool TURNING_RIGHT = false;
bool MOVEMENT_CHANGE = false;

glm::vec3 movement = glm::vec3( 0.0f );

/*static*/ void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	MOVEMENT_CHANGE = false;

	if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose( window, GLFW_TRUE );
	
	if( key == GLFW_KEY_ENTER && action == GLFW_PRESS )
	{
		::g_bIsWireframe = !::g_bIsWireframe;
	}
	
	const float CAMERASPEED = 0.1f;

	const float CAM_ACCELL_THRUST = 100.0f;	

	switch ( key )
	{
		case GLFW_KEY_A:		//
			::g_pSteeringManager->CIRCLE_RADIUS *= 0.95f;		
			break;

		case GLFW_KEY_D:		//
			::g_pSteeringManager->CIRCLE_RADIUS *= 1.05f;
			break;

		case GLFW_KEY_W:		//
			::g_pSteeringManager->CIRCLE_DISTANCE *= 1.05f;			
			break;

		case GLFW_KEY_S:		//
			::g_pSteeringManager->CIRCLE_DISTANCE *= 0.95f;
			break;

		case GLFW_KEY_Q:		//
			::g_pSteeringManager->ANGLE_CHANGE *= 0.95f;
			break;

		case GLFW_KEY_E:		// "Up" (along y axis)
			::g_pSteeringManager->ANGLE_CHANGE *= 1.05f;
			break;

		case GLFW_KEY_UP:
			if( action == GLFW_PRESS )
			{
				MOVEMENT_CHANGE = true;
				MOVING_FORWARD = true;
			}			
			else if( action == GLFW_RELEASE )
			{
				MOVEMENT_CHANGE = true;
				MOVING_FORWARD = false;
			}
			break;

		case GLFW_KEY_DOWN:
			if( action == GLFW_PRESS )
			{
				MOVEMENT_CHANGE = true;
				MOVING_BACKWARD = true;
			}	
			else if( action == GLFW_RELEASE )
			{
				MOVEMENT_CHANGE = true;
				MOVING_BACKWARD = false;
			}
			break;

		case GLFW_KEY_LEFT:
			if( action == GLFW_PRESS )
			{
				MOVEMENT_CHANGE = true;
				TURNING_LEFT = true;
			}	
			else if( action == GLFW_RELEASE )
			{
				MOVEMENT_CHANGE = true;
				TURNING_LEFT = false;
			}
			break;

		case GLFW_KEY_RIGHT:	
			if( action == GLFW_PRESS )
			{
				MOVEMENT_CHANGE = true;
				TURNING_RIGHT = true;
			}			
			else if( action == GLFW_RELEASE )
			{
				MOVEMENT_CHANGE = true;
				TURNING_RIGHT = false;
			}			
			break;
		}	
	return;
}

// Helper functions
bool isShiftKeyDown( int mods, bool bByItself /*=true*/ )
{
	if( bByItself )
	{	// shift by itself?
		if( mods == GLFW_MOD_SHIFT )	{ return true; }
		else							{ return false; }
	}
	else
	{	// shift with anything else, too
		if( ( mods && GLFW_MOD_SHIFT ) == GLFW_MOD_SHIFT )	{ return true; }
		else												{ return false; }
	}
	// Shouldn't never get here, so return false? I guess?
	return false;
}

bool isCtrlKeyDown( int mods, bool bByItself /*=true*/ )
{
	if( bByItself )
	{	// shift by itself?
		if( mods == GLFW_MOD_CONTROL )	{ return true; }
		else							{ return false; }
	}
	else
	{	// shift with anything else, too
		if( ( mods && GLFW_MOD_CONTROL ) == GLFW_MOD_CONTROL )	{ return true; }
		else												{ return false; }
	}
	// Shouldn't never get here, so return false? I guess?
	return false;
}

bool isAltKeyDown( int mods, bool bByItself /*=true*/ )
{
	if( bByItself )
	{	// shift by itself?
		if( mods == GLFW_MOD_ALT ) { return true; }
		else { return false; }
	}
	else
	{	// shift with anything else, too
		if( ( mods && GLFW_MOD_ALT ) == GLFW_MOD_ALT ) { return true; }
		else { return false; }
	}
	// Shouldn't never get here, so return false? I guess?
	return false;
}