#include "Pillars.h"         // prototypes and stuff

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

//VERTEX* g_Vertex = NULL;
//int g_nIndis = 0;
//int g_nVerts = 0;
//WORD* g_Index = NULL;

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

    AWESOMECOLOR ambient = {0.6f, 0.6f, 0.6f, 1.0f};
    g_pDevice->SetAmbientLight(ambient);

    UINT nGlobalDirectionalLight = 0;
    AWELIGHT globalDirectionalLight;
    globalDirectionalLight.Type = AWE_DIRECTIONAL_LIGHT;
    globalDirectionalLight.cDiffuseLight = { 1.0f, 1.0f, 1.0f, 1.0f };
    globalDirectionalLight.cSpecularLight = { 1.0f, 1.0f, 1.0f, 1.0f };

    globalDirectionalLight.vcDirectionW.Set(0.0f, 1.0f, -1.0f);
    globalDirectionalLight.vcDirectionW.Normalize();

    g_pDevice->AddLight(globalDirectionalLight, nGlobalDirectionalLight);
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

    //-------------------------------------------------------------------------------------------------------------
    //Terrain = g_pDevice->LoadMesh(L"terrain.obj", 0.54f, 0.32f, 0.07f, 1.0f);
    //g_pDevice->LoadMeshToGPU(Terrain);

    //Chinesse = g_pDevice->LoadMesh(L"Chinesse.obj");
    //g_pDevice->LoadMeshToGPU(Chinesse);

    /*std::wstring textureTilesAddress = L"tiles.jpg";
    std::wstring textureMirror = L"mirror.jpg";
    std::wstring textureCarusso = L"Texture_by_FE_Caruso.jpg";

    AWESOMECOLOR ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
    AWESOMECOLOR spec = { 0.35f, 0.35f, 0.35f, 1.0f };
    g_nSkinTiles = g_pDevice->GetSkinManager()->AddSkin(ambient, diff, diff, spec, 8.0f);
    g_pDevice->GetSkinManager()->AddTexture(g_nSkinTiles, textureTilesAddress, false, 1.0f);
    g_pDevice->GetSkinManager()->AddTexture(g_nSkinTiles, textureMirror, false, 1.0f);
    g_pDevice->GetSkinManager()->AddTexture(g_nSkinTiles, textureCarusso, false, 1.0f);*/


    //GenGrid(50, 50, 1.0f, 1.0f, AWEVector(0.0f, 0.0f, 0.0f));
    //-------------------------------------------------------------------------------------------------------------
    /*
    AwesomeOBJLoader loader;
    loader.loadOBJ(L"terrain.obj");

    std::wstring textureTilesAddress = L"tiles.jpg";
    AWESOMECOLOR diff = { 0.454f, 0.4f, 0.231f, 1.0f };
    AWESOMECOLOR ambient = { 0.3f, 0.3f,0.3f, 1.0f };
    AWESOMECOLOR spec = { 0.4f, 0.4f, 0.4f, 1.0f };
    g_nSkinTiles = g_pDevice->GetSkinManager()->AddSkin(ambient, diff, diff, spec, 8.0f);
    g_pDevice->GetSkinManager()->AddTexture(g_nSkinTiles, textureTilesAddress, false, 1.0f);
    g_pDevice->GetVertexManager()->CreateStaticBuffer(g_nSkinTiles, loader.getVerteces().size(), loader.getVertexSize(), loader.getVerteces().data(), 0, NULL, &g_sMesh);


    AwesomeOBJLoader loader2;
    loader2.loadOBJ(L"Chinesse.obj");

    std::wstring textureMirror = L"mirror.jpg";
    AWESOMECOLOR diff2= { 0.5f, 0.5f, 0.5f, 1.0f };
    AWESOMECOLOR ambient2 = { 0.3f, 0.3f,0.3f, 1.0f };
    AWESOMECOLOR spec2 = { 0.4f, 0.4f, 0.4f, 1.0f };
    UINT skin = g_pDevice->GetSkinManager()->AddSkin(ambient2, diff2, diff2, spec2, 8.0f);
    g_pDevice->GetSkinManager()->AddTexture(skin, textureMirror, false, 1.0f);

    AwesomeGeometryGenerator geometryGenerator;
    AwesomeGeometryGenerator::AwesomeMeshData mesh = geometryGenerator.CreateSphere(7.0f, 30, 30);
    //AwesomeGeometryGenerator::AwesomeMeshData mesh = geometryGenerator.CreateCylinder(4.0f, 1.0f, 7.0f, 30, 40);
    //AwesomeGeometryGenerator::AwesomeMeshData mesh = geometryGenerator.CreateGeosphere(7.0f, 5);
    //AwesomeGeometryGenerator::AwesomeMeshData mesh = geometryGenerator.CreateBox(10.0f, 7.0f, 7.0f, 4);
    g_pDevice->GetVertexManager()->CreateStaticBuffer(skin, mesh.Vertices.size(), mesh.getVertexSize(), mesh.Vertices.data(),
        mesh.Indices32.size(), mesh.GetIndices16().data(), &g_sMesh3);*/
        //-------------------------------------------------------------------------------------------------------------

    AWESOMECOLOR terrainDiff = { 0.454f, 0.4f, 0.231f, 1.0f };
    AWESOMECOLOR terrainAmbient = { 0.3f, 0.3f,0.3f, 1.0f };
    AWESOMECOLOR terrainSpec = { 0.4f, 0.4f, 0.4f, 1.0f };
    UINT terrainSkin = g_pDevice->GetSkinManager()->AddSkin(terrainAmbient, terrainDiff, terrainDiff, terrainSpec, 8.0f);


    AWESOMECOLOR diff = { 0.5f, 0.5f, 0.5f, 1.0f };
    AWESOMECOLOR ambient = { 0.3f, 0.3f,0.3f, 1.0f };
    AWESOMECOLOR spec = { 0.4f, 0.4f, 0.4f, 1.0f };
    UINT skin = g_pDevice->GetSkinManager()->AddSkin(ambient, diff, diff, spec, 8.0f);

    AwesomeGeometryGenerator geoGen;
    AwesomeGeometryGenerator::AwesomeMeshData box = geoGen.CreateBox(1.5f, 0.5f, 1.5f, 3);
    g_pDevice->GetVertexManager()->CreateStaticBuffer(skin, box.Vertices.size(), box.getVertexSize(), box.Vertices.data(),
        box.Indices32.size(), box.GetIndices16().data(), &g_sMeshBox);

    AwesomeGeometryGenerator::AwesomeMeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
    g_pDevice->GetVertexManager()->CreateStaticBuffer(terrainSkin, grid.Vertices.size(), grid.getVertexSize(), grid.Vertices.data(),
        grid.Indices32.size(), grid.GetIndices16().data(), &g_sMeshGrid);


    AwesomeGeometryGenerator::AwesomeMeshData sphere = geoGen.CreateSphere(0.5f, 100, 200);
    g_pDevice->GetVertexManager()->CreateStaticBuffer(skin, sphere.Vertices.size(), sphere.getVertexSize(), sphere.Vertices.data(),
        sphere.Indices32.size(), sphere.GetIndices16().data(), &g_sMeshSphere);


    AwesomeGeometryGenerator::AwesomeMeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 200, 200);
    g_pDevice->GetVertexManager()->CreateStaticBuffer(skin, cylinder.Vertices.size(), cylinder.getVertexSize(), cylinder.Vertices.data(),
        cylinder.Indices32.size(), cylinder.GetIndices16().data(), &g_sMeshCylinder);


    AwesomeOBJLoader loader2;
    loader2.loadOBJ(L"Chinesse.obj");
    g_pDevice->GetVertexManager()->CreateStaticBuffer(skin, loader2.getVerteces().size(), loader2.getVertexSize(), loader2.getVerteces().data(), 0, NULL, &g_sMeshChinesse);


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

    updateFPS();

    updateInput();

    // clear buffers and start scene
    g_pDevice->BeginRendering(true, true, true);

    AWEMatrix mWorld;
    mWorld.Translate(0.0f, 0.5f, 0.0f);
    AWEMatrix mScaling;
    mScaling._11 = mScaling._22 = mScaling._33 = 2.0f;
    g_pDevice->SetWorldTransformMatrix(mScaling * mWorld);
    g_pDevice->GetVertexManager()->Render(g_sMeshBox);

    mWorld.Identity();
    g_pDevice->SetWorldTransformMatrix(mWorld);
    g_pDevice->GetVertexManager()->Render(g_sMeshGrid);

    mWorld.Identity();
    mWorld.Translate(0, 10, 0);
    g_pDevice->SetWorldTransformMatrix(mWorld);
    g_pDevice->GetVertexManager()->Render(g_sMeshChinesse);

    // Rendering the spheres and columns
    for (int i = 0; i < 5; ++i)
    {
        AWEMatrix leftCylWorld;
        leftCylWorld.Translate(-5.0f, 1.5f, -10.0f + i * 5.0f);

        AWEMatrix rightCylWorld;
        rightCylWorld.Translate(+5.0f, 1.5f, -10.0f + i * 5.0f);

        AWEMatrix leftSphereWorld;
        leftSphereWorld.Translate(-5.0f, 3.5f, -10.0f + i * 5.0f);

        AWEMatrix rightSphereWorld;
        rightSphereWorld.Translate(+5.0f, 3.5f, -10.0f + i * 5.0f);

        g_pDevice->SetWorldTransformMatrix(leftCylWorld);
        g_pDevice->GetVertexManager()->Render(g_sMeshCylinder);

        g_pDevice->SetWorldTransformMatrix(rightCylWorld);
        g_pDevice->GetVertexManager()->Render(g_sMeshCylinder);

        g_pDevice->SetWorldTransformMatrix(leftSphereWorld);
        g_pDevice->GetVertexManager()->Render(g_sMeshSphere);

        g_pDevice->SetWorldTransformMatrix(rightSphereWorld);
        g_pDevice->GetVertexManager()->Render(g_sMeshSphere);
    }

    // flip backbuffer to front
    g_pDevice->EndRendering();

    return S_OK;
}
