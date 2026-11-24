#pragma once
////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "D3DClass.h"
#include "InputClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "TextureShaderClass.h"
#include "PositionClass.h"
#include "TimerClass.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;


////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
public:
    ApplicationClass();
    ApplicationClass(const ApplicationClass&);
    ~ApplicationClass();

    bool Initialize(int, int, HWND);
    void Shutdown();
    bool Frame(InputClass*);

private:
    bool IsKeyDown(unsigned char key);
    bool Render();
    void UpdateCamera(InputClass* Input);

private:
    XMFLOAT3 m_camPos;
    float m_yaw, m_pitch;     // 도(degree) 단위

private:
    D3DClass* m_Direct3D;
    CameraClass* m_Camera;
	TextureShaderClass* m_TextureShader;
    ModelClass* m_FloorModel, * m_BillboardModel;
	TimerClass* m_Timer;
};
#endif