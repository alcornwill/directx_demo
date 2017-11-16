//**************************************************************************//
// This is a modified version of the Microsoft sample code and loads a mesh.//
// it uses the helper class CDXUTSDKMesh, as there is no longer any built in//
// support for meshes in DirectX 11.										//
//																			//
// The CDXUTSDKMesh is NOT DorectX, not is the file format it uses, the		//
// .sdkmesh, a standard file format. You will hnot find the .sdkmesh format	//
// outside the MS sample code.  Both these things are provided as an entry	//
// point only.																//
//																			//
// Look for the Nigel style comments, like these, for the bits you need to  //
// look at.																	//
//																			//
// You may notice that this sample tries to create a DirectX11 rendering	//
// device, and if it can't do that creates a DirectX 9 device.  I'm not		//
// using DirectX9.															//
//**************************************************************************//


//**************************************************************************//
// Modifications to the MS sample code is copyright of Dr Nigel Barlow,		//
// lecturer in computing, University of Plymouth, UK.						//
// email: nigel@soc.plymouth.ac.uk.											//
//																			//
// Sdkmesh added to MS sample Tutorial09.									//
//																			//
// You may use, modify and distribute this (rather cack-handed in places)	//
// code subject to the following conditions:								//
//																			//
//	1:	You may not use it, or sell it, or use it in any adapted form for	//
//		financial gain, without my written premission.						//
//																			//
//	2:	You must not remove the copyright messages.							//
//																			//
//	3:	You should correct at least 10% of the typig abd spekking errirs.   //
//**************************************************************************//


//--------------------------------------------------------------------------------------
// File: Tutorial 09 - Meshes Using DXUT Helper Classes.cpp
//
// This sample shows a simple example of the Microsoft Direct3D's High-Level 
// Shader Language (HLSL) using the Effect interface. 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


#include "DXUT.h"
#include "DXUTgui.h"
#include "DXUTsettingsDlg.h"
#include "SDKmisc.h"

#include "Camera.h"
#include "ShaderPair.h"
#include "ObjMeshInfo.h"
#include "WalkingObject.h"

#include "gameContext.h"
#include "EventManager.h"
#include "RenderingContext.h"
#include "InputManager.h"
#include "GameObjectManager.h"
#include "MeshManager.h"
#include "FollowPlayer.h"
#include "ShellRenderer.h"

// UI control IDs
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4

// this is used to reduce the amount of global variables
GameContext * gameContext;

CDXUTDialogResourceManager  g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg             g_D3DSettingsDlg;       // Device settings dialog
CDXUTDialog                 g_HUD;                  // manages the 3D   
CDXUTDialog                 g_SampleUI;             // dialog for sample specific controls
CDXUTTextHelper*            g_pTxtHelper = nullptr;

bool                        g_bShowHelp = false;    // If true, it renders the UI control text
int							g_width = 800;
int							g_height = 600;

UINT                        g_iCBPSPerObjectBind = 0;
UINT                        g_iCBPSPerFrameBind = 1;

int mousePosX;
int mousePosY;

// Forward declarations 
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );

bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext );
void CALLBACK OnD3D11DestroyDevice( void* pUserContext );
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext );

void InitApp();
void RenderText();
void charStrToWideChar(WCHAR *dest, char *source);
void MyInit();
float GetDistanceFromLine(vector3 A, vector3 B, vector3 P);

