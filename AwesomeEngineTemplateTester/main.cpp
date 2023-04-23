#include "main.h"         // prototypes and stuff

//include our library
#pragma comment(lib, "AwesomeInput.lib")
#pragma comment(lib, "AwesomeMath.lib")
#pragma comment(lib, "AwesomeRenderer.lib")
#pragma comment(lib, "AwesomeGeneral.lib")


// windows stuff
HWND      g_hWnd = NULL;
HINSTANCE g_hInst = NULL;
TCHAR     g_szAppClass[] = TEXT("FrameWorktest");

// application stuff
BOOL g_bIsActive = FALSE;
bool g_bDone = false;

UINT Chinesse = 0;
UINT Terrain = 0;

// renderer object
LPAWERENDERER     g_pRenderer = NULL;
LPAWERENDERDEVICE g_pDevice = NULL;

// Input object
LPAWEINPUT g_pYInput;
LPAWEINPUTDEVICE g_pYInputDevice;

AwesomeFreeCam freeCam;
AwesomeTimer aweTimer(0, 0);

/**
 * WinMain function to get the thing started.
 */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX	wndclass;
    HRESULT     hr;
    HWND		hWnd;
    MSG			msg;
    ZeroMemory(&msg, sizeof(msg));
    // Set up window attributes
    wndclass.cbSize = sizeof(wndclass);
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
    wndclass.lpfnWndProc = MsgProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInst;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = g_szAppClass;
    wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (RegisterClassEx(&wndclass) == 0)
        return 0;

    // create window
    if (!(hWnd = CreateWindowEx(NULL, g_szAppClass,
        L"Awesome Engine v0.1 - By Yosbi Alves Saenz",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
        WS_MINIMIZEBOX | WS_VISIBLE,
        GetSystemMetrics(SM_CXSCREEN) / 2 - (1280/2),
        GetSystemMetrics(SM_CYSCREEN) / 2 - (720/2),
        1280, 720, NULL, NULL, hInst, NULL)))
        return 0;

    // dont show everything off yet
    ShowWindow(hWnd, SW_HIDE);

    g_hWnd = hWnd;
    g_hInst = hInst;


    // try to start the engine
    if (FAILED(hr = RendererStartup())) 
        return E_ABORT;

    // Try to start the input module
    else if (FAILED(InputStartup()))
    {
        return E_ABORT;
    }

    if (FAILED(hr = LoadAssets()))
        return E_ABORT;

    initCamera();

    ShowWindow(hWnd, SW_SHOW);
    
    // everything went smooth
    g_pDevice->SetClearColor(1.0f, 1.0f, 1.0f);

    // main loop
    while (!g_bDone) {
        // handle messages
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // do one frame
        if (g_bIsActive) 
            ProgramTick();
    }

    // cleanup stuff
    ProgramCleanup();

    UnregisterClass(g_szAppClass, hInst);

    // return back to windows
    return (int)msg.wParam;
} // WinMain
/*----------------------------------------------------------------*/

void initCamera() {
    
    AWEVector pos = AWEVector(0, 0, -30);
    freeCam.SetPos(pos);
    
    g_pDevice->setFoV(45.0f);
    g_pDevice->setClippingPlanes(0.1f, 1000.0f);
}

void updateCamera(float deltaT) {
    freeCam.Update(deltaT);
    g_pDevice->SetViewMatrix(freeCam.GetPos(), freeCam.GetDir(), freeCam.GetUp());
}

