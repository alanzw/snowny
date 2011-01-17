#include "StdAfx.h"
#include "Input.h"

#include <dinput.h>

LPDIRECTINPUT8			g_pDI; 
LPDIRECTINPUTDEVICE8	g_pJoystick;
DIJOYSTATE g_js;

//-----------------------------------------------------------------------------
// Name: EnumJoysticksCallback()
// Desc: Called once for each enumerated joystick. If we find one, create a
//       device interface on it so we can play with it.
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
									VOID* pContext )
{
	// Obtain an interface to the enumerated joystick.
	HRESULT hr = g_pDI->CreateDevice( pdidInstance->guidInstance, &g_pJoystick, NULL );

	// If it failed, then we can't use this joystick. (Maybe the user unplugged
	// it while we were in the middle of enumerating it.)
	if( FAILED( hr ) )
		return DIENUM_CONTINUE;

	// Stop enumeration. Note: we're just taking the first joystick we get. You
	// could store all the enumerated joysticks and let the user pick.
	return DIENUM_STOP;
}

//-----------------------------------------------------------------------------
// Name: EnumObjectsCallback()
// Desc: Callback function for enumerating objects (axes, buttons, POVs) on a 
//       joystick. This function enables user interface elements for objects
//       that are found to exist, and scales axes min/max values.
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
								  VOID* pContext )
{
	HWND hDlg = ( HWND )pContext;

	static int nSliderCount = 0;  // Number of returned slider controls
	static int nPOVCount = 0;     // Number of returned POV controls

	// For axes that are returned, set the DIPROP_RANGE property for the
	// enumerated axis in order to scale min/max values.
	if( pdidoi->dwType & DIDFT_AXIS )
	{
		DIPROPRANGE diprg;
		diprg.diph.dwSize = sizeof( DIPROPRANGE );
		diprg.diph.dwHeaderSize = sizeof( DIPROPHEADER );
		diprg.diph.dwHow = DIPH_BYID;
		diprg.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
		diprg.lMin = -1000;
		diprg.lMax = +1000;

		// Set the range for the axis
		if( FAILED( g_pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph ) ) )
			return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}

//-----------------------------------------------------------------------------
// Name: InitDirectInput()
// Desc: Initialize the DirectInput variables.
//-----------------------------------------------------------------------------
HRESULT InitDirectInput(HWND wnd)
{
	HRESULT hr;

	// Register with the DirectInput subsystem and get a pointer
	// to a IDirectInput interface we can use.
	// Create a DInput object
	if( FAILED( hr = DirectInput8Create( GetModuleHandle( NULL ), DIRECTINPUT_VERSION,
		IID_IDirectInput8, ( VOID** )&g_pDI, NULL ) ) )
		return hr;

	// Look for a simple joystick we can use for this sample program.
	if( FAILED( hr = g_pDI->EnumDevices( DI8DEVCLASS_GAMECTRL,
		EnumJoysticksCallback,
		NULL, DIEDFL_ATTACHEDONLY ) ) )
		return hr;

	// Make sure we got a joystick
	if( NULL == g_pJoystick )
	{
		MessageBox( NULL, TEXT( "Joystick not found. The sample will now exit." ),
			TEXT( "DirectInput Sample" ),
			MB_ICONERROR | MB_OK );
		return S_OK;
	}

	// Set the data format to "simple joystick" - a predefined data format 
	//
	// A data format specifies which controls on a device we are interested in,
	// and how they should be reported. This tells DInput that we will be
	// passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
	if( FAILED( hr = g_pJoystick->SetDataFormat( &c_dfDIJoystick ) ) )
		return hr;

	// Set the cooperative level to let DInput know how this device should
	// interact with the system and with other DInput applications.
	if( FAILED( hr = g_pJoystick->SetCooperativeLevel( wnd, DISCL_EXCLUSIVE |
		DISCL_FOREGROUND ) ) )
		return hr;

	// Enumerate the joystick objects. The callback function enabled user
	// interface elements for objects that are found, and sets the min/max
	// values property for discovered axes.
	if( FAILED( hr = g_pJoystick->EnumObjects( EnumObjectsCallback,
		( VOID* )wnd, DIDFT_ALL ) ) )
		return hr;

	return S_OK;
}

HRESULT UpdateInputState()
{
	HRESULT hr;
	DIJOYSTATE js;           // DInput joystick state 

	if( NULL == g_pJoystick )
		return S_OK;

	// Poll the device to read the current state
	hr = g_pJoystick->Poll();
	if( FAILED( hr ) )
	{
		// DInput is telling us that the input stream has been
		// interrupted. We aren't tracking any state between polls, so
		// we don't have any special reset that needs to be done. We
		// just re-acquire and try again.
		hr = g_pJoystick->Acquire();
		while( hr == DIERR_INPUTLOST )
			hr = g_pJoystick->Acquire();

		// hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
		// may occur when the app is minimized or in the process of 
		// switching, so just try again later 
		return S_OK;
	}

	// Get the input's device state
	if( FAILED( hr = g_pJoystick->GetDeviceState( sizeof( DIJOYSTATE ), &g_js ) ) )
		return hr; // The device should have been acquired during the Poll()

	return S_OK;
}


CInput::CInput()
{
}

CInput::~CInput(void)
{
}

BOOL CInput::Initialize( HWND wnd )
{
	return SUCCEEDED(InitDirectInput(wnd));
}

BOOL CInput::IsKeyDown( BYTE keyid )
{
	if (GetAsyncKeyState(keyid))
	{
		return TRUE;
	}
	UpdateInputState();
	switch (keyid)
	{
	case VK_UP:
		return g_js.lY < 0;
	case VK_DOWN:
		return g_js.lY > 0;
	case VK_LEFT:
		return g_js.lX < 0;
	case VK_RIGHT:
		return g_js.lX > 0;
	case VK_F2:
		return g_js.rgbButtons[0] & 0x80; 
	}

	return FALSE;
}