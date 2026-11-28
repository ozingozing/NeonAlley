Texture2D sceneTexture : register(t0);
SamplerState pointSampler : register(s0);

struct VSOut
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOut FullscreenVS(uint id : SV_VertexID)
{
    VSOut output;
    
    output.uv = float2((id << 1) & 2, id & 2);
    output.pos = float4(output.uv * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0);
    
    return output;
}

float4 ToneMapPS(VSOut input) : SV_TARGET
{
    // 2단계: HDR 텍스처를 그대로 복사
    return sceneTexture.Sample(pointSampler, input.uv);
}