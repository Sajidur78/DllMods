#pragma once

extern bool Im3d_Init(IDirect3DDevice9* pDevice, HWND window);
extern void Im3d_Shutdown();
extern void Im3d_NewFrame();
extern void Im3d_EndFrame();

extern void Im3d_SetViewMatrix(const Im3d::Mat4& view);
extern void Im3d_SetCamera(const Im3d::Vec3& pos, const Im3d::Vec3& rot);
extern void Im3d_SetCameraPos(const Im3d::Vec3& pos);
extern void Im3d_SetCameraRot(const Im3d::Vec3& rot);
extern void Im3d_SetCameraLookAt(const Im3d::Vec3& point);