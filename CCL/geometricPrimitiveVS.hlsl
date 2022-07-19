#include "geometricPrimitive.hlsli"

VS_OUT main( float4 position : POSITION, float4 normal : NORMAL ) {
	VS_OUT vout;
	vout.position = mul(position, mul(world, viewProjection));

	normal.w = 0;
	float4 Normal = normalize(mul(normal, world));
	float4 Light = normalize(-lightDirection);

	vout.color.rgb = materialColor.rgb * max(0, dot(Light, Normal));
	vout.color.a = materialColor.a;
	return vout;
}