/*********************************************************************************************************************

                                                       main.cpp

						                    Copyright 2002, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Sparkles/Test/main.cpp#5 $

	$NoKeywords: $

*********************************************************************************************************************/

#pragma warning( disable:4786 )

#include <windows.h>
#include <mmsystem.h>

#include "gl/gl.h"
#include "gl/glu.h"

#include "../Sparkles.h"

#include "Math/FastMath.h"
#include "TgaFile/TgaFile.h"
#include "Wx/Wx.h"
#include "Glx/Glx.h"
#include "WGlx/WGlx.h"

#include <stdlib.h>

static LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

static void InitializeRendering();
static void Display();
static void Reshape( int w, int h );

static bool Update( HWND hWnd );

static void InitializeParticleSystem();
static void UpdateParticleSystem( float dt );
static void CleanupParticleSystem();
static void DisplayParticles();

static char	s_AppName[]		= "Sparkles";
static char	s_TitleBar[]	= "Sparkles";

static Glx::Extension::PFNGLPOINTPARAMETERFEXT	glPointParameterfEXT;
static Glx::Extension::PFNGLPOINTPARAMETERFVEXT	glPointParameterfvEXT;

static Glx::Camera *	s_pCamera;
static float			s_CameraSpeed				= 2.f;

static WGlx::Font *		s_pFont;



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow )
{
	if ( Wx::RegisterWindowClass(	CS_OWNDC,
									( WNDPROC )WindowProc,
									hInstance,
									s_AppName,
									LoadCursor( NULL, IDC_ARROW )
								) == NULL )
	{
		MessageBox( NULL, "Wx::RegisterWindowClass() failed.", "Error", MB_OK );
		exit( 1 );
	}

	HWND hWnd = CreateWindowEx( 0,
								s_AppName,
								s_TitleBar,
								WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								0, 0, 500, 500,
								NULL,
								NULL,
								hInstance,
								NULL );

	if ( hWnd == NULL )
	{
		MessageBox( NULL, "CreateWindowEx() failed.", "Error", MB_OK );
		exit( 1 );
	}

	HDC const	hDC	= GetDC( hWnd );
	int			rv;

	WGlx::SetPixelFormat( hDC, 0, true );

	{
		WGlx::CurrentRenderingContext	rc( hDC );	// Current rendering context

		WGLX_BIND_EXTENSION( glPointParameterfEXT, GL_EXT_point_parameters, Glx::Extension::PFNGLPOINTPARAMETERFEXT );
		WGLX_BIND_EXTENSION( glPointParameterfvEXT, GL_EXT_point_parameters, Glx::Extension::PFNGLPOINTPARAMETERFVEXT );

		InitializeRendering();

		InitializeParticleSystem();

		s_pCamera	= new Glx::Camera( 90.f, 1.f, 1000.f, Vector3( 0.f, 180.f, 200.f ) );
//		if ( !s_pCamera ) throw std::bad_alloc();

		{
			LOGFONT	lf =
			{
				16, 0,
				0, 0,
				FW_BOLD, FALSE, FALSE, FALSE,
				ANSI_CHARSET,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH|FF_DONTCARE,
				"Courier New"
			};

			s_pFont = new WGlx::Font( hDC, lf );
//			if ( !s_pFont ) throw std::bad_alloc();
		}

		ShowWindow( hWnd, nCmdShow );

		rv = Wx::MessageLoop( hWnd, Update );

		delete s_pFont;
		delete s_pCamera;

		CleanupParticleSystem();
	}

	ReleaseDC( hWnd, hDC );
	DestroyWindow( hWnd );

	return rv;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static bool Update( HWND hWnd )
{
	static DWORD	oldTime		= timeGetTime();

	DWORD const		newTime		= timeGetTime();
	float const		dt			= ( newTime - oldTime ) * .001f;

	UpdateParticleSystem( dt );

	Display();

	oldTime = newTime;

	return true;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{ 
	switch( uMsg )
	{
//	case WM_PAINT:
//	{
//		static PAINTSTRUCT ps;
//
//		Display();
//		BeginPaint( hWnd, &ps );
//		EndPaint( hWnd, &ps );
//
//		return 0;
//	}

	case WM_SIZE:
		Reshape( LOWORD( lParam ), HIWORD( lParam ) );
		PostMessage( hWnd, WM_PAINT, 0, 0 );
		return 0;

	case WM_CHAR:
	{
		switch ( wParam )
		{
		case VK_ESCAPE:	// Quit
			PostQuitMessage( 0 );
			break;

		case ' ':	// Forward
			s_pCamera->Move( s_pCamera->GetDirection() * s_CameraSpeed );
			break;

		case 's':	// Backwards
			s_pCamera->Move( -s_pCamera->GetDirection() * s_CameraSpeed );
			break;

		case 'd':	// Strafe right
			s_pCamera->Move( s_pCamera->GetRight() * s_CameraSpeed );
			break;

		case 'a':	// Strafe left
			s_pCamera->Move( -s_pCamera->GetRight() * s_CameraSpeed );
			break;

		case 'w':	// Strafe up
			s_pCamera->Move( s_pCamera->GetUp() * s_CameraSpeed );
			break;

		case 'x':	// Strafe down
			s_pCamera->Move( -s_pCamera->GetUp() * s_CameraSpeed );
			break;
		}

		return 0;
	}

	case WM_KEYDOWN:
		switch ( wParam )
		{
		case VK_UP:
			s_pCamera->Turn( 5.f, Vector3::XAxis() );
			break;

		case VK_DOWN:
			s_pCamera->Turn( -5.f, Vector3::XAxis() );
			break;

		case VK_LEFT:
			s_pCamera->Turn( 5.f, Vector3::YAxis() );
			break;

		case VK_RIGHT:
			s_pCamera->Turn( -5.f, Vector3::YAxis() );
			break;

		case VK_PRIOR:
			s_pCamera->Turn( -5.f, Vector3::ZAxis() );
			break;

		case VK_INSERT:
			s_pCamera->Turn( 5.f, Vector3::ZAxis() );
			break;
		}
		return 0;

	case WM_CLOSE:
		PostQuitMessage( 0 );
		return 0;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam ); 
} 


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void InitializeRendering()
{
	glClearColor( .2f, .2f, .2f, 1.f );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static Sparkles::ParticleSystem *	s_pParticleSystem;
static Sparkles::XmlConfiguration *	s_pConfiguration;

static void InitializeParticleSystem()
{
	s_pConfiguration = new Sparkles::XmlConfiguration( "Configuration.xml" );
	if ( !s_pConfiguration ) throw std::bad_alloc();

	s_pParticleSystem	= new Sparkles::ParticleSystem( *s_pConfiguration );
	if ( !s_pParticleSystem ) throw std::bad_alloc();
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void UpdateParticleSystem( float dt )
{
	static float			theta		= 0.f;
	
//	theta += Math::TWO_PI * dt;
//	Vector3 const	epos( cos( theta ) * 10.f, 0.f, sin( theta ) * 10.f );
//	Vector3 const	evel( -sin( theta ) * 10.f * Math::TWO_PI, 0.f, cos( theta ) * 10.f * Math::TWO_PI );

//	s_pParticleSystem[0]->Update( epos, evel );

	s_pParticleSystem->Update( dt, *s_pCamera );
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void CleanupParticleSystem()
{
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void DisplayParticles()
{
	Glx::Enable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	s_pParticleSystem->Draw();

//	glPointSize( 1.f ); 

}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void DrawHud()
{
	// Switch to ortho projection for 2d

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0., 1., 0., 1., 0., 1. );

	// Reset view for hud

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	Glx::Disable( GL_TEXTURE_2D );

	static DWORD	oldTime		= timeGetTime();
	static DWORD	oldTime2	= timeGetTime();

	DWORD const		newTime	= timeGetTime();
	int const		dt		= newTime - oldTime;
	static int		dt2		= 1;

	static int		frameCounter	= 0;

	if ( ++frameCounter >= 100 )
	{
		dt2 = newTime - oldTime2;
		oldTime2 = newTime;
		frameCounter = 0;
	}

	if ( dt > 0 )
	{
		char buffer [ 256 ];

		sprintf( buffer, "Frame rate: %6.2f fps (%6.2f fps average)", 1. / ( dt * .001 ), 1. / ( dt2 * .00001 ) );

		glColor3f( 1.f, 1.f, 1.f );
		glRasterPos2f( .01f, .01f );
		s_pFont->DrawString( buffer );

		oldTime = newTime;
	}

	// Switch back to perspective projection

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void DrawGrid()
{
	int		i;

	Glx::Enable( GL_DEPTH_TEST );
	glDepthMask( GL_TRUE );

	glColor3fv( Glx::Rgba( .5f, .5f, .5f, .5f).m_C );
	glLineWidth( 1.f );

	glBegin( GL_LINES );

	for ( i = -100; i <= 100; i += 10 )
	{
		glVertex3f( float( i ), 0.f, -100.f );
		glVertex3f( float( i ), 0.f, 100.f );
	}

	for ( i = -100; i <= 100; i += 10 )
	{
		glVertex3f( -100.f, 0.f, float( i ) );
		glVertex3f(  100.f, 0.f, float( i ) );
	}

	glEnd();
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void Display()
{
//	glCullFace( GL_BACK );
//	Glx::Enable( GL_CULL_FACE );

	// Clear the framebuffer and depth buffer

	Glx::Enable( GL_DEPTH_TEST );
	glDepthMask( GL_TRUE );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Reset transformations

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Place the camera

	s_pCamera->Look();

	// Lights

//	s_pDirectionalLight->Apply();

	// Draw grid

	DrawGrid();

	// Draw the particles

	DisplayParticles();

	// Draw the hud

	DrawHud();

	// Display the scene

	glFlush();
	SwapBuffers( wglGetCurrentDC() );

	Glx::ReportAnyErrors();
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void Reshape( int w, int h )
{
	glViewport( 0, 0, GLsizei( w ), GLsizei( h ) );

	s_pCamera->Reshape( w, h );
}


