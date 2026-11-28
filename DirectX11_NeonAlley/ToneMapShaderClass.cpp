#include "ToneMapShaderClass.h"

ToneMapShaderClass::ToneMapShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_sampleState = 0;
}

ToneMapShaderClass::ToneMapShaderClass(const ToneMapShaderClass&)
{
}

ToneMapShaderClass::~ToneMapShaderClass()
{
}

bool ToneMapShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;

	error = wcscpy_s(vsFilename, 128, L"ToneMap.hlsl");
	if (error != 0)
	{
		return false;
	}

	// 픽셀 셰이더 파일명을 설정합니다.
	error = wcscpy_s(psFilename, 128, L"ToneMap.hlsl");
	if (error != 0)
	{
		return false;
	}

	// 버텍스 및 픽셀 셰이더를 초기화합니다.
	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ToneMapShaderClass::Shutdown()
{
	ShutdownShader();

	return;
}

bool ToneMapShaderClass::Render(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* sceneTexture)
{
	bool result;

	// 렌더링에 사용할 셰이더 매개변수를 설정합니다.
	result = SetShaderParameters(deviceContext, sceneTexture);
	if (!result)
	{
		return false;
	}

	// 이제 셰이더를 사용하여 준비된 버퍼를 렌더링합니다.
	RenderShader(deviceContext);

	return true;
}

bool ToneMapShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	// 이 함수에서 설정될 텍스처 샘플러의 설명을 담을 새로운 변수입니다.
	D3D11_SAMPLER_DESC samplerDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// 버텍스 쉐이더 코드 컴파일
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "FullscreenVS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// 셰이더 컴파일에 실패하면 오류 메시지가 작성되었을 것입니다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// 오류 메시지가 없으면 셰이더 파일을 찾지 못한 것입니다.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 픽셀 셰이더 코드를 컴파일합니다.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ToneMapPS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// 셰이더 컴파일에 실패하면 오류 메시지가 작성되었을 것입니다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// 오류 메시지가 없으면 파일을 찾지 못한 것입니다.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 버퍼로부터 버텍스 셰이더를 생성합니다.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// 버퍼로부터 픽셀 셰이더를 생성합니다.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// 더 이상 필요하지 않으므로 버텍스 셰이더 버퍼와 픽셀 셰이더 버퍼를 해제합니다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 텍스처 샘플러 상태를 생성합니다.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ToneMapShaderClass::ShutdownShader()
{
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// 픽셀 셰이더를 해제합니다.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// 버텍스 셰이더를 해제합니다.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

void ToneMapShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;


	// 오류 메시지 텍스트 버퍼에 대한 포인터를 가져옵니다.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// 메시지의 길이를 가져옵니다.
	bufferSize = errorMessage->GetBufferSize();

	// 오류 메시지를 기록할 파일을 엽니다.
	fout.open("shader-error.txt");

	// 오류 메시지를 기록합니다.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// 파일을 닫습니다.
	fout.close();

	// 오류 메시지를 해제합니다.
	errorMessage->Release();
	errorMessage = 0;

	// 사용자에게 컴파일 오류를 확인하도록 텍스트 파일 확인을 알리는 메시지 창을 띄웁니다.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool ToneMapShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture)
{
	// SetShaderParameters 함수는 이제 픽셀 셰이더에 텍스처를 설정하는 기능을 포함하도록 이전 튜토리얼에서 수정되었습니다.
	// 픽셀 셰이더에 셰이더 텍스처 리소스를 설정합니다.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	return true;
}

// RenderShader는 폴리곤을 렌더링하기 위해 셰이더 테크닉을 호출합니다.
void ToneMapShaderClass::RenderShader(ID3D11DeviceContext* deviceContext)
{
	// 버텍스 입력 레이아웃을 설정합니다.
	deviceContext->IASetInputLayout(nullptr);
	// 앞의 Draw가 남긴 상태에 기대지 않고 직접 설정합니다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 셰이더와 샘플러를 설정합니다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// 삼각형을 렌더링합니다.
	deviceContext->Draw(3, 0);

	// 다음 프레임에 m_SceneRT를 렌더 타깃으로 다시 쓸 수 있도록 SRV 슬롯을 비웁니다.
	ID3D11ShaderResourceView* nullSRV = nullptr;
	deviceContext->PSSetShaderResources(0, 1, &nullSRV);

	return;
}