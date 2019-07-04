#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"
#include "fractal.glsl"
#include "SDF_funcs.glsl"

#define PI acos(-1.)


uniform float cSkyLum;
uniform float cAmbLight;
uniform vec3 cAddSkyColor;
uniform float cMyst;
uniform vec3 cMediumColor;
uniform float cDebugPlane;


vec4 DecodeBGRM8(vec4 rgbm)
{
    vec4 r;
    const float MaxRange = 1000.0f;
    const float MaxValue = 255.0f * MaxRange;
    const float scale = 1.0f / log2(MaxValue);
    float M = exp2(rgbm.a / scale);
    float m = M / (255.0f * 255.0f);
    r.rgb = rgbm.rgb * m;
    r.a = M;
    return r;
}

vec3 distanceMeter(float dist, float rayLength, vec3 wP) {
    float idealGridDistance = 100./rayLength;
    float nearestBase = floor(log(idealGridDistance)/log(10.));
    //float relativeDist = abs(dist/camHeight);

    float largerDistance = pow(10.0,nearestBase+1.);
    float smallerDistance = pow(10.0,nearestBase);


    vec3 col = vec3(1.0);
    //col = max(vec3(0.),col);
    //if (sign(dist) < 0.) {
    //    col = col.grb*3.;
    //}

    float l0 = (pow(0.5+0.5*sin(dist*PI*2.*smallerDistance),10.0));
    float l1 = (pow(0.5+0.5*sin(dist*PI*2.*largerDistance),10.0));

    float x = fract(log(idealGridDistance)/log(10.));
    l0 = mix(l0,0.,smoothstep(0.5,1.0,x));
    l1 = mix(0.,l1,smoothstep(0.0,0.5,x));

    //float grid = (pow(0.5+0.5*cos(((0.5-fract(wP.x*0.1))*(0.5-fract(wP.z*0.1)))*PI*2.),1000.0));

    col.rgb *= 0.1+0.9*(1.-l0)*(1.-l1);
    //col.gb *= 1.-grid * 0.6;
    return col;
}

varying vec2 vScreenPos;
varying vec3 vFarRay;
//#ifdef SDFFEBUG
  varying vec3 fwd;
//#endif

void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    //#ifdef SDFFEBUG
      fwd = vec3(0.0,0.0,1.0) *  GetCameraRot();
    //#endif

    vScreenPos = GetScreenPosPreDiv(gl_Position);
    vFarRay = GetFarRay(gl_Position);
}

void PS()
{
    float depth = DecodeDepth(texture2D(sDepthBuffer, vScreenPos).rgb);
    vec3 localpos = vFarRay * depth;
    vec3 worldPos = cCameraPosPS + localpos;
    float depth2 = length(localpos);

    vec3 dir = normalize(vFarRay);

    vec4 normalInput = texture2D(sNormalBuffer, vScreenPos);
    vec3 normal = (normalInput.rgb * 2.0 - 1.0);
    float ao;
    vec3 bent_normal = (calcNormal(worldPos+normal*0.7, 5.0));

    float ao_free = pow(length(bent_normal)*0.3,4.);
    float ao_size = 0.8;
    bent_normal = normalize(bent_normal);
    float tap_result = max(sdfmap(worldPos + bent_normal*ao_size).x,0.);
    float ao2 = (tap_result+0.08)/ao_size;
    ao2 = max(ao2,0.03);
    ao2 = pow(ao2*2.,3.);
    //ao2 = clamp(ao2,0.,1.0);
    ao = ao2 * ao_free*0.2;
    float final_ao = 1. - (1.0 + ao / 3.) / (1.0 + ao) + tap_result*0.1;

    final_ao = clamp(final_ao,0.,1.0);
    //float ao2 = sdfmap(worldPos+bent_normal*5)
    vec3 col = vec3(0.);

    #ifdef FOG
      float depthclamp = depth2/cFarClipPS;
      //8.*myst + max(pow((1.0-depthclamp)*20.,0.8),0. )
      float fogMip = 8.*cMyst + max(pow((1.0-depthclamp)*25.*(1.-cMyst),0.8),0. );
      vec3 skycol = DecodeBGRM8(textureCube(sEnvCubeMap, dir,fogMip)).rgb;
      vec3 reflcol = DecodeBGRM8(textureLod(sEnvCubeMap,normal,4.+final_ao*10.)).rgb;
      vec4 albedo = texture2D(sAlbedoBuffer,vScreenPos);
      float ndot = max(dot(normal,bent_normal)*0.2+0.8,0.);
      float fog = clamp(pow(depthclamp,max(0.11-cMyst*0.05,0.01)),0.,1.);
      skycol += cAddSkyColor*0.02;
      reflcol += cAddSkyColor*0.01;
      reflcol *= albedo.rgb;
      skycol *=  min(pow(depthclamp,max(2.0-cMyst,0.1)),1.);

      col = cAmbLight * reflcol * ndot*(final_ao)*(1.-fog)+skycol * cSkyLum;
      col *= cMediumColor;
    #endif
    //if (sdfmap(worldPos + bent_normal).w<0.0) col = vec3(1.,0.1,0.02);
    //if (final_ao > 0.8) col = vec3(1.,0.,0.); else col = vec3(final_ao);
    #ifdef SDFFEBUG
      float plane = (cDebugPlane-cCameraPosPS.y)/dir.y;
      if (plane>0 && plane<depth2)
      {
        localpos = plane * normalize(localpos);
        depth2 = plane;
        worldPos = cCameraPosPS + localpos;

        float dist = sdfmap(worldPos).x;
        vec3 gradCol = normalize(vec3(1.8,.5+.3*sin(-1.*dist),.5+.5*cos(dist*2.)));
        col = mix(gradCol + 2.0 * col,col, clamp(dist * 0.1,0.,1.));
        
        col *= distanceMeter(dist,depth2,worldPos);

      }
      vec3 cprj = cCameraPosPS;
      vec3 fwdprj = fwd.xyz;
      cprj.y = cDebugPlane;
      fwdprj.y = 0;
      fwdprj = normalize(fwdprj);
      float check = 0.;
      for (int i = 0; i<12; i++)
      {
        check = sdfmap(cprj).x;
        float rad = length(worldPos-cprj);
        if (rad<check && check<(rad+1.)) col = normalize(vec3(1.+sin(i*-10.),1.+cos(i*-10.),1.7));
        cprj += fwdprj * check;// * fwdprj;//vec3(0.,0.,1.) *
      }
    #endif
    
    //col = vec3(final_ao);
    //if (final_ao < 0.00392) col = vec3(0.,0.,1.);
    //if (final_ao > 0.999) col = vec3(1.,0.3,0.3);
   

    //gl_FragData[0] = vec4(step(0.1,ao),step(0.5,ao),step(0.9,ao),1.0);
    //gl_FragData[0] = vec4(vec3(final_ao),0.);
    //if (vScreenPos.y>0.9)  gl_FragData[0] = vec4(vec3(1.0),0.);
    gl_FragData[0] = vec4(col,1.);
    //gl_FragData[0] = vec4(bent_normal,1.0);
    gl_FragData[1] = vec4(0.5 + bent_normal*0.5, final_ao );

}
