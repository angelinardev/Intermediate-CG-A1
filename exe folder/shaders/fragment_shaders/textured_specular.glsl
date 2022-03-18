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
	sampler2D Specular;
	float Shininess;
};
// Create a uniform for the material
uniform Material u_Material;

////////////////////////////////////////////////////////////////
///////////// Application Level Uniforms ///////////////////////
////////////////////////////////////////////////////////////////

#include "../fragments/multiple_point_lights.glsl"

////////////////////////////////////////////////////////////////
/////////////// Frame Level Uniforms ///////////////////////////
////////////////////////////////////////////////////////////////

#include "../fragments/frame_uniforms.glsl"
#include "../fragments/color_correction.glsl"
#include "../fragments/light_correction.glsl"

uniform bool toggle_diffuse = true;
uniform bool toggle_ambient = true;
uniform bool toggle_specular = true;
uniform bool spec_ramp = false;
uniform bool diff_ramp = false;


uniform float spec_increase = 0;

// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
void main() {
	// Normalize our input normal
	vec3 normal = normalize(inNormal);

	float specPower = texture(u_Material.Specular, inUV).r + spec_increase;

	//specPower = specPower + spec_increase;
	
	vec3 toEye = normalize(u_CamPos.xyz - inWorldPos);
	vec3 environmentDir = reflect(-toEye, normal);
	vec3 reflected = SampleEnvironmentMap(environmentDir);

	// Use the lighting calculation that we included from our partial file
	vec3 lightAccumulation = CalcAllLightContribution(inWorldPos, normal, u_CamPos.xyz, specPower, toggle_diffuse, toggle_specular, toggle_ambient, spec_ramp, diff_ramp);
	lightAccumulation = LightCorrect(lightAccumulation);

	// Get the albedo from the diffuse / albedo map
	vec4 textureColor = texture(u_Material.Diffuse, inUV);
	// combine for the final result
	vec3 result = lightAccumulation  * inColor * textureColor.rgb;

	frag_color = vec4(ColorCorrect(mix(result, reflected, specPower)), textureColor.a);
}