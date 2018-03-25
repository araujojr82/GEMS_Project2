//Include glad and GLFW in correct order
#include "globalOpenGL_GLFW.h"

#include <iostream>						// C++ cin, cout, etc.
#include <glm/vec3.hpp>					// glm::vec3
#include <glm/vec4.hpp>					// glm::vec4
#include <glm/mat4x4.hpp>				// glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>			// glm::value_ptr
#include <glm/gtc/quaternion.hpp>

#include <stdlib.h>
#include <stdio.h>
// Add the file stuff library (file stream>
#include <fstream>
#include <sstream>						// "String stream"f
#include <istream>
#include <string>
#include <vector>						// smart array, "array" in most languages
#include <random>
#include <chrono>

//#include "Utilities.h"
#include "ModelUtilities.h"
#include "cMesh.h"
#include "cShaderManager.h" 
#include "cGameObject.h"
#include "cVAOMeshManager.h"

#include "Physics.h"

#include "cLightManager.h"

// Include all the things that are accessed in other files
#include "globalGameStuff.h"	

#include "commands\cCommandScheduler.h"
#include "commands\cComMoveTo.h"

// The Game Boundaries:
const float maxX = 30.0f;
const float minX = -30.0f;
const float maxZ = 30.0f;
const float minZ = -30.0f;

cSteeringManager* g_pSteeringManager = NULL;

extern bool MOVING_FORWARD;
extern bool MOVING_BACKWARD;
extern bool TURNING_LEFT;
extern bool TURNING_RIGHT;
extern bool MOVEMENT_CHANGE;

// Euclides: Control selected object for movement
int g_GameObjNumber = 0;				// game object vector position number 
int g_LightObjNumber = 0;				// light object vector position

bool g_lookAtON = false;
bool g_bIsWireframe = false;

cCommandScheduler g_theScheduler;

#include "Thread\Opponent_Threaded.h"
#include "Thread\cOpponent.h"
iOpponentManager* g_pOpponentManager;

std::map< unsigned int, cGameObject*> mapOpponentID_to_GameObject;

// Local array to point the Opponents
std::vector<glm::vec3> vecOpponentAccel;
std::vector<eEnemyBehaviour> vecOpponentBehaviour;
std::vector< cGameObject* > g_vecOpponentsGO;


int g_NUMBER_OF_LIGHTS = 4;

std::vector< cGameObject* > g_vecGameObjects;

cGameObject* g_pThePlayerGO = NULL;

//cCamera* g_pTheCamera = NULL;
cMouseCamera* g_pTheMouseCamera = NULL;
float g_lastX = 0.0f;
float g_lastZ = 0.0f;
bool g_firstMouse = true;

cVAOMeshManager*	g_pVAOManager = 0;		// or NULL or nullptr

cShaderManager*		g_pShaderManager;		// Heap, new (and delete)
cLightManager*		g_pLightManager;

cBasicTextureManager*	g_pTextureManager = 0;

cDebugRenderer*			g_pDebugRenderer = 0;

// Other uniforms:
GLint uniLoc_materialDiffuse = -1;
GLint uniLoc_materialAmbient = -1;
GLint uniLoc_ambientToDiffuseRatio = -1; 	// Maybe	// 0.2 or 0.3
GLint uniLoc_materialSpecular = -1;  // rgb = colour of HIGHLIGHT only
									 // w = shininess of the 
GLint uniLoc_bIsDebugWireFrameObject = -1;

GLint uniLoc_eyePosition = -1;	// Camera position
GLint uniLoc_mModel = -1;
GLint uniLoc_mView = -1;
GLint uniLoc_mProjection = -1;

int g_FPS = 0;
int g_FrameCounter = 0;
double g_FrameClock = 0.0;

struct sWindowConfig
{
public:
	int height = 480;
	int width = 640;
	std::string title = "TO BE REPLACED...";
};

struct sGOparameters		// for the Game Objects' input file
{
	std::string meshname;
	int nObjects;
	float x, y, z, scale;
	std::string random;
	float rangeX, rangeY, rangeZ, rangeScale;
};

struct sMeshparameters		// for the Meshes' input file
{
	std::string meshname;
	std::string meshFilename;
};

//std::vector< pointTriangles > g_vecPoints;

// Forward declare the Functions
void loadConfigFile( std::string fileName, sWindowConfig& wConfig );
sGOparameters parseObjLine( std::ifstream &source );
void loadObjectsFile( std::string fileName );
sMeshparameters parseMeshLine( std::ifstream &source );
void loadMeshesFile( std::string fileName, GLint ShaderID );
void loadLightObjects();
void updateAllObjects( double deltaTime );
void newPlayerGO();
void DrawObject( cGameObject* pTheGO );
float generateRandomNumber( float min, float max );
void mouse_callback( GLFWwindow* window, double xpos, double ypos );
void scroll_callback( GLFWwindow* window, double xoffset, double yoffset );
void ProcessCameraInput( GLFWwindow *window, double deltaTime );

static void error_callback( int error, const char* description )
{
	fprintf( stderr, "Error: %s\n", description );
}


#include <math.h>

void turn_player( double deltaTime )
{

}

