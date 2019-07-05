#if !defined( SPARKLES_ENVIRONMENT_H_INCLUDED )
#define SPARKLES_ENVIRONMENT_H_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                     Environment.h

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/Environment.h#3 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "Math/Vector3.h"
#include "Math/Plane.h"

#include <vector>

namespace Sparkles
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class Environment
{
public:

	class BouncePlane
	{
	public:
		BouncePlane( Plane const & plane, float dampening )
			: m_Plane( plane ), m_Dampening( dampening )
		{
		}

		Plane	m_Plane;
		float	m_Dampening;
	};

	typedef Plane	ClipPlane;

	typedef std::vector< BouncePlane >	BouncePlaneList;
	typedef std::vector< ClipPlane >	ClipPlaneList;

	Environment( Vector3 const & gravity		= Vector3::Origin(),
				 Vector3 const & windVelocity	= Vector3::Origin(),
				 float airFriction				= 0.f,
				 Vector3 const & gustiness		= Vector3::Origin(),
				 BouncePlaneList const * pBPL	= 0,
				 ClipPlaneList const * pCPL		= 0 );
	virtual ~Environment();

	// Get/set gravity
	void					SetGravity( Vector3 const & gravity )			{ m_Gravity = gravity; }
	Vector3 const &		GetGravity() const								{ return m_Gravity; }
																			
	// Get/set wind velocity												
	void					SetWindVelocity( Vector3 const & wind )		{ m_WindVelocity = wind; }
	Vector3 const &		GetWindVelocity() const							{ return m_CurrentWindVelocity; }
																	
	// Get/set air friction											
	void					SetAirFriction( float airFriction )				{ m_AirFriction = airFriction; }
	float					GetAirFriction() const							{ return m_AirFriction; }
																	
	// Get/set gustiness											
	void					SetGustiness( Vector3 const & gustiness )		{ m_Gustiness = gustiness; }
	Vector3 const &		GetGustiness() const							{ return m_Gustiness; }

	// Get values associated with gravity and air friction
	Vector3 const &		GetTerminalVelocity() const						{ return m_TerminalVelocity; }
	float					GetEct1() const									{ return m_Ect1; }

	// Get/set bounce planes											
	void					SetBouncePlanes( BouncePlaneList const * pBPL )	{ m_BouncePlanes = pBPL; }
	BouncePlaneList	const *	GetBouncePlanes() const							{ return m_BouncePlanes; }

	// Get/set clip planes											
	void					SetClipPlanes( ClipPlaneList const * pCPL )		{ m_ClipPlanes = pCPL; }
	ClipPlaneList const *	GetClipPlanes() const							{ return m_ClipPlanes; }

	// Update
	void Update( float dt );

private:

	Vector3				m_Gravity;
	Vector3				m_WindVelocity;
	Vector3				m_Gustiness;
	Vector3				m_Gust;
	Vector3				m_CurrentWindVelocity;
	Vector3				m_TerminalVelocity;
	float					m_AirFriction;
	float					m_Ect1;
	BouncePlaneList	const *	m_BouncePlanes;
	ClipPlaneList const *	m_ClipPlanes;
};


} // namespace Sparkles


#endif // !defined( SPARKLES_ENVIRONMENT_H_INCLUDED )
