#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable


// In vulkan is possible separate uniform blocks... also if there is no differences (in this case) with new GPUs:
// e.g. with RTX3080 & 5xx drivers minBlockSize = 512 byte
// ... only on "very old" AMD RX580 minBlockSize was 80 byte
#ifdef VULKAN_BUILD
layout (std140, binding = 1) uniform uBuffer
{
    vec4 lightPos;
    vec4 PoV;
} u;
#else
layout (std140, binding = 0) uniform uBuffer
{
    mat4 pMat;
    mat4 vMat;
    mat4 mMat;
    mat4 cMat;
    mat4 lMat;
    vec3 lightPos;
    vec3 PoV;
} u;
#endif

layout (location = 0) in vsOut {
    vec4 vsColor;
    vec4 vsNormal;
    vec4 vsPos;
    float shininessExp;
};

layout (location = 0) out vec4 outColor;

const float ambientInt   = 0.925;
const float specularInt  = 1.;

#define USE_BLINN_PHONG

void main()
{
        vec3 V = normalize(u.PoV.xyz + vsPos.xyz);
        vec3 L = normalize(u.lightPos.xyz - vsPos.xyz);
        vec3 N = normalize(vsNormal.xyz);

#ifdef USE_BLINN_PHONG 
        vec3 H = normalize(L - V); // Half(angle)Vec
        float specular = pow(max(0.0, dot(H, N)), shininessExp);
#else // Phong
        vec3 R = reflect(L, N);
        float specular = pow(max(0.0, dot(R, V)), shininessExp);
#endif
        float diffuse = max(0.0, dot(N, L))*2.0;
        outColor = vec4(vsColor.rgb * (ambientInt + diffuse) + vec3(specular * specularInt), 1.0);
        outColor = min(vec4(1.0), outColor);

}
