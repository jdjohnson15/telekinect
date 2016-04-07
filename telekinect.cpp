
#include "stdafx.h"
#include <strsafe.h>
#include "telekinect.h"
#include "resource.h"
#include <queue>
#include <sstream>


std::vector<unsigned char> pngBits;

/// <summary>
/// Entry point for the application
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="hPrevInstance">always 0</param>
/// <param name="lpCmdLine">command line arguments</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
/// <returns>status</returns>
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	telekinect application;
    application.Run(hInstance, nCmdShow);
}

/// <summary>
/// Constructor
/// </summary>
telekinect::telekinect() :
	m_pD2DFactory(NULL),
	m_pDraw(NULL),
	m_pDraw2(NULL),
	m_hNextColorFrameEvent(INVALID_HANDLE_VALUE),
	m_hNextDepthFrameEvent(INVALID_HANDLE_VALUE),
	m_pColorStreamHandle(INVALID_HANDLE_VALUE),
	m_pDepthStreamHandle(INVALID_HANDLE_VALUE),
	minRange(DEPTH_SLIDER_MIN_MM),
	maxRange(DEPTH_SLIDER_MAX_MM),
	m_bSaveScreenshot(false),
	recordingStarted(false),
	frameCount(1),
	m_pNuiSensor(NULL),
	colorOffsetX(0),
	colorOffsetY(0),
	printPNG(false),
	netstatus(NO_ATTEMPT)
{
	m_depthRGBX = new BYTE[480 * 640 * 4];
	pngBits = new char[480 * 640 * 8];

}

/// <summary>
/// Destructor
/// </summary>
telekinect::~telekinect()
{
    if (m_pNuiSensor)
    {
        m_pNuiSensor->NuiShutdown();
    }

    if (m_hNextColorFrameEvent != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hNextColorFrameEvent);
    }
	if (m_hNextDepthFrameEvent != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hNextDepthFrameEvent);
	}

    // clean up Direct2D renderer
    delete m_pDraw;
    m_pDraw = NULL;
	delete m_pDraw2;
	m_pDraw2 = NULL;

	if (netstatus == SUCCESSFUL)
		connetionAttempt.join();

	//clean up server
	//delete server;
	//server = NULL;

    // clean up Direct2D
    SafeRelease(m_pD2DFactory);

    SafeRelease(m_pNuiSensor);
}

/// <summary>
/// Creates the main window and begins processing
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
int telekinect::Run(HINSTANCE hInstance, int nCmdShow)
{
    MSG       msg = {0};
    WNDCLASS  wc;

    // Dialog custom window class
    ZeroMemory(&wc, sizeof(wc));
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.cbWndExtra    = DLGWINDOWEXTRA;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
    wc.hIcon         = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_APP));
    wc.lpfnWndProc   = DefDlgProcW;
    wc.lpszClassName = L"ColorBasicsAppDlgWndClass";

    if (!RegisterClassW(&wc))
    {
        return 0;
    }

    // Create main application window
    HWND hWndApp = CreateDialogParamW(
        hInstance,
        MAKEINTRESOURCE(IDD_APP),
        NULL,
        (DLGPROC)telekinect::MessageRouter,
        reinterpret_cast<LPARAM>(this));

    // Show window
    ShowWindow(hWndApp, nCmdShow);

    const int eventCount = 1;
    HANDLE hEvents[eventCount];

	if (msg.message == WM_KEYDOWN) {
		OutputDebugString(L"it works\n");
	}

    // Main message loop
    while (WM_QUIT != msg.message)
    {
        hEvents[0] = m_hNextDepthFrameEvent;

        // Check to see if we have either a message (by passing in QS_ALLINPUT)
        // Or a Kinect event (hEvents)
        // Update() will check for Kinect events individually, in case more than one are signalled
        MsgWaitForMultipleObjects(eventCount, hEvents, FALSE, INFINITE, QS_ALLINPUT);

        // Explicitly check the Kinect frame event since MsgWaitForMultipleObjects
        // can return for other reasons even though it is signaled.
        Update();

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // If a dialog message will be taken care of by the dialog proc
            if ((hWndApp != NULL) && IsDialogMessageW(hWndApp, &msg))
            {
                continue;
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    return static_cast<int>(msg.wParam);
}

/// <summary>
/// Main processing function
/// </summary>
void telekinect::Update()
{
    if (NULL == m_pNuiSensor)
    {
        return;
    }

    if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hNextColorFrameEvent, 0) && WAIT_OBJECT_0 == WaitForSingleObject(m_hNextDepthFrameEvent, 0))
    {
        ProcessColor();
		ProcessDepth();
		Render();
    }
}

