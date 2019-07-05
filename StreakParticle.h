#if !defined( SPARKLES_STREAKPARTICLE_H_INCLUDED )
#define SPARKLES_STREAKPARTICLE_H_INCLUDED

#pragma once

/*********************************************************************************************************************

                                                      StreakParticle.h

						                    Copyright 2002, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/StreakParticle.h#3 $

	$NoKeywords: $

*********************************************************************************************************************/


#include "Particle.h"

#include "Math/Vector3.h"
#include "Glx/Rgba.h"

namespace Sparkles
{

class BasicEmitter;



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class StreakParticle : public Particle
{
public:
	StreakParticle() {};
	StreakParticle( BasicEmitter const * pEmitter,
				  float lifetime, float age,
				  Glx::Rgba const & color,
				  Vector3 const & position, Vector3 const & velocity );
	virtual ~StreakParticle();

	// Draw the particle
	virtual void Draw() const;
};


} // namespace Sparkles


#endif // !defined( SPARKLES_STREAKPARTICLE_H_INCLUDED )