void move_player( double deltaTime )
{
	float totalVelocity = 0.0f;
	glm::vec3 movement = glm::vec3( 0.0f );

	if( isnan( ::g_pThePlayerGO->vel.x ) ) ::g_pThePlayerGO->vel.x = 0.0f;
	if( isnan( ::g_pThePlayerGO->vel.y ) ) ::g_pThePlayerGO->vel.y = 0.0f;
	if( isnan( ::g_pThePlayerGO->vel.z ) ) ::g_pThePlayerGO->vel.z = 0.0f;

	if( MOVING_FORWARD )
	{		
		movement = glm::normalize( glm::vec3( ::g_pTheMouseCamera->Front.x, 0.0f, ::g_pTheMouseCamera->Front.z ) );
		movement *= 0.01f;
		::g_pThePlayerGO->vel += movement;

		if( abs( ::g_pThePlayerGO->vel.x ) < 0.001 ) ::g_pThePlayerGO->vel.x = 0.0f;
		if( abs( ::g_pThePlayerGO->vel.y ) < 0.001 ) ::g_pThePlayerGO->vel.y = 0.0f;
		if( abs( ::g_pThePlayerGO->vel.z ) < 0.001 ) ::g_pThePlayerGO->vel.z = 0.0f;
		
		totalVelocity = abs( ::g_pThePlayerGO->vel.x ) +
			abs( ::g_pThePlayerGO->vel.y ) +
			abs( ::g_pThePlayerGO->vel.z );
		if( totalVelocity > ::g_pThePlayerGO->maxVel )
		{
			::g_pThePlayerGO->vel = glm::normalize( ::g_pThePlayerGO->vel ) * ::g_pThePlayerGO->maxVel;			
		}
	}
	else if( MOVING_BACKWARD )
	{
		movement = glm::normalize( glm::vec3( ::g_pTheMouseCamera->Front.x, 0.0f, ::g_pTheMouseCamera->Front.z ) );
		movement *= 0.01f;
		::g_pThePlayerGO->vel -= movement;

		if( abs( ::g_pThePlayerGO->vel.x ) < 0.001 ) ::g_pThePlayerGO->vel.x = 0.0f;
		if( abs( ::g_pThePlayerGO->vel.y ) < 0.001 ) ::g_pThePlayerGO->vel.y = 0.0f;
		if( abs( ::g_pThePlayerGO->vel.z ) < 0.001 ) ::g_pThePlayerGO->vel.z = 0.0f;

		totalVelocity = abs( ::g_pThePlayerGO->vel.x ) +
			abs( ::g_pThePlayerGO->vel.y ) +
			abs( ::g_pThePlayerGO->vel.z );
		if( totalVelocity > ::g_pThePlayerGO->maxVel )
		{
			::g_pThePlayerGO->vel = glm::normalize( ::g_pThePlayerGO->vel ) * ::g_pThePlayerGO->maxVel;
		}
	}
	else
	{
		movement = ::g_pThePlayerGO->vel * 0.1f;
		::g_pThePlayerGO->vel -= movement;

		if( abs( ::g_pThePlayerGO->vel.x ) < 0.001 ) ::g_pThePlayerGO->vel.x = 0.0f;
		if( abs( ::g_pThePlayerGO->vel.y ) < 0.001 ) ::g_pThePlayerGO->vel.y = 0.0f;
		if( abs( ::g_pThePlayerGO->vel.z ) < 0.001 ) ::g_pThePlayerGO->vel.z = 0.0f;
	}
	::g_pThePlayerGO->position += ::g_pThePlayerGO->vel;

	if( TURNING_LEFT )
		::g_pThePlayerGO->adjustQOrientationFormDeltaEuler( glm::vec3( 0.0f, 0.05f, 0.0f ) );
	if( TURNING_RIGHT )
		::g_pThePlayerGO->adjustQOrientationFormDeltaEuler( glm::vec3( 0.0f, -0.05f, 0.0f ) );

	return;
}

void drawCapsule( glm::vec3 position )
{
	cGameObject* capsuleGO = new cGameObject();
	capsuleGO->meshName = "capsule";
	capsuleGO->bIsWireFrame = true;
	capsuleGO->position = position;
	DrawObject( capsuleGO );
	delete capsuleGO;
}

void drawTagCircle( cGameObject* pTheGO )
{
	cGameObject* tagCircleGO = new cGameObject();
	tagCircleGO->meshName = "circle";
	tagCircleGO->textureBlend[0] = 1.0f;
	tagCircleGO->position = pTheGO->position;

	switch( pTheGO->enemyType )
	{
		case ANGRY:
		{
			if( pTheGO->behaviour == eEnemyBehaviour::SEEK )
			{
				tagCircleGO->textureNames[0] = "red.bmp";
				//::g_pDebugRenderer->addCircle( pTheGO->position, pTheGO->range, glm::vec3( 1.0f, 0.0f, 0.0f ) );
			}
			else if( pTheGO->behaviour == eEnemyBehaviour::FLEE )
			{
				tagCircleGO->textureNames[0] = "orange.bmp";
				//::g_pDebugRenderer->addCircle( pTheGO->position, pTheGO->range, glm::vec3( 1.0f, 0.5f, 0.15f ) );
			}
			else
			{
				tagCircleGO->textureNames[0] = "white.bmp";
				//::g_pDebugRenderer->addCircle( pTheGO->position, pTheGO->range, glm::vec3( 1.0f, 1.0f, 1.0f ) );
			}
		}
		break;

		case CURIOUS:
		{
			if( pTheGO->behaviour == eEnemyBehaviour::APPROACH )
			{
				tagCircleGO->textureNames[0] = "blue.bmp";
				//::g_pDebugRenderer->addCircle( pTheGO->position, pTheGO->range, glm::vec3( 0.0f, 0.0f, 1.0f ) );
			}
			else if( pTheGO->behaviour == eEnemyBehaviour::EVADE )
			{
				tagCircleGO->textureNames[0] = "purple.bmp";
				//::g_pDebugRenderer->addCircle( pTheGO->position, pTheGO->range, glm::vec3( 0.5f, 0.25f, 0.5f ) );
			}
			else
			{
				tagCircleGO->textureNames[0] = "gray.bmp";
				//::g_pDebugRenderer->addCircle( pTheGO->position, pTheGO->range, glm::vec3( 0.5f, 0.5f, 0.5f ) );
			}
		}
		break;

		case FOLLOWER:
			if( pTheGO->behaviour == eEnemyBehaviour::SEEK )
			{
				tagCircleGO->textureNames[0] = "green.bmp";
				//::g_pDebugRenderer->addCircle( pTheGO->position, pTheGO->range, glm::vec3( 0.0f, 1.0f, 0.0f ) );
			}
			else
			{
				tagCircleGO->textureNames[0] = "yellow.bmp";
				//::g_pDebugRenderer->addCircle( pTheGO->position, pTheGO->range, glm::vec3( 1.0f, 1.0f, 0.0f ) );
			}
			break;

		case UNAVAIABLE:
			break;
	}

	DrawObject( tagCircleGO );
	delete tagCircleGO;
}