/// <summary>
/// Handles window messages, passes most to the class instance to handle
/// </summary>
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT CALLBACK telekinect::MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	telekinect* pThis = NULL;
    
    if (WM_INITDIALOG == uMsg)
    {
        pThis = reinterpret_cast<telekinect*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
        pThis = reinterpret_cast<telekinect*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pThis)
    {
        return pThis->DlgProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

/// <summary>
/// Handle windows messages for the class instance
/// </summary>
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT CALLBACK telekinect::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
    switch (message)
    {
        case WM_INITDIALOG:
        {
            // Bind application window handle
            m_hWnd = hWnd;

            // Init Direct2D
            D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);


			//intitialize slider controls

			SendDlgItemMessage(
				m_hWnd,
				IDC_DEPTH_SLIDER_MIN,
				TBM_SETRANGE,
				TRUE,
				MAKELPARAM(DEPTH_SLIDER_MIN_MM, DEPTH_SLIDER_MAX_MM));

			SendDlgItemMessage(
				m_hWnd,
				IDC_DEPTH_SLIDER_MAX,
				TBM_SETRANGE,
				TRUE,
				MAKELPARAM(DEPTH_SLIDER_MIN_MM, DEPTH_SLIDER_MAX_MM));

			SendDlgItemMessage(
				m_hWnd,
				IDC_OFFSET_SLIDER_X,
				TBM_SETRANGE,
				TRUE,
				MAKELPARAM(OFFEST_SLIDER_MIN, OFFEST_SLIDER_MAX));

			SendDlgItemMessage(
				m_hWnd,
				IDC_OFFSET_SLIDER_Y,
				TBM_SETRANGE,
				TRUE,
				MAKELPARAM(OFFEST_SLIDER_MIN, OFFEST_SLIDER_MAX));

			// Set slider positions
			SendDlgItemMessage(
				m_hWnd,
				IDC_DEPTH_SLIDER_MAX,
				TBM_SETPOS,
				TRUE,
				DEPTH_SLIDER_MAX_MM);

			SendDlgItemMessage(
				m_hWnd,
				IDC_DEPTH_SLIDER_MIN,
				TBM_SETPOS,
				TRUE,
				DEPTH_SLIDER_MIN_MM);

			SendDlgItemMessage(
				m_hWnd,
				IDC_OFFSET_SLIDER_X,
				TBM_SETPOS,
				TRUE,
				0);

			SendDlgItemMessage(
				m_hWnd,
				IDC_OFFSET_SLIDER_Y,
				TBM_SETPOS,
				TRUE,
				0);


			// Update slider text
			
			WCHAR str[MAX_PATH];
			swprintf_s(str, ARRAYSIZE(str), L"%d", minRange);
			SetDlgItemText(m_hWnd, IDC_MIN_DIST_TEXT, str);
			swprintf_s(str, ARRAYSIZE(str), L"%d",maxRange);
			SetDlgItemText(m_hWnd, IDC_MAX_DIST_TEXT, str);
			swprintf_s(str, ARRAYSIZE(str), L"%d", 0);
			SetDlgItemText(m_hWnd, IDC_X_OFFSET_TEXT, str);
			SetDlgItemText(m_hWnd, IDC_Y_OFFSET_TEXT, str);
			
			//swprintf_s(str, ARRAYSIZE(str), L"%d", m_params.m_cMaxIntegrationWeight);
			//SetDlgItemText(m_hWnd, IDC_INTEGRATION_WEIGHT_TEXT, str);
			



            // Create and initialize a new Direct2D image renderer (take a look at ImageRenderer.h)
            // We'll use this to draw the data we receive from the Kinect to the screen
            m_pDraw = new ImageRenderer();
            HRESULT hr = m_pDraw->Initialize(
				GetDlgItem(m_hWnd, IDC_VIDEOVIEW), 
				m_pD2DFactory, 
				cColorWidth, 
				cColorHeight, 
				cColorWidth * sizeof(long));

            if (FAILED(hr))
            {
                SetStatusMessage(L"Failed to initialize the Direct2D color draw device.");
            }
			m_pDraw2 = new ImageRenderer();
			hr = m_pDraw2->Initialize(
				GetDlgItem(m_hWnd, IDC_DEPTHVIEW), 
				m_pD2DFactory, 
				cColorWidth, 
				cColorHeight, 
				cColorWidth * sizeof(long));

			if (FAILED(hr))
			{
				SetStatusMessage(L"Failed to initialize the Direct2D depth draw device.");
			}
	
			
            // Look for a connected Kinect, and create it if found
            CreateFirstConnected();

        }
        break;


		//update sliders
		 case  WM_HSCROLL:
			 minRange = (int)SendDlgItemMessage(m_hWnd, IDC_DEPTH_SLIDER_MIN, TBM_GETPOS, 0, 0);

			 maxRange = (int)SendDlgItemMessage(m_hWnd, IDC_DEPTH_SLIDER_MAX, TBM_GETPOS, 0, 0);

			 colorOffsetX = (int)SendDlgItemMessage(m_hWnd, IDC_OFFSET_SLIDER_X, TBM_GETPOS, 0, 0);

			 colorOffsetY = (int)SendDlgItemMessage(m_hWnd, IDC_OFFSET_SLIDER_Y, TBM_GETPOS, 0, 0);

			 WCHAR str[MAX_PATH];
			 swprintf_s(str, ARRAYSIZE(str), L"%d", minRange);
			 SetDlgItemText(m_hWnd, IDC_MIN_DIST_TEXT, str);
			 swprintf_s(str, ARRAYSIZE(str), L"%d", maxRange);
			 SetDlgItemText(m_hWnd, IDC_MAX_DIST_TEXT, str);
			 swprintf_s(str, ARRAYSIZE(str), L"%d", colorOffsetX);
			 SetDlgItemText(m_hWnd, IDC_X_OFFSET_TEXT, str);
			 swprintf_s(str, ARRAYSIZE(str), L"%d", colorOffsetY);
			 SetDlgItemText(m_hWnd, IDC_Y_OFFSET_TEXT, str);

			 break;

        // If the titlebar X is clicked, destroy app
        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;

        case WM_DESTROY:
            // Quit the main message pump
            PostQuitMessage(0);
            break;

        // Handle button press
        case WM_COMMAND:
            // If it was for the screenshot control and a button clicked event, save a screenshot next frame 
            if (IDC_BUTTON_SCREENSHOT == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
            {
				frameCount = 1;
				m_bSaveScreenshot ? m_bSaveScreenshot = false : m_bSaveScreenshot = true;
            }
			else if (IDC_BUTTON_TOGGLE == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam)) {
				printPNG ? printPNG = false : printPNG = true;
				WCHAR str[MAX_PATH];
				printPNG ? swprintf_s(str, ARRAYSIZE(str), L"printing PNG") : swprintf_s(str, ARRAYSIZE(str), L"printing BMP", colorOffsetY);
				SetDlgItemText(m_hWnd, IDC_BUTTON_TOGGLE, str);
			}
            break;
    }

    return FALSE;
}

