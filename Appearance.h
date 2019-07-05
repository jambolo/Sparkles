#if !defined( SPARKLES_APPEARANCE_H_INCLUDED )
#define SPARKLES_APPEARANCE_H_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                     Appearance.h

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/Appearance.h#4 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include <cassert>

#include "Math/Matrix33.h"
#include "Glx/Camera.h"
#include "Glx/Material.h"
#include "Glx/Rgba.h"
#include "Glx/Texture.h"


namespace Sparkles
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// A class containing common appearance attributes for all particles associated with it.

class Appearance
{
public:

	/// Constructor
	Appearance( Glx::Rgba const & colorDelta = Glx::Rgba( 0.f, 0.f, 0.f, 0.f ),
				float radiusDelta = 0.f,
				float radialVelocity = 0.f,
				Glx::Material const * pMaterial = 0,
				float width = 1.f );

	/// Destructor
	~Appearance();

	/// Updates the appearance values
	void Update( float dt, Glx::Camera const & camera );

	/// Returns the current material
	Glx::Material const * GetMaterial() const		{ return m_pMaterial; }

	/// Returns the texture associated with the current material
	Glx::Texture const * GetTexture() const			{ assert( m_pMaterial != 0 ); return m_pMaterial->GetTexture(); }

	/// Returns the delta of the particle radii
	float GetRadiusDelta() const					{ return m_RadiusDelta; }

	/// Returns the radial velocity of the particles
	float GetRadialVelocity() const					{ return m_RadialVelocity; }

	/// Returns the current camera orientation
	Matrix33 const & GetCameraOrientation() const	{ return m_CameraOrientation; }

	/// Returns the color delta.
	Glx::Rgba const & GetColorDelta() const			{ return m_ColorDelta; }

	/// Returns the current line width.
	float GetLineWidth() const						{ return m_LineWidth; }

private:

	Glx::Material const *	m_pMaterial;
	float					m_RadiusDelta;
	float					m_RadialVelocity;
	Matrix33				m_CameraOrientation;
	float					m_LineWidth;
	Glx::Rgba				m_ColorDelta;
};



} // namespace Sparkles


#endif // !defined( SPARKLES_APPEARANCE_H_INCLUDED )
