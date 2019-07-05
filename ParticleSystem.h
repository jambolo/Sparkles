#if !defined( SPARKLES_PARTICLESYSTEM_H_INCLUDED )
#define SPARKLES_PARTICLESYSTEM_H_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                   ParticleSystem.h

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/ParticleSystem.h#3 $

	$NoKeywords: $

 ********************************************************************************************************************/


#include "Environment.h"
#include "Glx/Camera.h"
#include "Misc/Types.h"
#include <list>

namespace Sparkles
{

class BasicEmitter;
class Appearance;
class EmitterVolume;
class Configuration;


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class ParticleSystem
{
public:

	/// Default constructor
	ParticleSystem();

	/// Constructor
	ParticleSystem( Configuration const & c );

	/// Destructor
	~ParticleSystem();

	// Register/unregister an emitter (Unregister returns true if the emitter was found)
	void RegisterEmitter( BasicEmitter * pEmitter );
	bool UnregisterEmitter( BasicEmitter * pEmitter );

	// Register/unregister an appearance (Unregister returns true if the appearance was found)
	void RegisterAppearance( Appearance * pAppearance );
	bool UnregisterAppearance( Appearance * pAppearance );

	// Register/unregister an environment (Unregister returns true if the environment was found)
	void RegisterEnvironment( Environment * pEnvironment );
	bool UnregisterEnvironment( Environment * pEnvironment );

	// Update the system
	void	Update( float dt, Glx::Camera const & camera );

	// Draw all particles for all the emitters
	void	Draw() const;

private:

	typedef std::list< BasicEmitter * >	EmitterList;
	EmitterList	m_Emitters;																// Active emitters

	typedef std::list< Environment * >	EnvironmentList;
	EnvironmentList	m_Environments;														// Active environments

	typedef std::list< Appearance * >	AppearanceList;
	AppearanceList	m_Appearances;														// Active appearances
};



} // namespace Sparkles



#endif // !defined( SPARKLES_PARTICLESYSTEM_H_INCLUDED )