/// <summary>
/// Create the first connected Kinect found 
/// </summary>
/// <returns>indicates success or failure</returns>
HRESULT telekinect::CreateFirstConnected()
{
    INuiSensor * pNuiSensor;
    HRESULT hr;

    int iSensorCount = 0;
    hr = NuiGetSensorCount(&iSensorCount);
    if (FAILED(hr))
    {
        return hr;
    }

    // Look at each Kinect sensor
    for (int i = 0; i < iSensorCount; ++i)
    {
        // Create the sensor so we can check status, if we can't create it, move on to the next
        hr = NuiCreateSensorByIndex(i, &pNuiSensor);
        if (FAILED(hr))
        {
            continue;
        }

        // Get the status of the sensor, and if connected, then we can initialize it
        hr = pNuiSensor->NuiStatus();
        if (S_OK == hr)
        {
            m_pNuiSensor = pNuiSensor;
            break;
        }

        // This sensor wasn't OK, so release it since we're not using it
        pNuiSensor->Release();
    }

    if (NULL != m_pNuiSensor)
    {
        // Initialize the Kinect and specify that we'll be using color and depth
        hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_COLOR);
		if (FAILED(hr)) { return hr; }
		// Create an event that will be signaled when color data is available
		m_hNextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

		// Open a color image stream to receive color frames
		hr = m_pNuiSensor->NuiImageStreamOpen(
			NUI_IMAGE_TYPE_COLOR,
			NUI_IMAGE_RESOLUTION_640x480,
			0,
			2,
			m_hNextColorFrameEvent,
			&m_pColorStreamHandle);
		if (FAILED(hr)) { return hr; }
		// Create an event that will be signaled when depth data is available
		m_hNextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		// Open a depth image stream to receive depth frames
		hr = m_pNuiSensor->NuiImageStreamOpen(
			NUI_IMAGE_TYPE_DEPTH,
			NUI_IMAGE_RESOLUTION_640x480,
			0,
			2,
			m_hNextDepthFrameEvent,
			&m_pDepthStreamHandle);
		if (FAILED(hr)) { return hr; }
    }

    if (NULL == m_pNuiSensor || FAILED(hr))
    {
        SetStatusMessage(L"No ready Kinect found!");
        return E_FAIL;
    }

    return hr;
}

