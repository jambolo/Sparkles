/** @file *//********************************************************************************************************

                                                  Configuration.cpp

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/Configuration.cpp#5 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "PrecompiledHeaders.h"

#include "Configuration.h"

#include "Appearance.h"
#include "Emitter.h"
#include "EmitterVolume.h"
#include "Environment.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "SphereParticle.h"
#include "StreakParticle.h"
#include "TexturedParticle.h"

#include "Dxx/Dxx.h"
#include "Math/Quaternion.h"
#include "Math/Vector3.h"
#include "Misc/Trace.h"
#include "Misc/Exceptions.h"
#include "Msxmlx/Msxmlx.h"
#include "TgaFile/TgaFile.h"

#include <atlbase.h>
#include <atlcomcli.h>
#include <msxml2.h>

#include <sstream>


namespace
{



/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Vector3 GetVector3SubElement( IXMLDOMElement * pElement, char const * sName, Vector3 vDefault )
{
	HRESULT							hr;
	CComPtr< IXMLDOMNamedNodeMap >	pAttributes;

	hr = Msxmlx::GetSubElementAttributes( pElement, sName, &pAttributes );

	if ( pAttributes )
	{
		Vector3		v;

		v.m_X = Msxmlx::GetFloatAttribute( pAttributes, "x", vDefault.m_X );
		v.m_Y = Msxmlx::GetFloatAttribute( pAttributes, "y", vDefault.m_Y );
		v.m_Z = Msxmlx::GetFloatAttribute( pAttributes, "z", vDefault.m_Z );

		return v;
	}
	else
	{
		return vDefault;
	}
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Glx::Rgba GetRgbaSubElement( IXMLDOMElement * pElement, char const * sName, Glx::Rgba rgbaDefault )
{
	HRESULT							hr;
	CComPtr< IXMLDOMNamedNodeMap >	pAttributes;

	hr = Msxmlx::GetSubElementAttributes( pElement, sName, &pAttributes );

	if ( pAttributes )
	{
		Glx::Rgba	rgba;

		rgba.m_R = Msxmlx::GetFloatAttribute( pAttributes, "r", rgbaDefault.m_R );
		rgba.m_G = Msxmlx::GetFloatAttribute( pAttributes, "g", rgbaDefault.m_G );
		rgba.m_B = Msxmlx::GetFloatAttribute( pAttributes, "b", rgbaDefault.m_B );
		rgba.m_A = Msxmlx::GetFloatAttribute( pAttributes, "a", rgbaDefault.m_A );

		return rgba;
	}
	else
	{
		return rgbaDefault;
	}
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Quaternion GetQuatSubElement( IXMLDOMElement * pElement, char const * sName, Quaternion qDefault )
{
	HRESULT							hr;
	CComPtr< IXMLDOMNamedNodeMap >	pAttributes;

	hr = Msxmlx::GetSubElementAttributes( pElement, sName, &pAttributes );

	if ( pAttributes )
	{
		Quaternion	q;

		q.m_X = Msxmlx::GetFloatAttribute( pAttributes, "x", qDefault.m_X );
		q.m_Y = Msxmlx::GetFloatAttribute( pAttributes, "y", qDefault.m_Y );
		q.m_Z = Msxmlx::GetFloatAttribute( pAttributes, "z", qDefault.m_Z );
		q.m_W = Msxmlx::GetFloatAttribute( pAttributes, "w", qDefault.m_W );

		return q;
	}
	else
	{
		return qDefault;
	}
}

} // anonymous namespace


namespace Sparkles
{


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

XmlConfiguration::XmlConfiguration( char const * sFilename )
{
	CoInitialize( NULL );


	{
		CComPtr< IXMLDOMDocument2 >	pDoc;
		pDoc.CoCreateInstance( CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER );

		HRESULT			hr;
		VARIANT_BOOL	status;

		hr = pDoc->put_async( VARIANT_FALSE );
		if ( FAILED( hr ) )
		{
			trace( "ParticleSystem::ConfigureFromXml: Failed to set async property\n" );
			throw ConstructorFailedException( "Failed to set async property" );
		}

		hr = pDoc->load( CComVariant( sFilename ), &status );

		if ( status != VARIANT_TRUE )
		{
#if defined( _DEBUG )
			CComPtr< IXMLDOMParseError >	pObjError;
			CComBSTR						reason;

			hr = pDoc->get_parseError( &pObjError );
			hr = pObjError->get_reason( &reason );
			trace( "ParticleSystem::ConfigureFromXml: Failed to load DOM from %s (%S)\n", sFilename, reason );
#endif // defined( _DEBUG )

			throw ConstructorFailedException( "Failed to load DOM" );
		}

		// Parse the XML

		CComPtr< IXMLDOMElement >	pRoot;
		hr = pDoc->get_documentElement( &pRoot );
		if ( FAILED( hr ) )
		{
			trace( "ParticleSystem::ConfigureFromXml: Failed to get document element\n" );
			throw ConstructorFailedException( "Failed to get document element" );
		}

		Msxmlx::ForEachSubElement( pRoot, ProcessBouncePlaneList, (uint32)this );	// Process all the BouncePlaneLists
		Msxmlx::ForEachSubElement( pRoot, ProcessClipPlaneList, (uint32)this );		// Process all the ClipPlaneLists
		Msxmlx::ForEachSubElement( pRoot, ProcessEnvironment, (uint32)this );		// Process all the Environments
		Msxmlx::ForEachSubElement( pRoot, ProcessAppearance, (uint32)this );		// Process all the Appearances
		Msxmlx::ForEachSubElement( pRoot, ProcessVolume, (uint32)this );			// Process all the Volumes
		Msxmlx::ForEachSubElement( pRoot, ProcessEmitter, (uint32)this );			// Process all the Emitters
	}

	CoUninitialize();
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

XmlConfiguration::~XmlConfiguration()
{
	for ( EmitterMap::iterator pE = m_EmitterMap.begin(); pE != m_EmitterMap.end(); ++pE )
	{
		delete pE->second;
	}

	for ( EmitterVolumeMap::iterator pV = m_EmitterVolumeMap.begin(); pV != m_EmitterVolumeMap.end(); ++pV )
	{
		delete pV->second;
	}

	for ( EnvironmentMap::iterator pEnv = m_EnvironmentMap.begin(); pEnv != m_EnvironmentMap.end(); ++pEnv )
	{
		delete pEnv->second;
	}

	for ( AppearanceMap::iterator pApp = m_AppearanceMap.begin(); pApp != m_AppearanceMap.end(); ++pApp )
	{
		delete pApp->second;
	}

	for ( MaterialMap::iterator pM = m_MaterialMap.begin(); pM != m_MaterialMap.end(); ++pM )
	{
		Glx::Material * const	pMaterial	= pM->second;
		Glx::Texture * const	pTexture	= pMaterial->GetTexture();

		delete pMaterial;
		delete pTexture;
	}

	for ( ClipPlaneListMap::iterator pC = m_ClipPlaneListMap.begin(); pC != m_ClipPlaneListMap.end(); ++pC )
	{
		delete pC->second;
	}

	for ( BouncePlaneListMap::iterator pB = m_BouncePlaneListMap.begin(); pB != m_BouncePlaneListMap.end(); ++pB )
	{
		delete pB->second;
	}
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

bool XmlConfiguration::ProcessBouncePlaneList( IXMLDOMElement * pElement, uint32 context )
{
	XmlConfiguration * const	pConfiguration	= reinterpret_cast< XmlConfiguration * >( context );

	HRESULT		hr;
	CComBSTR	tag;
	hr = pElement->get_tagName( &tag );

	if ( tag == CComBSTR( "BouncePlaneList" ) )
	{
		std::string const	name =	Msxmlx::GetStringAttribute( pElement, "name" );

#if defined( _DEBUG )
		{
			std::ostringstream	msg;
			msg << "BouncePlaneList: " << name << std::endl;
			OutputDebugString( msg.str().c_str() );
		}
#endif // defined( _DEBUG )

		Environment::BouncePlaneList *	pBouncePlanes	= new Environment::BouncePlaneList;
		if ( !pBouncePlanes ) throw std::bad_alloc();

		// Process all the BouncePlanes in the list

		Msxmlx::ForEachSubElement( pElement, ProcessBouncePlane, (uint32)pBouncePlanes );

		// Save this BouncePlaneList

		pConfiguration->m_BouncePlaneListMap.insert( BouncePlaneListMap::value_type( name, pBouncePlanes ) );

	}
	return true;
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

bool XmlConfiguration::ProcessBouncePlane( IXMLDOMElement * pElement, uint32 context )
{
	Environment::BouncePlaneList * const	pBouncePlanes	=
		reinterpret_cast< Environment::BouncePlaneList * >( context );

	float const	x			= Msxmlx::GetFloatAttribute( pElement, "x" );
	float const	y			= Msxmlx::GetFloatAttribute( pElement, "y" );
	float const	z			= Msxmlx::GetFloatAttribute( pElement, "z" );
	float const	d			= Msxmlx::GetFloatAttribute( pElement, "d" );
	float const	dampening	= Msxmlx::GetFloatAttribute( pElement, "dampening" );

#if defined( _DEBUG )
		{
			std::ostringstream	msg;
			msg << "    BouncePlane: [" << x << " " << y << " " << z << " " << d << "], "
				<< dampening
				<< std::endl;
			OutputDebugString( msg.str().c_str() );
		}
#endif // defined( _DEBUG )

	// Add this BouncePlane to the list

	pBouncePlanes->push_back( Environment::BouncePlane( Plane( x, y, z, d ), dampening ) );

	return true;
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

bool XmlConfiguration::ProcessClipPlaneList( IXMLDOMElement * pElement, uint32 context )
{
	XmlConfiguration * const	pConfiguration	= reinterpret_cast< XmlConfiguration * >( context );

	HRESULT		hr;
	CComBSTR	tag;
	hr = pElement->get_tagName( &tag );

	if ( tag == CComBSTR( "ClipPlaneList" ) )
	{
		std::string const	name =	Msxmlx::GetStringAttribute( pElement, "name" );

#if defined( _DEBUG )
		{
			std::ostringstream	msg;
			msg << "ClipPlaneList: " << name << std::endl;
			OutputDebugString( msg.str().c_str() );
		}
#endif // defined( _DEBUG )

		Environment::ClipPlaneList *	pClipPlanes	= new Environment::ClipPlaneList;
		if ( !pClipPlanes ) throw std::bad_alloc();

		// Process all the ClipPlanes in the list

		Msxmlx::ForEachSubElement( pElement, ProcessClipPlane, (uint32)pClipPlanes );

		// Save this ClipPlaneList

		pConfiguration->m_ClipPlaneListMap.insert( ClipPlaneListMap::value_type( name, pClipPlanes ) );
	}

	return true;
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

bool XmlConfiguration::ProcessClipPlane( IXMLDOMElement * pElement, uint32 context )
{
	Environment::ClipPlaneList * const	pClipPlanes	=	reinterpret_cast< Environment::ClipPlaneList * >( context );

	float const	x			= Msxmlx::GetFloatAttribute( pElement, "x" );
	float const	y			= Msxmlx::GetFloatAttribute( pElement, "y" );
	float const	z			= Msxmlx::GetFloatAttribute( pElement, "z" );
	float const	d			= Msxmlx::GetFloatAttribute( pElement, "d" );

#if defined( _DEBUG )
		{
			std::ostringstream	msg;
			msg << "    ClipPlane: [" << x << " " << y << " " << z << " " << d << ']' << std::endl;
			OutputDebugString( msg.str().c_str() );
		}
#endif // defined( _DEBUG )

	// Add this ClipPlane to the list

	pClipPlanes->push_back( Environment::ClipPlane( x, y, z, d ) );

	return true;
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

bool XmlConfiguration::ProcessEnvironment( IXMLDOMElement * pElement, uint32 context )
{
	XmlConfiguration * const	pConfiguration	= reinterpret_cast< XmlConfiguration * >( context );

	HRESULT		hr;
	CComBSTR	tag;
	hr = pElement->get_tagName( &tag );

	if ( tag == CComBSTR( "Environment" ) )
	{
		/*
			<xsd:element name="Environment">
				<xsd:complexType>
					<xsd:all>
						<xsd:element name="Gravity" type="vector3" minOccurs="0" />
						<xsd:element name="WindVelocity" type="vector3" minOccurs="0" />
						<xsd:element name="Gustiness" type="vector3" minOccurs="0" />
					</xsd:all>
					<xsd:attribute name="name" type="xsd:string" use="required" />
					<xsd:attribute name="airFriction" type="xsd:float" use="optional" />
					<xsd:attribute name="bounce" type="xsd:string" use="optional" />
					<xsd:attribute name="clip" type="xsd:string" use="optional" />
				</xsd:complexType>
			</xsd:element>
		*/

		std::string const	name			= Msxmlx::GetStringAttribute( pElement, "name" );
		float const			airFriction		= Msxmlx::GetFloatAttribute(  pElement, "airFriction" );
		std::string const	bounce			= Msxmlx::GetStringAttribute( pElement, "bounce" );
		std::string const	clip			= Msxmlx::GetStringAttribute( pElement, "clip" );

		Vector3 const	gustiness			= GetVector3SubElement( pElement, "Gustiness", Vector3::Origin() );
		Vector3 const	gravity				= GetVector3SubElement( pElement, "Gravity", Vector3::Origin() );
		Vector3 const	windVelocity		= GetVector3SubElement( pElement, "WindVelocity", Vector3::Origin() );