// enter application
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	MyInit();

	// Set DXUT callbacks.														
	DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( OnKeyboard ); 
    DXUTSetCallbackFrameMove( OnFrameMove );

    DXUTSetCallbackD3D11DeviceAcceptable( IsD3D11DeviceAcceptable );
    DXUTSetCallbackD3D11DeviceCreated( OnD3D11CreateDevice );
    DXUTSetCallbackD3D11SwapChainResized( OnD3D11ResizedSwapChain );
    DXUTSetCallbackD3D11FrameRender( OnD3D11FrameRender );
    DXUTSetCallbackD3D11SwapChainReleasing( OnD3D11ReleasingSwapChain );
    DXUTSetCallbackD3D11DeviceDestroyed( OnD3D11DestroyDevice );

    InitApp();
    DXUTInit( true, true, nullptr ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"Endless Grass" );
    DXUTCreateDevice (D3D_FEATURE_LEVEL_9_2, true, 800, 600 );
    //DXUTCreateDevice(true, 640, 480);
    DXUTMainLoop(); // Enter into the DXUT render loop

    return DXUTGetExitCode();
}

#ifndef PI
# define PI           3.14159265358979323846  /* pi */
#endif

void MyInit()
{
	gameContext = new GameContext();

	// create gameobjects
	GameObject *player = new WalkingObject(); 
	player->scale = vector3(10, 10, 10);

	Camera *primaryCamera = new Camera();
	gameContext->renderingContext->primaryCamera = primaryCamera; // set as primary camera
	primaryCamera->parent = player;

	FollowPlayer * skybox = new FollowPlayer(0);
	skybox->player = player;
	skybox->scale = vector3(10000, 10000, 10000);
	skybox->shaderPair = gameContext->renderingContext->shaders[1]; // unlit shader

	FollowPlayer * clouds = new FollowPlayer(1);
	clouds->player = player;
	clouds->scale = vector3(10000, 1, 10000);
	clouds->positionOffset = vector3(0, 1000, 0);
	clouds->shaderPair = gameContext->renderingContext->shaders[3]; // cloud shader

	// also make some rocks
	GameObject * rock1 = new GameObject(2);
	rock1->scale = vector3(10, 10, 10);
	rock1->position = vector3(25, -5, -33);
    rock1->shaderPair = gameContext->renderingContext->shaders[1]; // unlit shader

	GameObject * rock2 = new GameObject(3);
	rock2->scale = vector3(10, 10, 10);
	rock2->position = vector3(-60, -5, 140);
	rock2->shaderPair = gameContext->renderingContext->shaders[1]; // unlit shader

	GameObject * rock3 = new GameObject(4);
	rock3->scale = vector3(10, 10, 10);
	rock3->position = vector3(-170, -5, 84);
	rock3->shaderPair = gameContext->renderingContext->shaders[1]; // unlit shader

	GameObject * rock4 = new GameObject(5);
	rock4->scale = vector3(10, 10, 10);
	rock4->position = vector3(130, -5, 105);
	rock4->shaderPair = gameContext->renderingContext->shaders[1]; // unlit shader

	GameObject * rock5 = new GameObject(6);
	rock5->scale = vector3(10, 10, 10);
	rock5->position = vector3(190, -5, -180);
	rock5->shaderPair = gameContext->renderingContext->shaders[1]; // unlit shader

	// the grass - this is the shell textured bit. mesh is generated.
	FollowPlayer *grass = new FollowPlayer(gameContext->meshManager->generateGrid(100, 100, 100, 100, true, L"Media\\Grass\\grass.png"));
	grass->player = player;
	grass->scale = vector3(4, 1, 4);
	grass->renderer = gameContext->renderingContext->renderers[1]; // shell renderer
	grass->shaderPair = gameContext->renderingContext->shaders[2]; // shell renderer shader

	// couldn't get XACT sound to work anymore...
	PlaySound(L"Media\\Sounds\\wind.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    // Initialize dialogs
    g_D3DSettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.Init( &g_DialogResourceManager );
    g_SampleUI.Init( &g_DialogResourceManager );

    g_HUD.SetCallback( OnGUIEvent ); int iY = 10;
    // g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 0, iY, 170, 23 ); this is totally broken for some reason
    // g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 0, iY += 26, 170, 23, VK_F3 );
    // g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 0, iY += 26, 170, 23, VK_F2 );

    g_SampleUI.SetCallback( OnGUIEvent ); iY = 10;
}


// Called right before creating a D3D9 or D3D11 device, allowing the app to modify the device settings as needed
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    // Uncomment this to get debug information from D3D11
    pDeviceSettings->d3d11.CreateFlags |= D3D11_CREATE_DEVICE_DEBUG;

    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( ( DXUT_D3D11_DEVICE == pDeviceSettings->ver &&
              pDeviceSettings->d3d11.DriverType == D3D_DRIVER_TYPE_REFERENCE ) )
        {
            DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
        }
    }

    return true;
}