//-----------------------------------------------------------------------------
// Name: InputStartup()
// Desc: Turn on the input module
//-----------------------------------------------------------------------------
HRESULT InputStartup()
{
    RECT mouseCage = { 0,0,0,0 };
    mouseCage.right = 1280;
    mouseCage.bottom = 720;

    // Create device
    g_pYInput = new AwesomeInput(g_hInst);
    if (FAILED(g_pYInput->CreateDevice())) return E_FAIL;

    // Get the device
    g_pYInputDevice = g_pYInput->GetDevice();
    if (g_pYInputDevice == NULL) return E_FAIL;

    // Init the device
    if (FAILED(g_pYInputDevice->Init(g_hWnd, &mouseCage)))
    {
        return E_FAIL;
    }

    return S_OK;
}
#include <iostream>
void updateInput()
{
    // get the imputs to control the camera
    g_pYInputDevice->Update();

    // rotation
    POINT p = g_pYInputDevice->GetMouseDelta();

    // front or back
    float Thrust = 0.0f;
    float Thrust2 = 0.0f;

    if (g_pYInputDevice->IsPressed(IDV_KEYBOARD, AWEVK_W))
    {
        Thrust = 5;
    }
    if (g_pYInputDevice->IsPressed(IDV_KEYBOARD, AWEVK_S))
    {
        Thrust -= 5;
    }
    //float Thrust = 0.0f;
    if (g_pYInputDevice->IsPressed(IDV_KEYBOARD, AWEVK_A))
    {
        Thrust2 = -5.0f;
    }
    if (g_pYInputDevice->IsPressed(IDV_KEYBOARD, AWEVK_D))
    {
        Thrust2 = 5.0f;
    }
    freeCam.SetRotationSpeedX(p.y);
    freeCam.SetRotationSpeedY(p.x);

    //freeCam.SetSpeed(Thrust);
    //freeCam.SetSlideSpeed(Thrust2);
    freeCam.SetThrust(Thrust);

    updateCamera(aweTimer.GetElapsed());
   
}

/**
 * MsgProc to deal with incoming windows messages.
 */
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    
        // our app has the focus
        case WM_ACTIVATE: {
            g_bIsActive = (BOOL)wParam;
        } break;
    
        // User press the x so we are ordered to suicide
        case WM_DESTROY: {
            g_bDone = true;
            PostQuitMessage(0);
            return 1;
        }

            // key was pressed
        case WM_KEYDOWN: {
            switch (wParam) {
                case VK_ESCAPE: {
                    g_bDone = true;
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    return 0;
                //case 'C':
                    
                } break;

            }
        } break;

        default: break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
/*----------------------------------------------------------------*/


/**
 * Create a render device and stuff.
 */
HRESULT RendererStartup() {

    // create a render objekt
    g_pRenderer = new AwesomeRenderer(g_hInst);

    // create a device for the chosen api
    if (FAILED(g_pRenderer->CreateDevice())) 
        return E_FAIL;

    // get a pointer on that device
    g_pDevice = g_pRenderer->GetDevice();
    if (g_pDevice == nullptr) 
        return E_FAIL;

    // init render device
    if (FAILED(g_pDevice->Init(g_hWnd))) {
        return E_FAIL;
    }

    return S_OK;
} // ProgramStartup
/*----------------------------------------------------------------*/

HRESULT LoadAssets() {
    Terrain = g_pDevice->LoadMesh(L"terrain.obj", 0.54f, 0.32f, 0.07f, 1.0f);
    g_pDevice->LoadMeshToGPU(Terrain);

    Chinesse = g_pDevice->LoadMesh(L"Chinesse.obj");
    g_pDevice->LoadMeshToGPU(Chinesse);
    return S_OK;
}
/**
 * Release the render device and stuff.
 */
HRESULT ProgramCleanup(void) {
    if (g_pRenderer) {
        delete g_pRenderer;
        g_pRenderer = NULL;
    }

    return S_OK;
} // ProgramCleanup
/*----------------------------------------------------------------*/


/**
 * Do one frame.
 */
HRESULT ProgramTick(void) {
   
    aweTimer.Update();

    updateInput();

    // clear buffers and start scene
    g_pDevice->BeginRendering(true, true, true);

    g_pDevice->RenderMesh(Terrain);

    g_pDevice->RenderMesh(Chinesse);

    // flip backbuffer to front
    g_pDevice->EndRendering();

    return S_OK;
} // Tick
/*----------------------------------------------------------------*/


