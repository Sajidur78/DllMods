#include "pch.h"
#include <DirectXMath.h>
#include "im3d_directx9.h"
#include "im3d_math.h"

using namespace Im3d;
using namespace DirectX;

Vec3 g_CameraPos{0, 0, 0};
Vec3 g_CameraDir{0, 0, 0};
XMMATRIX g_ViewMat{};
XMMATRIX g_ProjMat{};

IDirect3DDevice9* g_Device{};
HWND g_Window{};
float g_FovY{ MATHF_DEGTORAD(45) };

#define D3DFVF_IM3D (D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE)

#define IM3D_WIDTH_DEFAULT 1280
#define IM3D_HEIGHT_DEFAULT 720

bool Im3d_Init(IDirect3DDevice9* pDevice, HWND window)
{
	Im3d_SetCamera(g_CameraPos, { 90, 0 , 0 });
	g_Device = pDevice;
	g_Window = window;
	g_Device->AddRef();
	return true;
}

void Im3d_CalculateView()
{
	g_ViewMat = XMMatrixLookToRH(
		{ g_CameraPos.x, g_CameraPos.y, g_CameraPos.z },
		{ g_CameraDir.x, g_CameraDir.y, g_CameraDir.z },
		{ 0, 1, 0 });
}

void Im3d_CalculateProjection(float width, float height)
{
	g_ProjMat = XMMatrixPerspectiveFovRH(g_FovY, width / height, 0.1f, 5000);
}

void Im3d_Shutdown()
{
	if (g_Device)
	{
		g_Device->Release();
		g_Device = nullptr;
	}

	g_Window = nullptr;
}

void Im3d_NewFrame()
{
	RECT windowRect{};
	POINT cursorNative{};

	Vec2 cursorPos{};
	GetWindowRect(g_Window, &windowRect);
	const HWND foreWindow = GetForegroundWindow();
	
	if (foreWindow == g_Window || IsChild(foreWindow, g_Window))
		if (GetCursorPos(&cursorNative) && ScreenToClient(g_Window, &cursorNative))
		{
			cursorPos.x = cursorNative.x;
			cursorPos.y = cursorNative.y;
		}
	
	auto& appData = GetAppData();
	appData.m_deltaTime = 1.0f / 60.0f;
	appData.m_viewportSize = 
	{
		static_cast<float>(windowRect.right) - static_cast<float>(windowRect.left),
		static_cast<float>(windowRect.bottom) - static_cast<float>(windowRect.top)
	};

	appData.m_viewOrigin = g_CameraPos;
	appData.m_viewDirection = g_CameraDir;
	appData.m_projOrtho = false;
	appData.m_projScaleY = tanf(g_FovY * 0.5f) * 2.0f;
	
	cursorPos = (cursorPos / appData.m_viewportSize) * 2.0f - Vec2(1.0f, 1.0f);
	cursorPos.y = -cursorPos.y; // window origin is top-left, ndc is bottom-left
	Vec3 rayDirection;
	
	const Vec3 rayOrigin = appData.m_viewOrigin;
	rayDirection.x = cursorPos.x / g_ViewMat.r[0].m128_f32[0];
	rayDirection.y = cursorPos.y / g_ViewMat.r[1].m128_f32[1];
	rayDirection.z = -1.0f;
	rayDirection = Mat4(1) * Vec4(Normalize(rayDirection), 0.0f);

	appData.m_cursorRayOrigin = rayOrigin;
	appData.m_cursorRayDirection = rayDirection;
	
	appData.m_keyDown[Mouse_Left] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

	// The following key states control which gizmo to use for the generic Gizmo() function. Here using the left ctrl
	// key as an additional predicate.
	bool ctrlDown = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0;
	appData.m_keyDown[Key_L] = ctrlDown && (GetAsyncKeyState(0x4c) & 0x8000) != 0;
	appData.m_keyDown[Key_T] = ctrlDown && (GetAsyncKeyState(0x54) & 0x8000) != 0;
	appData.m_keyDown[Key_R] = ctrlDown && (GetAsyncKeyState(0x52) & 0x8000) != 0;
	appData.m_keyDown[Key_S] = ctrlDown && (GetAsyncKeyState(0x53) & 0x8000) != 0;

	// Enable gizmo snapping by setting the translation/rotation/scale increments to be > 0
	appData.m_snapTranslation = ctrlDown ? 0.1f : 0.0f;
	appData.m_snapRotation = ctrlDown ? Radians(30.0f) : 0.0f;
	appData.m_snapScale = ctrlDown ? 0.5f : 0.0f;
	Im3d_CalculateProjection(appData.m_viewportSize.x, appData.m_viewportSize.y);
	NewFrame();
}

