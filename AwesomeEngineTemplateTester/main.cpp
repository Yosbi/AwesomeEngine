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
std::wstring g_windowCaption = L"Awesome Engine v0.1 - By Yosbi Alves Saenz";

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

UINT g_sMesh = 0;

VERTEX* g_Vertex = NULL;
int g_nIndis = 0;
int g_nVerts = 0;
WORD* g_Index = NULL;

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
    g_pDevice->SetClearColor(0.0f, 1.0f, 1.0f);

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

void initCamera() {
    AWEVector pos = AWEVector(0, 30, -30);
    AwesomeFreeCam *freeCam1 = new AwesomeFreeCam();
    freeCam1->SetPos(pos);
    freeCam1->SetRotation(( 45 * AWEPI) / 180.0f, 0.0f, 0.0f);
       
    pos = AWEVector(0, 10, 30);
    AwesomeFreeCam* freeCam2 = new AwesomeFreeCam();
    freeCam2->SetPos(pos);
    freeCam2->SetRotation((10 * AWEPI) / 180.0f, AWEPI, 0.0f);


    pos = AWEVector(0.0f, 100, 0.0f);
    AwesomeFreeCam* freeCam3 = new AwesomeFreeCam();
    freeCam3->SetPos(pos);
    freeCam3->SetRotation((90 * AWEPI) / 180.0f, 0.0f, 0.0f);

    pos = AWEVector(0, 0.0, -30);
    AwesomeFirstPersonCam* fpCam = new AwesomeFirstPersonCam();
    fpCam->SetPos(pos);

    g_vcCameras.push_back(freeCam1);
    g_vcCameras.push_back(freeCam2);
    g_vcCameras.push_back(freeCam3);
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

    
    GenGrid(50, 50, 1.0f, 1.0f, AWEVector(0.0f, 0.0f, 0.0f));

    /*AwesomeOBJLoader loader;
    loader.loadOBJ(L"terrain.obj");

    AWESOMECOLOR diff = { 1.0f, 0.0f, 0.0f, 1.0f };
    AWESOMECOLOR ambient = { 1.0f, 0.0f, 0.0f, 1.0f };
    AWESOMECOLOR spec = { 0.35f, 0.35f, 0.35f, 1.0f };
    g_nSkinTiles = g_pDevice->GetSkinManager()->AddSkin(ambient, diff, diff, spec, 8.0f);
    g_pDevice->GetVertexManager()->CreateStaticBuffer(VID_UU, g_nSkinTiles, loader.getVerteces().size(), 0, loader.getVerteces().data(), NULL, &g_sMesh);*/


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

    if (timeCount > 1.0f) {
        int fps = std::round(g_aweTimer.GetFPS());
        std::wstring newCaption = g_windowCaption + L" | " + std::to_wstring(fps);

        // Now set the new caption to the main window. 
        SetWindowText(g_hWnd, newCaption.c_str());

        timeCount = 0.0f;
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

    g_pDevice->GetVertexManager()->Render(g_sMesh);
    //g_pDevice->RenderMesh(Terrain);

    //g_pDevice->RenderMesh(Chinesse);

    // flip backbuffer to front
    g_pDevice->EndRendering();

    return S_OK;
} 

//-----------------------------------------------------------------------------
// Name: GenGrid
// Desc: Generates a grid
//-----------------------------------------------------------------------------
void GenGrid(int numVertRows, int numVertCols, float dx, float dz, const AWEVector& center)
{
    g_nVerts = numVertRows * numVertCols;
    int numCellRows = numVertRows - 1;
    int numCellCols = numVertCols - 1;

    int numTris = numCellRows * numCellCols * 2;

    float width = (float)numCellCols * dx;
    float depth = (float)numCellRows * dz;

    // Build vertices.	

    g_Vertex = new VERTEX[g_nVerts];

    // Offsets to translate grid from quadrant 4 to center of 
    // coordinate system.
    float xOffset = -width * 0.5f;
    float zOffset = depth * 0.5f;

    float texScale = 0.2f; // scale the texture cood
    int k = 0;
    for (float i = 0; i < numVertRows; ++i)
    {
        for (float j = 0; j < numVertCols; ++j)
        {
            // Negate the depth coordinate to put in quadrant four.  
            // Then offset to center about coordinate system.
            g_Vertex[k].x = j * dx + xOffset;
            g_Vertex[k].z = -i * dz + zOffset;
            g_Vertex[k].y = 0.0f;

            // Translate so that the center of the grid is at the
            // specified 'center' parameter.
            AWEMatrix T;
            T.SetTranslation(AWEVector(center.x, center.y, center.z), true);

            AWEVector vc(g_Vertex[k].x, g_Vertex[k].y, g_Vertex[k].z);
            vc = vc * T;

            g_Vertex[k].x = vc.x;
            g_Vertex[k].y = vc.y;
            g_Vertex[k].z = vc.z;
            g_Vertex[k].vcN[0] = g_Vertex[k].vcN[2] = 0.0f;
            g_Vertex[k].vcN[1] = 1.0f;
            g_Vertex[k].tu = i * texScale;
            g_Vertex[k].tv = j * texScale;

            ++k; // Next vertex
        }
    }

    // Set the texture coord

    // Build indices.
    g_nIndis = numTris * 3;
    g_Index = new WORD[g_nIndis];


    // Generate indices for each quad.
    k = 0;
    for (WORD i = 0; i < (WORD)numCellRows; ++i)
    {
        for (WORD j = 0; j < (WORD)numCellCols; ++j)
        {
            g_Index[k] = i * numVertCols + j;
            g_Index[k + 1] = i * numVertCols + j + 1;
            g_Index[k + 2] = (i + 1) * numVertCols + j;

            g_Index[k + 3] = (i + 1) * numVertCols + j;
            g_Index[k + 4] = i * numVertCols + j + 1;
            g_Index[k + 5] = (i + 1) * numVertCols + j + 1;

            // next quad
            k += 6;
        }
    }

    AWESOMECOLOR diff = { 1.0f, 1.0f, 1.0f, 1.0f };
    AWESOMECOLOR ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
    AWESOMECOLOR spec = { 0.4f, 0.4f, 0.4f, 1.0f };
    float fSpec = 8.0f;
    UINT z = -99;
    // Floor
    z = g_pDevice->GetSkinManager()->AddSkin(ambient, diff, diff, spec, fSpec);
    //g_pDevice->GetSkinManager()->AddTexture(z, "wood-floorboards.dds", false, 0.0f, NULL, 0);
    g_pDevice->GetVertexManager()->CreateStaticBuffer(VID_UU, z, g_nVerts, g_nIndis, g_Vertex, g_Index, &g_sMesh);

    
    //delete[] verts;
    //delete[] indices;
}
