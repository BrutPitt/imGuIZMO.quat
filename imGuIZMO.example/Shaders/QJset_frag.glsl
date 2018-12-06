#ifdef GL_ES
precision highp float;
#endif

uniform vec2 mouse;
uniform vec2 resolution;

uniform float Qx, Qy, Qz, Qw;
uniform float lightR, lightG, lightB;
uniform float qjR, qjG, qjB;
uniform float Iter;

uniform bool isFullRender;
//mat3 Orientation = mat3(1);// mat3(1.,0.,0., 0.,1.,0., 0.,0.,1.);
//uniform mat3 Orientation;
//uniform vec3 vA, vB, vC;

/*
mat3 Orientation = mat3(0.802921832,	
		0.179758698,
		0.568333745	,
		0.0201827362,
		0.961097002	,
		0.275472611	,
    	0.595742464,
		0.209712431,
		1.0	);
*/

const float BOUNDING_RADIUS_2 = 3.0;
const float ESCAPE_THRESHOLD = 10.;
const float DEL = .00001;

/*
uniform vec3 Eye;
uniform vec3 Light;
uniform vec3 LightColor;
uniform vec4 Quat;
uniform float Epsilon;
*/

vec3 Eye = vec3(0., 0., 2.2);
mat3 Orientation = mat3(mouse.x,mouse.y,mouse.x, mouse.y,mouse.x,mouse.y,mouse.x, mouse.y,1.0);
vec3 Light = vec3(3., 3., 3.);
//vec3 LightColor = vec3(0.95, 0.7, 0.1);
//vec4 Quat = vec4(0.25, 0.5, 0.0, 0.25);
float Epsilon = 0.003;

float Slice = .0;
const int maxIterations = 10;
const int maxRayMarch = 220;

vec3 origin, dir;


vec4 QuatMul(vec4 q1, vec4 q2) {
    vec4 r;
    r.x = q1.x * q2.x - dot(q1.yzw, q2.yzw);
    r.yzw = q1.x * q2.yzw + q2.x * q1.yzw + cross(q1.yzw, q2.yzw);
    return r;
}

vec4 QuatSq(vec4 q) {
    vec4 r;
    r.x = q.x * q.x - dot(q.yzw, q.yzw);
    r.yzw = 2. * q.x * q.yzw;
    return r;
}

void IterateIntersect(inout vec4 z, inout vec4 zp, vec4 c) 
{
		for (int h = 0; h < 11; h++) {
			zp = 2.0 * QuatMul(z, zp);
			z = QuatSq(z) + c;
			if (dot(z, z) > ESCAPE_THRESHOLD) break;
		}
}



bool jinteresct(in vec3 rO, in vec3 rD, in vec4 c, out float resT, out vec4 resC )
{
    float mz2,md2,dist;
	//float res=1000.0;
	vec4 z,nz;
	float epsilon = isFullRender ? 0.01 : 0.01;

	
    float t = 0.0;
	for( int j=0; j<220; j++ )
	{
	    vec3 p = rO + t*rD;

		z = vec4(p,0.0);
		md2 = 1.0;
		mz2 = dot(z,z);

        vec4 trap = vec4(abs(z.xyz),dot(z,z));

		for(int i=0;i<12;i++)
		{
			// |dz|^2 -> 4*|dz|^2
			md2*=4.0*mz2;
			// z -> z2 + c
	        nz.x=z.x*z.x-dot(z.yzw,z.yzw);
			nz.yzw=2.0*z.x*z.yzw;
			z=nz+c;

	        trap = min( trap, vec4(abs(z.xyz),dot(z,z)) );

			mz2 = dot(z,z);
			if(mz2>4.0)
            {
                break;
            }
		}

		dist = 0.25*sqrt(mz2/md2)*log(mz2);

		if(dist<epsilon)
		{
            resC = trap;
			resT = t;
			return true;
		}
		
		t += dist;

	}

	return false;
}


