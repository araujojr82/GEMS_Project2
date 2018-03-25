#ifndef _cMouseCamera_HG_
#define _cMouseCamera_HG_

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <iostream>

class cGameObject;

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = -26.0f;
const float SPEED = 8.0f;
const float SENSITIVTY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class cMouseCamera
{
public:

	enum eMode
	{
		MANUAL,			// Move along the axes (lookat)
		FOLLOW_CAMERA,	// Follows a target (lookat)
		FLY_CAMERA		// Movement based on direction of gaze
						// Use quaternion orientation
						// "catch"  is no LOOKAT
	};

	cGameObject* target;

	eMode cameraMode;

	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Eular Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;					//Distance from the target
	float angleAroundTarget;

	// Constructor with vectors
	cMouseCamera( glm::vec3 position = glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3 up = glm::vec3( 0.0f, 1.0f, 0.0f ),
		float yaw = YAW, float pitch = PITCH ) : Front( glm::vec3( 0.0f, 0.0f, 1.0f ) ),
		MovementSpeed( SPEED ),
		MouseSensitivity( SENSITIVTY ),
		Zoom( ZOOM )
	{
		cameraMode = MANUAL;
		angleAroundTarget = 0.0f;
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	// Constructor with scalar values
	cMouseCamera( float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch ) :
		Front( glm::vec3( 0.0f, 0.0f, 1.0f ) ),
		MovementSpeed( SPEED ),
		MouseSensitivity( SENSITIVTY ),
		Zoom( ZOOM )
	{
		cameraMode = MANUAL;
		angleAroundTarget = 0.0f;
		Position = glm::vec3( posX, posY, posZ );
		WorldUp = glm::vec3( upX, upY, upZ );
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	void setTarget( cGameObject* theTarget )
	{
		this->target = theTarget;

		return;
	}

	void updateCamera()
	{
		this->Pitch = calculatePitch();
		this->Yaw = calculateYaw();
		updateCameraVectors();
	}

	float calculatePitch()
	{
		glm::vec3 pitchRef; 
		pitchRef.x = this->target->position.x;
		pitchRef.y = this->Position.y;
		pitchRef.z = this->target->position.z;

		glm::vec3 da = glm::normalize( pitchRef - this->Position );
		glm::vec3 db = glm::normalize( this->target->position - this->Position );
		float cos =  glm::acos( glm::dot( da, db ) );
		cos *= -1;
		return glm::degrees( cos );
	}

	float getYawInDegrees( glm::quat theQuat )
	{
		float yaw = 0.0f;
		glm::vec3 targetEulerRot = glm::eulerAngles( theQuat );
		float zDegrees = glm::degrees( targetEulerRot.z );
		float yDegrees = glm::degrees( targetEulerRot.y );
		
		if( zDegrees != 0.0f )
		{
			yaw = ( zDegrees / 2 ) + ( ( zDegrees / 2 ) - abs( yDegrees ) );
			if( yDegrees < 0 )
			{				
				yaw *= -1;
			}			
		}
		else
		{
			yaw = yDegrees;
		}		

		return yaw;
	}

	float calculateYaw()
	{
		float newYaw = getYawInDegrees( target->qOrientation );
		newYaw -= 90.0f;
		newYaw *= -1;
		return newYaw;
	}


	float calculateDistance()
	{
		return glm::distance( this->Position, target->position );
	}

	float calculateHorizontalDistance()
	{
		return this->Zoom * glm::cos( glm::radians( this->Pitch ) );
		//float distance = calculateDistance();
		//float hDist = distance * glm::cos( glm::radians( this->Pitch ) );
		//return hDist;
	}

	float calculateVerticalDistance()
	{
		return this->Zoom * glm::sin( glm::radians( this->Pitch ) );
		//float distance = calculateDistance();
		//float vDist = distance * glm::sin( glm::radians( this->Pitch ) );
		//vDist *= -1;
		//return vDist;
	}

	void moveCamera()
	{
		float horizontalDistance = calculateHorizontalDistance();
		float verticalDistance = calculateVerticalDistance();
		calculateCameraPosition( horizontalDistance, verticalDistance );
	}

	void calculateCameraPosition( float hDist, float vDist )
	{
		glm::vec3 targetEulerRot = glm::eulerAngles( target->qOrientation );

		float yAngle = getYawInDegrees( target->qOrientation );
		
		float thetaAngle = yAngle + angleAroundTarget;
		float offsetX = hDist * glm::sin( glm::radians( thetaAngle ) );
		float offsetZ = hDist * glm::cos( glm::radians( thetaAngle ) );

		this->Position.x = this->target->position.x - offsetX;
		this->Position.y = this->target->position.y - vDist;
		this->Position.z = this->target->position.z - offsetZ;
		this->Yaw = calculateYaw();
		this->Pitch = calculatePitch();
		this->updateCameraVectors();
	}

	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		//return glm::lookAt( Position, Position + Front, Up );

		// Based on the mode, calculate the view matrix
		switch( cameraMode )
		{
		case cMouseCamera::eMode::MANUAL:
			// Use LookAT
			return glm::lookAt( Position, Position + Front, Up );
			break;


		case cMouseCamera::eMode::FOLLOW_CAMERA:
			/*glm::mat4 matView = glm::lookAt( this->eye,
			this->target,
			this->Up )
			return matView;*/
			break;
		case cMouseCamera::eMode::FLY_CAMERA:
			//// Use same process as with drawing an object:
			//// Combine transform with rotation, and return that
			//glm::mat4 matCamView = glm::mat4( 1.0f );

			//glm::mat4 trans = glm::mat4x4( 1.0f );
			//trans = glm::translate( trans, this->eye );
			//matCamView = matCamView * trans;

			//// Like many things in GML, the conversion is done in the constructor
			//glm::mat4 postRotQuat = glm::mat4( this->qOrientation );
			//matCamView = matCamView * postRotQuat;

			//return matCamView;
			break;
		}

		return glm::mat4( 1.0f );
	}

	// Processes input received from any keyboard-like input system. 
	// Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard( Camera_Movement direction, float deltaTime )
	{
		float velocity = MovementSpeed * deltaTime;
		if( direction == FORWARD )
			Position += Front * velocity;
		else if( direction == BACKWARD )
			Position -= Front * velocity;
		else if( direction == LEFT )
			Position -= Right * velocity;
		else if( direction == RIGHT )
			Position += Right * velocity;
		else if( direction == UP )
			Position += Up * velocity;
		else if( direction == DOWN )
			Position -= Up * velocity;
	}

	// Processes input received from a mouse input system. 
	// Expects the offset value in both the x and y direction.
	void ProcessMouseMovement( float xoffset, float zoffset, GLboolean constrainPitch = true )
	{
		xoffset *= MouseSensitivity;
		zoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += zoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if( constrainPitch )
		{
			if( Pitch > 89.0f )
				Pitch = 89.0f;
			if( Pitch < -89.0f )
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Eular angles
		updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. 
	// Only requires input on the vertical wheel-axis
	void ProcessMouseScroll( float zoffset )
	{
		if( Zoom >= 1.0f && Zoom <= 45.0f )
			Zoom -= zoffset;
		if( Zoom <= 1.0f )
			Zoom = 1.0f;
		if( Zoom >= 45.0f )
			Zoom = 45.0f;
	}

	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos( glm::radians( Yaw ) ) * cos( glm::radians( Pitch ) );
		front.y = sin( glm::radians( Pitch ) );
		front.z = sin( glm::radians( Yaw ) ) * cos( glm::radians( Pitch ) );
		Front = glm::normalize( front );
		// Also re-calculate the Right and Up vector
		// Normalize the vectors, because their length gets closer to 0
		// the more you look up or down which results in slower movement.
		Right = glm::normalize( glm::cross( Front, WorldUp ) );
		Up = glm::normalize( glm::cross( Right, Front ) );
	}

private:

};
#endif