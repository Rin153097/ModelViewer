#include "sprite.hlsli"
SamplerState linear_sampler_state : register(s1);
SamplerState anisotropic_sampler_state : register(s2);

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);


float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = diffuseMap.Sample(anisotropic_sampler_state, pin.texcoord);
	float alpha = color.a;

#if 1 // Inverse Gamma
	const float GAMMA = 2.2f;
	color.rgb = pow(color.rgb, GAMMA);
#endif

	return float4(color.rgb, alpha) * pin.color;
}