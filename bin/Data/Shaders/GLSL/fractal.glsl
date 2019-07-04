#define pi 3.14159

uniform float cIterations;
uniform vec3 cFcParams;
uniform mat3 cFcRot;
/*mat3 rot = mat3(
  0.754,0.4893,0.4381,
  0.5279,-0.0548,-0.8475,
  -0.3908,0.8703,-0.2997
  );//*/
/*mat3 rot = mat3(
  0.9997,-0.0116,-0.1215,
  0.0287,0.992,-0.1256,
  0.0226,0.1253,0.9918
  );//*/



float hash(float h) {
	return fract(sin(h) * 43758.5453123);
}

vec3 pointRepetition(vec3 point, vec3 c)
{
	point.x = mod(point.x, c.x) - 0.5*c.x;
	point.z = mod(point.z, c.z) - 0.5*c.z;
	return point;
}

float noise3d(vec3 x) {
	vec3 p = floor(x);
	vec3 f = fract(x);
	f = f * f * (3.0 - 2.0 * f);

	float n = p.x + p.y * 157.0 + 113.0 * p.z;
	return mix(
			mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),
					mix(hash(n + 157.0), hash(n + 158.0), f.x), f.y),
			mix(mix(hash(n + 113.0), hash(n + 114.0), f.x),
					mix(hash(n + 270.0), hash(n + 271.0), f.x), f.y), f.z);
}

float apo(vec3 pos, float seed,vec3 CSize, vec3 C)
{
  float dist;
  //vec3 CSize = vec3(1., 1., 1.3);
  vec3 p = pos.xzy;
  float scale = 1.0;
  p *= cFcRot;
  float r2 = 0.;
  float k = 0.;
  float uggg = 0.;
  for( int i=0; i < 12;i++ )
  {
      p = 2.0*clamp(p, -CSize, CSize) - p;
      r2 = dot(p,p);
      //r2 = dot(p,p+sin(p.z*.3)); //Alternate fractal
      k = max((2.0)/(r2), seed); //.378888 //.13345 max((2.6)/(r2), .03211); //max((1.8)/(r2), .0018);
      p     *= k;
      scale *= k;
      uggg += r2;
      p+=C;
       //p.xyz = vec3(-1.0*p.z,1.0*p.x,1.0*p.y);
  }
  float l = length(p.xy);
  float rxy = l - 4.0;
  float n = 1.0 * p.z;
  rxy = max(rxy, -(n) / 4.);
  dist = (rxy) / abs(scale);
  return dist;
}

