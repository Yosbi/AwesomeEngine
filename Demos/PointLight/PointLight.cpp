#include "PointLight.h"         // prototypes and stuff

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

UINT Chinesse = 0;
UINT Terrain = 0;

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
UINT g_nSkinTiles;

// Timer
AwesomeTimer g_aweTimer(0, 0);

// Animations
AwesomeLinearInterpolation g_posLerp;
AwesomeLinearInterpolation g_dirLerp;
AwesomeLinearInterpolation g_upLerp;

UINT g_sMeshBox = 0;
UINT g_sMeshGrid = 0;
UINT g_sMeshSphere = 0;
UINT g_sMeshCylinder = 0;
UINT g_sMeshChinesse = 0;
UINT g_sMeshHada = 0; 
UINT g_sMeshSpark = 0;

AWEVector g_vcHadaPos;

//VERTEX* g_Vertex = NULL;
//int g_nIndis = 0;
//int g_nVerts = 0;
//WORD* g_Index = NULL;

AWESOMECOLOR g_whiteColor = {1.0f, 1.0f, 1.0f, 1.0f };
AWESOMECOLOR g_blackColor = {0.0f, 0.0f, 0.0f, 1.0f };
AWESOMECOLOR g_redColor = { 1.0f, 0.0f, 0.0f, 1.0f };
AWESOMECOLOR g_greenColor = { 0.0f, 1.0f, 0.0f, 1.0f };
AWESOMECOLOR g_blueColor = { 0.0f, 0.0f, 1.0f, 1.0f };
AWESOMECOLOR g_yellowColor = { 0.0f, 1.0f, 1.0f, 1.0f };

AWELIGHT g_globalDirectionalLight;
UINT g_nGlobalDirectionalLight = 0;

AwesomeBezier g_bezier;
std::vector<AWEVector> g_bezierVectors0;
std::vector<AWEVector> g_bezierVectors1;
int g_nbezierAnimIndex = 0;

ParticleSystemSparks g_fairySparks;


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

    if (FAILED(hr = LoadAssets()))
        return E_ABORT;


    initScene();

    initCamera();

    ShowWindow(hWnd, SW_SHOW);


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
    g_pDevice->SetClearColor(0.690196097f, 0.768627524f, 0.870588303f); // A light sky blue

    AWESOMECOLOR ambientLight = { 0.20f,0.20f,0.20f };
    g_pDevice->SetAmbientLight(ambientLight);

    g_globalDirectionalLight.Type = AWE_POINT_LIGHT;
    g_globalDirectionalLight.cDiffuseLight = { 0.5f, 1.0f, 1.0f, 1.0f };
    g_globalDirectionalLight.cSpecularLight = { 0.5f, 1.0f, 1.0f, 1.0f };

    g_globalDirectionalLight.vcPositionW.Set(0.0f, 3.0f, 0.0f);
    g_globalDirectionalLight.vcPositionW.Normalize();
    g_globalDirectionalLight.fAttenuation0 = 1.0f;
    
    g_pDevice->AddLight(g_globalDirectionalLight, g_nGlobalDirectionalLight);


    g_bezierVectors0.push_back(AWEVector(0, 0.5f, -10.0f));
    g_bezierVectors0.push_back(AWEVector(0, 6, -5));
    g_bezierVectors0.push_back(AWEVector(0, 12, 0));
    g_bezierVectors0.push_back(AWEVector(0, 6, 5));
    g_bezierVectors0.push_back(AWEVector(0, 0.5f, 10.0f));
    g_bezier.Set(g_bezierVectors0);


    g_bezierVectors1.push_back(AWEVector(0, 0.5f, 10.0f));
    g_bezierVectors1.push_back(AWEVector(0, 3, 5));
    g_bezierVectors1.push_back(AWEVector(0, 6, 0));
    g_bezierVectors1.push_back(AWEVector(0, 3, -5));
    g_bezierVectors1.push_back(AWEVector(0, 0.5f, -10.0f));
}

