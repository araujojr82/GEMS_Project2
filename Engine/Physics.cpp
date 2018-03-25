#include "Physics.h"
#include "cGameObject.h"
#include "cTriangle.h"

#include <glm/glm.hpp>		// glm::distance

bool PenetrationTestSphereSphere( cGameObject* pA, cGameObject* pB )
{
	// If the distance between the two sphere is LT the sum or the radii,
	//	they are touching or penetrating

	float totalRadii = pA->radius + pB->radius;

	// The Pythagorean distance 
	float distance = glm::distance( pA->position, pB->position );

	if ( distance <= totalRadii )
	{
		return true;
	}

	return false;
}

glm::vec3 sTriAABB::ClosestPtPointTriangle( glm::vec3 p )
{
	return this->ClosestPtPointTriangle( p, this->verts[0], this->verts[1], this->verts[2] );
}

glm::vec3 sTriAABB::ClosestPtPointTriangle( glm::vec3 p, glm::vec3 a,
	glm::vec3 b, glm::vec3 c )
{
	// Check if P in vertex region outside A
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 ap = p - a;
	float d1 = glm::dot( ab, ap );
	float d2 = glm::dot( ac, ap );
	if( d1 <= 0.0f && d2 <= 0.0f ) return a; // barycentric coordinates (1,0,0)

											 // Check if P in vertex region outside B
	glm::vec3 bp = p - b;
	float d3 = glm::dot( ab, bp );
	float d4 = glm::dot( ac, bp );
	if( d3 >= 0.0f && d4 <= d3 ) return b; // barycentric coordinates (0,1,0)

										   // Check if P in edge region of AB, if so return projection of P onto AB
	float vc = d1*d4 - d3*d2;
	if( vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f ) {
		float v = d1 / ( d1 - d3 );
		return a + v * ab; // barycentric coordinates (1-v,v,0)
	}

	// Check if P in vertex region outside C
	glm::vec3 cp = p - c;
	float d5 = glm::dot( ab, cp );
	float d6 = glm::dot( ac, cp );
	if( d6 >= 0.0f && d5 <= d6 ) return c; // barycentric coordinates (0,0,1)

										   // Check if P in edge region of AC, if so return projection of P onto AC
	float vb = d5*d2 - d1*d6;
	if( vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f ) {
		float w = d2 / ( d2 - d6 );
		return a + w * ac; // barycentric coordinates (1-w,0,w)
	}

	// Check if P in edge region of BC, if so return projection of P onto BC
	float va = d3*d6 - d5*d4;
	if( va <= 0.0f && ( d4 - d3 ) >= 0.0f && ( d5 - d6 ) >= 0.0f ) {
		float w = ( d4 - d3 ) / ( ( d4 - d3 ) + ( d5 - d6 ) );
		return b + w * ( c - b ); // barycentric coordinates (0,1-w,w)
	}

	// P inside face region. Compute Q through its barycentric coordinates (u,v,w)
	float denom = 1.0f / ( va + vb + vc );
	float v = vb * denom;
	float w = vc * denom;
	return a + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w
}

void bounceSphereAgainstPlane( cGameObject* pA, cGameObject* pB, glm::vec3 tNormal )
{
	// Relative velocity
	glm::vec3 relativeVel = pB->vel - pA->vel;

	if( tNormal.x > 0.0f ) tNormal.x = 1.0f;
	if( tNormal.y > 0.0f ) tNormal.y = 1.0f;
	if( tNormal.z > 0.0f ) tNormal.z = 1.0f;

	if( tNormal.x < 0.0f ) tNormal.x = -1.0f;
	if( tNormal.y < 0.0f ) tNormal.y = -1.0f;
	if( tNormal.z < 0.0f ) tNormal.z = -1.0f;

	//if( tNormal.x > 1.0f ) tNormal.x = -1.0f;
	//if( tNormal.y > 1.0f ) tNormal.y = -1.0f;
	//if( tNormal.z > 1.0f ) tNormal.z = -1.0f;

	//if( tNormal.x < -1.0f ) tNormal.x = 1.0f;
	//if( tNormal.y < -1.0f ) tNormal.y = 1.0f;
	//if( tNormal.z < -1.0f ) tNormal.z = 1.0f;

	//tNormal *= glm::vec3( -1.0f );

	glm::normalize( tNormal );

	glm::vec3 result = glm::vec3( glm::dot( relativeVel.x, tNormal.x ),
								  glm::dot( relativeVel.y, tNormal.y ),
								  glm::dot( relativeVel.z, tNormal.z ) );

	//// Moving away from each other? Do nothing!
	//if( result.x > 0.0f && 
	//	result.y > 0.0f && 
	//	result.z > 0.0f ) {
	//	return;
	//}

	if( result.x > 0.0f ) pA->vel.x = 0.0f;
	if( result.y > 0.0f ) pA->vel.y = 0.0f;
	if( result.z > 0.0f ) pA->vel.z = 0.0f;

}

