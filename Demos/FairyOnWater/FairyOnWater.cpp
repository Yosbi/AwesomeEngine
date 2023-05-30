#include "FairyOnWater.h"         // prototypes and stuff

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

UINT g_sMeshFairy = 0;
UINT g_sMeshSpark = 0;
UINT g_sMeshWaves = 0;

AWEVector g_vcHadaPos;

//VERTEX* g_Vertex = NULL;
//int g_nIndis = 0;
//int g_nVerts = 0;
//WORD* g_Index = NULL;

AWESOMECOLOR g_whiteColor = { 1.0f, 1.0f, 1.0f, 1.0f };
AWESOMECOLOR g_blackColor = { 0.0f, 0.0f, 0.0f, 1.0f };
AWESOMECOLOR g_redColor = { 1.0f, 0.0f, 0.0f, 1.0f };
AWESOMECOLOR g_greenColor = { 0.0f, 1.0f, 0.0f, 1.0f };
AWESOMECOLOR g_blueColor = { 0.0f, 0.0f, 1.0f, 1.0f };
AWESOMECOLOR g_yellowColor = { 0.0f, 1.0f, 1.0f, 1.0f };

AWELIGHT g_globalDirectionalLight;
UINT g_nGlobalDirectionalLight = 0;

AWELIGHT g_FairyLight;
UINT g_nFairyLight = 0;

AwesomeBezier g_bezier;
std::vector<AWEVector> g_bezierVectors0;
std::vector<AWEVector> g_bezierVectors1;
int g_nbezierAnimIndex = 0;

ParticleSystemSparks g_fairySparks;

UINT numColsRowsWater = 250;
float waterSize = 50.0f;
AwesomeGeometryGenerator::AwesomeMeshData g_cpuWaves;
Waves g_waves(numColsRowsWater, numColsRowsWater, 1.0f, 0.03f, 8.0f, 0.2f);



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
    //g_pDevice->SetClearColor(0.690196097f, 0.768627524f, 0.870588303f); // A light sky blue
    g_pDevice->SetClearColor(0.0f, 0.0f, 0.0f);

    AWESOMECOLOR ambientLight = { 0.35f,0.35f,0.35f };
    g_pDevice->SetAmbientLight(ambientLight);

    g_globalDirectionalLight.Type = AWE_DIRECTIONAL_LIGHT;
    g_globalDirectionalLight.cDiffuseLight = { 0.6f, 0.6f, 0.6f, 1.0f };
    g_globalDirectionalLight.cSpecularLight = { 1.0f, 1.0f, 1.0f, 1.0f };
    g_globalDirectionalLight.vcDirectionW.Set(0.0f, 1.0f, 1.0f);
    g_globalDirectionalLight.vcDirectionW.Normalize();
    g_pDevice->AddLight(g_globalDirectionalLight, g_nGlobalDirectionalLight);

    g_FairyLight.Type = AWE_POINT_LIGHT;
    g_FairyLight.cDiffuseLight = { 0.5f, 1.0f, 1.0f, 1.0f };
    g_FairyLight.cSpecularLight = { 0.5f, 1.0f, 1.0f, 1.0f };
    g_FairyLight.vcPositionW.Set(0.0f, 3.0f, 0.0f);
    g_FairyLight.vcPositionW.Normalize();
    g_FairyLight.fAttenuation0 = 1.0f;
    g_FairyLight.fAttenuation1 = 1.0f;
    g_FairyLight.fAttenuation2 = 1.0f;
    g_pDevice->AddLight(g_FairyLight, g_nFairyLight);


    g_bezierVectors0 = ComputeNewBezierPoints(AWEVector(0.0f, 0.0f, 0.0f));
    g_bezier.Set(g_bezierVectors0);


    /*g_bezierVectors1.push_back(AWEVector(0, 0.5f, 10.0f));
    g_bezierVectors1.push_back(AWEVector(0, 3, 5));
    g_bezierVectors1.push_back(AWEVector(0, 6, 0));
    g_bezierVectors1.push_back(AWEVector(0, 3, -5));
    g_bezierVectors1.push_back(AWEVector(0, 0.5f, -10.0f));*/
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

    UINT hadaSkin = g_pDevice->GetSkinManager()->AddSkin(g_yellowColor, g_yellowColor, emissiveWhite, g_yellowColor, 8.0f);

    AwesomeGeometryGenerator geoGen;
    AwesomeGeometryGenerator::AwesomeMeshData fairy = geoGen.CreateSphere(0.2f, 5, 5);
    g_pDevice->GetVertexManager()->CreateStaticBuffer(hadaSkin, fairy.Vertices.size(), fairy.getVertexSize(), fairy.Vertices.data(),
        fairy.Indices32.size(), fairy.GetIndices16().data(), &g_sMeshFairy);

    AwesomeGeometryGenerator::AwesomeMeshData sparks = geoGen.CreateSphere(0.1f, 5, 5);
    g_pDevice->GetVertexManager()->CreateStaticBuffer(hadaSkin, sparks.Vertices.size(), sparks.getVertexSize(), sparks.Vertices.data(),
        sparks.Indices32.size(), sparks.GetIndices16().data(), &g_sMeshSpark);

    // Loading waves

    AWESOMECOLOR diff = { 0.0f, 0.2f, 0.6f, 1.0f };
    AWESOMECOLOR ambient = { 0.5f, 0.5f,0.5f, 1.0f };
    AWESOMECOLOR spec = { 1.0f, 1.0f, 1.0f, 1.0f };
    UINT sea = g_pDevice->GetSkinManager()->AddSkin(ambient, diff, g_blackColor, spec, 16.0f);

    g_cpuWaves = geoGen.CreateGrid(waterSize, waterSize, numColsRowsWater, numColsRowsWater);
    g_pDevice->GetVertexManager()->CreateDynamicBuffer(sea, g_cpuWaves.Vertices.size(), g_cpuWaves.Vertices.data(),
        g_cpuWaves.Indices32.size(), g_cpuWaves.GetIndices16().data(), &g_sMeshWaves);

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