void drawRange()
{
	glm::vec3 color = glm::vec3( 1.0f );

	glm::vec3 point1 = glm::vec3( maxX, 0.0f, maxZ );
	glm::vec3 point2 = glm::vec3( maxX, 0.0f, minZ );
	glm::vec3 point3 = glm::vec3( minX, 0.0f, minZ );
	glm::vec3 point4 = glm::vec3( minX, 0.0f, maxZ );
	
	glm::vec3 point5 = glm::vec3( maxX, 1.0f, maxZ );
	glm::vec3 point6 = glm::vec3( maxX, 1.0f, minZ );
	glm::vec3 point7 = glm::vec3( minX, 1.0f, minZ );
	glm::vec3 point8 = glm::vec3( minX, 1.0f, maxZ );

	::g_pDebugRenderer->addLine( point1, point2, color, true );
	::g_pDebugRenderer->addLine( point2, point3, color, true );
	::g_pDebugRenderer->addLine( point3, point4, color, true );
	::g_pDebugRenderer->addLine( point4, point1, color, true );
														
	::g_pDebugRenderer->addLine( point5, point6, color, true );
	::g_pDebugRenderer->addLine( point6, point7, color, true );
	::g_pDebugRenderer->addLine( point7, point8, color, true );
	::g_pDebugRenderer->addLine( point8, point5, color, true );
														
	::g_pDebugRenderer->addLine( point1, point5, color, true );
	::g_pDebugRenderer->addLine( point2, point6, color, true );
	::g_pDebugRenderer->addLine( point3, point7, color, true );
	::g_pDebugRenderer->addLine( point4, point8, color, true );
}

void checkBoundaries( cGameObject* pTheGO )
{
	if( pTheGO->position.x > maxX ) pTheGO->position.x = minX;
	if( pTheGO->position.x < minX ) pTheGO->position.x = maxX;
	if( pTheGO->position.z > maxZ ) pTheGO->position.z = minZ;
	if( pTheGO->position.z < minZ ) pTheGO->position.z = maxZ;
}


void collisionCheck()
{
	unsigned int sizeOfVector = ( unsigned int )g_vecGameObjects.size();

	for( int i = 0; i != sizeOfVector; i++ )
	{
		cGameObject* pTheOtherGO = ::g_vecGameObjects[i];
		if( pTheOtherGO->type != eTypeOfGO::CHARACTER ) continue; 
		
		if( ::g_pThePlayerGO == pTheOtherGO ) continue;	// Don't test itself
		
		float distance = glm::distance( ::g_pThePlayerGO->position, pTheOtherGO->position );
		if( distance < 0.5f )	//They are overlapping
		{
			//Decrease player health by 0.01
			::g_pThePlayerGO->health -= 0.1f;
			if( ::g_pThePlayerGO->health < 0 ) ::g_pThePlayerGO->health = 0.0f;
		}
	}

	return;
}

