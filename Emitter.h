/** @file *//********************************************************************************************************

                                                      Emitter.h

						                    Copyright 2004, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/Emitter.h#8 $

	$NoKeywords: $

 ********************************************************************************************************************/

#pragma once

#include "Particle.h"
#include "SphereParticle.h"
#include "StreakParticle.h"
#include "TexturedParticle.h"
#include "Misc/Assert.h"
#include <memory>
#include <boost/noncopyable.hpp>


namespace Sparkles
{

class EmitterVolume;
class Appearance;
class Environment;


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

class BasicEmitter : public boost::noncopyable
{
public:
	BasicEmitter( Particle * pParticles,
				  EmitterVolume const * pVol,
				  Environment const * pEnv,
				  Appearance const * pApp,
				  int n );

	virtual ~BasicEmitter();

	Particle *				GetParticles()				{ return m_paParticles; }
	Particle const *		GetParticles() const		{ return m_paParticles; }
	EmitterVolume const *	GetEmitterVolume() const	{ return m_pEmitterVolume; }
	Appearance const *		GetAppearance() const		{ return m_pAppearance; }
	Environment const *		GetEnvironment() const		{ return m_pEnvironment; }
	int						GetNumParticles() const		{ return m_NumParticles; }
	Vector3 const &			GetCurrentPosition() const	{ return m_Position; }
	Vector3 const &			GetCurrentVelocity() const	{ return m_Velocity; }
	bool					IsEnabled() const			{ return m_Enabled; }

	// Enable/Disable
	bool			Enable( bool enable );

	// Update the emitter's position and velocity
	void			Update( Vector3 const & position, Vector3 const & velocity );

	// Update the emitter's particles
	virtual void	Update( float dt ) = 0;

	// Update the emitter and its particles
	virtual void	Update( float dt, Vector3 const & position, Vector3 const & velocity ) = 0;

	// Draw all the emitter's particles
	virtual void	Draw() const = 0;

	/// Returns the address of an individual particle
	virtual Particle *			GetParticle( int i )		= 0;
	virtual Particle const *	GetParticle( int i ) const	= 0;

private:

	// Particle data

	Particle *				m_paParticles;			// Particle data
	EmitterVolume const *	m_pEmitterVolume;		// Emitter shape
	Appearance const *		m_pAppearance;			// Common appearance parameters
	Environment const *		m_pEnvironment;			// Common motion parameters
	int						m_NumParticles;			// Number of particles

	// Emitter state

	bool					m_Enabled;				// Enabled or not
	Vector3					m_Position;				// Current position
	Vector3					m_Velocity;				// Current velocity
};




/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

template < class T >
class Emitter : public BasicEmitter
{
public:

	typedef T	ParticleType;

	Emitter( EmitterVolume const * pVol,
			 Environment const * pEnv,
			 Appearance const * pApp,
			 int n );
	Emitter( std::auto_ptr< T > pParticles,
			 EmitterVolume const * pVol,
			 Environment const * pEnv,
			 Appearance const * pApp,
			 int n );

	virtual ~Emitter();

	// Return a pointer to the particle array
	T *			GetParticles()			{ return static_cast< T * >( BasicEmitter::GetParticles() ); }
	T const *	GetParticles() const	{ return static_cast< T const * >( BasicEmitter::GetParticles() ); }

	// Return a pointer to a particular particle
	T *			GetParticle( int i )		{ assert_limits( 0, i, GetNumParticles()-1 ); return GetParticles() + i; }
	T const *	GetParticle( int i ) const	{ assert_limits( 0, i, GetNumParticles()-1 ); return GetParticles() + i; }

	// Update the emitter's particles
	virtual void Update( float dt );

	// Update the emitter and its particles
	virtual void Update( float dt, Vector3 const & position, Vector3 const & velocity );

	// Draw all the emitter's particles
	virtual void Draw() const;
};


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

/// Point particle emitter
typedef Emitter< Particle >			ParticleEmitter;

/// Textured particle emitter
typedef Emitter< TexturedParticle >	TexturedEmitter;

/// Sphere particle emitter
typedef Emitter< SphereParticle >	SphereEmitter;

/// Streak (line) particle emitter
typedef Emitter< StreakParticle >	StreakEmitter;


} // namespace Sparkles


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

// Inline functions

#include "Emitter.inl"
