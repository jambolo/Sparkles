/** @file *//********************************************************************************************************

                                                     Particle.cpp

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/Particle.cpp#3 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "PrecompiledHeaders.h"

#include "Particle.h"

#include "Emitter.h"
#include "Environment.h"
#include "Appearance.h"

#include "Misc/Max.h"

#include "Math/Plane.h"
#include "Math/Point.h"
#include "Math/FastMath.h"

namespace Sparkles
{



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

Particle::Particle( BasicEmitter const * pEmitter,
					float lifetime, float age,
					Glx::Rgba const & color,
					Vector3 const & position, Vector3 const & velocity )
	: m_pEmitter( pEmitter ),
	m_Lifetime( lifetime ),
	m_Age( age ),
	m_InitialColor( color ),
	m_InitialPosition( position ),
	m_InitialVelocity( velocity )
{
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

Particle::~Particle()
{
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

void Particle::Initialize( float lifetime, float age,
						   Glx::Rgba const & color,
						   Vector3 const & position, Vector3 const & velocity )
{
	m_Lifetime			= lifetime;
	m_Age				= age;
	m_InitialColor		= color;
	m_InitialPosition	= position;
	m_InitialVelocity	= velocity;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

bool Particle::Update( float dt )
{
	bool	reborn	= false;

	m_Age += dt;

	// If the particle has not been born yet then do nothing

	if ( m_Age < 0.f )
	{
		return false;
	}

	// See if the particle is (re)born this frame

	if ( m_Age < dt )
	{
		reborn = true;
	}
	else if ( m_Age >= m_Lifetime )
	{
		m_Age -= m_Lifetime;
		reborn = true;
	}

	// If (re)born, then reset to initial values and adjust dt

	if ( reborn )
	{
		m_Velocity	= m_pEmitter->GetCurrentVelocity() + m_InitialVelocity;
		m_Position	= m_pEmitter->GetCurrentPosition() + m_InitialPosition;
		m_Color		= m_InitialColor;
		dt			= m_Age;
	}

	Environment const * const	pE	= m_pEmitter->GetEnvironment();
	Appearance const * const	pA	= m_pEmitter->GetAppearance();

	// Update velocity and position

	Vector3			dv;
	Vector3			ds;
	float const		c	= pE->GetAirFriction();
	Vector3 const &	g	= pE->GetGravity();

	if ( Math::IsCloseToZero( c ) )
	{
		dv	= g * dt;
		ds	= ( m_Velocity + dv * .5f ) * dt;
	}
	else
	{
		Vector3 const &	terminalVelocity	= pE->GetTerminalVelocity();
		float const		ect1				= pE->GetEct1();;

		dv = ( terminalVelocity - m_Velocity ) * ect1;
		ds = terminalVelocity * dt - dv * Math::frcp( c );
	}

	m_Velocity += dv;
	m_Position += ds;

	// Check for collision with clip planes

	Environment::ClipPlaneList const *	pClipPlanes	= pE->GetClipPlanes();

	if ( pClipPlanes != 0 )
	{
		for ( Environment::ClipPlaneList::const_iterator pC	= pClipPlanes->begin(); pC != pClipPlanes->end(); ++pC )
		{
			if ( pC->IsBehind( m_Position ) )
			{
				m_Age -= m_Lifetime;
				return reborn;
			}
		}
	}

	// Check for collision with bounce planes

	Environment::BouncePlaneList const *	pBouncePlanes	= pE->GetBouncePlanes();

	if ( pBouncePlanes != 0 )
	{
		for ( Environment::BouncePlaneList::const_iterator pB = pBouncePlanes->begin(); pB != pBouncePlanes->end(); ++pB )
		{
			if ( pB->m_Plane.IsBehind( m_Position ) )
			{
				float const	f	= 1.f + pB->m_Dampening;
				m_Velocity -=  pB->m_Plane.m_N * ( f * Dot( pB->m_Plane.m_N, m_Velocity ) );
				m_Position -=  pB->m_Plane.m_N * ( f * Distance( pB->m_Plane, Point( m_Position ) ) );
			}
		}
	}

	// Update color

	m_Color.m_R	= limit( 0.f, m_Color.m_R + pA->GetColorDelta().m_R * dt, 1.f );
	m_Color.m_G	= limit( 0.f, m_Color.m_G + pA->GetColorDelta().m_G * dt, 1.f );
	m_Color.m_B	= limit( 0.f, m_Color.m_B + pA->GetColorDelta().m_B * dt, 1.f );
	m_Color.m_A	= limit( 0.f, m_Color.m_A + pA->GetColorDelta().m_A * dt, 1.f );

	return reborn;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

void Particle::Draw() const
{
	// Don't draw particles that haven't been born yet

	if ( m_Age < 0.f )
	{
		return;
	}

	Glx::Rgba const &	color		= GetColor();
	Vector3 const &	position	= GetPosition();

	glColor4fv( color.m_C );
	glVertex3fv( position.m_V );
}


///********************************************************************************************************************/
///*																													*/
///*																													*/
///********************************************************************************************************************/
//
//Glx::Rgba Particle::GetColor() const
//{
//	float const	r	= limit( 0., m_InitialColor.m_R + m_ColorDelta.m_R * m_Age, 1. );
//	float const	g	= limit( 0., m_InitialColor.m_G + m_ColorDelta.m_G * m_Age, 1. );
//	float const	b	= limit( 0., m_InitialColor.m_B + m_ColorDelta.m_B * m_Age, 1. );
//	float const	a	= limit( 0., m_InitialColor.m_A + m_ColorDelta.m_A * m_Age, 1. );
//
//	return Glx::Rgba( r, g, b, a );
//}
//
//
///********************************************************************************************************************/
///*																													*/
///*																													*/
///********************************************************************************************************************/
//
//Vector3 Particle::GetPosition() const
//{
//	Environment const &	e	= *m_pEmitter->GetEnvironment();
//
//	if ( Math::IsCloseToZero( e.GetAirFriction() ) )
//	{
//		Vector3	position	= e.GetGravity();
//
//		position *= ( .5f * m_Age );
//		position += m_AbsoluteInitialVelocity;
//		position *= m_Age;
//		position += m_AbsoluteInitialPosition;
//
//		return position;
//
////		return m_AbsoluteInitialPosition
////			   + ( m_AbsoluteInitialVelocity
////				   + e.GetGravity() * ( .5f * m_Age )
////				 ) * m_Age;
//	}
//	else
//	{
//		// How this was derived:
//		// Acceleration due to air friction = -c * ( v - w ), so
//		//
//		//		ds2/dt2 = dv/dt = g - c * ( v - w )
//		//
//		// also note that "terminal velocity" vT = g/c + w:
//		//
//		//		ds2/dt2 = dv/dt = c * vT - cv
//		//
//		//		ds/dt = v = vT - e**( -c * t ) * e**( -c * D ) / c
//		//
//		// Assuming v = v0 at t = 0 ==> e**( -c * D ) / c = ( vT - v0 ):
//		//
//		//		ds/dt = v = vT - ( vT - v0 ) * e**( -c * t )
//		//
//		//		s = vT * t + ( vT - v0 ) / c * e**( -c * t ) + E
//		//
//		// Assuming s = s0 at t = 0 ==> E = s0 - ( vT - v0 ) / c:
//		//
//		//		s = s0 + vT * t - ( vT - v0 ) * ( 1 - e**( -c * t ) ) / c
//		//
//		// c <== e.GetAirFriction(), w <== e.GetWindVelocity(), t <== m_Age,
//		// g <== e.GetGravity(), v0 <== m_AbsoluteInitialVelocity,
//		// s0 <== m_AbsoluteInitialPosition
//		//
//
//		double const	ect1c	= ( 1. - exp( -e.GetAirFriction() * m_Age ) ) / e.GetAirFriction();
//
//		return m_AbsoluteInitialPosition
//			   + m_TerminalVelocity * m_Age
//			   - ( m_TerminalVelocity - m_AbsoluteInitialVelocity ) * ect1c;
//
//	}
//}
//
///********************************************************************************************************************/
///*																													*/
///*																													*/
///********************************************************************************************************************/
//
//Vector3 Particle::GetVelocity() const
//{
//	Environment const &	e	= *m_pEmitter->GetEnvironment();
//
//	if ( Math::IsCloseToZero( e.GetAirFriction() ) )
//	{
//		return m_AbsoluteInitialVelocity + e.GetGravity() * m_Age;
//	}
//	else
//	{
//		// How this was derived:
//		// Acceleration due to air friction = -c * ( v - w ), so
//		//
//		//		dv/dt = g - c * ( v - w )
//		//
//		// also note that "terminal velocity" vT = g/c + w:
//		//
//		//		dv/dt = c * vT - cv
//		//
//		//		v = vT - e**( -c * t ) * e**( -c * D ) / c
//		//
//		// Assuming v = v0 at t = 0, e**( -c * D ) / c = ( vT - v0 ):
//		//
//		//		v = vT - ( vT - v0 ) * e**( -c * t )
//		//
//		// c <== e.GetAirFriction(), w <== e.GetWindVelocity(), t <== m_Age,
//		// g <== e.GetGravity(), v0 <== m_AbsoluteInitialVelocity,
//
//		float const		ect		= exp( -e.GetAirFriction() * m_Age );
//
//		return m_TerminalVelocity - ( m_TerminalVelocity - m_AbsoluteInitialVelocity ) * ect;
//	}
//}



} // namespace Sparkles