/// <summary>
/// Get the name of the file where screenshot will be stored.
/// </summary>
/// <param name="screenshotName">
/// [out] String buffer that will receive screenshot file name.
/// </param>
/// <param name="screenshotNameSize">
/// [in] Number of characters in screenshotName string buffer.
/// </param>
/// <returns>
/// S_OK on success, otherwise failure code.
/// </returns>
HRESULT GetScreenshotFileName(wchar_t *screenshotName, UINT screenshotNameSize, int frameCount)
{
    wchar_t *knownPath = NULL;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Pictures, 0, NULL, &knownPath);

    if (SUCCEEDED(hr))
    {
        // File name will be "frame_$(frameCount)"
        StringCchPrintfW(screenshotName, screenshotNameSize, L"frames/frame_%i.bmp", frameCount);
    }

    CoTaskMemFree(knownPath);
    return hr;
}

/// <summary>
/// Handle new color data
/// </summary>
/// <returns>indicates success or failure</returns>
void telekinect::ProcessColor()
{
    HRESULT hr;
    NUI_IMAGE_FRAME imageFrame;

    // Attempt to get the color frame
    hr = m_pNuiSensor->NuiImageStreamGetNextFrame(m_pColorStreamHandle, 0, &imageFrame);
    if (FAILED(hr))
    {
        return;
    }

    INuiFrameTexture * pTexture = imageFrame.pFrameTexture;
    NUI_LOCKED_RECT LockedRect;

    // Lock the frame data so the Kinect knows not to modify it while we're reading it
    pTexture->LockRect(0, &LockedRect, NULL, 0);

    // Make sure we've received valid data
    if (LockedRect.Pitch != 0)
    {
		displayPSize = LockedRect.size;
		displayPBits = LockedRect.pBits;
    }

    // We're done with the texture so unlock it
    pTexture->UnlockRect(0);

	//process offset
	/*int index;
	BYTE* tempDisplayPBits = new BYTE[480 * 640 * 4];
	for (int i = 0; i < 480; ++i) {//row
		for (int j = 0; j < 640; ++j) {//collumn
			index = (j + colorOffsetX)*4 + (i + colorOffsetY) * 640 * 4; //start of target pixel to be moved into new array
			for (int k = 0; k < 4; ++k) {//RGBA
				if (index < displayPSize) {
					*(tempDisplayPBits++) = displayPBits[index+k];
				}
				else
					*(tempDisplayPBits++) = 0; 
			}
		}	
	}
	//displayPBits = tempDisplayPBits;*/

    // Release the frame
    m_pNuiSensor->NuiImageStreamReleaseFrame(m_pColorStreamHandle, &imageFrame);
}