std::vector<AWEVector> ComputeNewBezierPoints(AWEVector vcInitial)
{
    std::vector<AWEVector> newBezier;
    newBezier.push_back(vcInitial);
    newBezier.push_back(AWEVector(vcInitial.x,3.0f, vcInitial.z));

    int newX = Rand(-1, 1) * Rand(3, 5);
    int newZ = Rand(-1, 1) * Rand(3, 5);

    newX += vcInitial.x;
    newZ += vcInitial.z;

    if (newX >= waterSize/2 - 1)
        newX -= 5;
    if (newX <= 1)
        newX += 5;

    if (newZ >= waterSize/2 - 1)
        newZ -= 5;
    if (newZ <= 1)
        newZ += 5;

    newBezier.push_back(AWEVector(newX, 3.0f, newZ));
    newBezier.push_back(AWEVector(newX, 0.5f, newZ));

    return newBezier;

}

void updateLight() {
    bool disturbWater = false;

    // Updating / Setting animation
    AWEVector collisionPoint;
    if (g_bezier.GetAnimationFinished())
    {
        disturbWater = true;
        /*g_nbezierAnimIndex++;
        if (g_nbezierAnimIndex == 1)
        {
            collisionPoint = g_bezierVectors1.at(0);
            g_bezier.Set(g_bezierVectors1);
            g_nbezierAnimIndex = -1;
        }


        if (g_nbezierAnimIndex == 0)
        {
            collisionPoint = g_bezierVectors0.at(0);
            g_bezier.Set(g_bezierVectors0);
        }*/
        collisionPoint = g_bezierVectors0.at(g_bezierVectors0.size() - 1);
        g_bezierVectors0 = ComputeNewBezierPoints(collisionPoint);
        g_bezier.Set(g_bezierVectors0);

    }

    if (disturbWater)
    {
        float r = RandF(0.1f, 0.3f);

        g_waves.Disturb( ( (-collisionPoint.z + waterSize/2) * numColsRowsWater) / waterSize, 
                         ( (collisionPoint.x + waterSize/2) * numColsRowsWater) / waterSize, r);
    }

    //OutputDebugStringA(("X: " + std::to_string(v.x) + ", Y: " + std::to_string(v.y) +", Z: " + std::to_string(v.z) + "\n").c_str());

    AWEVector v = g_bezier.Update(g_aweTimer.GetElapsed());

    // Updating the light position
    g_FairyLight.vcPositionW.x = v.x;
    g_FairyLight.vcPositionW.y = v.y;
    g_FairyLight.vcPositionW.z = v.z;
    g_pDevice->UpdateLight(g_nFairyLight, g_FairyLight);

    // Updating the fairy position
    g_vcHadaPos.Set(g_FairyLight.vcPositionW.x, g_FairyLight.vcPositionW.y, g_FairyLight.vcPositionW.z);


    static float time = 0.0f;
    time += g_aweTimer.GetElapsed();

    if (time >= 0.05f) {
        g_fairySparks.EmitSpark(g_FairyLight.vcPositionW);
        time = 0.0f;
    }
    g_fairySparks.Update(g_aweTimer.GetElapsed());
    //OutputDebugStringA(("Num particles: " + std::to_string(g_fairySparks.particles.size()) + "\n").c_str());

}