void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	// doesn't matter who updates first so might as well be an event
	__raise gameContext->eventManager->OnFrameMove(fTime, fElapsedTime);
}

// Render the help and statistics text
void RenderText()
{
    UINT nBackBufferHeight = ( DXUTIsAppRenderingWithD3D9() ) ? DXUTGetD3D9BackBufferSurfaceDesc()->Height :
            DXUTGetDXGIBackBufferSurfaceDesc()->Height;

    g_pTxtHelper->Begin();
    g_pTxtHelper->SetInsertionPos( 2, 0 );
    g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    g_pTxtHelper->DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
    g_pTxtHelper->DrawTextLine( DXUTGetDeviceStats() );

	wchar_t message[22];
	wsprintf(message, L"MousePos: %i, %i \n", mousePosX, mousePosY);
	g_pTxtHelper->DrawTextLine(message);

    // Draw help
    if( g_bShowHelp )
    {
        g_pTxtHelper->SetInsertionPos( 2, nBackBufferHeight - 20 * 6 );
        g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
        g_pTxtHelper->DrawTextLine( L"Controls:" );

        g_pTxtHelper->SetInsertionPos( 20, nBackBufferHeight - 20 * 5 );
        g_pTxtHelper->DrawTextLine( L"Move around: Cursor keys\n"
			                        L"Change camera: space\n");

        g_pTxtHelper->SetInsertionPos( 550, nBackBufferHeight - 20 * 5 );
        g_pTxtHelper->DrawTextLine( L"Hide help: F1\n"
                                    L"Quit: ESC\n");
    }
    else
    {
        g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
        g_pTxtHelper->DrawTextLine( L"Press F1 for help" );
    }

    g_pTxtHelper->End();
}

// Handle messages to the application
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
	void* pUserContext)
{
	// using raw input because stack overflow told me not to use DirectInput
	// how would you move first person style camera with mouse, re-center the cursor every frame?
	switch (uMsg)
	{
		case WM_MOUSEMOVE:
		{
			mousePosX = LOWORD(lParam);
			mousePosY = HIWORD(lParam);
			break;
		}
	}

	// Pass messages to dialog resource manager calls so GUI state is updated correctly
	*pbNoFurtherProcessing = g_DialogResourceManager.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;

	// Pass messages to settings dialog if its active
	if (g_D3DSettingsDlg.IsActive())
	{
		g_D3DSettingsDlg.MsgProc(hWnd, uMsg, wParam, lParam);
		return 0;
	}

	// Give the dialogs a chance to handle the message first
	*pbNoFurtherProcessing = g_HUD.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;
	*pbNoFurtherProcessing = g_SampleUI.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;

	return 0;
}

// Handle key presses 
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
    if( bKeyDown )
    {
        switch( nChar )
        {
            case VK_F1:
                g_bShowHelp = !g_bShowHelp; break;
		}
    }

	// player controls go in the input manager
	gameContext->inputManager->onKeyboard(nChar, bKeyDown);
}

// Handles the GUI events
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN:
            DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:
            DXUTToggleREF(); break;
        case IDC_CHANGEDEVICE:
            g_D3DSettingsDlg.SetActive( !g_D3DSettingsDlg.IsActive() ); break;
    }
}

// Reject any D3D11 devices that aren't acceptable by returning false
bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    return true;
}

