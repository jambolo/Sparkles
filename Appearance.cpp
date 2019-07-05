/** @file *//********************************************************************************************************

                                                     Appearance.cpp

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/Appearance.cpp#4 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "PrecompiledHeaders.h"

#include "Appearance.h"

namespace Sparkles
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

Appearance::Appearance( Glx::Rgba const & colorDelta	/* = Glx::Rgba( 0.f, 0.f, 0.f, 0.f )*/,
					    float radiusDelta				/* = 0.f*/,
						float radialVelocity			/* = 0.f*/,
						Glx::Material const * pMaterial	/* = 0*/,
						float width						/* = 1.f*/ )
	: m_ColorDelta( colorDelta ),
	m_RadiusDelta( radiusDelta ),
	m_RadialVelocity( radialVelocity ),
	m_pMaterial( pMaterial ),
	m_CameraOrientation( Matrix33::Identity() ),
	m_LineWidth( width )
{
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

Appearance::~Appearance()
{
}



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

void Appearance::Update( float dt, Glx::Camera const & camera )
{
	m_CameraOrientation = camera.GetOrientation().GetRotationMatrix33();
}



} // namespace Sparkles
