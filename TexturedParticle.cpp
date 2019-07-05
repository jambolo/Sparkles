/** @file *//********************************************************************************************************

                                                 TexturedParticle.cpp

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/TexturedParticle.cpp#3 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "PrecompiledHeaders.h"

#include "TexturedParticle.h"

#include "Emitter.h"
#include "Environment.h"
#include "Appearance.h"
#include "Math/Vector3.h"
#include "Math/FastMath.h"
#include "Math/Matrix33.h"

namespace Sparkles
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

TexturedParticle::TexturedParticle( BasicEmitter const * pEmitter,
									float lifetime, float age,
									Glx::Rgba const & color,
									Vector3 const & position, Vector3 const & velocity,
									float radius,
									float orientation/* = 0.f*/ )
	: Particle( pEmitter, lifetime, age, color, position, velocity ),
	m_InitialRadius( radius ),
	m_InitialOrientation( orientation )
{
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

TexturedParticle::~TexturedParticle()
{
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

void TexturedParticle::Initialize( float lifetime, float age,
								   Glx::Rgba const & color,
								   Vector3 const & position, Vector3 const & velocity,
								   float radius,
								   float orientation )
{
	Particle::Initialize( lifetime, age, color, position, velocity );

	m_InitialRadius			= radius;
	m_InitialOrientation	= orientation;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

bool TexturedParticle::Update( float dt )
{
	// Update base class

	bool const	reborn	= Particle::Update( dt );

	if ( reborn )
	{
		m_Radius		= m_InitialRadius;
		m_Orientation	= m_InitialOrientation;
		dt				= m_Age;
	}

	Appearance const * const	a	= m_pEmitter->GetAppearance();

	m_Radius		+= dt * a->GetRadiusDelta();
	m_Orientation	+= dt * a->GetRadialVelocity();

	return reborn;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

void TexturedParticle::Draw() const
{
	// Don't draw particles that haven't been born yet

	if ( m_Age < 0.f )
	{
		return;
	}

	float const			radius				= GetRadius();
	Glx::Rgba const		color				= GetColor();
	Vector3 const		position			= GetPosition();
	float const			orientation			= GetOrientation();
	Matrix33 const &	cameraOrientation	= m_pEmitter->GetAppearance()->GetCameraOrientation();

	float	s, c;
	Math::fsincos( orientation, &s, &c );

	// Rotated (and scaled) X and Y axes

	Vector3 const	rx	= ( cameraOrientation.GetX() * c + cameraOrientation.GetY() * s ) * radius;
	Vector3 const	ry	= ( cameraOrientation.GetY() * c - cameraOrientation.GetX() * s ) * radius;

	glColor4fv( color.m_C );

	glTexCoord2f( 0.f, 0.f );
	glVertex3fv( ( position - rx - ry ).m_V );

	glTexCoord2f( 1.f, 0.f );
	glVertex3fv( ( position + rx - ry ).m_V );

	glTexCoord2f( 1.f, 1.f );
	glVertex3fv( ( position + rx + ry ).m_V );

	glTexCoord2f( 0.f, 1.f );
	glVertex3fv( ( position - rx + ry ).m_V );
}


} // namespace Sparkles
