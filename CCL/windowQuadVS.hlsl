#include "windowQuad.hlsli"

VS_OUT main(float4 position : POSITION, float4 color : COLOR, float2 texcoord : VIEW_COORD) {
	VS_OUT vout;
	vout.position = float4(position.xyz, 1.0);
	vout.texcoord = texcoord;
	vout.color = color;

	return vout;
}