// Create any D3D11 resources that aren't dependant on the back buffer
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                      void* pUserContext )
{
    HRESULT hr;

    ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();
    V_RETURN( g_DialogResourceManager.OnD3D11CreateDevice( pd3dDevice, pd3dImmediateContext ) );
    V_RETURN( g_D3DSettingsDlg.OnD3D11CreateDevice( pd3dDevice ) );
    g_pTxtHelper = new CDXUTTextHelper(pd3dDevice, pd3dImmediateContext, &g_DialogResourceManager, 15 );

	gameContext->renderingContext->constructShaders(pd3dDevice);
	gameContext->meshManager->createMeshes(pd3dDevice);

    return S_OK;
}

// Create any D3D11 resources that depend on the back buffer
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D11ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );
    V_RETURN( g_D3DSettingsDlg.OnD3D11ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );

	g_width  = pBackBufferSurfaceDesc->Width;
	g_height = pBackBufferSurfaceDesc->Height;

	// We need to recalculate this every time we resize the screen, different aspect ratio
	gameContext->renderingContext->recalculateProjectionMatrix(g_width, g_height);

	// adjust UI position
	g_HUD.SetLocation( g_width - 170, 0 );
    g_HUD.SetSize( 170, 170 );
	g_SampleUI.SetLocation( g_width - 170, g_height - 300 );
    g_SampleUI.SetSize( 170, 300 );

    return S_OK;
}

// Render the scene using the D3D11 device
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
                                  float fElapsedTime, void* pUserContext )
{
    HRESULT hr;

    // If the settings dialog is being shown, then render it instead of rendering the app's scene
    if( g_D3DSettingsDlg.IsActive() )
    {
        g_D3DSettingsDlg.OnRender( fElapsedTime );
        return;
    }

    // Clear the render target and depth stencil
    float ClearColor[4] = { 0.0f, 0.25f, 0.25f, 0.55f };
    ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();
    pd3dImmediateContext->ClearRenderTargetView( pRTV, ClearColor );
    ID3D11DepthStencilView* pDSV = DXUTGetD3D11DepthStencilView();
    pd3dImmediateContext->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH, 1.0, 0 );

	// set blend state
	pd3dImmediateContext->OMSetBlendState(gameContext->renderingContext->blendState, 0, 0xffffffff);

	// update per frame constant buffers
	gameContext->renderingContext->renderPerFrame(pd3dDevice, pd3dImmediateContext);
	
	// update transformation matrices
	gameContext->gameObjectManager->updateTransforms();

	// update viewMatrix
	// we'd probably want to do this for each camera, if there were more than 1
	gameContext->renderingContext->primaryCamera->update(fTime, fElapsedTime);

	// render
	gameContext->gameObjectManager->render(pd3dDevice, pd3dImmediateContext);

	//**************************************************************************//
	// Render what is rather grandly called the head up display.				//
	//**************************************************************************//
	DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"HUD / Stats" );
    g_HUD.OnRender( fElapsedTime );
    g_SampleUI.OnRender( fElapsedTime );
    RenderText();
    DXUT_EndPerfEvent();
}

//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext )
{
    g_DialogResourceManager.OnD3D11ReleasingSwapChain();
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D11DestroyDevice();
    g_D3DSettingsDlg.OnD3D11DestroyDevice();
    DXUTGetGlobalResourceCache().OnDestroyDevice();
    SAFE_DELETE( g_pTxtHelper );

	ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();
	SAFE_RELEASE(pRTV);
	ID3D11DepthStencilView* pDSV = DXUTGetD3D11DepthStencilView();
	SAFE_RELEASE(pDSV);

	// destroy camera
	delete gameContext->renderingContext->primaryCamera;

	// this should release all directX resources and release all memory allocated with new
	delete gameContext;
	GameInfo::gameContext = NULL;

	//ID3D11Device *pd3dDevice = DXUTGetD3D11Device();
	//ID3D11Debug *d3dDebug = NULL;
	//pd3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	//d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL); 
	//SAFE_RELEASE(d3dDebug);
}