void telekinect::ProcessDepth() {
	HRESULT hr;
	NUI_IMAGE_FRAME imageFrame;
	
	// Attempt to get the depth frame
	
	hr = m_pNuiSensor->NuiImageStreamGetNextFrame(m_pDepthStreamHandle, 0, &imageFrame);
	if (FAILED(hr))
	{
		return;
	}
	
	BOOL nearMode;
	INuiFrameTexture* pTexture;

	// Get the depth image pixel texture
	hr = m_pNuiSensor->NuiImageFrameGetDepthImagePixelFrameTexture(
		m_pDepthStreamHandle, &imageFrame, &nearMode, &pTexture);
	if (FAILED(hr))
	{
		goto ReleaseFrame;
	}
	NUI_LOCKED_RECT LockedRect;

	BYTE* dummy = m_depthRGBX;
	// Lock the frame data so the Kinect knows not to modify it while we're reading it
	pTexture->LockRect(0, &LockedRect, NULL, 0);

	// Make sure we've received valid data
	
	if (LockedRect.Pitch != 0)
	{

		SmoothDepth(pngBits, dummy, displayPBits, 640, 480, LockedRect,1, 0);
		
	}
	
	// We're done with the texture so unlock it
	pTexture->UnlockRect(0);

	pTexture->Release();

ReleaseFrame:
	// Release the frame
	m_pNuiSensor->NuiImageStreamReleaseFrame(m_pDepthStreamHandle, &imageFrame);
}

void telekinect::Render()
{
	HRESULT hr;

	// Draw the data with Direct2D
	m_pDraw->Draw(displayPBits, displayPSize);
	m_pDraw2->Draw(m_depthRGBX, displayPSize);

	//send image buffer (both color and depth) to remote client
	//server->sendBuffer(pngBits, sizeof(pngBits));
	
	if (netstatus == NO_ATTEMPT)
		connetionAttempt = std::thread(&telekinect::launchServer, this);

	Globals::data = pngBits; //this puts all of the kinect data into the global buffer so all the connected clients can see it.

	WCHAR statusMessage[cStatusMessageMaxLen];

	// If the user pressed the screenshot button, save a screenshot
	if (m_bSaveScreenshot)
	{
		// Write out the bitmap to disk

		TCHAR pszDest[30];
		size_t cchDest = 30;

		LPCTSTR pszFormat = TEXT("%s%d%s");
		TCHAR* f = TEXT("frames/");
		TCHAR* e = TEXT(".bmp");

		HRESULT hr = StringCchPrintf(pszDest, cchDest, pszFormat, f, frameCount, e);

		hr = SaveBitmapToFile(reinterpret_cast<const unsigned char*>(pngBits), cColorWidth, cColorHeight*2, pszDest);

		if (SUCCEEDED(hr))
		{
			// Set the status bar to show where the screenshot was saved
			StringCchPrintf(statusMessage, cStatusMessageMaxLen, L"recording to frames folder");
			++frameCount;
		}
		else
		{
			StringCchPrintf(statusMessage, cStatusMessageMaxLen, L"Failed to write frame file!");
		}

		
	}
	else {
		StringCchPrintf(statusMessage, cStatusMessageMaxLen, L"not recording");
	}
	SetStatusMessage(statusMessage);

}

/// <summary>
/// Set the status bar message
/// </summary>
/// <param name="szMessage">message to display</param>
void telekinect::SetStatusMessage(WCHAR * szMessage)
{
    SendDlgItemMessageW(m_hWnd, IDC_STATUS, WM_SETTEXT, 0, (LPARAM)szMessage);
}

/// <summary>
/// Set the network status message
/// </summary>
/// <param name="szMessage">message to display</param>
void telekinect::SetNetworkStatus(WCHAR * szMessage)
{
	SendDlgItemMessageW(m_hWnd, IDC_STATUS, WM_SETTEXT, 0, (LPARAM)szMessage);
}

