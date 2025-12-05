#include "PostEffect.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    return float4((texcolor.rgb), 1);
    
    
    //float u_ = 1.0f / 1280.0f;
    //float v_ = 1.0f / 720.0f;
    //float4 colorWeight = { 0, 0, 0, 0 };
    
    //colorWeight += tex.Sample(smp, float2(input.uv.x - u_, input.uv.y - v_));
    //colorWeight += tex.Sample(smp, float2(input.uv.x, input.uv.y - v_));
    //colorWeight += tex.Sample(smp, float2(input.uv.x + u_, input.uv.y - v_));
    
    //colorWeight += tex.Sample(smp, float2(input.uv.x - u_, input.uv.y));
    //colorWeight += tex.Sample(smp, float2(input.uv.x, input.uv.y));
    //colorWeight += tex.Sample(smp, float2(input.uv.x + u_, input.uv.y));
    
    //colorWeight += tex.Sample(smp, float2(input.uv.x - u_, input.uv.y + v_));
    //colorWeight += tex.Sample(smp, float2(input.uv.x, input.uv.y + v_));
    //colorWeight += tex.Sample(smp, float2(input.uv.x + u_, input.uv.y + v_));
    
    //return float4((colorWeight.rgb / 9), 1);
}