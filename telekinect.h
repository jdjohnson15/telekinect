//------------------------------------------------------------------------------
// <copyright file="ColorBasics.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once
#include "ByteSender.h"
#include "globals.h"
#include "ImageRenderer.h"
#include "resource.h"
#include "NuiApi.h"
#include "lodepng.h"
#include <thread> 
#include <vector>
#include <Winuser.h>
#include <time.h>
#include <thread>
#include <chrono>
//#include <system.net.socket>

class telekinect
{
    static const int        cColorWidth  = 640;
    static const int        cColorHeight = 480;

    static const int        cStatusMessageMaxLen = MAX_PATH*2;

public:
    /// <summary>
    /// Constructor
    /// </summary>
	telekinect();

    /// <summary>
    /// Destructor
    /// </summary>
    ~telekinect();

    /// <summary>
    /// Handles window messages, passes most to the class instance to handle
    /// </summary>
    /// <param name="hWnd">window message is for</param>
    /// <param name="uMsg">message</param>
    /// <param name="wParam">message data</param>
    /// <param name="lParam">additional message data</param>
    /// <returns>result of message processing</returns>
    static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Handle windows messages for a class instance
    /// </summary>
    /// <param name="hWnd">window message is for</param>
    /// <param name="uMsg">message</param>
    /// <param name="wParam">message data</param>
    /// <param name="lParam">additional message data</param>
    /// <returns>result of message processing</returns>
    LRESULT CALLBACK        DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Creates the main window and begins processing
    /// </summary>
    /// <param name="hInstance"></param>
    /// <param name="nCmdShow"></param>
    int                     Run(HINSTANCE hInstance, int nCmdShow);

	void launchServer();
	void processData();

private:

	char*					pngBits;

	bool					printPNG;

	BYTE*					m_depthRGBX;

	BYTE*					displayPBits;

	int						displayPSize;
	
	int						minRange;

	int						maxRange;

	int						colorOffsetX;

	int						colorOffsetY;

    HWND                    m_hWnd;

    bool                    m_bSaveScreenshot;

	bool					recordingStarted;

	int frameCount;

    // Current Kinect
    INuiSensor*             m_pNuiSensor;

	//server object
	ByteSender*				server;
	std::thread				connetionAttempt;
	enum					Connecting{NO_ATTEMPT, ATTEMPTING, SUCCESSFUL};
	Connecting				netstatus;

    // Direct2D
    ImageRenderer*          m_pDraw;
	ImageRenderer*          m_pDraw2;
    ID2D1Factory*           m_pD2DFactory;
    
    HANDLE                  m_pColorStreamHandle;
	HANDLE					m_pDepthStreamHandle;
    HANDLE                  m_hNextColorFrameEvent;
	HANDLE                  m_hNextDepthFrameEvent;

    /// <summary>
    /// Main processing function
    /// </summary>
    void                    Update();

    /// <summary>
    /// Create the first connected Kinect found 
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code</returns>
    HRESULT                 CreateFirstConnected();

    /// <summary>
    /// Handle new color data
    /// </summary>
    void                    ProcessColor();

	/// <summary>
	/// Handle new depth data
	/// </summary>
	void                    ProcessDepth();

	/// <summary>
	/// Handle drawing content on the screen and exporting files
	/// </summary>
	void					Render();

    /// <summary>
    /// Set the status bar message
    /// </summary>
    /// <param name="szMessage">message to display</param>
    void                    SetStatusMessage(WCHAR* szMessage);

	/// <summary>
	/// Set the network status message
	/// </summary>
	/// <param name="szMessage">message to display</param>
	void telekinect::SetNetworkStatus(WCHAR * szMessage);


	/// <summary>
	/// Save combined depth and color image data at current frame to disk as a png
	/// </summary>
	/// <param name="pngBits">image data to save</param>
	/// <param name="lWidth">width (in pixels) of input image data</param>
	/// <param name="lHeight">height (in pixels) of input image data</param>
	/// <returns>S_OK on success, otherwise failure code</returns>
	HRESULT                 SaveBitmapToFile(const unsigned char* pngBits, LONG lWidth, LONG lHeight, LPCWSTR lpszFilePath);


	/// <summary>
	/// Smooths depth image
	/// </summary>
	/// <param name="pBitmapBits">image data to save</param>
	/// <param name="lWidth">width (in pixels) of input image data</param>
	/// <param name="lHeight">height (in pixels) of input image data</param>
	/// <param name="wBitsPerPixel">bits per pixel of image data</param>
	void					SmoothDepth(char* depthPNG, BYTE* pBitmapBits, BYTE* displayPBits, LONG width, LONG height, NUI_LOCKED_RECT LockedRect, UINT smooth, UINT average);

};