int main( void )
{
	GLFWwindow* window;
	GLint mvp_location; //vpos_location, vcol_location;
	glfwSetErrorCallback( error_callback );

	if( !glfwInit() )
		exit( EXIT_FAILURE );

	sWindowConfig wConfig;

	loadConfigFile( "config.txt", wConfig );
	loadObjectsFile( "objects.txt" );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 2 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );

	window = glfwCreateWindow( wConfig.width, wConfig.height,
		wConfig.title.c_str(),
		NULL, // glfwGetPrimaryMonitor(), //
		NULL );
	if( !window )
	{
		glfwTerminate();
		exit( EXIT_FAILURE );
	}

	glfwSetKeyCallback( window, key_callback );
	
	glfwMakeContextCurrent( window );
	gladLoadGLLoader( ( GLADloadproc )glfwGetProcAddress );
	glfwSwapInterval( 1 );
	glfwSetCursorPosCallback( window, mouse_callback );
	glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
	glfwSetScrollCallback( window, scroll_callback );

	std::cout << glGetString( GL_VENDOR ) << " "
		<< glGetString( GL_RENDERER ) << ", "
		<< glGetString( GL_VERSION ) << std::endl;
	std::cout << "Shader language version: " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << std::endl;

	::g_pShaderManager = new cShaderManager();

	cShaderManager::cShader vertShader;
	cShaderManager::cShader fragShader;

	vertShader.fileName = "simpleVert.glsl";
	fragShader.fileName = "simpleFrag.glsl";

	// General error string, used for a number of items during start up
	std::string error;

	::g_pShaderManager->setBasePath( "assets//shaders//" );

	// Shader objects are passed by reference so that
	//	we can look at the results if we wanted to. 
	if( !::g_pShaderManager->createProgramFromFile(
		"mySexyShader", vertShader, fragShader ) )
	{
		std::cout << "Oh no! All is lost!!! Blame Loki!!!" << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;
		// Should we exit?? 
		return -1;
		//		exit(
	}
	std::cout << "The shaders compiled and linked OK" << std::endl;

	::g_pDebugRenderer = new cDebugRenderer();
	if( !::g_pDebugRenderer->initialize( error ) )
	{
		std::cout << "Warning: couldn't init the debug renderer." << std::endl;
	}

	//Load models
	::g_pVAOManager = new cVAOMeshManager();

	GLint sexyShaderID = ::g_pShaderManager->getIDFromFriendlyName( "mySexyShader" );

	loadMeshesFile( "meshlist.txt", sexyShaderID );
	
	// Create a map and a mesh based on it
	newPlayerGO();

	GLint currentProgID = ::g_pShaderManager->getIDFromFriendlyName( "mySexyShader" );

	// Get the uniform locations for this shader
	mvp_location = glGetUniformLocation( currentProgID, "MVP" );		// program, "MVP");
	uniLoc_materialDiffuse = glGetUniformLocation( currentProgID, "materialDiffuse" );
	uniLoc_materialAmbient = glGetUniformLocation( currentProgID, "materialAmbient" );
	uniLoc_ambientToDiffuseRatio = glGetUniformLocation( currentProgID, "ambientToDiffuseRatio" );
	uniLoc_materialSpecular = glGetUniformLocation( currentProgID, "materialSpecular" );
	uniLoc_eyePosition = glGetUniformLocation( currentProgID, "eyePosition" );

	uniLoc_mModel = glGetUniformLocation( currentProgID, "mModel" );
	uniLoc_mView = glGetUniformLocation( currentProgID, "mView" );
	uniLoc_mProjection = glGetUniformLocation( currentProgID, "mProjection" );

	::g_pLightManager = new cLightManager();

	::g_pLightManager->CreateLights( g_NUMBER_OF_LIGHTS );	// There are 10 lights in the shader

	// Change Other lights parameters==========================
	{
		::g_pLightManager->vecLights[0].position = glm::vec3( 50.0f, 50.0f, 50.0f );
		::g_pLightManager->vecLights[0].attenuation.y = 0.03f;	// Linear

		::g_pLightManager->vecLights[1].position = glm::vec3( 50.0f, -50.0f, 50.0f );

		::g_pLightManager->vecLights[2].position = glm::vec3( -50.0f, -50.0f, 50.0f );
		::g_pLightManager->vecLights[1].attenuation.y = 0.01f;	// Linear

		::g_pLightManager->vecLights[3].position = glm::vec3( -50.0f, 50.0f, 50.0f );
		
	}
	//=========================================================
	::g_pLightManager->LoadShaderUniformLocations( currentProgID );

	//loadLightObjects();

	// Texture 
	::g_pTextureManager = new cBasicTextureManager();
	::g_pTextureManager->SetBasePath( "assets/textures" );
	if( !::g_pTextureManager->Create2DTextureFromBMPFile( "rick_texture.bmp", true ) )
	{
		std::cout << "Didn't load the texture. Oh no!" << std::endl;
	}
	else
	{
		std::cout << "Texture is loaded! Hazzah!" << std::endl;
	}
	::g_pTextureManager->Create2DTextureFromBMPFile( "morty.bmp", true );
	::g_pTextureManager->Create2DTextureFromBMPFile( "scary.bmp", true );
	::g_pTextureManager->Create2DTextureFromBMPFile( "meeseeks.bmp", true );		
	::g_pTextureManager->Create2DTextureFromBMPFile( "moon.bmp", true );
	::g_pTextureManager->Create2DTextureFromBMPFile( "red.bmp", true );	
	::g_pTextureManager->Create2DTextureFromBMPFile( "blue.bmp", true );
	::g_pTextureManager->Create2DTextureFromBMPFile( "white.bmp", true );
	::g_pTextureManager->Create2DTextureFromBMPFile( "orange.bmp", true );
	::g_pTextureManager->Create2DTextureFromBMPFile( "purple.bmp", true );
	::g_pTextureManager->Create2DTextureFromBMPFile( "gray.bmp", true );
	::g_pTextureManager->Create2DTextureFromBMPFile( "green.bmp", true );
	::g_pTextureManager->Create2DTextureFromBMPFile( "yellow.bmp", true );

	::g_pSteeringManager = new cSteeringManager();

	::g_pThePlayerGO = findObjectByFriendlyName( PLAYERNAME, ::g_vecGameObjects );


	glm::vec3 camPos = ::g_pThePlayerGO->position + glm::vec3( 0.0f, 20.0f, 40.0f );
	glm::vec3 camUp = glm::vec3( 0.0f, 1.0f, 0.0f );
	::g_pTheMouseCamera = new cMouseCamera( camPos, camUp ); // camYaw, camPitch );
	// Follow the player
	::g_pTheMouseCamera->setTarget( ::g_pThePlayerGO );
	::g_pTheMouseCamera->moveCamera();	

	glEnable( GL_DEPTH );

	// Gets the "current" time "tick" or "step"
	double lastTimeStep = glfwGetTime();

	//drawRange();

	g_FPS = 0;
	g_FrameCounter = 0;
	g_FrameClock = 0.0;

	::g_pOpponentManager->SetIsUpdatingOnAllOpponents( true );

	// Main game or application loop
	while( !glfwWindowShouldClose( window ) )
	{
		g_FrameCounter++;

		float ratio;
		int width, height;
		glm::mat4x4 matProjection;			// was "p"

		glfwGetFramebufferSize( window, &width, &height );
		ratio = width / ( float )height;
		glViewport( 0, 0, width, height );

		// Clear colour AND depth buffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		::g_pShaderManager->useShaderProgram( "mySexyShader" );
		GLint shaderID = ::g_pShaderManager->getIDFromFriendlyName( "mySexyShader" );

		// Update all the light uniforms...
		// (for the whole scene)
		::g_pLightManager->CopyLightInformationToCurrentShader();

		// Changing the projection to include zoom 
		// Projection and view don't change per scene (maybe)
		matProjection = glm::perspective( glm::radians( ::g_pTheMouseCamera->Zoom ), // 0.6f,
			ratio,		// Aspect ratio
			1.0f,			// Near (as big as possible)
			100000.0f );	// Far (as small as possible)

		//matProjection = ::g_p3Pcamera->getProjectionMatrix();

		// View or "camera" matrix
		glm::mat4 matView = glm::mat4( 1.0f );	// was "v"

		// Now the view matrix is taken right from the camera class
		//matView = ::g_pTheCamera->getViewMatrix();
		matView = ::g_pTheMouseCamera->GetViewMatrix();
		//matView = ::g_p3Pcamera->getViewMatrix();


		glUniformMatrix4fv( uniLoc_mView, 1, GL_FALSE,
			( const GLfloat* )glm::value_ptr( matView ) );
		glUniformMatrix4fv( uniLoc_mProjection, 1, GL_FALSE,
			( const GLfloat* )glm::value_ptr( matProjection ) );

		// Set ALL texture units and binding for ENTIRE SCENE (is faster)
		{
			// Set sampler in the shader
			// NOTE: You shouldn't be doing this during the draw call...
			GLint curShaderID = ::g_pShaderManager->getIDFromFriendlyName( "mySexyShader" );
			GLint textSampler00_ID = glGetUniformLocation( curShaderID, "myAmazingTexture00" );
			GLint textSampler01_ID = glGetUniformLocation( curShaderID, "myAmazingTexture01" );
			// And so on (up to 10, or whatever number of textures)... 

			GLint textBlend00_ID = glGetUniformLocation( curShaderID, "textureBlend00" );
			GLint textBlend01_ID = glGetUniformLocation( curShaderID, "textureBlend01" );

			// This connects the texture sampler to the texture units... 
			glUniform1i( textSampler00_ID, 0 );		// Enterprise
			glUniform1i( textSampler01_ID, 1 );		// GuysOnSharkUnicorn
		}


		// Enable blend ("alpha") transparency for the scene
		// NOTE: You "should" turn this OFF, then draw all NON-Transparent objects
		//       Then turn ON, sort objects from far to near ACCORDING TO THE CAMERA
		//       and draw them
		glEnable( GL_BLEND );		// Enables "blending"
									//glDisable( GL_BLEND );
									// Source == already on framebuffer
									// Dest == what you're about to draw
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );



		// Draw the scene
		unsigned int sizeOfVector = ( unsigned int )::g_vecGameObjects.size();	//*****//
		for( int index = 0; index != sizeOfVector; index++ )
		{
			cGameObject* pTheGO = ::g_vecGameObjects[index];

			checkBoundaries( pTheGO );

			DrawObject( pTheGO );

			if( pTheGO->type == eTypeOfGO::CHARACTER )
			{
				if( pTheGO->team == eTeam::ENEMY )
				{
					drawTagCircle( pTheGO );
				}
				else
				{
					::g_pDebugRenderer->addCircle( pTheGO->position, 0.25f, glm::vec3( 1.0f, 1.0f, 1.0f ) );
				}
				//drawCapsule( pTheGO->position );
			}

		}//for ( int index = 0...

		::g_pDebugRenderer->RenderDebugObjects( matView, matProjection );

		std::stringstream ssTitle;
		ssTitle << "GEMS: Project 2"
			<< " | FPS: " << g_FPS;
			//<< "Cam.Pos: "
			//<< g_pTheMouseCamera->Position.x << ", "
			//<< g_pTheMouseCamera->Position.y << ", "
			//<< g_pTheMouseCamera->Position.z << " Yaw: "
			//<< g_pTheMouseCamera->Yaw << " Pitch: "
			//<< g_pTheMouseCamera->Pitch << " Zoom: "
			//<< g_pTheMouseCamera->Zoom;
			//<< "Player Health: " << ::g_pThePlayerGO->health;

		glfwSetWindowTitle( window, ssTitle.str().c_str() );

		glfwSwapBuffers( window );
		glfwPollEvents();

		// Essentially the "frame time"
		// Now many seconds that have elapsed since we last checked
		double curTime = glfwGetTime();
		double deltaTime = curTime - lastTimeStep;
		
		move_player( deltaTime );

		//updateAllObjects( deltaTime );
		::g_pSteeringManager->updateAll( deltaTime );		

		// Check for collisions with the player and update it's health
		collisionCheck();

		// Update camera
		//ProcessCameraInput( window, deltaTime );
		// No need to update the camera if nothing has changed
		if( ::g_pThePlayerGO->position != ::g_pThePlayerGO->prevPosition ||
			::g_pThePlayerGO->qOrientation != ::g_pThePlayerGO->prevOrientation )
		{
			::g_pTheMouseCamera->moveCamera();
		}
		::g_pThePlayerGO->prevOrientation = ::g_pThePlayerGO->qOrientation;
		::g_pThePlayerGO->prevPosition = ::g_pThePlayerGO->position;

		if( g_FrameClock < 1.0 )
		{
			g_FrameClock += deltaTime;
		}
		else
		{
			g_FPS = g_FrameCounter;

			g_FrameCounter = 0;
			g_FrameClock = deltaTime;
		}

		lastTimeStep = curTime;

	}// while ( ! glfwWindowShouldClose(window) )


	glfwDestroyWindow( window );
	glfwTerminate();

	// 
	delete ::g_pShaderManager;
	delete ::g_pVAOManager;

	//    exit(EXIT_SUCCESS);
	return 0;
}