void Im3d_SetupRenderState()
{
	g_Device->SetVertexShader(nullptr);
	g_Device->SetPixelShader(nullptr);

	g_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_Device->SetRenderState(D3DRS_LIGHTING, false);
	g_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
	g_Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	g_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	g_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);
	g_Device->SetRenderState(D3DRS_FOGENABLE, false);
	
	g_Device->SetTransform(D3DTS_VIEW, reinterpret_cast<D3DMATRIX*>(&g_ViewMat));
	g_Device->SetTransform(D3DTS_PROJECTION, reinterpret_cast<D3DMATRIX*>(&g_ProjMat));
}

D3DPRIMITIVETYPE Im3d_GetPrimType(DrawPrimitiveType type)
{
	switch(type)
	{
	case DrawPrimitive_Points:
		return D3DPT_POINTLIST;

	case DrawPrimitive_Triangles:
		return D3DPT_TRIANGLELIST;

	case DrawPrimitive_Lines:
		return D3DPT_LINELIST;
		
	case DrawPrimitive_Count:
		return D3DPT_FORCE_DWORD;
	}

	return D3DPT_FORCE_DWORD;
}

void Im3d_Draw(const DrawList& drawList)
{
	const D3DPRIMITIVETYPE primType = Im3d_GetPrimType(drawList.m_primType);
	if (primType == D3DPT_FORCE_DWORD)
		return;
	
	IDirect3DVertexBuffer9* pBuffer{};
	constexpr size_t stride = sizeof(*drawList.m_vertexData);
	const size_t bufferSize = drawList.m_vertexCount * stride;
	g_Device->CreateVertexBuffer(bufferSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
		D3DFVF_IM3D, D3DPOOL_DEFAULT, &pBuffer, nullptr);

	if (!pBuffer)
		return;

	void* pBuf{};
	pBuffer->Lock(0, bufferSize, &pBuf, D3DLOCK_DISCARD);
	memcpy(pBuf, drawList.m_vertexData, bufferSize);
	auto* pBegin = static_cast<VertexData*>(pBuf);
	auto* pEnd = static_cast<VertexData*>(pBuf) + drawList.m_vertexCount;

	for (auto* it = pBegin; it != pEnd; it++)
	{
		Color color = it->m_color.v;
		it->m_color = ((color & 0xFFFFFF00) >> 8 | color & 0x000000FF);
	}
	
	pBuffer->Unlock();

	g_Device->SetStreamSource(0, pBuffer, 0, stride);
	g_Device->SetFVF(D3DFVF_IM3D);
	g_Device->DrawPrimitive(primType, 0, drawList.m_vertexCount);
	
	pBuffer->Release();
}

void Im3d_EndFrame()
{
	EndFrame();
	IDirect3DStateBlock9* pState{};
	g_Device->CreateStateBlock(D3DSBT_ALL, &pState);
	if (!pState)
		return;

	D3DMATRIX last_view, last_projection;
	g_Device->GetTransform(D3DTS_VIEW, &last_view);
	g_Device->GetTransform(D3DTS_PROJECTION, &last_projection);

	Im3d_SetupRenderState();
	for (size_t i = 0; i < GetDrawListCount(); i++)
		Im3d_Draw(GetDrawLists()[i]);
	
	g_Device->SetTransform(D3DTS_VIEW, &last_view);
	g_Device->SetTransform(D3DTS_PROJECTION, &last_projection);
	
	pState->Apply();
	pState->Release();
}

void Im3d_SetViewMatrix(const Im3d::Mat4& view)
{
	*reinterpret_cast<Mat4*>(&g_ViewMat) = view;
}

void Im3d_SetCamera(const Vec3& pos, const Vec3& rot)
{
	g_CameraPos = pos;
	const float xRad = MATHF_DEGTORAD(rot.x);
	const float yRad = MATHF_DEGTORAD(rot.x);
	const float yCos = cosf(yRad);
	
	g_CameraDir = 
	{
		cosf(xRad) * yCos,
		sinf(yRad),
		sinf(xRad) * yCos
	};
	
	Im3d_CalculateView();
}

void Im3d_SetCameraPos(const Vec3& pos)
{
	g_CameraPos = pos;
	Im3d_CalculateView();
}

void Im3d_SetCameraRot(const Vec3& rot)
{
	Im3d_SetCamera(g_CameraPos, rot);
}

void Im3d_SetCameraLookAt(const Vec3& point)
{
	g_CameraDir = Normalize(g_CameraPos - point);
	Im3d_CalculateView();
}