#if defined( _DEBUG )
		{
			std::ostringstream	msg;
			msg << "Environment: " << name << "( "
				<< "[" << gravity << "], "
				<< "[" << windVelocity << "], "
				<< airFriction << ", "
				<< "[" << gustiness << "], "
				<< "\"" << bounce << "\", "
				<< "\"" << clip << "\" )"
				<< std::endl;
			OutputDebugString( msg.str().c_str() );
		}
#endif // defined( _DEBUG )

		Environment *	pEnvironment	= new Environment( gravity, windVelocity, airFriction, gustiness );
		if ( !pEnvironment ) throw std::bad_alloc();

		pEnvironment->SetBouncePlanes( pConfiguration->m_BouncePlaneListMap[ bounce ] );
		pEnvironment->SetClipPlanes( pConfiguration->m_ClipPlaneListMap[ clip ] );

		// Save this Environment

		pConfiguration->m_EnvironmentMap.insert( EnvironmentMap::value_type( name, pEnvironment ) );
	}

	return true;
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

bool XmlConfiguration::ProcessAppearance(  IXMLDOMElement * pElement, uint32 context )
{
	XmlConfiguration * const	pConfiguration	= reinterpret_cast< XmlConfiguration * >( context );

	HRESULT		hr;
	CComBSTR	tag;
	hr = pElement->get_tagName( &tag );

	if ( tag == CComBSTR( "Appearance" ) )
	{
		/*
			<xsd:element name="Appearance">
				<xsd:complexType>
					<xsd:all>
						<xsd:element name="ColorDelta" type="glColor" minOccurs="0" />
					</xsd:all>
					<xsd:attribute name="name" type="xsd:string" use="required" />
					<xsd:attribute name="radiusDelta" type="xsd:float" use="optional" />
					<xsd:attribute name="radialVelocity" type="xsd:float" use="optional" />
					<xsd:attribute name="texture" type="xsd:anyURI" use="optional" />
					<xsd:attribute name="size" type="xsd:float" use="optional" />
				</xsd:complexType>
			</xsd:element>
		*/

		std::string const	name			= Msxmlx::GetStringAttribute( pElement, "name" );
		float const			radiusDelta		= Msxmlx::GetFloatAttribute(  pElement, "radiusDelta" );
		float const			radialVelocity	= Msxmlx::GetFloatAttribute(  pElement, "radialVelocity" );
		std::string const	texture			= Msxmlx::GetStringAttribute( pElement, "texture" );
		float const			size			= Msxmlx::GetFloatAttribute(  pElement, "size" );

		Glx::Rgba const		colorDelta		=         GetRgbaSubElement( pElement, "ColorDelta",
																		 Glx::Rgba( 0.f, 0.f, 0.f, 0.f ) );
		Glx::Material *		pMaterial		= 0;


#if defined( _DEBUG )
		{
			std::ostringstream	msg;
			msg << "Appearance: " << name << "( "
				<< "[" << colorDelta << "], "
				<< radiusDelta << ", "
				<< radialVelocity << ", "
				<< "\"" << texture << "\", "
				<< size << " )"
				<< std::endl;
			OutputDebugString( msg.str().c_str() );
		}
#endif // defined( _DEBUG )


		// If there is a texture, then load it

		if ( texture.size() > 0 )
		{
			Glx::Texture *	pTexture		= 0;
			unsigned char *	aTextureData	= 0;

			try
			{
				// Open the particle texture image

				TgaFile	file( texture.c_str() );
				assert( file.m_ImageType == TgaFile::IMAGE_TRUECOLOR );

				// Allocate a buffer for the texture

				aTextureData = new unsigned char[ file.m_Width * file.m_Height * 4 ];
				if ( !aTextureData ) throw std::bad_alloc();
				
				// Read the file into the buffer

				bool const	ok = file.Read( aTextureData );
				assert( ok );

				// Create a texture with the buffer

				pTexture = new Glx::Texture( file.m_Width, file.m_Height, aTextureData,
											GL_BGRA_EXT, GL_UNSIGNED_BYTE, GL_CLAMP );
				if ( !pTexture ) throw std::bad_alloc();

				pMaterial = new Glx::Material( pTexture );
				if ( !pMaterial ) throw std::bad_alloc();

				// Save the material (and the texture)
				pConfiguration->m_MaterialMap.insert( MaterialMap::value_type( name, pMaterial ) );
			}
			catch ( ... )
			{
				delete pMaterial;
				delete pTexture;
				pMaterial = 0;
			}

			delete[] aTextureData;
		}


		Appearance *	pAppearance	= new Appearance( colorDelta, radiusDelta, radialVelocity, pMaterial, size );
		if ( !pAppearance ) throw std::bad_alloc();

		// Save Appearance

		pConfiguration->m_AppearanceMap.insert( AppearanceMap::value_type( name, pAppearance ) );
	}

	return true;
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

bool XmlConfiguration::ProcessVolume( IXMLDOMElement * pElement, uint32 context )
{
	XmlConfiguration * const	pConfiguration	= reinterpret_cast< XmlConfiguration * >( context );

	HRESULT		hr;
	CComBSTR	tag;
	hr = pElement->get_tagName( &tag );

	if ( tag == CComBSTR( "Volume" ) )
	{
		/*
			<xsd:element name="Volume">
				<xsd:complexType>
					<xsd:group />
					<xsd:attribute name="name" type="xsd:string" use="required" />
					<xsd:attribute name="type" type="volumetype" use="required" />
					<xsd:attribute name="length" type="float" use="optional" />
					<xsd:attribute name="width" type="float" use="optional" />
					<xsd:attribute name="height" type="float" use="optional" />
					<xsd:attribute name="depth" type="float" use="optional" />
					<xsd:attribute name="radius" type="float" use="optional" />
				</xsd:complexType>
			</xsd:element>
		*/

		std::string const	name		= Msxmlx::GetStringAttribute( pElement, "name" );
		std::string const	type		= Msxmlx::GetStringAttribute( pElement, "type" );
		float const			length		= Msxmlx::GetFloatAttribute(  pElement, "length", 1. );
		float const			width		= Msxmlx::GetFloatAttribute(  pElement, "width", 1. );
		float const			height		= Msxmlx::GetFloatAttribute(  pElement, "height", 1. );
		float const			depth		= Msxmlx::GetFloatAttribute(  pElement, "depth", 1. );
		float const			radius		= Msxmlx::GetFloatAttribute(  pElement, "radius", 1. );

#if defined( _DEBUG )
		{
			std::ostringstream	msg;
			msg << "Volume: " << name << "( " << type << "( "
				<< length << ", "
				<< width << ", "
				<< height << ", "
				<< depth << ", "
				<< radius << " ) )" << std::endl;
			OutputDebugString( msg.str().c_str() );
		}
#endif // defined( _DEBUG )

		EmitterVolume *	pVolume = 0;

		if ( type == "point" )
		{
			pVolume = new EmitterPoint( timeGetTime() );
			if ( !pVolume ) throw std::bad_alloc();
		}
		else if ( type == "line" )
		{
			pVolume = new EmitterLine( timeGetTime(), length );
			if ( !pVolume ) throw std::bad_alloc();
		}
		else if ( type == "rectangle" )
		{
			pVolume = new EmitterRectangle( timeGetTime(), Vector2( width, height ) );
			if ( !pVolume ) throw std::bad_alloc();
		}
		else if ( type == "circle" )
		{
			pVolume = new EmitterCircle( timeGetTime(), radius );
			if ( !pVolume ) throw std::bad_alloc();
		}
		else if ( type == "sphere" )
		{
			pVolume = new EmitterSphere( timeGetTime(), radius );
			if ( !pVolume ) throw std::bad_alloc();
		}
		else if ( type == "box" )
		{
			pVolume = new EmitterBox( timeGetTime(), Vector3( width, height, depth ) );
			if ( !pVolume ) throw std::bad_alloc();
		}
		else if ( type == "cylinder" )
		{
			pVolume = new EmitterCylinder( timeGetTime(), radius, height );
			if ( !pVolume ) throw std::bad_alloc();
		}
		else if ( type == "cone" )
		{
			pVolume = new EmitterCone( timeGetTime(), radius, height );
			if ( !pVolume ) throw std::bad_alloc();
		}

		// Save this Volume

		pConfiguration->m_EmitterVolumeMap.insert( EmitterVolumeMap::value_type( name, pVolume ) );
	}

	return true;
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

bool XmlConfiguration::ProcessEmitter( IXMLDOMElement * pElement, uint32 context )
{
	XmlConfiguration * const	pConfiguration	= reinterpret_cast< XmlConfiguration * >( context );

	HRESULT						hr;
	CComBSTR					emitterTag;

	hr = pElement->get_tagName( &emitterTag );

	if ( emitterTag == CComBSTR( "Emitter" ) )
	{
		/*
			<xsd:element name="Emitter">
				<xsd:complexType>
					<xsd:sequence>
						<xsd:element ref="Position" minOccurs="0" />
						<xsd:element ref="Orientation" minOccurs="0" />
						<xsd:element ref="Velocity" minOccurs="0" />
						<xsd:element ref="Particle" minOccurs="0" maxOccurs="unbounded" />
					</xsd:sequence>
					<xsd:attribute name="name" type="xsd:string" use="required" />
					<xsd:attribute name="type" type="emitterType" use="required" />
					<xsd:attribute name="volume" type="xsd:string" use="required" />
					<xsd:attribute name="environment" type="xsd:string" use="required" />
					<xsd:attribute name="appearance" type="xsd:string" use="required" />
					<xsd:attribute name="lifetime" type="xsd:float" use="optional" />
					<xsd:attribute name="minSpeed" type="xsd:float" use="required" />
					<xsd:attribute name="maxSpeed" type="xsd:float" use="required" />
					<xsd:attribute name="count" type="xsd:integer" use="optional" />
					<xsd:attribute name="spread" type="xsd:float" use="optional" />
					<xsd:attribute name="color" type="rgba" use="optional" />
					<xsd:attribute name="radius" type="xsd:float" use="optional" />
				</xsd:complexType>
			</xsd:element>
		*/

		long				actualCount;

		std::string const	name		= Msxmlx::GetStringAttribute( pElement, "name" );
		std::string const	type		= Msxmlx::GetStringAttribute( pElement, "type" );
		std::string const	volume		= Msxmlx::GetStringAttribute( pElement, "volume" );
		std::string const	environment	= Msxmlx::GetStringAttribute( pElement, "environment" );
		std::string const	appearance	= Msxmlx::GetStringAttribute( pElement, "appearance" );
		int const			count		= Msxmlx::GetIntAttribute(    pElement, "count" );
		float const			spread		= Msxmlx::GetFloatAttribute(  pElement, "spread" );
		float const			lifetime	= Msxmlx::GetFloatAttribute(  pElement, "lifetime", 1.f );
		uint32 const		color		= Msxmlx::GetHexAttribute(    pElement, "color" );
		float const			radius		= Msxmlx::GetFloatAttribute(  pElement, "radius" );
		float const			minSpeed	= Msxmlx::GetFloatAttribute(  pElement, "minSpeed" );
		float const			maxSpeed	= Msxmlx::GetFloatAttribute(  pElement, "maxSpeed" );

		Vector3 const		position	= GetVector3SubElement( pElement, "Position",		Vector3::Origin() );
		Quaternion const	orientation	= GetQuatSubElement(    pElement, "Orientation",	Quaternion::Identity() );
		Vector3 const		velocity	= GetVector3SubElement( pElement, "Velocity",		Vector3::Origin() );

#if defined( _DEBUG )
		{
			std::ostringstream	msg;
			msg << "Emitter: " << name << "( "
				<< type << "( "
				<< "\"" << volume << "\", "
				<< "\"" << environment << "\", "
				<< "\"" << appearance << "\", "
				<< count << ", "
				<< spread << ", "
				<< lifetime << ", "
				<< color << ", "
				<< radius << ", "
				<< minSpeed << ", "
				<< maxSpeed << ", "
				<< "[" << velocity << "], "
				<< "[" << position << "], "
				<< "[" << orientation << "] ) )"
				<< std::endl;
			OutputDebugString( msg.str().c_str() );
		}
#endif // defined( _DEBUG )


		// If there is a list of particles, use that count instead of the supplied count

		CComPtr< IXMLDOMNode >		pParticleListNode		= NULL;
		CComPtr< IXMLDOMNodeList >	pParticleListNodeList	= NULL;
		pElement->selectSingleNode( CComBSTR( "ParticleList" ), &pParticleListNode );
		if ( pParticleListNode )
		{
			hr = pElement->get_childNodes( &pParticleListNodeList );
			hr = pParticleListNodeList->get_length( &actualCount );

#if defined( _DEBUG )
			if ( count != 0 && count != actualCount )
			{
				std::ostringstream	msg;
				msg << "ParticleSystem::ProcessEmitter: Warning - " << name << ":count is "
					<< count << ", but the actual count is "
					<< actualCount << "."
					<< std::endl;
				OutputDebugString( msg.str().c_str() );
			}
#endif // defined( _DEBUG )
		}
		else
		{
			actualCount = count;
		}

		if ( actualCount == 0 )
		{
			actualCount = 1;
		}

		BasicEmitter *	pEmitter = 0;

		if ( type == "point" )
		{
			pEmitter = new ParticleEmitter( pConfiguration->m_EmitterVolumeMap[ volume ],
											pConfiguration->m_EnvironmentMap[ environment ],
											pConfiguration->m_AppearanceMap[ appearance ],
											actualCount );
			if ( !pEmitter ) throw std::bad_alloc();

			ProcessParticles( static_cast< ParticleEmitter * >( pEmitter ),
							  pParticleListNodeList, actualCount,
							  pConfiguration, lifetime, color, spread, minSpeed, maxSpeed );
		}
		else if ( type == "streak" )
		{
			pEmitter = new StreakEmitter( pConfiguration->m_EmitterVolumeMap[ volume ],
										  pConfiguration->m_EnvironmentMap[ environment ],
										  pConfiguration->m_AppearanceMap[ appearance ],
										  actualCount );
			if ( !pEmitter ) throw std::bad_alloc();

			ProcessStreakParticles( static_cast< StreakEmitter * >( pEmitter ),
									pParticleListNodeList, actualCount,
									pConfiguration, lifetime, color, spread, minSpeed, maxSpeed );
		}
		else if ( type == "textured" )
		{
			pEmitter = new TexturedEmitter( pConfiguration->m_EmitterVolumeMap[ volume ],
											pConfiguration->m_EnvironmentMap[ environment ],
											pConfiguration->m_AppearanceMap[ appearance ],
											actualCount );
			if ( !pEmitter ) throw std::bad_alloc();

			ProcessTexturedParticles( static_cast< TexturedEmitter * >( pEmitter ),
									  pParticleListNodeList, actualCount,
									  pConfiguration, lifetime, color, spread, minSpeed, maxSpeed );
		}
		else if ( type == "sphere" )
		{
			pEmitter = new SphereEmitter( pConfiguration->m_EmitterVolumeMap[ volume ],
										  pConfiguration->m_EnvironmentMap[ environment ],
										  pConfiguration->m_AppearanceMap[ appearance ],
										  actualCount );
			if ( !pEmitter ) throw std::bad_alloc();

			ProcessSphereParticles( static_cast< SphereEmitter * >( pEmitter ),
									pParticleListNodeList, actualCount,
									pConfiguration, lifetime, color, spread, minSpeed, maxSpeed );
		}

		// Save this Emitter

		pConfiguration->m_EmitterMap.insert( EmitterMap::value_type( name, pEmitter ) );
	}

	return true;
}




/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void XmlConfiguration::ProcessParticles( ParticleEmitter * pEmitter,
										 IXMLDOMNodeList * pNodeList,
										 int numParticles,
										 Configuration const * pConfiguration,
										 float globalLifetime, uint32 globalColor,
										 float spread, float minSpeed, float maxSpeed )
{
	Particle * const		paParticles				= pEmitter->GetParticles();
	RandomFloat				rng( timeGetTime() );
	Dxx::RandomDirection	rdir( timeGetTime()+1 );

	for ( int i = 0; i < numParticles; i++ )
	{
		float		lifetime	= globalLifetime;
		float		age			= rng.Get( -lifetime, 0.f );
		uint32		color		= ( globalColor != 0 ) ? globalColor :
															( i & 1 ? 0 : 0x000000ff ) |
															( i & 2 ? 0 : 0x0000ff00 ) |
															( i & 4 ? 0 : 0x00ff0000 ) |
																		  0xff000000;
		float		radius		= 1.f;
		float		orientation	= 0.f;
		Vector3		position	= pEmitter->GetEmitterVolume()->Next();
		Vector3		velocity;

		{
			Vector3 const	v	= rdir.Get( spread ) * rng.Get( minSpeed, maxSpeed );

			velocity.m_X	= v.m_Y;
			velocity.m_Y	= v.m_X;
			velocity.m_Z	= v.m_Z;
		}

		// If we are processing a list of particle elements, then load non-default values from the next element
		
		if ( pNodeList )
		{
			CComPtr< IXMLDOMNode >		pNode;
			HRESULT						hr;
			
			hr = pNodeList->nextNode( &pNode );
			ProcessParticle( pNode, &lifetime, &age, &color, &radius, &orientation, &position, &velocity );
		}

		// Initialize the particle

		Glx::Rgba	rgba	= Glx::Rgba( float( color & 0x000000ffU ) / float( 0x000000ffU ),
										 float( color & 0x0000ff00U ) / float( 0x0000ff00U ),
										 float( color & 0x00ff0000U ) / float( 0x00ff0000U ),
										 float( color & 0xff000000U ) / float( 0xff000000U ) );

		paParticles[ i ].Initialize( lifetime, age, rgba, position, velocity );
	}
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void XmlConfiguration::ProcessStreakParticles( StreakEmitter * pEmitter,
											   IXMLDOMNodeList * pNodeList,
											   int numParticles,
											   Configuration const * pConfiguration,
											   float globalLifetime, uint32 globalColor,
                                               float spread, float minSpeed, float maxSpeed )
{
	StreakParticle * const	paParticles				= pEmitter->GetParticles();
	RandomFloat				rng( timeGetTime() );
	Dxx::RandomDirection	rdir( timeGetTime()+1 );

	for ( int i = 0; i < numParticles; i++ )
	{
		float		lifetime	= globalLifetime;
		float		age			= rng.Get( -lifetime, 0.f );
		uint32		color		= ( globalColor != 0 ) ? globalColor :
															( i & 1 ? 0 : 0x000000ff ) |
															( i & 2 ? 0 : 0x0000ff00 ) |
															( i & 4 ? 0 : 0x00ff0000 ) |
																		  0xff000000;
		float		radius		= 1.f;
		float		orientation	= 0.f;
		Vector3		position	= pEmitter->GetEmitterVolume()->Next();
		Vector3		velocity;

		{
			Vector3 const	v	= rdir.Get( spread ) * rng.Get( minSpeed, maxSpeed );

			velocity.m_X	= v.m_Y;
			velocity.m_Y	= v.m_X;
			velocity.m_Z	= v.m_Z;
		}

		// If we are processing a list of particle elements, then load non-default values from the next element
		
		if ( pNodeList )
		{
			CComPtr< IXMLDOMNode >		pNode;
			HRESULT						hr;

			hr = pNodeList->nextNode( &pNode );
			ProcessParticle( pNode, &lifetime, &age, &color, &radius, &orientation, &position, &velocity );
		}

		// Initialize the particle

		Glx::Rgba	rgba	= Glx::Rgba( float( color & 0x000000ffU ) / float( 0x000000ffU ),
										 float( color & 0x0000ff00U ) / float( 0x0000ff00U ),
										 float( color & 0x00ff0000U ) / float( 0x00ff0000U ),
										 float( color & 0xff000000U ) / float( 0xff000000U ) );

		paParticles[ i ].Initialize( lifetime, age, rgba, position, velocity );
	}
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void XmlConfiguration::ProcessTexturedParticles( TexturedEmitter * pEmitter,
												 IXMLDOMNodeList * pNodeList,
												 int numParticles,
												 Configuration const * pConfiguration,
												 float globalLifetime, uint32 globalColor,
												 float spread, float minSpeed, float maxSpeed )
{
	TexturedParticle * const	paParticles				= pEmitter->GetParticles();
	RandomFloat					rng( timeGetTime() );
	Dxx::RandomDirection		rdir( timeGetTime()+1 );

	for ( int i = 0; i < numParticles; i++ )
	{
		float		lifetime	= globalLifetime;
		float		age			= rng.Get( -lifetime, 0.f );
		uint32		color		= ( globalColor != 0 ) ? globalColor :
															( i & 1 ? 0 : 0x000000ff ) |
															( i & 2 ? 0 : 0x0000ff00 ) |
															( i & 4 ? 0 : 0x00ff0000 ) |
																		  0xff000000;
		float		radius		= 1.f;
		float		orientation	= 0.f;
		Vector3		position	= pEmitter->GetEmitterVolume()->Next();
		Vector3		velocity;

		{
			Vector3 const	v	= rdir.Get( spread ) * rng.Get( minSpeed, maxSpeed );

			velocity.m_X	= v.m_Y;
			velocity.m_Y	= v.m_X;
			velocity.m_Z	= v.m_Z;
		}

		// If we are processing a list of particle elements, then load non-default values from the next element
		
		if ( pNodeList )
		{
			CComPtr< IXMLDOMNode >		pNode;
			HRESULT						hr;

			hr = pNodeList->nextNode( &pNode );
			ProcessParticle( pNode, &lifetime, &age, &color, &radius, &orientation, &position, &velocity );
		}

		// Initialize the particle

		Glx::Rgba	rgba	= Glx::Rgba( float( color & 0x000000ffU ) / float( 0x000000ffU ),
										 float( color & 0x0000ff00U ) / float( 0x0000ff00U ),
										 float( color & 0x00ff0000U ) / float( 0x00ff0000U ),
										 float( color & 0xff000000U ) / float( 0xff000000U ) );

		paParticles[ i ].Initialize( lifetime, age, rgba, position, velocity, radius, orientation );
	}
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void XmlConfiguration::ProcessSphereParticles( SphereEmitter * pEmitter,
											   IXMLDOMNodeList * pNodeList,
											   int numParticles,
											   Configuration const * pConfiguration,
											   float globalLifetime, uint32 globalColor,
											   float spread, float minSpeed, float maxSpeed )
{
	SphereParticle * const	paParticles				= pEmitter->GetParticles();
	RandomFloat				rng( timeGetTime() );
	Dxx::RandomDirection	rdir( timeGetTime()+1 );

	for ( int i = 0; i < numParticles; i++ )
	{
		float		lifetime	= globalLifetime;
		float		age			= rng.Get( -lifetime, 0.f );
		uint32		color		= ( globalColor != 0 ) ? globalColor :
															( i & 1 ? 0 : 0x000000ff ) |
															( i & 2 ? 0 : 0x0000ff00 ) |
															( i & 4 ? 0 : 0x00ff0000 ) |
																		  0xff000000;
		float		radius		= 1.f;
		float		orientation	= 0.f;
		Vector3		position	= pEmitter->GetEmitterVolume()->Next();
		Vector3		velocity;

		{
			Vector3 const	v	= rdir.Get( spread ) * rng.Get( minSpeed, maxSpeed );

			velocity.m_X	= v.m_Y;
			velocity.m_Y	= v.m_X;
			velocity.m_Z	= v.m_Z;
		}

		// If we are processing a list of particle elements, then load non-default values from the next element
		
		if ( pNodeList )
		{
			CComPtr< IXMLDOMNode >		pNode;
			HRESULT						hr;

			hr = pNodeList->nextNode( &pNode );
			ProcessParticle( pNode, &lifetime, &age, &color, &radius, &orientation, &position, &velocity );
		}

		// Initialize the particle

		Glx::Rgba	rgba	= Glx::Rgba( float( color & 0x000000ffU ) / float( 0x000000ffU ),
										 float( color & 0x0000ff00U ) / float( 0x0000ff00U ),
										 float( color & 0x00ff0000U ) / float( 0x00ff0000U ),
										 float( color & 0xff000000U ) / float( 0xff000000U ) );

//		paParticles[ i ].Initialize( lifetime, age, rgba, position, velocity );
	}
}





/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void XmlConfiguration::ProcessParticle( IXMLDOMNode *	pNode,
										float *			pLifetime,
										float *			pAge,
										uint32 *		pColor,
										float *			pRadius,
										float *			pOrientation,
										Vector3 *		pVelocity,
										Vector3 *		pPosition )
{
	HRESULT		hr;
	
	if ( Msxmlx::IsElementNode( pNode ) )
	{
		CComQIPtr< IXMLDOMElement >	pElement( pNode );
		CComBSTR					tag;

		hr = pElement->get_tagName( &tag );

		if ( tag == CComBSTR( "Particle" ) )
		{
			/*
				<xsd:element name="Particle">
					<xsd:complexType>
						<xsd:all>
							<xsd:element ref="Position" />
							<xsd:element ref="Velocity" />
						</xsd:all>
						<xsd:attribute name="lifetime" type="xsd:float" use="optional" />
						<xsd:attribute name="age" type="xsd:float" use="optional" />
						<xsd:attribute name="color" type="rgba" use="optional" />
						<xsd:attribute name="radius" type="xsd:float" use="optional" />
						<xsd:attribute name="orientation" type="xsd:float" use="optional" />
					</xsd:complexType>
				</xsd:element>
			*/

			*pPosition		= GetVector3SubElement(     	pElement, "Position", 		*pPosition );
			*pVelocity		= GetVector3SubElement(     	pElement, "Velocity", 		*pVelocity );
			*pLifetime		= Msxmlx::GetFloatAttribute(	pElement, "lifetime", 		*pLifetime );
			*pAge			= Msxmlx::GetFloatAttribute(	pElement, "age",	  		*pAge );
			*pColor			= Msxmlx::GetHexAttribute(  	pElement, "color",	  		*pColor );
			*pRadius		= Msxmlx::GetFloatAttribute(	pElement, "radius",	  		*pRadius );
			*pOrientation	= Msxmlx::GetFloatAttribute(	pElement, "orientation",	*pOrientation );

#if defined( _DEBUG )
			{
				std::ostringstream	msg;
				msg << "    Particle: "
					<< *pLifetime << ", "
					<< *pAge << ", "
					<< *pColor << ", "
					<< *pRadius << ", "
					<< *pOrientation << ", "
					<< "[" << *pVelocity << "],"
					<< "[" << *pPosition << "] )"
					<< std::endl;
				OutputDebugString( msg.str().c_str() );
			}
#endif // defined( _DEBUG )
		}
	}
}



} // namespace Sparkles