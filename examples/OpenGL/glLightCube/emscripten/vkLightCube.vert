#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#ifdef VULKAN_BUILD
#define VTX_INSTANCE gl_InstanceIndex
layout (std140, binding = 0) uniform uBuffer
{
    mat4 pMat;
    mat4 vMat;
    mat4 mMat;
    mat4 cMat;
    mat4 lMat;
} u;

#else
#define VTX_INSTANCE gl_InstanceID
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

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 inColor;

layout (location = 0) out vsOut {
    vec4 vsColor;
    vec4 vsNormal;
    vec4 vsPos;
    float shininessExp;
};

void main()
{
    if(VTX_INSTANCE==1) {       // cube
        vsColor = vec4(inColor, 1.0);
        vsNormal = vec4(mat3(u.mMat) * normal, 1.0);
        vsPos = u.mMat * vec4(pos, 1.0);

        gl_Position = u.pMat * u.vMat * u.cMat * vsPos;
        shininessExp = 500.f;   // BlinnPhong: use (about!) shininessExp*3 for similar Phong rendering apparence (view fragment)
    }
    else {                          // light
        vsNormal = vec4(mat3(u.lMat) * normal, 1.0);
        vsPos = u.lMat * vec4(pos, 1.0);
        vsColor = vec4(1.0, 1.0, 0.5, 1.0);        // light-cube have uniform color...
        shininessExp = 0.f;                         // and shininessExp have no sense
        gl_Position = u.pMat * u.vMat * u.cMat * u.lMat * vec4(pos*.1, 1.0);   // pos*.1 = reducing light-cube
    }
}