//Load Config.txt
void loadConfigFile( std::string fileName, sWindowConfig& wConfig )
{
	// TODO change this config formating
	std::ifstream infoFile( fileName );
	if( !infoFile.is_open() )
	{	// File didn't open...
		std::cout << "Can't find config file" << std::endl;
		std::cout << "Using defaults" << std::endl;
	}
	else
	{	// File DID open, so read it... 
		std::string a;

		infoFile >> a;	// "Game"	//std::cin >> a;
		infoFile >> a;	// "Config"
		infoFile >> a;	// "width"
		infoFile >> wConfig.width;	// 1080
		infoFile >> a;	// "height"
		infoFile >> wConfig.height;	// 768
		infoFile >> a;		// Title_Start

		std::stringstream ssTitle;		// Inside "sstream"
		bool bKeepReading = true;
		do
		{
			infoFile >> a;		// Title_End??
			if( a != "Title_End" )
			{
				ssTitle << a << " ";
			}
			else
			{	// it IS the end! 
				bKeepReading = false;
				wConfig.title = ssTitle.str();
			}
		} while( bKeepReading );
	}
}

// Generate real random numbers
float generateRandomNumber( float min, float max )
{

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

	std::default_random_engine generator( seed );
	std::uniform_real_distribution<float> distribution( min, max );

	float randomNumber = 0.0;

	randomNumber = distribution( generator );
	return randomNumber;

}