vec3 NormEstimate(vec3 p, vec4 c, float slice) {
    vec3 N;
    float gradX, gradY, gradZ;
    vec4 gx1 = vec4(p - vec3(DEL, 0., 0.), slice);
    vec4 gx2 = vec4(p + vec3(DEL, 0., 0.), slice);
    vec4 gy1 = vec4(p - vec3(0., DEL, 0.), slice);
    vec4 gy2 = vec4(p + vec3(0., DEL, 0.), slice);
    vec4 gz1 = vec4(p - vec3(0., 0., DEL), slice);
    vec4 gz2 = vec4(p + vec3(0., 0., DEL), slice);
    for (int i = 0; i < maxIterations; i++) {
        gx1 = QuatSq(gx1) + c;
        gx2 = QuatSq(gx2) + c;
        gy1 = QuatSq(gy1) + c;
        gy2 = QuatSq(gy2) + c;
        gz1 = QuatSq(gz1) + c;
        gz2 = QuatSq(gz2) + c;
    }
    gradX = length(gx2) - length(gx1);
    gradY = length(gy2) - length(gy1);
    gradZ = length(gz2) - length(gz1);
    N = normalize(vec3(gradX, gradY, gradZ));
    return N;
}

float IntersectQJulia(vec4 c, float epsilon, float slice) {
    float dist;
    for(int i = 0; i<150; i++) {
        vec4 z = vec4(origin, slice);
        vec4 zp = vec4(1., 0., 0., 0.);
        IterateIntersect(z, zp, c);
        float normZ = length(z);
        dist = 0.5 * normZ * log(normZ) / length(zp);
        origin += dir * dist;
        if (dist < epsilon  || dot(origin, origin) > BOUNDING_RADIUS_2) break;
    }
    return dist;
}

vec3 Phong(vec3 light, vec3 eye, vec3 pt, vec3 N) {
    vec3 diffuse = vec3(lightR, lightG, lightB);
    float specularExponent = 16.;
    float specularity = 0.5;
    vec3 L = normalize(light - pt);
    vec3 E = normalize(eye - pt);
    float NdotL = dot(N, L);
    vec3 R = L - 2. * NdotL * N;
    diffuse += abs(N) * 0.3;
    return vec3(0.1) + diffuse * max(NdotL, 0.0) + specularity * pow(max(dot(E, R), 0.0), specularExponent);
}

const float NR = .66;


void main() {
	float colorMul = .2;
	vec4 Quat = vec4(Qx, Qy, Qz, Qw);
	mat3 orient = Orientation;



    vec3 color = vec3(0.,0.,0.);
	vec2 fragCoord = gl_FragCoord.xy*2.;// resolution*mouse
	vec2 sampleCoord[5];
	 sampleCoord[0] = vec2(-NR, -NR);
	 sampleCoord[1] = vec2( NR, -NR);
	 sampleCoord[2] = vec2( NR,  NR);
	 sampleCoord[3] = vec2(-NR,  NR);
	 sampleCoord[4] = vec2( 0.,  0.);

    for (int s = 0; s < 5; ++s) {
		vec2 coord = isFullRender ? fragCoord + sampleCoord[s] : fragCoord;

        //vec3 ray = vec3(((coord.x - resolution.x) / resolution.x) , ((coord.y - resolution.y) / resolution.y), -1.);
		vec3 ray = vec3(((coord - resolution) / resolution) , -1.);
        dir = normalize(orient * -ray);
        origin = orient * -Eye;
		float dist;
        //dist = IntersectQJulia(Quat, Epsilon, Slice);
		

		//color = vec3(dist);
		vec4 col;
		
        if (jinteresct(origin, dir, Quat, dist,col)) {
		//if (dist < Epsilon) {

            vec3 N = NormEstimate(origin+dist*dir, Quat, Slice);
            color += Phong(orient * Light, orient * Eye, origin, -N);
			//color = vec3(col);
        }

		if(!isFullRender) { colorMul = 1.0; break; }
    }
    gl_FragColor = vec4(color * colorMul, 1.);
}
		
	
