/** @file *//********************************************************************************************************

                                                   Environment.cpp

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/Environment.cpp#5 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "PrecompiledHeaders.h"

#include "Environment.h"

#include "Misc/Random.h"
#include "Math/FastMath.h"


namespace Sparkles
{



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

Environment::Environment( Vector3 const & gravity/*			= Vector3::Origin()*/,
						  Vector3 const & windVelocity/*	= Vector3::Origin()*/,
						  float airFriction/*				= 0.f*/,
						  Vector3 const & gustiness/*		= Vector3::Origin()*/,
						  BouncePlaneList const * pBPL/*	= 0*/,
						  ClipPlaneList const * pCPL/*		= 0*/ )
	: m_Gravity( gravity ),
	m_WindVelocity( windVelocity ),
	m_AirFriction( airFriction ),
	m_Gustiness( gustiness ),
	m_BouncePlanes( pBPL ),
	m_ClipPlanes( pCPL ),
	m_Gust( Vector3::Origin() ),
	m_CurrentWindVelocity( windVelocity )
{
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

Environment::~Environment()
{
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

void Environment::Update( float dt )
{
	m_CurrentWindVelocity = m_WindVelocity;

	if ( !Math::IsCloseToZero( m_Gustiness.Length2() ) )
	{
		RandomFloat	rng( timeGetTime() );

		m_Gust.m_X += rng.Get( float( Math::TWO_PI ) ) * dt;
		m_Gust.m_Y += rng.Get( float( Math::TWO_PI ) ) * dt;

		float	cx, sx, cy, sy;

		Math::fsincos( m_Gust.m_X, &sx, &cx );
		Math::fsincos( m_Gust.m_Y, &sy, &cy );

		m_CurrentWindVelocity += Vector3( cx * sy * m_Gustiness.m_X,
										  cy      * m_Gustiness.m_Y,
										  sx * sy * m_Gustiness.m_Z );
	}

	if ( !Math::IsCloseToZero( m_AirFriction ) )
	{
		m_TerminalVelocity	= m_CurrentWindVelocity + m_Gravity * Math::frcp( m_AirFriction );
		m_Ect1				= 1.f - expf( -m_AirFriction * dt );
	}
	else
	{
		m_TerminalVelocity	= Vector3::Origin();
		m_Ect1				= 0.f;
	}
}


} // namespace Sparkles