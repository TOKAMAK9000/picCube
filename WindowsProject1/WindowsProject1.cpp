#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// global declarations
LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 d3ddev;
LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;
LPDIRECT3DINDEXBUFFER9 i_buffer = NULL;
LPDIRECT3DTEXTURE9 texture = NULL; 

void initD3D(HWND hWnd);
void render_frame(void);
void cleanD3D(void);
void init_graphics(void);
void init_light(void); 

struct CUSTOMVERTEX { FLOAT X, Y, Z; D3DVECTOR NORMAL; FLOAT U, V; };
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"WindowClass";

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL, L"WindowClass", L"Our Direct3D Program",
		WS_OVERLAPPEDWINDOW, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	initD3D(hWnd);

	MSG msg;

	while (TRUE)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;

		render_frame();

		if (KEY_DOWN(VK_ESCAPE))
			PostMessage(hWnd, WM_DESTROY, 0, 0);
	}

	cleanD3D();

	return msg.wParam;
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	init_graphics();  
	init_light(); 

	d3ddev->SetRenderState(D3DRS_LIGHTING, TRUE); 
	d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE); 
	d3ddev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50)); 
}


void render_frame(void)
{
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();

	d3ddev->SetFVF(CUSTOMFVF);

	D3DXMATRIX matView;
	D3DXMatrixLookAtLH(&matView,
		&D3DXVECTOR3(0.0f, 8.0f, 25.0f), 
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f));  
	d3ddev->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection,
		D3DXToRadian(25),
		(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT,
		1.0f,   
		100.0f);  
	d3ddev->SetTransform(D3DTS_PROJECTION, &matProjection);

	static float index = 0.0f; index += 0.015f;
	D3DXMATRIX matRotateY;
	D3DXMatrixRotationY(&matRotateY, index);
	d3ddev->SetTransform(D3DTS_WORLD, &(matRotateY));

	d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
	d3ddev->SetIndices(i_buffer);

	d3ddev->SetTexture(0, texture);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);

	d3ddev->EndScene();

	d3ddev->Present(NULL, NULL, NULL, NULL);
}


void cleanD3D(void)
{
	v_buffer->Release();
	i_buffer->Release();
	texture->Release(); 
	d3ddev->Release();
	d3d->Release();
}


void init_graphics(void)
{
	D3DXCreateTextureFromFile(d3ddev,   
		L"C:\\Users\\TOKAMAK\\Desktop\\new\\image.png",  
		&texture);   

	CUSTOMVERTEX vertices[] =
	{
		{ -3.0f, -3.0f, 3.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, },    // side 1
		{ 3.0f, -3.0f, 3.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, },
		{ -3.0f, 3.0f, 3.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, },
		{ 3.0f, 3.0f, 3.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, },

		{ -3.0f, -3.0f, -3.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, },    // side 2
		{ -3.0f, 3.0f, -3.0f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f, },
		{ 3.0f, -3.0f, -3.0f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f, },
		{ 3.0f, 3.0f, -3.0f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f, },

		{ -3.0f, 3.0f, -3.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, },    // side 3
		{ -3.0f, 3.0f, 3.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, },
		{ 3.0f, 3.0f, -3.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, },
		{ 3.0f, 3.0f, 3.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, },

		{ -3.0f, -3.0f, -3.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f, },    // side 4
		{ 3.0f, -3.0f, -3.0f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f, },
		{ -3.0f, -3.0f, 3.0f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f, },
		{ 3.0f, -3.0f, 3.0f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f, },

		{ 3.0f, -3.0f, -3.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, },    // side 5
		{ 3.0f, 3.0f, -3.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, },
		{ 3.0f, -3.0f, 3.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, },
		{ 3.0f, 3.0f, 3.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, },

		{ -3.0f, -3.0f, -3.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, },    // side 6
		{ -3.0f, -3.0f, 3.0f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f, },
		{ -3.0f, 3.0f, -3.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f, },
		{ -3.0f, 3.0f, 3.0f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f, },
	};

	d3ddev->CreateVertexBuffer(24 * sizeof(CUSTOMVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&v_buffer,
		NULL);

	VOID* pVoid;  

	// lock v_buffer and load the vertices into it
	v_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, vertices, sizeof(vertices));
	v_buffer->Unlock();

	// create the indices using an int array
	short indices[] =
	{
		0, 1, 2,    // side 1
		2, 1, 3,
		4, 5, 6,    // side 2
		6, 5, 7,
		8, 9, 10,    // side 3
		10, 9, 11,
		12, 13, 14,    // side 4
		14, 13, 15,
		16, 17, 18,    // side 5
		18, 17, 19,
		20, 21, 22,    // side 6
		22, 21, 23,
	};

	d3ddev->CreateIndexBuffer(36 * sizeof(short),
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&i_buffer,
		NULL);

	i_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, indices, sizeof(indices));
	i_buffer->Unlock();
}


void init_light(void)
{
	D3DLIGHT9 light;  
	D3DMATERIAL9 material; 

	ZeroMemory(&light, sizeof(light)); 
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f); 
	light.Direction = D3DXVECTOR3(-1.0f, -0.3f, -1.0f);

	d3ddev->SetLight(0, &light);  
	d3ddev->LightEnable(0, TRUE);   

	ZeroMemory(&material, sizeof(D3DMATERIAL9));  
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f); 
	material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f); 

	d3ddev->SetMaterial(&material); 
}
