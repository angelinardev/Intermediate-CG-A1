#version 430
#pragma once
#include "../fragments/fs_common_inputs.glsl"

// We output a single color to the color buffer
layout(location = 0) out vec4 frag_color;

////////////////////////////////////////////////////////////////
/////////////// Instance Level Uniforms ////////////////////////
////////////////////////////////////////////////////////////////

// Represents a collection of attributes that would define a material
// For instance, you can think of this like material settings in 
// Unity
struct Material {
	sampler2D Diffuse;
	float     Shininess;
};
// Create a uniform for the material
uniform Material u_Material;

////////////////////////////////////////////////////////////////
///////////// Application Level Uniforms ///////////////////////
////////////////////////////////////////////////////////////////
#include "../fragments/frame_uniforms.glsl"

#include "../fragments/multiple_point_lights.glsl"
#include "../fragments/light_correction.glsl"

////////////////////////////////////////////////////////////////
/////////////// Frame Level Uniforms ///////////////////////////
////////////////////////////////////////////////////////////////

uniform float     iTime;

uniform bool toggle_diffuse = true;
uniform bool toggle_ambient = true;
uniform bool toggle_specular = true;
uniform bool spec_ramp = false;
uniform bool diff_ramp = false;

uniform bool activated = true;

//https://www.shadertoy.com/view/MtXfDj
// GooFunc - now with technical parameters for you to play with :)
float GooFunc(vec2 uv,float zoom,float distortion, float gooeyness,float wibble)
{
    float s = sin(iTime*0.1);
    float s2 = 0.5+sin(iTime*1.8);
    vec2 d = uv*(distortion+s*.3);
    d.x += iTime*0.25+sin(d.x+d.y + iTime*0.3)*wibble;
    d.y += iTime*0.25+sin(d.x + iTime*0.3)*wibble;
    float v1=length(0.5-fract(d.xy))+gooeyness;
    d = (1.0-zoom)*0.5+(uv*zoom);					// try removing this :)
    float v2=length(0.5-fract(d.xy));
    v1 *= 1.0-v2*v1;
    v1 = v1*v1*v1;
    v1 *= 1.9+s2*0.2;
    return v1;
}
// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
void main() {
	// Normalize our input normal
	vec3 normal = normalize(inNormal);

	// Use the lighting calculation that we included from our partial file
	vec3 lightAccumulation = CalcAllLightContribution(inWorldPos, normal, u_CamPos.xyz, u_Material.Shininess,toggle_diffuse, toggle_specular, toggle_ambient, spec_ramp, diff_ramp);
    lightAccumulation = LightCorrect(lightAccumulation);

    if (activated)
	{
        float distortion = 4.0;						// increase or decrease to suit your taste.
        float zoom = 0.9;							// zoom value
        float gooeyness = 0.95;						// smaller = more gooey bits
        float wibble = 0.6;							// tweak the wibble!
        float goo = GooFunc(inUV, zoom, distortion, gooeyness,wibble);
	    const vec4 col1 = vec4(0.0,.1,.1,1.0); //inside color
        const vec4 col2 = vec4(0.5,0.9,0.3,1.0); //outside color
        float saturation = 2.4;
        frag_color = mix(col2,col1,goo)*saturation;
    }

	// Get the albedo from the diffuse / albedo map
	vec4 textureColor = texture(u_Material.Diffuse, inUV); //background

    if (activated)
    {
        float avg = max(max(frag_color.r,frag_color.g),frag_color.b);		//float avg = k.g;	//(k.r+k.g+k.b)/3.0;
        float alpha=1.0;
        if (avg<=0.4)
        {
        // darken & alpha edge of goo...
            avg = clamp(avg,0.0,1.0);
            frag_color*=avg+0.2;						// 0.0 = black edges
            alpha = clamp((avg*avg)*5.5,0.0,1.0);
        }
    }
	// combine for the final result
	vec3 result = lightAccumulation  * inColor * textureColor.rgb;
    frag_color = mix(textureColor, frag_color, textureColor.a);
	//frag_color = vec4(result, textureColor.a);
}