/// <summary>
/// Save combined depth and color image data at current frame to disk as a png
/// </summary>
/// <param name="pngBits">image data to save</param>
/// <param name="lWidth">width (in pixels) of input image data</param>
/// <param name="lHeight">height (in pixels) of input image data</param>
/// <returns>S_OK on success, otherwise failure code</returns>
HRESULT telekinect::SaveBitmapToFile(const unsigned char* pngBits, LONG lWidth, LONG lHeight, LPCWSTR lpszFilePath)
{

	if (!printPNG) {

		int wBitsPerPixel = 32;
		DWORD dwByteCount = lWidth * lHeight * (wBitsPerPixel / 8);

		BITMAPINFOHEADER bmpInfoHeader = { 0 };

		bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);  // Size of the header
		bmpInfoHeader.biBitCount = wBitsPerPixel;             // Bit count
		bmpInfoHeader.biCompression = BI_RGB;                    // Standard RGB, no compression
		bmpInfoHeader.biWidth = lWidth;                    // Width in pixels
		bmpInfoHeader.biHeight = -lHeight;                  // Height in pixels, negative indicates it's stored right-side-up
		bmpInfoHeader.biPlanes = 1;                         // Default
		bmpInfoHeader.biSizeImage = dwByteCount;               // Image size in bytes

		BITMAPFILEHEADER bfh = { 0 };

		bfh.bfType = 0x4D42;                                           // 'M''B', indicates bitmap
		bfh.bfOffBits = bmpInfoHeader.biSize + sizeof(BITMAPFILEHEADER);  // Offset to the start of pixel data
		bfh.bfSize = bfh.bfOffBits + bmpInfoHeader.biSizeImage;        // Size of image + headers

		// Create the file on disk to write to
		HANDLE hFile = CreateFileW(lpszFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		// Return if error opening file
		if (NULL == hFile)
		{
			return E_ACCESSDENIED;
		}

		DWORD dwBytesWritten = 0;

		// Write the bitmap file header
		if (!WriteFile(hFile, &bfh, sizeof(bfh), &dwBytesWritten, NULL))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		// Write the bitmap info header
		if (!WriteFile(hFile, &bmpInfoHeader, sizeof(bmpInfoHeader), &dwBytesWritten, NULL))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		// Write  RGB Data
		if (!WriteFile(hFile, pngBits, bmpInfoHeader.biSizeImage, &dwBytesWritten, NULL))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}


		// Close the file
		CloseHandle(hFile);
	}

	else {



		std::vector<unsigned char> png;
		lodepng::State state; //optionally customize this one

		std::ostringstream tempfilename;
		std::string zeroes = "0000";
		tempfilename << "frames/save." << frameCount << ".png";
		const std::string temp = tempfilename.str();
		const char* filename = temp.c_str();

		unsigned char* buffer;
		size_t buffersize;
		unsigned error = lodepng_encode_memory(&buffer, &buffersize, pngBits, lWidth, lHeight, LCT_RGBA, 8);

		if (!error)
			lodepng_save_file(buffer, buffersize, filename);

		else
			return E_FAIL;

	}

	//send pngBits to client

	//SetNetworkStatus(server->sendBuffer(reinterpret_cast<const char*>(pngBits), sizeof(pngBits)));

    return S_OK;
}

