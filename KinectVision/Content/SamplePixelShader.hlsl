cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
    float4 vLightDir[2];
    float4 vLightColor[2];
};

// Per-pixel color data with normals
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 normal : TEXCOORD0;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 finalColor = 0;

    //do NdotL lighting for 2 lights
    for (int i = 0; i<2; i++)
    {
        finalColor += saturate(dot((float3)vLightDir[i], input.normal) * vLightColor[i]);
    }
    finalColor.a = 1;
    return finalColor;
}
