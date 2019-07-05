#if !defined( SPARKLES_EMITTER_INL_INCLUDED )
#define SPARKLES_EMITTER_INL_INCLUDED

#pragma once

/*********************************************************************************************************************

                                                     Emitter.inl

						                    Copyright 2002, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/Emitter.inl#6 $

	$NoKeywords: $

*********************************************************************************************************************/

#include "Emitter.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <gl/gl.h>

#include <memory>
#include <cassert>

#include "Glx/Enable.h"

#include "StreakParticle.h"
#include "TexturedParticle.h"
#include "Appearance.h"


namespace Sparkles
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

inline bool BasicEmitter::Enable( bool enabled )
{
	bool const	oldState	= m_Enabled;

	m_Enabled = enabled;

	return oldState;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

inline void BasicEmitter::Update( Vector3 const & position, Vector3 const & velocity )
{
	m_Position	= position;
	m_Velocity	= velocity;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

template< class T >
Emitter< T >::Emitter( EmitterVolume const * pVol,
					   Environment const * pEnv,
					   Appearance const * pApp,
					   int n )
	: BasicEmitter( new T[ n ], pVol, pEnv, pApp, n )
{
	if ( !GetParticles() ) throw std::bad_alloc();

	// Point the particles' emitter value here

	T *	const	paParticles	= GetParticles();

	for ( int i = 0; i < GetNumParticles(); i++ )
	{
		paParticles[ i ].Bind( this );
	}
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

template< class T >
Emitter< T >::Emitter( std::auto_ptr< T > pParticles,
					   EmitterVolume const * pVol,
					   Environment const * pEnv,
					   Appearance const * pApp,
					   int n )
	: BasicEmitter( pParticles.release(), pVol, pEnv, pApp, n )
{
	// Point the particles' emitter value here

	T *	const	paParticles	= GetParticles();

	for ( int i = 0; i < GetNumParticles(); i++ )
	{
		paParticles[ i ].Bind( this );
	}
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

template< class T >
Emitter< T >::~Emitter()
{
	delete[] GetParticles();
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

template< class T >
void Emitter< T >::Update( float dt )
{
	T *	const	paParticles	= GetParticles();

	for ( int i = 0; i < GetNumParticles(); i++ )
	{
		paParticles[ i ].Update( dt );
	}
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

template< class T >
void Emitter< T >::Update( float dt, Vector3 const & position, Vector3 const & velocity )
{
	// Update the emitter's position and velocity

	BasicEmitter::Update( position, velocity );

	// Update the emitter's particles

	Update( dt );
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

template< class T >
inline void Emitter< T >::Draw() const
{
	T const * const	paParticles	= GetParticles();

	for ( int i = 0; i < GetNumParticles(); i++ )
	{
		if ( paParticles[ i ].m_Age >= 0.f )
		{
			paParticles[ i ].Draw();
		}
	}
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

template<>
inline void Emitter< Particle >::Draw() const
{
//	GLfloat pointSizeRange[ 2 ]; 
//	GLfloat pointParameters[ 3 ];
//	GLint	viewportParameters[ 4 ];
//
//	glGetIntegerv( GL_VIEWPORT, viewportParameters );
//	glGetFloatv( GL_POINT_SIZE_RANGE, pointSizeRange ); 
//
//	pointParameters[ 0 ] = 0.f;
//	pointParameters[ 1 ] = 0.f;
//	pointParameters[ 2 ] = tanf( float( Math::ToRadians( s_pCamera->GetAngleOfView() ) ) * .5f ) * 2.f / viewportParameters[3];
//	pointParameters[ 2 ] *= pointParameters[ 2 ];
//
//	glPointParameterfvEXT( Glx::Extension::GL_DISTANCE_ATTENUATION_EXT, pointParameters );
//
//	glPointParameterfEXT( Glx::Extension::GL_POINT_SIZE_MIN_EXT, 0.f );
//	glPointParameterfEXT( Glx::Extension::GL_POINT_SIZE_MAX_EXT, 10000.f );
//
//	glPointSize( 1.f ); 
//
//	Glx::Enable( GL_POINT_SMOOTH );
//	glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );

	Glx::Disable( GL_TEXTURE_1D );
	Glx::Disable( GL_TEXTURE_2D );
	Glx::Disable( GL_LIGHTING );

	glBegin( GL_POINTS );

	{
		Particle const * const	paParticles	= GetParticles();

		for ( int i = 0; i < GetNumParticles(); i++ )
		{
			if ( paParticles[ i ].m_Age >= 0.f )
			{
				paParticles[ i ].Draw();
			}
		}
	}

	glEnd();
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

template<>
inline void Emitter< StreakParticle >::Draw() const
{
	glLineWidth( GetAppearance()->GetLineWidth() );
	Glx::Enable( GL_LINE_SMOOTH );
	glHint( GL_LINE_SMOOTH_HINT, GL_FASTEST );
	glShadeModel( GL_SMOOTH );
	Glx::Disable( GL_TEXTURE_1D );
	Glx::Disable( GL_TEXTURE_2D );
	Glx::Disable( GL_LIGHTING );


	glBegin( GL_LINES );

	{
		StreakParticle const * const	paParticles	= GetParticles();

		for ( int i = 0; i < GetNumParticles(); i++ )
		{
			if ( paParticles[ i ].m_Age >= 0.f )
			{
				paParticles[ i ].Draw();
			}
		}
	}

	glEnd();
}



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

template<>
inline void Emitter< TexturedParticle >::Draw() const
{
	glShadeModel( GL_FLAT );
	Glx::Enable( GL_TEXTURE_2D );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	Glx::Disable( GL_LIGHTING );

	assert( GetAppearance()->GetTexture() != 0 );

	GetAppearance()->GetTexture()->Apply();

	glBegin( GL_QUADS );

	{
		TexturedParticle const * const	paParticles	= GetParticles();

		for ( int i = 0; i < GetNumParticles(); i++ )
		{
			if ( paParticles[ i ].m_Age >= 0.f )
			{
				paParticles[ i ].Draw();
			}
		}
	}

	glEnd();
}



} // namespace Sparkles


#endif // !defined( SPARKLES_EMITTER_INL_INCLUDED )