/// <summary>
/// Smooths depth image
/// </summary>
/// <param name="depthPNG">image data to save</param>
/// <param name="pBitmapBits">image data to save</param>
/// <param name="lWidth">width (in pixels) of input image data</param>
/// <param name="lHeight">height (in pixels) of input image data</param>
/// <param name="LockedRect">bits per pixel of image data</param>
/// <param name="smooth">bits per pixel of image data</param>
/// <param name="average">bits per pixel of image data</param>
void telekinect::SmoothDepth(char* pngBits, BYTE* depthBits, BYTE* displayPBits, LONG width, LONG height, NUI_LOCKED_RECT LockedRect, UINT smooth, UINT average)
{
	const int length = 480 * 640;
	short depthArray[length];

	std::queue <short> depthQueue;

	const NUI_DEPTH_IMAGE_PIXEL * pBufferRun = reinterpret_cast<const NUI_DEPTH_IMAGE_PIXEL *>(LockedRect.pBits);
	const NUI_DEPTH_IMAGE_PIXEL * pBufferEnd = pBufferRun + (cColorWidth * cColorHeight);
	int index = 0;
	// Process each pixel
	while (pBufferRun < pBufferEnd)
	{
		depthArray[index++] = pBufferRun->depth;
		++pBufferRun;
	}

	if (smooth) {
		int widthBound = width - 1;
		int heightBound = height - 1;
		// 	Process each pixel in the row
		for (int depthArrayRowIndex = 0; depthArrayRowIndex < height; depthArrayRowIndex++)
		{
			for (int depthArrayColumnIndex = 0; depthArrayColumnIndex < width; depthArrayColumnIndex++)
			{
				int depthIndex = depthArrayColumnIndex + (depthArrayRowIndex * width);

				// consider any pixel with a depth of 0 as a possible candidate for filtering.
				if (depthArray[depthIndex] == 0)
				{
					// From the depth index, we can determine the X and Y coordinates that the index
					// will appear in the image. We use this to help us define our filter matrix.
					int x = depthIndex % width;
					int y = (depthIndex - x) / width;

					short filterCollection[24][2];

					// The inner and outer band counts are used later to compare against the threshold 
					// values set in the UI to identify a positive filter result.
					int innerBandCount = 0;
					int outerBandCount = 0;

					for (int yi = -2; yi < 3; yi++)
					{
						for (int xi = -2; xi < 3; xi++)
						{

							if (xi != 0 || yi != 0)
							{
								// We then create our modified coordinates for each pass
								int xSearch = x + xi;
								int ySearch = y + yi;


								if (xSearch >= 0 && xSearch <= widthBound &&
									ySearch >= 0 && ySearch <= heightBound)
								{
									int index = xSearch + (ySearch * width);
									// We only want to look for non-0 values
									if (depthArray[index] != 0)
									{
										// We want to find count the frequency of each depth
										for (int i = 0; i < 24; i++)
										{
											if (filterCollection[i][0] == depthArray[index])
											{

												filterCollection[i][1]++;
												break;
											}
											else if (filterCollection[i, 0] == 0)
											{

												filterCollection[i][0] = depthArray[index];
												filterCollection[i][1]++;
												break;
											}
										}

										if (yi != 2 && yi != -2 && xi != 2 && xi != -2)
											innerBandCount++;
										else
											outerBandCount++;
									}
								}
							}
						}
					}


					if (innerBandCount >= 1 || outerBandCount >= 4)
					{
						short frequency = 0;
						short depth = 0;
						// This loop will determine the statistical mode
						// of the surrounding pixels for assignment to
						// the candidate.
						for (int i = 0; i < 24; i++)
						{
							// This means we have reached the end of our
							// frequency distribution and can break out of the
							// loop to save time.
							if (filterCollection[i][0] == 0)
								break;
							if (filterCollection[i][1] > frequency)
							{
								depth = filterCollection[i][0];
								frequency = filterCollection[i][1];
							}
						}
						depthArray[depthIndex] = depth;
						if (average)
							depthQueue.push(depth);
					}
				}
			}
		}
	}

	if (average) {
		int sumDepthArray[length];

		int Denominator = 0;
		int Count = 1;


		while (!depthQueue.empty())
		{
			// Process each pixel in the row
			for (int i = 0; i < length; i++)
			{
				sumDepthArray[i] += depthQueue.front() * Count;
			}
			Denominator += Count;
			Count++;
			depthQueue.pop();
		}

		// Once we have summed all of the information on a weighted basis,
		// we can divide each pixel by our denominator to get a weighted average.
		for (int i = 0; i < length; i++)
		{
			if (Denominator)
				depthArray[i] = (short)(sumDepthArray[i] / Denominator);

		}
	}


	// end pixel is start + width*height - 1

	index = 0;
	int depthIndex = 480 * 640 * 4;
	int colorIndex = 0;
	char* colorRGB = (char*)displayPBits;
	UINT range = maxRange - minRange;
	if (range <= 0)
		range = 1;

	while (index < length)
	{
		// discard the portion of the depth that contains only the player indexf
		// Increment our index into the Kinect's depth buffer
		USHORT depth = depthArray[index++];
		depth -= minRange;


		//BYTE intensity = static_cast<BYTE>(depth < (short)DEPTH_SLIDER_MIN_MM || depth > (short)DEPTH_SLIDER_MAX_MM ? 0 : 255 - (256 * depth / 0x0fa0));
		BYTE intensity = static_cast<BYTE>(depth < 0 || depth >(short)range ? 0 : 255 - (256 * depth / range));
		//BYTE intensity = static_cast<BYTE>(depth >= 100 && depth <= 4095 ? depth % 256 : 0);
		if (printPNG) {
			if (intensity == 0) { //if the image should be transparent 

				pngBits[depthIndex++] = 255; //// <- depth partition of png file
				pngBits[colorIndex + 2] = 255; //// <- color partition of png file
				*(depthBits++) = 255; //// <- depth image displayed in the window
				*(colorRGB++) = 255; //// <- color image displayed in the window
				// Write out green byte
				pngBits[depthIndex++] = 255;
				pngBits[colorIndex + 1] = 255;
				*(depthBits++) = 255;
				*(colorRGB++) = 255;
				// Write out red byte
				pngBits[depthIndex++] = 255;
				pngBits[colorIndex] = 255;
				*(depthBits++) = 255;
				*(colorRGB++) = 255;
				colorIndex += 3;
				// write out alpha byte
				pngBits[depthIndex++] = 0;
				pngBits[colorIndex++] = 0;
				*(depthBits++) = 0;
				*(colorRGB++) = 0;
			}
			else { // process normally
				   // Write out blue byte
				pngBits[depthIndex++] = 255 - intensity;
				pngBits[colorIndex + 2] = *(colorRGB++);
				*(depthBits++) = intensity;

				// Write out green byte
				pngBits[depthIndex++] = 0;
				pngBits[colorIndex + 1] = *(colorRGB++);
				*(depthBits++) = 0;

				// Write out red byte
				pngBits[depthIndex++] = intensity;
				pngBits[colorIndex] = *(colorRGB++);
				*(depthBits++) = 255 - intensity;
				colorIndex += 3;
				// write out alpha 
				pngBits[depthIndex++] = 255;
				pngBits[colorIndex++] = 255;
				*(depthBits++) = 255;
				*(colorRGB++) = 255;
			}
		}
		else { //write as bitmap
			if (intensity == 0) {
				pngBits[depthIndex++] = 255; //// <- depth partition of png file
				pngBits[colorIndex + 2] = 255; //// <- color partition of png file
				*(depthBits++) = 255; //// <- depth image displayed in the window
				*(colorRGB++) = 255; //// <- color image displayed in the window
									 // Write out green byte
				pngBits[depthIndex++] = 255;
				pngBits[colorIndex + 1] = 255;
				*(depthBits++) = 255;
				*(colorRGB++) = 255;
				// Write out red byte
				pngBits[depthIndex++] = 255;
				pngBits[colorIndex] = 255;
				*(depthBits++) = 255;
				*(colorRGB++) = 255;
				colorIndex += 3;
				// write out alpha byte
				pngBits[depthIndex++] = 0;
				pngBits[colorIndex++] = 0;
				*(depthBits++) = 0;
				*(colorRGB++) = 0;
			}
			else { // process normally
				   // Write out blue byte
				pngBits[depthIndex++] = intensity;
				pngBits[colorIndex++] = *(colorRGB++);
				*(depthBits++) = intensity;

				// Write out green byte
				pngBits[depthIndex++] = 0;
				pngBits[colorIndex++] = *(colorRGB++);
				*(depthBits++) = 0;

				// Write out red byte
				pngBits[depthIndex++] = 255 - intensity;
				pngBits[colorIndex++] = *(colorRGB++);
				*(depthBits++) = 255 - intensity;

				// write out alpha 
				pngBits[depthIndex++] = 255;
				pngBits[colorIndex++] = 255;
				*(depthBits++) = 255;
				*(colorRGB++) = 255;
			}
		}
	}
}

void telekinect::launchServer()
{
	Globals::data = (char*)malloc(sizeof(char) * (DEFAULT_SENDBUFLEN + 1)); //allocate memory on the heap to allow multiple client threads to access same buffer 
	netstatus = ATTEMPTING;
	server = new ByteSender();
	netstatus = SUCCESSFUL;
}


void telekinect::processData()
{
	Globals::data = pngBits;
	printf("%s\n", Globals::data);
}
