////////////////////////////////////////////////////////////////////////////////
// Filename: ApplicationClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "ApplicationClass.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_FloorModel = 0;
	m_BillboardModel = 0;
	m_Timer = 0;
	m_SceneRT = 0;
	m_ToneMapShader = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}

// The model initialization now takes
// in the filename of the model file it is loading.
// In this tutorial we will use the cube.txt file
// so this model loads in a 3D cube object for rendering.
bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char modelFilename[128], textureFilename1[128];
	bool result;


	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass;

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// "평소와 마찬가지로 카메라 객체를 생성합니다. 하지만 카메라가 회전할 때마다 뷰(View) 매트릭스가 수정되므로,
	// 시작할 때 뷰 매트릭스의 복사본을 미리 만들어 둡니다.
	// 매 프레임마다 동일한 위치에 텍스트를 렌더링하려면 이 수정되지 않은 뷰 매트릭스가 필요합니다."
	m_Camera = new CameraClass;

	m_camPos = XMFLOAT3(-4.0f, 5.0f, -4.0f);
	m_Camera->SetPosition(m_camPos.x, m_camPos.y, m_camPos.z);
	m_Camera->SetRotation(5.f, 45.0f, 0.0f);
	m_yaw = 45.0f;
	m_pitch = 15.0f;

	// Create and initialize the texture shader object.
	m_TextureShader = new TextureShaderClass;

	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Set the filenames for the floor model object.
	strcpy_s(modelFilename, "../Resource/floor.txt");
	strcpy_s(textureFilename1, "../Resource/grid01.tga");

	// Create and initialize the floor model object.
	m_FloorModel = new ModelClass;

	result = m_FloorModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename1);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the floor model object.", L"Error", MB_OK);
		return false;
	}

	// Set the filenames for the billboard model object.
	strcpy_s(modelFilename, "../Resource/square.txt");
	strcpy_s(textureFilename1, "../Resource/stone01.tga");

	// Create and initialize the billboard model object.
	m_BillboardModel = new ModelClass;

	result = m_BillboardModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename1);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the billboard model object.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the render to texture object.
	m_SceneRT = new RenderTextureClass;
	result = m_SceneRT->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR, RenderTextureFormat::RT_FORMAT_HDR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the tone map shader object.
	m_ToneMapShader = new ToneMapShaderClass;
	result = m_ToneMapShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the tone map shader object.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the timer object.
	m_Timer = new TimerClass;
	m_Timer->Initialize();

	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the timer object.
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	if (m_ToneMapShader)
	{
		m_ToneMapShader->Shutdown();
		delete m_ToneMapShader;
		m_ToneMapShader = 0;
	}

	if (m_SceneRT)
	{
		m_SceneRT->Shutdown();
		delete m_SceneRT;
		m_SceneRT = 0;
	}

	// Release the billboard model object.
	if (m_BillboardModel)
	{
		m_BillboardModel->Shutdown();
		delete m_BillboardModel;
		m_BillboardModel = 0;
	}

	// Release the floor model object.
	if (m_FloorModel)
	{
		m_FloorModel->Shutdown();
		delete m_FloorModel;
		m_FloorModel = 0;
	}

	// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	return;
}


bool ApplicationClass::Frame(InputClass* Input)
{
	static float rotation = 0.0f;
	static float glassTranslation = 0.0f;
	const float moveSpeed = 0.05f;
	bool result;

	// Check if the user pressed escape and wants to exit the application.
	if (Input->IsEscapePressed())
	{
		return false;
	}

	UpdateCamera(Input);


	// Update the rotation variable each frame.
	rotation -= 0.0174532925f * 0.25f;
	if (rotation < 0.0f)
	{
		rotation += 360.0f;
	}

	glassTranslation += 0.002f;
	if (glassTranslation > 1.0f)
	{
		glassTranslation -= 1.0f;
	}

	// Render the graphics scene.
	result = Render();
	if (!result)
	{
		return false;
	}
	return true;
}

