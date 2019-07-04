#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"
#include "fractal.glsl"
#include "SDF_funcs.glsl"

//varying vec2 vScreenPos;
varying vec2 vTexCoord;

void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    //vScreenPos = GetScreenPosPreDiv(gl_Position);
    vTexCoord = GetQuadTexCoord(gl_Position);
}

void PS()
{
    vec4 pos = texture2D(sDiffMap, vTexCoord);
    float rmc; 
    vec3 normal;
    if (pos.x == 0.0)
    {
        rmc = 9e20;
    } else {
        rmc = sdfmap(pos.xyz).x;
        normal = calcNormal(pos.xyz, pos.w);
    }
    gl_FragColor = vec4(normal, rmc);
}