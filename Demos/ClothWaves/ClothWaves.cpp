#include "ClothWaves.h"         // prototypes and stuff

//include our library
#pragma comment(lib, "AwesomeInput.lib")
#pragma comment(lib, "AwesomeMath.lib")
#pragma comment(lib, "AwesomeRenderer.lib")
#pragma comment(lib, "AwesomeGeneral.lib")


// windows stuff
HWND      g_hWnd = NULL;
HINSTANCE g_hInst = NULL;
TCHAR     g_szAppClass[] = TEXT("FrameWorktest");
std::wstring g_windowCaption = L"Pillars - Awesome Engine v0.1 - By Yosbi Alves Saenz";

// application stuff
BOOL g_bIsActive = FALSE;
bool g_bDone = false;

// renderer object
LPAWERENDERER     g_pRenderer = NULL;
LPAWERENDERDEVICE g_pDevice = NULL;

// Input object
LPAWEINPUT g_pYInput;
LPAWEINPUTDEVICE g_pYInputDevice;

// Cameras
std::vector<AwesomeBaseCam*> g_vcCameras;
USHORT g_nSelectedCamera = 0;

// Skin manager
AwesomeSkinManager* g_pAwesomeSkinManager;

// Timer
AwesomeTimer g_aweTimer(0, 0);

// Animations
AwesomeLinearInterpolation g_posLerp;
AwesomeLinearInterpolation g_dirLerp;
AwesomeLinearInterpolation g_upLerp;

UINT g_sMeshWaves = 0;

UINT g_nWavesShader = 0;

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
        g_windowCaption.c_str(),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
        WS_MINIMIZEBOX | WS_VISIBLE,
        GetSystemMetrics(SM_CXSCREEN) / 2 - (1280 / 2),
        GetSystemMetrics(SM_CYSCREEN) / 2 - (720 / 2),
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

    if (FAILED(initShaders()))
        return E_ABORT;

    if (FAILED(hr = LoadAssets()))
        return E_ABORT;

    initScene();

    initCamera();

    ShowWindow(hWnd, SW_SHOW);

    // everything went smooth


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
}


void initScene() {
    g_pDevice->SetClearColor(1, 1, 1); 

    AWESOMECOLOR ambient = { 0.6f, 0.6f, 0.6f, 1.0f };
    g_pDevice->SetAmbientLight(ambient);

    UINT nGlobalDirectionalLight = 0;
    AWELIGHT globalDirectionalLight;
    globalDirectionalLight.Type = AWE_DIRECTIONAL_LIGHT;
    globalDirectionalLight.cDiffuseLight = { 1.0f, 1.0f, 1.0f, 1.0f };
    globalDirectionalLight.cSpecularLight = { 1.0f, 1.0f, 1.0f, 1.0f };

    globalDirectionalLight.vcDirectionW.Set(0.0f, 0.0f, -1.0f);
    globalDirectionalLight.vcDirectionW.Normalize();

    g_pDevice->AddLight(globalDirectionalLight, nGlobalDirectionalLight);

}

HRESULT initShaders() {
    return g_pDevice->CreatePipelineStateObject(L"ClothWavesVertexShader.cso", L"ClothWavesPixelShader.cso", false, g_nWavesShader);
}


void initCamera() {
    AWEVector pos = AWEVector(0, 30, -30);
    /*AwesomeFreeCam* freeCam1 = new AwesomeFreeCam();
    freeCam1->SetPos(pos);
    freeCam1->SetRotation((45 * AWEPI) / 180.0f, 0.0f, 0.0f);

    pos = AWEVector(0, 10, 30);
    AwesomeFreeCam* freeCam2 = new AwesomeFreeCam();
    freeCam2->SetPos(pos);
    freeCam2->SetRotation((10 * AWEPI) / 180.0f, AWEPI, 0.0f);


    pos = AWEVector(0.0f, 100, 0.0f);
    AwesomeFreeCam* freeCam3 = new AwesomeFreeCam();
    freeCam3->SetPos(pos);
    freeCam3->SetRotation((90 * AWEPI) / 180.0f, 0.0f, 0.0f);*/

    pos = AWEVector(0, 7, -30);
    AwesomeFirstPersonCam* fpCam = new AwesomeFirstPersonCam();
    fpCam->SetPos(pos);

    /*g_vcCameras.push_back(freeCam1);
    g_vcCameras.push_back(freeCam2);
    g_vcCameras.push_back(freeCam3);*/
    g_vcCameras.push_back(fpCam);

    g_pDevice->setFoV(45.0f);
    g_pDevice->setClippingPlanes(0.1f, 1000.0f);
}

