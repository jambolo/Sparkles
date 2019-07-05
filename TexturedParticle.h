#if !defined( SPARKLES_TEXTUREDPARTICLE_H_INCLUDED )
#define SPARKLES_TEXTUREDPARTICLE_H_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                     TexturedParticle.h

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/TexturedParticle.h#3 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "Particle.h"

#include "Glx/DisplayList.h"

namespace Sparkles
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class TexturedParticle : public Particle
{
public:
	TexturedParticle() {};
	TexturedParticle( BasicEmitter const * pEmitter,
					  float lifetime, float age,
					  Glx::Rgba const & color,
					  Vector3 const & position, Vector3 const & velocity,
					  float radius,
					  float orientation );
	virtual ~TexturedParticle();

	// Particle overrides

	// Initialize after using the default constructor
	void Initialize( float lifetime, float age,
					 Glx::Rgba const & color,
					 Vector3 const & position, Vector3 const & velocity,
					 float radius,
					 float orientation = 0.f );

	// Update the particle
	virtual bool Update( float dt );
	
	// Draw the particle
	virtual void Draw() const;

	// Get the particle's current radius
	float GetRadius() const						{ return m_Radius; }
	
	// Get the particle's current orientation
	float GetOrientation() const				{ return m_Orientation; }

	float				m_InitialRadius;		// Radius (distance from center to edge) at birth
	float				m_Radius;				// Current radius
						
	float				m_InitialOrientation;	// Orientation at birth
	float				m_Orientation;			// Current orientation
};



} // namespace Sparkles


#endif // !defined( SPARKLES_TEXTUREDPARTICLE_H_INCLUDED )
