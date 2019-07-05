/** @file *//********************************************************************************************************

                                                  SphereParticle.cpp

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/SphereParticle.cpp#3 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "PrecompiledHeaders.h"

#include "SphereParticle.h"

#include "Emitter.h"
#include "Environment.h"
#include "Appearance.h"

namespace Sparkles
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

SphereParticle::SphereParticle( BasicEmitter const * pEmitter,
			  float lifetime, float age,
			  float radius, float radiusDelta,
			  Glx::Rgba const & color,
			  Vector3 const & position, Vector3 const & velocity,
			  Quaternion const & orientation, Quaternion const & radialVelocity )
	: Particle( pEmitter,
				lifetime, age,
				color,
				position, velocity ),
	m_InitialOrientation( orientation ),
	m_RadialVelocity( radialVelocity )
{
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

SphereParticle::~SphereParticle()
{
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

void SphereParticle::Initialize( float lifetime, float age,
								 float radius, float radiusDelta,
								 Glx::Rgba const & color,
								 Vector3 const & position,
								 Vector3 const & velocity,
								 Quaternion const & orientation,
								 Quaternion const & radialVelocity )
{
	Particle::Initialize( lifetime, age, color, position, velocity );

	m_InitialOrientation	= orientation;
	m_RadialVelocity		= radialVelocity;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

void SphereParticle::Draw() const
{
	// Don't draw particles that haven't been born yet

	if ( m_Age < 0.f )
	{
		return;
	}

	Appearance const &	a	= *m_pEmitter->GetAppearance();
	Environment const &	e	= *m_pEmitter->GetEnvironment();

	float const			radius		= GetRadius();
	Glx::Rgba const		color		= GetColor();
	Vector3 const		velocity	= GetVelocity();
	Vector3 const		position	= GetPosition();
	Quaternion const	orientation = GetOrientation();

}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

float SphereParticle::GetRadius() const
{
	return m_InitialRadius + m_RadiusDelta * m_Age;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

Quaternion SphereParticle::GetOrientation() const
{
	return m_InitialOrientation * m_RadialVelocity.Pow( m_Age );
}




} // namespace Sparkles
