/** @file *//********************************************************************************************************

                                                     Emitter.cpp

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/Emitter.cpp#5 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "PrecompiledHeaders.h"

#include "Emitter.h"

#include "Math/Vector3.h"

#include "Particle.h"

namespace Sparkles
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

BasicEmitter::BasicEmitter( Particle * paParticles,
							EmitterVolume const * pVol,
							Environment const * pEnv,
							Appearance const * pApp,
							int n )
	: m_paParticles( paParticles ),
	m_pEmitterVolume( pVol ),
	m_pEnvironment( pEnv ),
	m_pAppearance( pApp ),
	m_NumParticles( n ),
	m_Enabled( true ),
	m_Position( Vector3::Origin() ),
	m_Velocity( Vector3::Origin() )
{
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

BasicEmitter::~BasicEmitter()
{
}



} // namespace Sparkles