bool CheckDistance( glm::vec3 position, glm::vec3 fromPoint, float minDistance, float maxDistance )
{
	float thisDistance = 0.0f;
	thisDistance = glm::distance( position, fromPoint );

	if( thisDistance >= minDistance || thisDistance <= maxDistance ) return true;

	return false;
}

glm::vec3 GetRandomPosition()
{
	glm::vec3 center = glm::vec3( 0.0f );
	glm::vec3 newPosition = glm::vec3( 0.0f );
	bool validPosition = false;

	while( !validPosition )
	{
		newPosition = glm::vec3( generateRandomNumber( -200, 200 ),
			generateRandomNumber( -200, 200 ),
			generateRandomNumber( -200, 200 ) );
		validPosition = CheckDistance( newPosition, center, 30.0f, 100.0f );
	}

	return newPosition;
}

void newPlayerGO()
{
	// Create a new GO
	cGameObject* pTempGO3 = new cGameObject();

	cMesh theBallMesh;
	::g_pVAOManager->lookupMeshFromName( "rick", theBallMesh );
	
	pTempGO3->meshName = "rick";
	pTempGO3->diffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	
	pTempGO3->position = glm::vec3( 0.0f, 0.0f, 0.0f );
	pTempGO3->prevPosition = pTempGO3->position;
	pTempGO3->scale = 1.0f;
	pTempGO3->overwrtiteQOrientationFormEuler( glm::vec3( 0.0f, 0.0f, 0.0f ) );
	pTempGO3->vel = glm::vec3( 0.0f );
	pTempGO3->friendlyName = "player";
	pTempGO3->typeOfObject = SPHERE;
	pTempGO3->bIsUpdatedInPhysics = true;
	pTempGO3->mass = 0.1;
	pTempGO3->inverseMass = 1.0f / pTempGO3->mass;

	pTempGO3->radius = theBallMesh.maxExtent / 2;

	pTempGO3->textureBlend[0] = 1.0f;
	pTempGO3->textureNames[0] = "rick_texture.bmp";

	pTempGO3->type = eTypeOfGO::CHARACTER;
	pTempGO3->team = eTeam::PLAYER;
	pTempGO3->enemyType = eEnemyType::UNAVAIABLE;
	pTempGO3->range = 4.0f;
	pTempGO3->maxVel = 0.05f;
	pTempGO3->health = 100.0f;

	::g_vecGameObjects.push_back( pTempGO3 );
	::g_GameObjNumber = ::g_vecGameObjects.size() - 1;

}

//Load objects.txt
void loadObjectsFile( std::string fileName )
{
	//sGOparameters sGOpar;
	std::vector <sGOparameters> allObjects;

	std::ifstream objectsFile( fileName );
	if( !objectsFile.is_open() )
	{	// File didn't open...
		std::cout << "Can't find config file" << std::endl;
		std::cout << "Using defaults" << std::endl;
	}
	else
	{	// File DID open, so loop through the file and pushback to structure
		while( !objectsFile.eof() && objectsFile.is_open() ) {
			allObjects.push_back( parseObjLine( objectsFile ) );
		}
		objectsFile.close();  //Closing "costfile.txt"
	}

	for( int index = 0; index != allObjects.size(); index++ )
	{
		// Check, Number of Objects must be at least 1
		if( allObjects[index].nObjects == 0 ) allObjects[index].nObjects = 1;

		if( allObjects[index].meshname == "scary" )
		{	// I'm gonna create the opponent manager here

			::g_pOpponentManager = new cOpponentManager();
			::g_pOpponentManager->Init( allObjects[index].nObjects );
		}

		// Create the number of gameObjects specified in the file for each line 
		for( int i = 0; i != allObjects[index].nObjects; i++ )
		{
			// Create a new GO
			cGameObject* pTempGO = new cGameObject();

			pTempGO->meshName = allObjects[index].meshname; // Set the name of the mesh

			//pTempGO->diffuseColour = glm::vec4( 0.5f, 0.5f, 0.5f, 1.0f );
			pTempGO->diffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
			//pTempGO->rotation = glm::vec3( 0.0f );
			pTempGO->overwrtiteQOrientationFormEuler( glm::vec3( 0.0f, 0.0f, 0.0f ) );


			// SOME OBJECTS ARE RANDOMLY PLACED WHEN RANDOM=TRUE ON FILE
			if( allObjects[index].random == "true" )
			{
				pTempGO->position.x = generateRandomNumber( -allObjects[index].rangeX, allObjects[index].rangeX );
				pTempGO->position.y = generateRandomNumber( -allObjects[index].rangeY, allObjects[index].rangeY );
				pTempGO->position.z = generateRandomNumber( -allObjects[index].rangeZ, allObjects[index].rangeZ );
				pTempGO->scale = allObjects[index].rangeScale;
			}
			else
			{   // position and scale are fixed
				pTempGO->position.x = allObjects[index].x;
				pTempGO->position.y = allObjects[index].y;
				pTempGO->position.z = allObjects[index].z;
				pTempGO->scale = allObjects[index].scale;
			}
			// NO VELOCITY
			pTempGO->vel = glm::vec3( 0.0f );

			if( pTempGO->meshName == "rick" )
			{
				pTempGO->textureNames[0] = "rick_texture.bmp";
				pTempGO->textureBlend[0] = 1.0f;
				pTempGO->type = eTypeOfGO::CHARACTER;
				pTempGO->team = eTeam::PLAYER;
				pTempGO->enemyType = eEnemyType::UNAVAIABLE;
			}

			else if( pTempGO->meshName == "scary" )
			{
				pTempGO->textureNames[0] = "scary.bmp";
				pTempGO->textureBlend[0] = 1.0f;
				pTempGO->type = eTypeOfGO::CHARACTER;
				pTempGO->team = eTeam::ENEMY;
				pTempGO->enemyType = eEnemyType::ANGRY;
				pTempGO->range = 6.0f;
				pTempGO->health = 100.0f;
				pTempGO->maxVel = 1.5f;

				{	// This will create the Thread for this Opponent
					cOpponent* pCurrentOpponent = NULL;
					::g_pOpponentManager->CreateOpponentThread( pTempGO, pCurrentOpponent );

					mapOpponentID_to_GameObject[pCurrentOpponent->getOpponentID()] = pTempGO;

					::g_vecOpponentsGO.push_back( pTempGO );
				}
			}
				
			else if( pTempGO->meshName == "terrain" )
			{
				pTempGO->textureNames[0] = "moon.bmp";
				pTempGO->textureBlend[0] = 1.0f;
				pTempGO->type = eTypeOfGO::TERRAIN;
				pTempGO->team = eTeam::NONE;
				pTempGO->enemyType = eEnemyType::UNAVAIABLE;
			}
			
			::g_vecGameObjects.push_back( pTempGO );
		}
	}
}