void ApplicationClass::UpdateCamera(InputClass* Input)
{
	const float lookSpeed = 0.1f;
	const float moveSpeed = Input->IsKeyDown(DIK_LSHIFT) ? 0.3f : 0.1f;

	// 1.마우스 -> 각도
	int dx, dy;
	Input->GetMouseDelta(dx, dy);
	m_yaw += dx * lookSpeed;
	m_pitch += dy * lookSpeed;
	if (m_pitch > 89.0f) m_pitch = 89.0f;
	if (m_pitch < -89.0f) m_pitch = -89.0f;
	if (m_yaw > 360.0f) m_yaw -= 360.0f;
	if (m_yaw < 0.0f) m_yaw += 360.0f;

	// 2.각도 -> 앞/오른쪽 방향
	float yaw = XMConvertToRadians(m_yaw);
	float pitch = XMConvertToRadians(m_pitch);
	XMFLOAT3 foward(cosf(pitch) * sinf(yaw), -sinf(pitch), cosf(pitch) * cosf(yaw));
	XMFLOAT3 right(cosf(yaw), 0.0f, -sinf(yaw));

	// 3.키보드 -> 이동
	if (Input->IsKeyDown(DIK_W)) m_camPos.x += foward.x * moveSpeed, m_camPos.y += foward.y * moveSpeed, m_camPos.z += foward.z * moveSpeed;
	if (Input->IsKeyDown(DIK_S)) m_camPos.x -= foward.x * moveSpeed, m_camPos.y -= foward.y * moveSpeed, m_camPos.z -= foward.z * moveSpeed;
	if (Input->IsKeyDown(DIK_A)) m_camPos.x -= right.x * moveSpeed, m_camPos.z -= right.z * moveSpeed;
	if (Input->IsKeyDown(DIK_D)) m_camPos.x += right.x * moveSpeed, m_camPos.z += right.z * moveSpeed;
	if (Input->IsKeyDown(DIK_SPACE)) m_camPos.y += moveSpeed;
	if (Input->IsKeyDown(DIK_LCONTROL)) m_camPos.y -= moveSpeed;

	// 5. 카메라에 적용
	m_Camera->SetPosition(m_camPos.x, m_camPos.y, m_camPos.z);
	m_Camera->SetRotation(m_pitch, m_yaw, 0.0f);
	m_Camera->Render();
}

// Render 함수는 씬을 백 버퍼(Back Buffer)에 렌더링하고,
// 반사 및 굴절 렌더 투 텍스처(Render to Texture)를 사용하여 물을 렌더링하는 곳입니다.
bool ApplicationClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;
	XMFLOAT3 cameraPosition, modelPosition;
	double angle;
	float pi, rotation;
	bool result;

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 장면을 HDR 텍스처에 그린다
	m_SceneRT->SetRenderTarget(m_Direct3D->GetDeviceContext());
	m_SceneRT->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Put the floor model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_FloorModel->Render(m_Direct3D->GetDeviceContext());

	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_FloorModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_FloorModel->GetTexture());
	if (!result)
	{
		return false;
	}

	// Get the position of the camera.
	cameraPosition = m_Camera->GetPosition();

	// Set the position of the billboard model.
	modelPosition.x = 0.0f;
	modelPosition.y = 1.5f;
	modelPosition.z = 0.0f;

	// 아크탄젠트(atan2) 함수를 사용하여 빌보드 모델이 현재 카메라 위치를 바라보도록 적용할 회전 각도를 계산합니다.
	pi = 3.14159265358979323846f;
	angle = atan2(modelPosition.x - cameraPosition.x, modelPosition.y - cameraPosition.z) * (180.0f / pi);

	// 회전 각도를 라디안(Radian) 단위로 변환합니다.
	rotation = (float)angle * 0.0174532925f;

	// 월드 매트릭스를 사용하여 원점에서 빌보드의 회전을 설정합니다.
	worldMatrix = XMMatrixRotationY(rotation);

	// 빌보드 모델의 평행이동(Translation) 매트릭스를 설정합니다.
	translateMatrix = XMMatrixTranslation(modelPosition.x, modelPosition.y, modelPosition.z);

	// 마지막으로 회전 매트릭스와 평행이동 매트릭스를 결합하여 빌보드 모델의 최종 월드 매트릭스를 생성합니다.
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);

	// 그리기를 준비하기 위해 빌보드 모델의 버텍스 및 인덱스 버퍼를 그래픽 파이프라인에 올립니다.
	m_BillboardModel->Render(m_Direct3D->GetDeviceContext());

	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(),
		m_BillboardModel->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix,
		m_BillboardModel->GetTexture());
	if (!result)
	{
		return false;
	}

	// ===== 전체 화면 패스: HDR 텍스처 → 백버퍼 =====
	// 그릴 곳을 다시 백버퍼로 되돌리고 뷰포트도 화면 크기로 되돌립니다.
	m_Direct3D->SetBackBufferRenderTarget();
	m_Direct3D->ResetViewport();

	// 전체 화면 삼각형은 가림 판단이 필요 없으므로 깊이 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// Tone map the rendered scene.
	result = m_ToneMapShader->Render(m_Direct3D->GetDeviceContext(), m_SceneRT->GetShaderResourceView());
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}