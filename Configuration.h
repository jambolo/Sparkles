#if !defined( SPARKLES_CONFIGURATION_H_INCLUDED )
#define SPARKLES_CONFIGURATION_H_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                   Configuration.h

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/Configuration.h#1 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "Environment.h"
#include "Emitter.h"
#include "Misc/Types.h"
#include <map>
#include <string>

struct IXMLDOMElement;
struct IXMLDOMNodeList;
struct IXMLDOMNode;
class Vector3;

namespace Sparkles
{

class Appearance;
class EmitterVolume;


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// A generic configuration for ParticleSystem
//
/// @note	The ParticleSystem using this configuration maintains references only. The data must be valid for the
///			lifetime of the ParticleSystem that references it.

class Configuration
{
public:

	/// Constructor
	Configuration()				{}

	/// Destructor
	virtual ~Configuration()	{}

	typedef std::map< std::string, BasicEmitter * >					EmitterMap;			///< A list of Emitters
	typedef std::map< std::string, EmitterVolume * >				EmitterVolumeMap;	///< A list of EmitterVolumes
	typedef std::map< std::string, Environment * >					EnvironmentMap;		///< A list of Environments
	typedef std::map< std::string, Appearance * >					AppearanceMap;		///< A list of Appearances
	typedef std::map< std::string, Glx::Material * >				MaterialMap;		///< A list of Materials
	typedef std::map< std::string, Environment::ClipPlaneList * >	ClipPlaneListMap;	///< A list of ClipPlaneLists
	typedef std::map< std::string, Environment::BouncePlaneList * >	BouncePlaneListMap;	///< A list of BouncePlaneLists

	EmitterMap			m_EmitterMap;						///< The list of Emitters, indexed by name
	EmitterVolumeMap	m_EmitterVolumeMap;					///< The list of EmitterVolumes, indexed by name
	EnvironmentMap		m_EnvironmentMap;					///< The list of Environments, indexed by name
	AppearanceMap		m_AppearanceMap;					///< The list of Appearances, indexed by name
	MaterialMap			m_MaterialMap;						///< The list of Materials, indexed by name
	ClipPlaneListMap	m_ClipPlaneListMap;					///< The list of ClipPlaneLists, indexed by name
	BouncePlaneListMap	m_BouncePlaneListMap;				///< The list of BouncePlaneLists, indexed by name
};


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// A configuration loaded from an XML file

class XmlConfiguration : public Configuration
{
public:

	/// Constructor
	XmlConfiguration( char const * sFilename );

	/// Destructor
	virtual ~XmlConfiguration();

private:

	/// @name XML support
	//@{
	static bool ProcessBouncePlane( IXMLDOMElement * pElement, uint32 context );
	static bool ProcessBouncePlaneList( IXMLDOMElement * pElement, uint32 context );
	static bool ProcessClipPlane( IXMLDOMElement * pElement, uint32 context );
	static bool ProcessClipPlaneList( IXMLDOMElement * pElement, uint32 context );
	static bool ProcessEnvironment( IXMLDOMElement * pElement, uint32 context );
	static bool ProcessAppearance(  IXMLDOMElement * pElement, uint32 context );
	static bool ProcessVolume( IXMLDOMElement * pElement, uint32 context );
	static bool ProcessEmitter( IXMLDOMElement * pElement, uint32 context );
	static void ProcessParticles( ParticleEmitter *		pEmitter,
								  IXMLDOMNodeList *		pNodeList,
								  int					numParticles,
								  Configuration const *	pConfiguration,
								  float					globalLifetime,
								  uint32				globalColor,
								  float					spread,
								  float					minSpeed,
								  float					maxSpeed );
	static void ProcessStreakParticles( StreakEmitter *			pEmitter,
										IXMLDOMNodeList *		pNodeList,
										int						numParticles,
										Configuration const *	pConfiguration,
										float					globalLifetime,
										uint32					globalColor,
										float					spread,
										float					minSpeed,
										float					maxSpeed );
	static void ProcessTexturedParticles( TexturedEmitter * 	pEmitter,
										  IXMLDOMNodeList * 	pNodeList,
										  int					numParticles,
										  Configuration const * pConfiguration,
										  float					globalLifetime,
										  uint32				globalColor,
										  float					spread,
										  float					minSpeed,
										  float					maxSpeed );
	static void ProcessSphereParticles( SphereEmitter *			pEmitter,
										IXMLDOMNodeList *		pNodeList,
										int						numParticles,
										Configuration const *	pConfiguration,
										float					globalLifetime,
										uint32					globalColor,
										float					spread,
										float					minSpeed,
										float					maxSpeed );
	static void ProcessParticle( IXMLDOMNode *	pNode,
								 float *		pLifetime,
								 float *		pAge,
								 uint32 *		pColor,
								 float *		pRadius,
								 float *		pOrientation,
								 Vector3 *		pVelocity,
								 Vector3 *		pPosition );
	//@}
};


} // namespace Sparkles


#endif // !defined( SPARKLES_CONFIGURATION_H_INCLUDED )
