#if !defined( SPARKLES_SPHEREPARTICLE_H_INCLUDED )
#define SPARKLES_SPHEREPARTICLE_H_INCLUDED

#pragma once

/*****************************************************************************

                               SphereParticle.h

						Copyright 2002, John J. Bolton
	----------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/SphereParticle.h#3 $

	$NoKeywords: $

*****************************************************************************/

#include "Particle.h"

#include "Math/Quaternion.h"

namespace Sparkles
{



/****************************************************************************/
/*																			*/
/*																			*/
/****************************************************************************/

class SphereParticle : public Particle
{
public:
	SphereParticle() {};
	SphereParticle( BasicEmitter const * pEmitter,
			  float lifetime, float age,
			  float radius, float radiusDelta,
			  Glx::Rgba const & color,
			  Vector3 const & position, Vector3 const & velocity,
			  Quaternion const & orientation, Quaternion const & radialVelocity );
	virtual ~SphereParticle();

	// Particle overrides

	// Initialize after using the default constructor
	void Initialize( float lifetime, float age,
					 float radius, float radiusDelta,
					 Glx::Rgba const & color,
					 Vector3 const & position, Vector3 const & velocity,
					 Quaternion const & orientation, Quaternion const & radialVelocity );

	//	// Update the particle (dt = 0 allows for external control and non-linear effects)
	//	virtual void Update( float dt = 0.f );
	//
	// Draw the particle
	virtual void Draw() const;

	// Get the particle's current radius
	float GetRadius() const;

	// Get the particle's current orientation
	Quaternion GetOrientation() const;

	float		m_InitialRadius;		// Radius at birth
	float		m_RadiusDelta;			// Radius change

	Quaternion	m_InitialOrientation;	// Orientation at birth
	Quaternion	m_RadialVelocity;		// Radial velocity
};



} // namespace Sparkles


#endif // !defined( SPARKLES_SPHEREPARTICLE_H_INCLUDED )