vec2 sdfmap(vec3 pos)
{
  float dist = 10000.;
  float tex = 0;
  #ifdef FCTYP_1
     vec3 p = pos * 0.002;
    vec4 q = vec4(p - 1.0, 1);
    for(int i = 0; i < 7; i++) {
      q.xyz = abs(q.xyz + 1.3) - 1.0;
      q /= clamp(dot(q.xyz, q.xyz), 0.0, 0.8);
      q.xyz *= cFcRot;
      q *= 1.567+cFcParams.x;// + p.y*0.8;
      q = q.zxyw;
      q+= vec4(0.2119,0.8132,0.077213,0.);
    }
    for(int i = 0; i < 4; i++) {
      q.xyz = abs(q.xyz + 1.3) - 1.0;
      q /= clamp(dot(q.xyz, q.xyz), 0.0, 0.8);
      q *= 1.867;// + p.y*0.8;
    }
    dist = (length(q.xyz) - max(-240-p.y*700.,2.5))/q.w * 300.;

  #elif defined FCTYP_11
    vec3 p = pos * 0.01;
    vec4 q = vec4(p, 1);
    for(int i = 0; i < 12; i++) {
      q.xyz = abs(q.xyz + 1.0+cFcParams.y) - 1.0;
      q /= clamp(dot(q.xyz, q.xyz), 0.0, 0.8);
      q.xyz *= cFcRot;
      q *= 1.567+cFcParams.x;// + p.y*0.8;
    }
    dist = (length(q.xyz) -3.5 )/q.w * 100.;
    tex = q.y;
  #elif defined FCTYP_2
      dist = apo(pos, 0.01, vec3(1.4,0.87, 1.1), vec3(0.02,-0.33332,-0.09092));
  #elif defined FCTYP_3

    vec3 p = pos * 0.001;
    vec4 q = vec4(p - 1.0, 1);
    for(int i = 0; i < 11; i++) {
      //q.xyz = mod(q.xyz,2.0)-0.5*q.xyz;
       q.xyz = abs(q.xyz + 1.0) - 1.0;
      q.xyz = 2.0*clamp(q.xyz, -73.0174, 73.0174) - q.xyz;
      q /= min(dot(q.xyz, q.xyz), 0.9);
      q *= 1.817;// + p.y*0.8;
      //q += vec4(0.2,.02,-.2,0.2);
      //q.xyz *= rot;
    }
    dist = (length(q.xz) - 2.1)/q.w * 700.;
  #elif defined FCTYP_4
     vec3 CSize = vec3(1.,1., 1.3);
    vec3 p = pos.yzx;
    float scale = 1.0;
    //float r2;
    for( int i=0; i < 8;i++ )
    {
        p = 2.0*clamp(p, -CSize, CSize) - p;
        float r2 = dot(p,p);
        //float r2 = dot(p,p+sin(p.z*.5)); //Alternate fractal
        float k = max((2.)/(r2), .1274);
        p     *= k;
        p *=cFcRot;
        //p= p.yzx;
        p.xyz = vec3(1.0*p.z,1.0*p.x,-1.0*p.y);
        scale *= k;
    }
    tex = p.y;
   CSize = vec3(1.2,0.4, 1.4);
    for( int i=0; i < 4;i++ )
    {
        p = 2.0*clamp(p, -CSize, CSize) - p;
        float r2 = dot(p,p);
        //float r2 = dot(p,p+sin(p.z*.3)); //Alternate fractal
        float k = max((1.6)/(r2), 0.0274);
        p     *= k;
        scale *= k;
    }
    float l = length(p.xyz);
    float rxy = l - 1.4;
    float n = 1.0 * p.z;
    rxy = max(rxy, -(n) / 4.);
    dist = (rxy) / abs(scale);
    dist *=1.5;
    //tex = p.z;

  #elif defined FCTYP_5

 vec3 p = pos * 0.002;
 p*=cFcRot;


    vec4 q = vec4(p - 1.0, 1);


    for(int i = 0; i < 7; i++) {


      q.xyz = abs(q.xyz + 1.3) - 1.0;


      q /= clamp(dot(q.xyz, q.xyz), 0.0, 0.8);


      q *= 1.867;// + p.y*0.8;


      q = q.zxyw;


      q+= vec4(0.2119,0.8132,0.077213,0.);


    }



    for(int i = 0; i < 4; i++) {


      q.xyz = abs(q.xyz + 1.3) - 1.0;


      q /= clamp(dot(q.xyz, q.xyz), 0.0, 0.8);


      q *= 1.867;// + p.y*0.8;



    }


    dist = (length(q.yz) - 1.2)/q.w * 300.;
  #elif defined FCTYP_6
     vec3 CSize = vec3(1.4,0.87, 1.1);
    vec3 p = pos.xzy;
    float scale = 1.0;
    //float r2;
    for( int i=0; i < 4;i++ )
    {
        p = 2.0*clamp(p, -CSize, CSize) - p;
        float r2 = dot(p,p);
        //float r2 = dot(p,p+sin(p.z*.5)); //Alternate fractal
        float k = max((2.)/(r2), .17);
        p     *= k;
        //p *=rot;
        //p= p.yzx;
        p+=vec3(0.2,0.2,-0.5);
        scale *= k;
    }
    tex = p.x;
    for( int i=0; i < 8;i++ )
    {
        p = 2.0*clamp(p, -CSize, CSize) - p;
        float r2 = dot(p,p);
        //float r2 = dot(p,p+sin(p.z*.3)); //Alternate fractal
        float k = max((2.)/(r2), .027);
        p     *= k;
        scale *= k;
    }
    float l = length(p.xy);
    float rxy = l - 4.0;
    float n = 1.0 * p.z;
    rxy = max(rxy, -(n) / 4.);
    dist = (rxy) / abs(scale);
    dist *=1.5;
    //tex = p.z;

  #elif defined FCTYP_7
    vec3 p = pos.yzx * 0.05;
    vec4 q = vec4(p - 1.0, 1);
    for(int i = 0; i < 8; i++) {
      q.xyz = abs(q.xyz + cFcParams.z) - 1.0;
      q /= clamp(dot(q.xyz, q.xyz), 0.12, 1.0);
      q *= 1.0+cFcParams.x;// + p.y*0.8;
      q.xyz*=cFcRot;
    }
    tex = q.x;
    for(int i = 0; i < 2; i++) {
      q.xyz = abs(q.xyz + cFcParams.z) - 1.0;
      q /= clamp(dot(q.xyz, q.xyz), 0.12, 1.0);
      q *= 1.0+cFcParams.x;// + p.y*0.8;
      q.xyz*=cFcRot;
    }
    dist = (length(q.xyz) - 1.+cFcParams.y)/q.w * 19.;
  #else
    dist = apo(pos, .0274, vec3(1., 1., 1.3), vec3(0.));

  #endif

  return vec2(dist,tex);
}
