#ifdef GL_ES
precision highp float;
#endif
uniform vec2 mouse;
uniform vec2 resolution;


bool jinteresct(in vec3 rO, in vec3 rD, in vec4 c, out float resT, out vec4 resC )
{
    float mz2,md2,dist;
	float res=1000.0;
	vec4 z,nz;

	
    float t = 0.0;
	for( int j=0; j<220; j++ )
	{
	    vec3 p = rO + t*rD;

		z = vec4(p,0.0);
		md2 = 1.0;
		mz2 = dot(z,z);

        vec4 trap = vec4(abs(z.xyz),dot(z,z));

		for(int i=0;i<11;i++)
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

		if(dist<0.0005)
		{
            resC = trap;
			resT = t;
			return true;
		}
		
		t += dist;

	}

	return false;
}


vec3 calcNormal(in vec3 p, in vec4 c)
{
    vec4 nz, ndz;

    vec4 z = vec4(p,0.0);

	vec4 dz0 = vec4(1.0,0.0,0.0,0.0);
	vec4 dz1 = vec4(0.0,1.0,0.0,0.0);
	vec4 dz2 = vec4(0.0,0.0,1.0,0.0);
    vec4 dz3 = vec4(0.0,0.0,0.0,1.0);

  	for(int i=0;i<11;i++)
    {
        vec4 mz = vec4(z.x,-z.y,-z.z,-z.w);

		// derivative
		dz0=vec4(dot(mz,dz0),z.x*dz0.yzw+dz0.x*z.yzw);
		dz1=vec4(dot(mz,dz1),z.x*dz1.yzw+dz1.x*z.yzw);
		dz2=vec4(dot(mz,dz2),z.x*dz2.yzw+dz2.x*z.yzw);
        dz3=vec4(dot(mz,dz3),z.x*dz3.yzw+dz3.x*z.yzw);

        // z = z2 + c
		nz.x=dot(z, mz);
		nz.yzw=2.0*z.x*z.yzw;
        z=nz+c;

	    if(dot(z,z)>4.0)
            break;
    }

	return normalize(vec3(dot(z,dz0),
	                      dot(z,dz1),
	                      dot(z,dz2)));
}



void main(void)
{
    vec2 xy = -1.0 + 2.0*gl_FragCoord.xy / resolution.xy;

	vec2 s = xy*vec2(1.75,1.0);

    //float time = iGlobalTime*.15;
	float time = mouse.x;

	vec3 light1 = vec3(  0.577, 0.577,  0.577 );
	vec3 light2 = vec3( -0.707, 0.000, -0.707 );


	float r = 1.4+0.15*cos(0.0+0.29*time);
	vec3 campos = vec3(           r*cos(0.3+0.37*time), 
					    0.3 + 0.8*r*cos(1.0+0.33*time), 
					              r*cos(2.2+0.31*time) );
	vec3 camtar = vec3(0.0,0.0,0.0);

	float roll = 0.1*cos(0.1*time);
	vec3 cw = normalize(camtar-campos);
	vec3 cp = vec3(sin(roll), cos(roll),0.0);
	vec3 cu = normalize(cross(cw,cp));
	vec3 cv = normalize(cross(cu,cw));
	vec3 rd = normalize( s.x*cu + s.y*cv + 2.0*cw );


	vec4 cc = vec4( 0.4*cos(0.5+1.2*time)-.3, 
				    0.4*cos(3.9+1.7*time), 
				    0.4*cos(1.4+1.3*time),
                    0.4*cos(1.1+2.5*time));
	//if( length(cc)<0.50 ) cc=0.50*normalize(cc);
	//if( length(cc)>0.95 ) cc=0.95*normalize(cc);

	vec3 rgb;
	vec4 col;
    float t;
    if( !jinteresct(campos,rd,cc,t,col) )
    {
     	rgb = vec3(1.0)*(0.7+0.3*rd.y);

		rgb += vec3(0.8,0.7,0.5)*pow( clamp(dot(rd,light1),0.0,1.0), 48.0 );
	}
	else
	{
		vec3 xyz = campos + t*rd;

        vec3 nor = calcNormal( xyz, cc );

		float dif1 = clamp( dot( light1, nor ), 0.0, 1.0 );
		float dif2 = clamp( 0.5 + 0.5*dot( light2, nor ), 0.0, 1.0 );
		float ao = clamp(2.5*col.w-0.15,0.0,1.0);
		float lt1;
		vec4 lc;
		if( dif1>0.001 ) if( jinteresct(xyz+light1*0.01,light1,cc,lt1,lc) ) dif1 = 0.0;

		rgb = vec3(1.0,1.0,1.0)*0.3;

		rgb = mix( rgb, vec3(1.0,0.5,0.0), 2.0*col.x );
		rgb = mix( rgb, vec3(0.7,0.4,0.0), 4.0*col.y );
		rgb = mix( rgb, vec3(1.0,1.0,1.0), 3.0*col.z );

		vec3 brdf  = 1.5*vec3(0.17,0.19,0.20)*(0.6+0.4*nor.y)*(0.1+0.9*ao);
		     brdf += 2.0*vec3(1.00,0.95,0.80)*dif1*(0.5+0.5*ao);
		     brdf += 1.5*vec3(0.14,0.14,0.14)*dif2*ao;

		rgb *= brdf;
	}

	rgb = sqrt(rgb);

	vec2 uv = xy*0.5+0.5;
	rgb *= 0.7 + 0.3*pow(16.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y),0.25);
	gl_FragColor=vec4(rgb,1.0);
}
