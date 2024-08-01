#include "common.hlsli"
#include "shadow.hlsli"

#ifndef USE_SUNMASK
float3x4 m_sunmask;
#endif

Texture2D s_water;

float4 main(float2 tc : TEXCOORD0, float2 tcJ : TEXCOORD1, float4 pos2d : SV_Position) : SV_Target
{
    IXrayGbuffer O;
    GbufferUnpack(tc.xy, pos2d.xy, O);

    float4 _P = float4(O.Point.xyz, 1.0f);
    float4 PS = mul(m_shadow, _P);

    float s = shadow(PS);
    float2 tc1 = mul(m_sunmask, _P).xy;
    tc1 = frac(tc1 * 0.5f);

    float4 water = s_water.SampleLevel(smp_linear, tc1, 0);

    water.xyz = 2.0f * water.xzy - 1.0f;
    water.xyz = mul(m_V, water.xyz);
    water *= s;

    return float4(water.xyz, s * 0.5f);
}

