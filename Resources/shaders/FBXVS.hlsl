#include "FBX.hlsli"
//エントリーポイント
//VSOutput main(VSInput input) {
//	//法線にワールド行列による回転・スケーリングを適用
//	float4 wnormal = normalize(mul(world, float4(input.normal, 0)));
//	//ピクセルシェーダーに渡す値
//	VSOutput output;
//	//行列による座標変換
//	output.svpos = mul(mul(viewproj, world), input.pos);
//	//ワールド法線を次のステージに移す
//	output.normal = wnormal.xyz;
//	//入力値をそのまま次のステージに渡す
//	output.uv = input.uv;
//
//	return output;
//}
//エントリーポイント
VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	VSOutput output; // ピクセルシェーダーに渡す値
	output.svpos = mul(mul(mul(projection, view), matWorld), pos);
	output.normal = normal;
	output.uv = uv;
	return output;
}