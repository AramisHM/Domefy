#include "Uniforms.glsl"
#include "Samplers.glsl"
//#define DEFERRED;
#include "Transform.glsl"
#include "ScreenPos.glsl"
#include "fractal.glsl"
#include "SDF_funcs.glsl"
//out vec4 fragData[4];
//#define gl_FragData fragData

#if defined STEP32
const int raysteps = 32;
#elif defined STEP96
const int raysteps = 96;
#elif defined STEP192
const int raysteps = 192;
#else
const int raysteps = 16;
#endif

uniform float cRAY_STEPS;
uniform vec4 cTexParam;

varying vec2 vScreenPos;
//varying vec3 direction;
varying mat4 cViewProjPS;
varying float fov;
varying vec3 FrustumSizePS;
varying mat4 ViewPS;


void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    vec2 pos = GetScreenPosPreDiv(gl_Position);
    pos = pos;

    vScreenPos = pos;
    //cViewProjPS = cViewProj;
    vec3 pos3 = vec3(pos,1.0) * cFrustumSize;
    fov = atan(cFrustumSize.y/cFrustumSize.z);
    FrustumSizePS = cFrustumSize;
    ViewPS = cView;
    //direction = normalize(mat3(cView) * pos3);

}


void PS()
{
  vec3 locDir = normalize(vec3(vScreenPos * 2.0 -1.0,1.0) * FrustumSizePS);
  vec3 direction = (mat3(ViewPS) * locDir  );
  vec3 origin = cCameraPosPS;
  vec3 intersection;
  //vec3 direction = camMat * normalize( vec3(uv.xy,2.0) );
  float PREdepth =  texture2D(sSpecMap, vScreenPos).r;

  vec2 distance = vec2(0.);
  float totalDistance = PREdepth;// * cFarClipPS;
  float lfog = 0.;
  float pxsz = fov * cGBufferInvSize.y;

  float distTrsh = 0.002;
  int stps = 0;


  for(int i =0 ;  i < cRAY_STEPS; ++i) ////// Rendering main scene
   {

        intersection = origin + direction * totalDistance;

        distance = sdfmap(intersection);
        totalDistance += distance.x;
        //#ifdef PREMARCH
        distTrsh = pxsz * totalDistance * 1.4142;
        #ifdef PREMARCH
        totalDistance -= distTrsh * 0.7;
        #else
          distTrsh *= 0.4;
        #endif

      if(distance.x <= distTrsh || totalDistance >= cFarClipPS) break;
      //  #else
      //    if(distance.w <= 0.002 || totalDistance >= cFarClipPS) break;
      // #endif



      // stps = i;
   }

   #ifndef PREMARCH

    float fdepth = (totalDistance*locDir.z)/cFarClipPS;
    vec3 diffColor = vec3(0.5);

    float depth = DecodeDepth(texture2D(sDepthBuffer, vScreenPos).rgb);

    if (fdepth>depth) discard;
    vec3 normal = normalize(calcNormal(intersection, max(pow(totalDistance,1.25) * pxsz,0.01)));

    vec3 txt = texture2D(sEmissiveMap,vec2(intersection.x * cTexParam.x,intersection.z * cTexParam.x) ).rgb;
    vec4 col = texture2D(sDiffMap, vec2(txt.g,distance.y*cTexParam.w+intersection.y * cTexParam.y + txt.r * cTexParam.z));

   //OUTPUT
    gl_FragData[0] = col;//vec4(diffColor.rgb, 1.0 );
    gl_FragData[1] = vec4(0.5 + normal*0.5, 1.0);// * 0.5 + 0.5
    gl_FragData[2] = vec4(EncodeDepth(fdepth), 0.0);//
   
  #else
    gl_FragColor =  vec4(min(totalDistance, cFarClipPS),0. , 0. , 0.);
  #endif
}
