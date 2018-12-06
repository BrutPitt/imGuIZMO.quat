//#version 430 core
layout (location = 2) in vec2 vPos;

#ifdef QJSET_USE_PIPELINE
out gl_PerVertex
{
	vec4 gl_Position;
};
#endif

void main(void)
{
    gl_Position = vec4(vPos.xy,.0f,1.f);
}