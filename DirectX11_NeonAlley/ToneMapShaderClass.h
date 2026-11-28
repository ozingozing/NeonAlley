#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class ToneMapShaderClass
{
public:
    ToneMapShaderClass();
	ToneMapShaderClass(const ToneMapShaderClass&);
	~ToneMapShaderClass();

    bool Initialize(ID3D11Device* device, HWND hwnd);
    void Shutdown();
    bool Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* sceneTexture);

private:
    bool InitializeShader(ID3D11Device*, HWND, WCHAR* vsFilename, WCHAR* psFilename);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);

    // 셰이더에 매개변수를 설정하는 함수
    bool SetShaderParameters(ID3D11DeviceContext*, ID3D11ShaderResourceView*);
    // 셰이더를 사용하여 렌더링하는 함수
    void RenderShader(ID3D11DeviceContext*);

private:
    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11SamplerState* m_sampleState;
};

