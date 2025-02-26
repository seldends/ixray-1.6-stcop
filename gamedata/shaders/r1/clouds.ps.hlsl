#include "common.hlsli"

struct v2p
{
    float4 color : COLOR0; // rgb. intensity
    float2 tc0 : TEXCOORD0;
    float2 tc1 : TEXCOORD1;
};

uniform sampler2D s_clouds0 : register(s0);
uniform sampler2D s_clouds1 : register(s1);

// Pixel
float4 main(v2p I) : COLOR
{
    float4 s0 = tex2D(s_clouds0, I.tc0);
    float4 s1 = tex2D(s_clouds1, I.tc1);
    float4 mix = I.color * (s0 + s1);

    return float4(mix.rgb, mix.a);
}
