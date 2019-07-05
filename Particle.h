#if !defined( SPARKLES_PARTICLE_H_INCLUDED )
#define SPARKLES_PARTICLE_H_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                     Particle.h

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/Particle.h#3 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "Math/Vector3.h"
#include "Glx/Rgba.h"

namespace Sparkles
{

class BasicEmitter;


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class Particle
{
public:
	Particle() {};
	Particle( BasicEmitter const * pEmitter,
			  float lifetime, float age,
			  Glx::Rgba const & color,
			  Vector3 const & position, Vector3 const & velocity );
	virtual ~Particle();

	// Initialize after using the default constructor
	void Initialize( float lifetime, float age,
					 Glx::Rgba const & color,
					 Vector3 const & position, Vector3 const & velocity );

	// Bind to an emitter
	void Bind( BasicEmitter * pEmitter )	{ m_pEmitter = pEmitter; }

	// Update the particle. Returns true if the particle was reborn.
	virtual bool Update( float dt );

	// Draw the particle
	virtual void Draw() const;

	// Get the particle's current color
	Glx::Rgba GetColor() const				{ return m_Color; }

	// Get the particle's current position
	Vector3 GetPosition() const			{ return m_Position; }

	// Get the particle's current velocity
	Vector3 GetVelocity() const			{ return m_Velocity; }

	BasicEmitter const *		m_pEmitter;		// This particle's emitter

	// Age data

	float	m_Lifetime;					// Max age in seconds
	float	m_Age;						// Current age in seconds

	// Appearance data

	Glx::Rgba	m_InitialColor;			// Color at birth
	Glx::Rgba	m_Color;				// Current color

	// Motion data

	Vector3	m_InitialPosition;		// Position at birth relative to emitter
	Vector3	m_InitialVelocity;		// Velocity at birth relative to emitter

	Vector3	m_Position;				// Current position
	Vector3	m_Velocity;				// Current velocity
};


} // namespace Sparkles


#endif // !defined( SPARKLES_PARTICLE_H_INCLUDED )
