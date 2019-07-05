/** @file *//********************************************************************************************************

                                                  ParticleSystem.cpp

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/ParticleSystem.cpp#4 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "PrecompiledHeaders.h"

#include "ParticleSystem.h"

#include "Appearance.h"
#include "Configuration.h"
#include "Emitter.h"
#include "EmitterVolume.h"
#include "Environment.h"

#include "Glx/Camera.h"


namespace Sparkles
{


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

ParticleSystem::ParticleSystem()
{
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

ParticleSystem::ParticleSystem( Configuration const & c )
{
	Configuration::EnvironmentMap::const_iterator	pEnv;
	Configuration::AppearanceMap::const_iterator	pApp;
	Configuration::EmitterMap::const_iterator		pE;

	for ( pEnv = c.m_EnvironmentMap.begin(); pEnv != c.m_EnvironmentMap.end(); ++pEnv )
	{
		RegisterEnvironment( pEnv->second );
	}

	for ( pApp = c.m_AppearanceMap.begin(); pApp != c.m_AppearanceMap.end(); ++pApp )
	{
		RegisterAppearance( pApp->second );
	}

	for ( pE = c.m_EmitterMap.begin(); pE != c.m_EmitterMap.end(); ++pE )
	{
		RegisterEmitter( pE->second );
	}

}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

ParticleSystem::~ParticleSystem()
{
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/


void ParticleSystem::RegisterEmitter( BasicEmitter * pEmitter )
{
	m_Emitters.push_back( pEmitter );
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

bool ParticleSystem::UnregisterEmitter( BasicEmitter * pEmitter )
{
	EmitterList::iterator const	i	= std::find( m_Emitters.begin(), m_Emitters.end(), pEmitter );

	if ( i != m_Emitters.end() )
	{
		m_Emitters.erase( i );
	}

	return ( i != m_Emitters.end() );
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void ParticleSystem::RegisterAppearance( Appearance * pAppearance )
{
	m_Appearances.push_back( pAppearance );
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

bool ParticleSystem::UnregisterAppearance( Appearance * pAppearance )
{
	AppearanceList::iterator const	i	= std::find( m_Appearances.begin(), m_Appearances.end(), pAppearance );

	if ( i != m_Appearances.end() )
	{
		m_Appearances.erase( i );
	}

	return ( i != m_Appearances.end() );
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void ParticleSystem::RegisterEnvironment( Environment * pEnvironment )
{
	m_Environments.push_back( pEnvironment );
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

bool ParticleSystem::UnregisterEnvironment( Environment * pEnvironment )
{
	EnvironmentList::iterator const	i	= std::find( m_Environments.begin(), m_Environments.end(), pEnvironment );

	if ( i != m_Environments.end() )
	{
		m_Environments.erase( i );
	}

	return ( i != m_Environments.end() );
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void ParticleSystem::Update( float dt, Glx::Camera const & camera )
{
	// Update all the appearances

	for ( AppearanceList::iterator ppApp = m_Appearances.begin(); ppApp != m_Appearances.end(); ++ppApp )
	{
		Appearance * const	pAppearance	= *ppApp;	// Convenience

		pAppearance->Update( dt, camera );
	}

	// Update all the environments

	for ( EnvironmentList::iterator ppEnv = m_Environments.begin(); ppEnv != m_Environments.end(); ++ppEnv )
	{
		Environment * const	pEnvironment	= *ppEnv;	// Convenience

		pEnvironment->Update( dt );
	}

	// Update all the emitters

	for ( EmitterList::iterator ppEmitter = m_Emitters.begin(); ppEmitter != m_Emitters.end(); ++ppEmitter )
	{
		BasicEmitter * const	pEmitter	= *ppEmitter;	// Convenience

		if ( pEmitter && pEmitter->IsEnabled() )
		{
			pEmitter->Update( dt );
		}
	}
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void ParticleSystem::Draw() const
{
	// Test the Z-buffer but don't write to it. That means that obscured particles will not be drawn, but anything
	// drawn after will be drawn on top of them regardless of Z. Thus, particles should be drawn last (but before
	// the hud). The purpose of NOT writing to the Z-buffer is to prevent the transparent pixels in a particle from
	// obscuring anything behind them, as a result of not sorting the particles by Z.

	Glx::Enable( GL_DEPTH_TEST );
	glDepthMask( GL_FALSE );

	// For each emitter, draw all its particles

	for ( EmitterList::const_iterator i = m_Emitters.begin(); i != m_Emitters.end(); ++i )
	{
		BasicEmitter * const	pEmitter	= *i;	// Convenience

		if ( pEmitter && pEmitter->IsEnabled() )
		{
			pEmitter->Draw();
		}
	}
}



} // namespace Sparkles