// Parse the file line to fit into the structure
sGOparameters parseObjLine( std::ifstream &source ) {

	sGOparameters sGOpar;

	//Scanning a line from the file
	source >> sGOpar.meshname >> sGOpar.nObjects
		>> sGOpar.x >> sGOpar.y >> sGOpar.z >> sGOpar.scale
		>> sGOpar.random
		>> sGOpar.rangeX >> sGOpar.rangeY >> sGOpar.rangeZ
		>> sGOpar.rangeScale;


	return sGOpar;
}

//Load meshlist.txt
void loadMeshesFile( std::string fileName, GLint ShaderID )
{
	std::vector <sMeshparameters> allMeshes;

	std::ifstream objectsFile( fileName );
	if( !objectsFile.is_open() )
	{	// File didn't open...
		std::cout << "Can't find config file" << std::endl;
		std::cout << "Using defaults" << std::endl;
	}
	else
	{	// File DID open, so loop through the file and pushback to structure
		while( !objectsFile.eof() && objectsFile.is_open() ) {
			allMeshes.push_back( parseMeshLine( objectsFile ) );
		}
		objectsFile.close();  //Closing "costfile.txt"
	}

	for( int index = 0; index != allMeshes.size(); index++ )
	{
		cMesh testMesh;
		testMesh.name = allMeshes[index].meshname;
		//if( !LoadPlyFileIntoMesh( allMeshes[index].meshFilename, testMesh ) )
		if( !LoadPlyFileIntoMeshWithUV( allMeshes[index].meshFilename, testMesh ) )
		{
			std::cout << "Didn't load model" << std::endl;
			// do something??
		}
		if( testMesh.name == "ball" )
		{
			if( !::g_pVAOManager->loadMeshIntoVAO( testMesh, ShaderID, true ) )
			{
				std::cout << "Could not load mesh into VAO" << std::endl;
			}
		}
		else
		{
			if( !::g_pVAOManager->loadMeshIntoVAO( testMesh, ShaderID ) )
			{
				std::cout << "Could not load mesh into VAO" << std::endl;
			}
		}		
	}
}

// Parse the file line to fit into the structure
sMeshparameters parseMeshLine( std::ifstream &source ) {

	sMeshparameters sMeshpar;

	//Scanning a line from the file
	source >> sMeshpar.meshname >> sMeshpar.meshFilename;

	return sMeshpar;
}

void loadLightObjects()
{
	for( int index = 0; index < g_NUMBER_OF_LIGHTS; index++ )
	{
		// Create a new GO
		cGameObject* pTempGO = new cGameObject();

		pTempGO->meshName = "sphere"; // Set the name of the mesh

									  // position is based on light position
		pTempGO->position = ::g_pLightManager->vecLights[index].position;

		if( index == 0 ) pTempGO->scale = 3.0f;
		else pTempGO->scale = 1.0f;

		// Each light is initially white
		pTempGO->diffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );

		pTempGO->bIsLight = true;
		pTempGO->myLight = index;

		::g_vecGameObjects.push_back( pTempGO );
	}
}

void updateAllObjects( double deltaTime )
{
	for( int i = 0; i != ::g_theScheduler.commandGroups.size(); i++ )
	{
		if( !g_theScheduler.commandGroups[i]->isDone() )
		{
			// Run the Command groups in sequence
			// Each command group runs its commands in paralel
			g_theScheduler.commandGroups[i]->update( deltaTime );
			break;
		}
	}

	return;
}