void initCamera() {
    AWEVector pos = AWEVector(0, 30, -30);

    pos = AWEVector(0, 7, -30);
    AwesomeFirstPersonCam* fpCam = new AwesomeFirstPersonCam();
    fpCam->SetPos(pos);
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
        thrust = 5;
    }
    if (g_pYInputDevice->IsPressed(IDV_KEYBOARD, AWEVK_S))
    {
        thrust -= 5;
    }
    //float Thrust = 0.0f;
    if (g_pYInputDevice->IsPressed(IDV_KEYBOARD, AWEVK_A))
    {
        thrust2 = -5.0f;
    }
    if (g_pYInputDevice->IsPressed(IDV_KEYBOARD, AWEVK_D))
    {
        thrust2 = 5.0f;
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


HRESULT LoadAssets() {

    AWESOMECOLOR emissiveWhite = { -0.5f, 1.0f, 1.0f, 1.0f };
    
    UINT terrainSkin = g_pDevice->GetSkinManager()->AddSkin(g_blueColor, g_blueColor, g_blackColor, g_whiteColor, 16.0f);
    UINT cylSkin = g_pDevice->GetSkinManager()->AddSkin(g_redColor, g_redColor, g_blackColor, g_whiteColor, 8.0f);
    UINT spheSkin = g_pDevice->GetSkinManager()->AddSkin(g_greenColor, g_greenColor, g_blackColor, g_whiteColor, 8.0f);
    UINT hadaSkin = g_pDevice->GetSkinManager()->AddSkin(g_yellowColor, g_yellowColor, emissiveWhite, g_yellowColor, 8.0f);

    AwesomeGeometryGenerator geoGen;
    AwesomeGeometryGenerator::AwesomeMeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
    g_pDevice->GetVertexManager()->CreateStaticBuffer(terrainSkin, grid.Vertices.size(), grid.getVertexSize(), grid.Vertices.data(),
        grid.Indices32.size(), grid.GetIndices16().data(), &g_sMeshGrid);

/*AwesomeGeometryGenerator::AwesomeMeshData box = geoGen.CreateBox(1.5f, 0.5f, 1.5f, 3);
    g_pDevice->GetVertexManager()->CreateStaticBuffer(skin, box.Vertices.size(), box.getVertexSize(), box.Vertices.data(),
        box.Indices32.size(), box.GetIndices16().data(), &g_sMeshBox);*/

    AwesomeGeometryGenerator::AwesomeMeshData sphere = geoGen.CreateSphere(0.5f, 100, 200);
    g_pDevice->GetVertexManager()->CreateStaticBuffer(spheSkin, sphere.Vertices.size(), sphere.getVertexSize(), sphere.Vertices.data(),
        sphere.Indices32.size(), sphere.GetIndices16().data(), &g_sMeshSphere);


    AwesomeGeometryGenerator::AwesomeMeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 200, 200);
    g_pDevice->GetVertexManager()->CreateStaticBuffer(cylSkin, cylinder.Vertices.size(), cylinder.getVertexSize(), cylinder.Vertices.data(),
        cylinder.Indices32.size(), cylinder.GetIndices16().data(), &g_sMeshCylinder);


    AwesomeOBJLoader loader2;
    loader2.loadOBJ(L"Chinesse.obj");
    g_pDevice->GetVertexManager()->CreateStaticBuffer(spheSkin, loader2.getVerteces().size(), loader2.getVertexSize(), loader2.getVerteces().data(), 0, NULL, &g_sMeshChinesse);

    AwesomeGeometryGenerator::AwesomeMeshData hada = geoGen.CreateSphere(0.2f, 20, 20);
    g_pDevice->GetVertexManager()->CreateStaticBuffer(hadaSkin, hada.Vertices.size(), hada.getVertexSize(), hada.Vertices.data(),
        hada.Indices32.size(), hada.GetIndices16().data(), &g_sMeshHada);

    AwesomeGeometryGenerator::AwesomeMeshData sparks = geoGen.CreateSphere(0.1f, 5, 5);
    g_pDevice->GetVertexManager()->CreateStaticBuffer(hadaSkin, sparks.Vertices.size(), sparks.getVertexSize(), sparks.Vertices.data(),
        sparks.Indices32.size(), sparks.GetIndices16().data(), &g_sMeshSpark);

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

void updateLight() {
    
    // Updating / Setting animation
    if (g_bezier.GetAnimationFinished())
    {
        g_nbezierAnimIndex++;
        if (g_nbezierAnimIndex == 1)
        {
            g_bezier.Set(g_bezierVectors1);
            g_nbezierAnimIndex = -1;
        }
           

        if (g_nbezierAnimIndex == 0)
        {
            g_bezier.Set(g_bezierVectors0);
        }
    }
    AWEVector v = g_bezier.Update(g_aweTimer.GetElapsed());
    //OutputDebugStringA(("X: " + std::to_string(v.x) + ", Y: " + std::to_string(v.y) +", Z: " + std::to_string(v.z) + "\n").c_str());


    // Updating the light position
    g_globalDirectionalLight.vcPositionW.x = v.x;
    g_globalDirectionalLight.vcPositionW.y = v.y;
    g_globalDirectionalLight.vcPositionW.z = v.z;
    g_pDevice->UpdateLight(g_nGlobalDirectionalLight, g_globalDirectionalLight);

    // Updating the fairy position
    g_vcHadaPos.Set(g_globalDirectionalLight.vcPositionW.x, g_globalDirectionalLight.vcPositionW.y, g_globalDirectionalLight.vcPositionW.z);


    static float time = 0.0f;
    time += g_aweTimer.GetElapsed();

    if (time >= 0.05f) {
        g_fairySparks.EmitSpark(g_globalDirectionalLight.vcPositionW);
        time = 0.0f;
    }
    g_fairySparks.Update(g_aweTimer.GetElapsed());
    OutputDebugStringA(("Num particles: " + std::to_string(g_fairySparks.particles.size()) + "\n").c_str());

}
/**
 * Do one frame.
 */
HRESULT ProgramTick(void) {

    g_aweTimer.Update();

    updateFPS();

    updateInput();

    updateLight();

    Render();

    return S_OK;
}


void Render()
{
    // clear buffers and start scene
    g_pDevice->BeginRendering(true, true, true);

    AWEMatrix mWorld;
    /*mWorld.Translate(0.0f, 0.5f, 0.0f);
    AWEMatrix mScaling;
    mScaling._11 = mScaling._22 = mScaling._33 = 2.0f;
    g_pDevice->SetWorldTransformMatrix(mScaling * mWorld);
    g_pDevice->GetVertexManager()->Render(g_sMeshBox);*/

    mWorld.Identity();
    mWorld.Translate(g_vcHadaPos.x, g_vcHadaPos.y, g_vcHadaPos.z);
    g_pDevice->SetWorldTransformMatrix(mWorld);
    g_pDevice->GetVertexManager()->Render(g_sMeshHada);

    mWorld.Identity();
    g_pDevice->SetWorldTransformMatrix(mWorld);
    g_pDevice->GetVertexManager()->Render(g_sMeshGrid);

    // Rendering the spheres and columns
    for (int i = 0; i < 5; ++i)
    {
        AWEMatrix leftCylWorld;
        leftCylWorld.Translate(-5.0f, 1.5f, -10.0f + i * 5.0f);

        AWEMatrix rightCylWorld;
        rightCylWorld.Translate(+5.0f, 1.5f, -10.0f + i * 5.0f);

        AWEMatrix leftSphereWorld;
        AWEMatrix rightSphereWorld;

        g_pDevice->SetWorldTransformMatrix(leftCylWorld);
        g_pDevice->GetVertexManager()->Render(g_sMeshCylinder);

        g_pDevice->SetWorldTransformMatrix(rightCylWorld);
        g_pDevice->GetVertexManager()->Render(g_sMeshCylinder);

        if (i % 2 == 0) {
            leftSphereWorld.Scale(0.07f, 0.07f, 0.07f);
            leftSphereWorld.Translate(-5.0f, 3.5f, -10.0f + i * 5.0f);

            rightSphereWorld.Scale(0.07f, 0.07f, 0.07f);
            rightSphereWorld.Translate(+5.0f, 3.5f, -10.0f + i * 5.0f);

            g_pDevice->SetWorldTransformMatrix(leftSphereWorld);
            g_pDevice->GetVertexManager()->Render(g_sMeshChinesse);

            g_pDevice->SetWorldTransformMatrix(rightSphereWorld);
            g_pDevice->GetVertexManager()->Render(g_sMeshChinesse);
        }
        else
        {
            leftSphereWorld.Translate(-5.0f, 3.5f, -10.0f + i * 5.0f);
            rightSphereWorld.Translate(+5.0f, 3.5f, -10.0f + i * 5.0f);

            g_pDevice->SetWorldTransformMatrix(leftSphereWorld);
            g_pDevice->GetVertexManager()->Render(g_sMeshSphere);

            g_pDevice->SetWorldTransformMatrix(rightSphereWorld);
            g_pDevice->GetVertexManager()->Render(g_sMeshSphere);
        }
    }

    // Rendering particles
    int i = 1;
    for ( const Particle &particle : g_fairySparks.particles) 
    {
        AWEMatrix particleWorld;
        particleWorld.Identity();
        particleWorld.Scale(particle.m_scale, particle.m_scale, particle.m_scale);
        particleWorld.Translate(particle.m_vcPosition.x, particle.m_vcPosition.y, particle.m_vcPosition.z);

        AWELIGHT sparkLight;
        sparkLight.Type = AWE_POINT_LIGHT;
        sparkLight.cDiffuseLight = { 0.3f, 1.0f, 1.0f, 1.0f };
        sparkLight.cSpecularLight = { 0.3f, 1.0f, 1.0f, 1.0f };

        sparkLight.fAttenuation0 = 2 - particle.m_lifetime;
        sparkLight.fAttenuation1 = 2 - particle.m_lifetime;
        sparkLight.fAttenuation2 = 2 - particle.m_lifetime;
        sparkLight.vcPositionW = particle.m_vcPosition;
        g_pDevice->UpdateLight(i, sparkLight);

        g_pDevice->SetWorldTransformMatrix(particleWorld);
        g_pDevice->GetVertexManager()->Render(g_sMeshSpark);
        i++;
    }

    for (i; i < MAX_LIGHTS; i++)
    {
        AWELIGHT lightOff;
        lightOff.Type = AWE_NOTSET_LIGHT;
        g_pDevice->UpdateLight(i, lightOff);
    }

    // flip backbuffer to front
    g_pDevice->EndRendering();
}