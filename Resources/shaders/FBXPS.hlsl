#include "FBX.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

//float4 main(VSOutput input) : SV_TARGET
//{
//	float4 texcolor = tex.Sample(smp,input.uv);
//	//Lambert反射
//	float3 light = normalize(float3(1 ,-1, 1));
//	float diffuse = saturate(dot(-light, input.normal));
//	float brightness = diffuse + 0.3f;
//	float4 shadercolor = float4(brightness, brightness, brightness, 1.0f);
//
//	//陰影とテクスチャのいろを合成
//	return shadercolor * texcolor;
//}

float4 main(VSOutput input) : SV_TARGET
{
	float3 light = normalize(float3(1,-1,1)); // 右下奥　向きのライト
	float light_diffuse = saturate(dot(-light, input.normal));
	float3 shade_color;
	shade_color = m_ambient; // アンビエント項
	shade_color += m_diffuse * light_diffuse; // ディフーズ項
	float4 texcolor = tex.Sample(smp, input.uv);
	return float4(texcolor.rgb * shade_color, texcolor.a * m_alpha);
}