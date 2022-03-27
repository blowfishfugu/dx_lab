struct VSOut
{
	//x,y,z,w
	float4 pos : SV_Position;
	//r,g,b
	float3 color : Color;
};

VSOut main(float2 pos : Position, float3 color: Color)
{
	VSOut vso;
	vso.pos=float4(pos.x, pos.y, 0.0f, 1.0f);
	vso.color = color;
	return vso;
}