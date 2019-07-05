#if !defined( SPARKLES_EMITTERVOLUME_H_INCLUDED )
#define SPARKLES_EMITTERVOLUME_H_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                     EmitterVolume.h

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/EmitterVolume.h#3 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Misc/Random.h"

namespace Sparkles
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class EmitterVolume
{
public:
	EmitterVolume( unsigned int seed );
	virtual ~EmitterVolume();

	virtual Vector3	Next() const = 0;

protected:

	mutable RandomFloat	m_Rng;
};


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class EmitterPoint : public EmitterVolume
{
public:
	EmitterPoint( unsigned int seed );
	virtual ~EmitterPoint();

	Vector3	Next() const	{ return Vector3::Origin(); }
};


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class EmitterLine : public EmitterVolume
{
public:
	EmitterLine( unsigned int seed, float size );
	virtual ~EmitterLine();

	Vector3	Next() const;

private:

	float	m_Size;
};


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class EmitterRectangle : public EmitterVolume
{
public:
	EmitterRectangle( unsigned int seed, Vector2 const & size );
	virtual ~EmitterRectangle();

	Vector3	Next() const;

private:

	Vector2	m_Size;
};


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class EmitterCircle : public EmitterVolume
{
public:
	EmitterCircle( unsigned int seed, float radius );
	virtual ~EmitterCircle();

	Vector3	Next() const;

private:

	float		m_Radius;
};


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class EmitterSphere : public EmitterVolume
{
public:
	EmitterSphere( unsigned int seed, float radius );
	virtual ~EmitterSphere();

	Vector3	Next() const;

private:

	float	m_Radius;
};


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class EmitterBox : public EmitterVolume
{
public:
	EmitterBox( unsigned int seed, Vector3 const & size );
	virtual ~EmitterBox();

	Vector3	Next() const;

private:

	Vector3	m_Size;
};


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class EmitterCylinder : public EmitterVolume
{
public:
	EmitterCylinder( unsigned int seed, float radius, float height );
	virtual ~EmitterCylinder();

	Vector3	Next() const;

private:

	float		m_Radius;
	float		m_Height;
};


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class EmitterCone : public EmitterVolume
{
public:
	EmitterCone( unsigned int seed, float radius, float height );
	virtual ~EmitterCone();

	Vector3	Next() const;

private:

	float		m_Radius;
	float		m_Height;
};



} // namespace Sparkles


#endif // !defined( SPARKLES_EMITTERVOLUME_H_INCLUDED )
