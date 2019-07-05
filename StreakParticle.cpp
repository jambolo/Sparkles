/*****************************************************************************

                              StreakParticle.cpp

						Copyright 2001, John J. Bolton
	----------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/StreakParticle.cpp#3 $

	$NoKeywords: $

*****************************************************************************/

#include "PrecompiledHeaders.h"

#include "StreakParticle.h"

namespace Sparkles
{



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

StreakParticle::StreakParticle( BasicEmitter const * pEmitter,
					float lifetime, float age,
					Glx::Rgba const & color,
					Vector3 const & position, Vector3 const & velocity )
	: Particle( pEmitter, lifetime, age, color, position, velocity )
{
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

StreakParticle::~StreakParticle()
{
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

void StreakParticle::Draw() const
{
	// Don't draw particles that haven't been born yet

	if ( m_Age < 0.f )
	{
		return;
	}

	Glx::Rgba const &	color		= GetColor();
	Vector3 const &	position	= GetPosition();

	glColor4f( color.m_R, color.m_G, color.m_B, 0.f );
	glVertex3fv( ( position - m_Velocity * .2f ).m_V  );

	glColor4fv( color.m_C );
	glVertex3fv( position.m_V );
}



} // namespace Sparkles
