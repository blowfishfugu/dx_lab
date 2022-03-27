struct VSOut
{
	//r,g,b
	float3 color : Color;
	//x,y,z,w
	float4 pos : SV_Position;
};

cbuffer CBuf : register(b0)
{
	row_major matrix transform;
};

VSOut main(float2 pos : Position, float3 color: Color)
{
	VSOut vso;
	vso.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f), transform);
	vso.color = color;
	return vso;
}