void updateWaves()
{
    // Every quarter second, generate a random wave.
    /*static float t_base = 0.0f;
    static float t_passed = 0.0f;
    t_passed += g_aweTimer.GetElapsed();

    if ((t_passed - t_base) >= 0.1f)
    {
        t_base += 0.1f;

        int i = Rand(4, g_waves.RowCount() - 5);
        int j = Rand(4, g_waves.ColumnCount() - 5);

        float r = RandF(0.2f, 0.5f);

        g_waves.Disturb(i, j, r);
    }-*/

    // Update the wave simulation.
    g_waves.Update(g_aweTimer.GetElapsed());

    UINT count = g_waves.VertexCount();
    UINT size = g_cpuWaves.Vertices.size();
    // Update the wave vertex buffer with the new solution.
    for (int i = 0; i < g_waves.VertexCount(); ++i)
    {
        AWEVector v = g_waves.Position(i);
        //g_cpuWaves.Vertices[i].x = v.x;
        g_cpuWaves.Vertices[i].y = v.y;
        //g_cpuWaves.Vertices[i].z = v.z;

        AWEVector n = g_waves.Normal(i);
        g_cpuWaves.Vertices[i].vcN[0] = n.x;
        g_cpuWaves.Vertices[i].vcN[1] = n.y;
        g_cpuWaves.Vertices[i].vcN[2] = n.z;

    }

    g_pDevice->GetVertexManager()->UpdateDynamicBuffer(g_sMeshWaves, g_cpuWaves.Vertices.data());

}


/**
 * Do one frame.
 */
HRESULT ProgramTick(void) {

    g_aweTimer.Update();

    updateFPS();

    updateInput();

    updateLight();

    updateWaves();

    Render();

    return S_OK;
}


void Render()
{
    // clear buffers and start scene
    g_pDevice->BeginRendering(true, true, true);

    // Rendering particles first as they have light
    int i = 2;
    //std::reverse(g_fairySparks.particles.begin(), g_fairySparks.particles.end()); 
    for (const Particle& particle : g_fairySparks.particles)
    {
        if (i >= MAX_LIGHTS)
            break;

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


    AWEMatrix mWorld;
    mWorld.Identity();
    mWorld.Translate(g_vcHadaPos.x, g_vcHadaPos.y, g_vcHadaPos.z);
    g_pDevice->SetWorldTransformMatrix(mWorld);
    g_pDevice->GetVertexManager()->Render(g_sMeshFairy);

    mWorld.Identity();
    g_pDevice->SetWorldTransformMatrix(mWorld);
    g_pDevice->GetVertexManager()->Render(g_sMeshWaves);


    // flip backbuffer to front
    g_pDevice->EndRendering();
}