void updateCamera(float deltaT) {
    // if there is no animation we can update
    if (g_dirLerp.GetAnimationFinished()) {
        AwesomeBaseCam* cam = g_vcCameras.at(g_nSelectedCamera);
        cam->Update(deltaT);
        g_pDevice->SetViewMatrix(cam->GetPos(), cam->GetDir(), cam->GetUp());
    }
    else
    {
        g_pDevice->SetViewMatrix(g_posLerp.Update(deltaT), g_dirLerp.Update(deltaT), g_upLerp.Update(deltaT));
    }
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

void updateInput()
{
    // get the imputs to control the camera
    g_pYInputDevice->Update();

    // rotation
    POINT p = g_pYInputDevice->GetMouseDelta();

    // front or back
    float thrust = 0.0f;
    float thrust2 = 0.0f;

    if (g_pYInputDevice->IsPressed(IDV_KEYBOARD, AWEVK_W))
    {
        thrust = 20;
    }
    if (g_pYInputDevice->IsPressed(IDV_KEYBOARD, AWEVK_S))
    {
        thrust -= 20;
    }
    //float Thrust = 0.0f;
    if (g_pYInputDevice->IsPressed(IDV_KEYBOARD, AWEVK_A))
    {
        thrust2 = -20.0f;
    }
    if (g_pYInputDevice->IsPressed(IDV_KEYBOARD, AWEVK_D))
    {
        thrust2 = 20.0f;
    }

    // if there is no animation we can update
    if (g_dirLerp.GetAnimationFinished())
    {
        AwesomeBaseCam* cam = g_vcCameras.at(g_nSelectedCamera);
        AWECAMERATYPE cameraType = cam->GetCameraType();

        // Only first person camera can move
        if (cameraType == AWECAMERATYPE::FIRST_PERSON_CAM)
        {
            ((AwesomeFirstPersonCam*)(cam))->SetRotationSpeedX(-p.y);
            ((AwesomeFirstPersonCam*)(cam))->SetRotationSpeedY(-p.x);

            ((AwesomeFirstPersonCam*)(cam))->SetSpeed(thrust);
            ((AwesomeFirstPersonCam*)(cam))->SetSlideSpeed(thrust2);
        }
    }

    updateCamera(g_aweTimer.GetElapsed());

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
        case 'C':
            if (g_posLerp.GetAnimationFinished()) {
                AwesomeBaseCam* oldCam = g_vcCameras.at(g_nSelectedCamera);
                AWEVector oldPos = oldCam->GetPos();
                AWEVector oldDir = oldCam->GetDir();
                AWEVector oldUp = oldCam->GetUp();

                g_nSelectedCamera++;
                if (g_nSelectedCamera % g_vcCameras.size() == 0)
                    g_nSelectedCamera = 0;

                AwesomeBaseCam* newCam = g_vcCameras.at(g_nSelectedCamera);
                AWEVector newPos = newCam->GetPos();
                AWEVector newDir = newCam->GetDir();
                AWEVector newUp = newCam->GetUp();

                g_posLerp.Set(oldPos, newPos);
                g_dirLerp.Set(oldDir, newDir);
                g_upLerp.Set(oldUp, newUp);
            }


            //g_positionLerp
            break;

        } break;

        }
    } break;

    default: break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

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
}



float GetHillsHeight(float x, float z)
{
    return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

AWEVector GetHillsNormal(float x, float z)
{
    // n = (-df/dx, 1, -df/dz)
    AWEVector n(
        -0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
        1.0f,
        -0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));

    n.Normalize();

    return n;
}

HRESULT LoadAssets() {

    AWESOMECOLOR diff = { 0.0f, 0.0f, 1.0f, 1.0f };
    AWESOMECOLOR ambient = { 0.0f, 0.0f, 1.0f, 1.0f };
    AWESOMECOLOR spec = { 1.0f, 1.0f, 1.0f, 1.0f };
    UINT sea = g_pDevice->GetSkinManager()->AddSkin(ambient, diff, diff, spec, 16.0f);

    // Loading waves
    AwesomeGeometryGenerator geoGen;
    AwesomeGeometryGenerator::AwesomeMeshData cpuWaves = geoGen.CreateGrid(50.0f, 50.0f, 200, 200);
    g_pDevice->GetVertexManager()->CreateStaticBuffer(sea, cpuWaves.Vertices.size(), cpuWaves.getVertexSize(), cpuWaves.Vertices.data(),
        cpuWaves.Indices32.size(), cpuWaves.GetIndices16().data(), &g_sMeshWaves);


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
}

void updateFPS() {

    static double timeCount = 0.0f;
    timeCount += g_aweTimer.GetElapsed();

    static int fps = 0;
    fps += std::round(g_aweTimer.GetFPS());

    static int passes = 0;
    passes++;

    if (timeCount > 1.0f) {
        std::wstring newCaption = g_windowCaption + L" | " + std::to_wstring(fps / passes);

        // Now set the new caption to the main window. 
        SetWindowText(g_hWnd, newCaption.c_str());

        timeCount = 0.0f;
        fps = 0;
        passes = 0;
    }
}

/**
 * Do one frame.
 */
HRESULT ProgramTick(void) {

    g_aweTimer.Update();

    g_pDevice->UpdateDeltaTime(g_aweTimer.GetElapsed());

    updateFPS();

    updateInput();

    // clear buffers and start scene
    g_pDevice->BeginRendering(true, true, true);

    /*AWESOMECOLOR  cLight = {1.0f, 1.0f, 1.0f, 1.0f};
    AWEVector vcLight = AWEVector(0.0f, 0.0f, -1.0f);
    vcLight.Normalize();
    g_pDevice->SetLightDirection(vcLight);
    g_pDevice->SetDiffuseLight(cLight);
    g_pDevice->SetSpecularLight(cLight);*/

    AWEMatrix mWorld;
    mWorld.Identity();
    mWorld.RotaY(-0.4f);
    g_pDevice->SetWorldTransformMatrix(mWorld);
    g_pDevice->SetActivePipelineStateObject(g_nWavesShader);
    g_pDevice->GetVertexManager()->Render(g_sMeshWaves);



    // flip backbuffer to front
    g_pDevice->EndRendering();

    return S_OK;
}
