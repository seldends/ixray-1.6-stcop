#include "common.hlsli"
uniform float4 b_params;

// Pixel
float4 main(p_build I) : SV_Target
{
    // hi-rgb.base-lum
    float3 s0 = s_image.Sample(smp_rtlinear, I.Tex0.xy);
    float3 s1 = s_image.Sample(smp_rtlinear, I.Tex1.xy);
    float3 s2 = s_image.Sample(smp_rtlinear, I.Tex2.xy);
    float3 s3 = s_image.Sample(smp_rtlinear, I.Tex3.xy);

    float3 avg = ((s0 + s1) + (s2 + s3)) / (2.0f * def_hdr);
    float hi = dot(avg, 1.0f) - b_params.x; // assume def_hdr equal to 3.0

    return float4(avg, hi);
}