// Draw a single object
void DrawObject( cGameObject* pTheGO )
{
	// Is there a game object? 
	if( pTheGO == 0 )	//if ( ::g_GameObjects[index] == 0 )
	{	// Nothing to draw
		return;		// Skip all for loop code and go to next
	}

	// Was near the draw call, but we need the mesh name
	std::string meshToDraw = pTheGO->meshName;		//::g_GameObjects[index]->meshName;

	sVAOInfo VAODrawInfo;
	if( ::g_pVAOManager->lookupVAOFromName( meshToDraw, VAODrawInfo ) == false )
	{	// Didn't find mesh
		std::cout << "WARNING: Didn't find mesh " << meshToDraw << " in VAO Manager" << std::endl;
		return;
	}

	// There IS something to draw

	// 'model' or 'world' matrix
	glm::mat4x4 mModel = glm::mat4x4( 1.0f );	//		mat4x4_identity(m);

	glm::mat4 trans = glm::mat4x4( 1.0f );
	trans = glm::translate( trans,
		pTheGO->position );
	mModel = mModel * trans;

	// Now with quaternion rotation
	// Like many things in GML, the conversion is done in the constructor
	glm::mat4 postRotQuat = glm::mat4( pTheGO->qOrientation );
	mModel = mModel * postRotQuat;

	float finalScale = pTheGO->scale;

	glm::mat4 matScale = glm::mat4x4( 1.0f );
	matScale = glm::scale( matScale,
		glm::vec3( finalScale,
			finalScale,
			finalScale ) );
	mModel = mModel * matScale;


	glUniformMatrix4fv( uniLoc_mModel, 1, GL_FALSE,
		( const GLfloat* )glm::value_ptr( mModel ) );

	glm::mat4 mWorldInTranpose = glm::inverse( glm::transpose( mModel ) );

	glUniform4f( uniLoc_materialDiffuse,
		pTheGO->diffuseColour.r,
		pTheGO->diffuseColour.g,
		pTheGO->diffuseColour.b,
		pTheGO->diffuseColour.a );

	if( pTheGO->bIsWireFrame )
	{
		glUniform1f( uniLoc_bIsDebugWireFrameObject, 1.0f );	// TRUE
	}
	else
	{
		glUniform1f( uniLoc_bIsDebugWireFrameObject, 0.0f );	// FALSE
	}

	// Set up the textures
	std::string textureName = pTheGO->textureNames[0];
	GLuint texture00Number
		= ::g_pTextureManager->getTextureIDFromName( textureName );
	// Texture binding... (i.e. set the 'active' texture
	GLuint texture00Unit = 0;							// Texture units go from 0 to 79 (at least)
	glActiveTexture( texture00Unit + GL_TEXTURE0 );		// GL_TEXTURE0 = 33984
	glBindTexture( GL_TEXTURE_2D, texture00Number );

	// 0 
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D,
		::g_pTextureManager->getTextureIDFromName( pTheGO->textureNames[0] ) );
	
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D,
		::g_pTextureManager->getTextureIDFromName( pTheGO->textureNames[1] ) );
	
	// Set sampler in the shader
	// NOTE: You shouldn't be doing this during the draw call...
	GLint curShaderID = ::g_pShaderManager->getIDFromFriendlyName( "mySexyShader" );
	GLint textSampler00_ID = glGetUniformLocation( curShaderID, "myAmazingTexture00" );
	GLint textSampler01_ID = glGetUniformLocation( curShaderID, "myAmazingTexture01" );
	
	GLint textBlend00_ID = glGetUniformLocation( curShaderID, "textureBlend00" );
	GLint textBlend01_ID = glGetUniformLocation( curShaderID, "textureBlend01" );

	// And the blending values
	glUniform1f( textBlend00_ID, pTheGO->textureBlend[0] );
	glUniform1f( textBlend01_ID, pTheGO->textureBlend[1] );

	if( g_bIsWireframe || pTheGO->bIsWireFrame )
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );	// Default
//		glEnable(GL_DEPTH_TEST);		// Test for z and store in z buffer
		glDisable( GL_CULL_FACE );
	}
	else
	{
		//glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );	// Default
		glPolygonMode( GL_FRONT, GL_FILL );	// Default
		glEnable( GL_DEPTH_TEST );		// Test for z and store in z buffer
		glEnable( GL_CULL_FACE );
	}

	glCullFace( GL_BACK );	

	glBindVertexArray( VAODrawInfo.VAO_ID );

	glDrawElements( GL_TRIANGLES,
		VAODrawInfo.numberOfIndices,		// testMesh.numberOfTriangles * 3,	// How many vertex indices
		GL_UNSIGNED_INT,					// 32 bit int 
		0 );
	// Unbind that VAO
	glBindVertexArray( 0 );

	return;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback( GLFWwindow* window, double xpos, double zpos )
{
	if( ::g_firstMouse )
	{
		::g_lastX = ( float )xpos;
		::g_lastZ = ( float )zpos;
		::g_firstMouse = false;
	}

	float xoffset = ( float )xpos - ::g_lastX;
	float zoffset = ::g_lastZ - ( float )zpos; // reversed since y-coordinates go from bottom to top

	::g_lastX = ( float )xpos;
	::g_lastZ = ( float )zpos;

	float rateOfTurn = xoffset * -0.01;

	//::g_pTheBall->adjustQOrientationFormDeltaEuler( glm::vec3( 0.0f, rateOfTurn, 0.0f ) );
	//::g_pTheMouseCamera->ProcessMouseMovement( xoffset, zoffset );	
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback( GLFWwindow* window, double xoffset, double zoffset )
{
	::g_pTheMouseCamera->ProcessMouseScroll( ( float )zoffset );
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void ProcessCameraInput( GLFWwindow *window, double deltaTime )
{
	if( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
		glfwSetWindowShouldClose( window, true );

	if( glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS )
		::g_pTheMouseCamera->ProcessKeyboard( FORWARD, ( float )deltaTime );
	if( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS )
		::g_pTheMouseCamera->ProcessKeyboard( BACKWARD, ( float )deltaTime );
	if( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS )
		::g_pTheMouseCamera->ProcessKeyboard( LEFT, ( float )deltaTime );
	if( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS )
		::g_pTheMouseCamera->ProcessKeyboard( RIGHT, ( float )deltaTime );
	if( glfwGetKey( window, GLFW_KEY_Q ) == GLFW_PRESS )
		::g_pTheMouseCamera->ProcessKeyboard( UP, ( float )deltaTime );
	if( glfwGetKey( window, GLFW_KEY_E ) == GLFW_PRESS )
		::g_pTheMouseCamera->ProcessKeyboard( DOWN, ( float )deltaTime );
}