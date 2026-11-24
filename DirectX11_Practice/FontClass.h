////////////////////////////////////////////////////////////////////////////////
// Filename: fontclass.h
////////////////////////////////////////////////////////////////////////////////
// 먼저 새로운 FontClass를 살펴보겠습니다. 이 클래스는 폰트의 텍스처,
// 텍스트 파일에서 가져온 폰트 데이터, 그리고 폰트 데이터를 사용하여
// 버텍스 버퍼를 생성하는 함수를 처리합니다.
// 개별 문장의 폰트 데이터를 저장하는 버텍스 버퍼는 TextClass에 있으며,
// 이 클래스 안에는 없습니다. 따라서 이 클래스는 문장을 구성하고 렌더링 준비가 되면
// 폰트 텍스처에 접근할 수 있도록 해주는 역할만 합니다.

#ifndef _FONTCLASS_H_
#define _FONTCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <directxmath.h>
#include <fstream>
using namespace DirectX;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: FontClass
////////////////////////////////////////////////////////////////////////////////
class FontClass
{
private:
    
    // FontType 구조체는 글꼴 인덱스 파일에서 읽어온 인덱싱 데이터를 저장하는 데 사용됩니다.
    // left와 right는 TU 텍스처 좌표이고, size는 문자의 너비를 픽셀 단위로 나타냅니다.
    struct FontType
    {
        float left, right;
        int size;
    };
    
    // VertexType 구조체는 텍스트 문자를 렌더링하는 데 사용되는
    // 사각형을 구성하는 실제 정점 데이터를 위한 것입니다.
    // 개별 문자는 두 개의 삼각형으로 이루어진 사각형을 구성해야 합니다.
    // 이 삼각형에는 위치 및 텍스처 데이터만 포함됩니다.
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
    };

public:
    FontClass();
    FontClass(const FontClass&);
    ~FontClass();

    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int);
    void Shutdown();

    ID3D11ShaderResourceView* GetTexture();
    
    // BuildVertexArray 함수는 입력으로 주어진 문장을 렌더링하는 데
    // 필요한 삼각형들의 정점 배열을 생성하고 반환합니다.
    // 이 함수는 새로운 TextClass에서 호출되어 렌더링에 필요한
    // 모든 문장의 정점 배열을 생성합니다. 따라서, 렌더링할 문장을 함수에 입력하면
    // 미리 생성된 정점 배열을 반환받아 TextClass에 저장할 수 있습니다.
    // 간단히 말해, 이 함수는 정점 배열을 생성하는 빌더 클래스입니다.
    void BuildVertexArray(void*, char*, float, float);
    int GetSentencePixelLength(char*);
    int GetFontHeight();

private:
    bool LoadFontData(char*);
    void ReleaseFontData();
    bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
    void ReleaseTexture();

private:
    FontType* m_Font;
    TextureClass* m_Texture;
    float m_fontHeight;
    int m_spaceSize;
};

#endif