float calculateMovAngle( glm::vec3 vel )
{
	float movAngle = 0.0f;

	// Calculate the moving angle using the velocity
	movAngle = atan2( vel.x, vel.z );

	// Convert it from radians to degrees
	movAngle = ( movAngle * 180 ) / M_PI;

	return movAngle;
}

glm::vec3 returnNormal( glm::vec3 vertex[3] ){
	glm::vec3 vertA = vertex[0];
	glm::vec3 vertB = vertex[1];
	glm::vec3 vertC = vertex[2];

	glm::vec3 theNormal = glm::vec3( 0.0f );

	glm::vec3 vectorU = ( vertB - vertA );
	glm::vec3 vectorV = ( vertC - vertA );

	theNormal.x = ( vectorU.y * vectorV.z ) - ( vectorU.z * vectorV.y );
	theNormal.y = ( vectorU.z * vectorV.x ) - ( vectorU.x * vectorV.z );
	theNormal.z = ( vectorU.x * vectorV.y ) - ( vectorU.y * vectorV.x );

	return theNormal;
}

glm::vec3 returnNormal2( glm::vec3 vertex[3] ){
	glm::vec3 vertA = vertex[0];
	glm::vec3 vertB = vertex[1];
	glm::vec3 vertC = vertex[2];

	glm::vec3 theNormal = glm::vec3( 0.0f );

	glm::vec3 vectorU = ( vertB - vertA );
	glm::vec3 vectorV = ( vertC - vertA );

	//theNormal.x = ( vectorU.y * vectorV.z ) - ( vectorU.z * vectorV.y );
	//theNormal.y = ( vectorU.z * vectorV.x ) - ( vectorU.x * vectorV.z );
	//theNormal.z = ( vectorU.x * vectorV.y ) - ( vectorU.y * vectorV.x );

	theNormal = glm::normalize( glm::cross( vectorU, vectorV ) );
	
	return theNormal;

}


std::vector< pointTriangles > findClosestPointsOfAABB( glm::vec3 thePoint, cAABBv2* theAABB )
{
	std::vector< pointTriangles > newVecPoints;

	cAABBv2* theCurAABB = theAABB;

	int numberOfTriangles = theCurAABB->vecTriangles.size();

	//std::fill( g_vecPoints.begin(), g_vecPoints.end(), glm::vec3( NULL ) );
	newVecPoints.clear();

	for( int triIndex = 0; triIndex != numberOfTriangles; triIndex++ )
	{
		// Get reference object for current triangle
		// (so the line below isn't huge long...)
		sTriAABB curTriangle = theCurAABB->vecTriangles[triIndex];

		//cPhysTriangle& curTriangle = theAABB.vecTriangles[triIndex];

		pointTriangles theClosestPoint;
		theClosestPoint.point = curTriangle.ClosestPtPointTriangle( thePoint );
		theClosestPoint.triangle = curTriangle;

		newVecPoints.push_back( theClosestPoint );

		//DrawDebugSphere( theClosestPoint.point, glm::vec4( 1, 1, 1, 1 ), 0.2f );
	}

	//collisionThingy:
	//	- Which objects collided (sphere - triangle/sphere - sphere)
	//	- Where, the speed, etc.


	//	vecMyColisions.push_Back( collisionThingy );
	// Calc response...
	//		glm::reflect( 

	return newVecPoints;
}