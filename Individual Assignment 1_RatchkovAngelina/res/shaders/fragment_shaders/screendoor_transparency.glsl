#version 430
#pragma once
#include "../fragments/fs_common_inputs.glsl"

// We output a single color to the color buffer
layout(location = 0) out vec4 frag_color;

// Represents a collection of attributes that would define a material
// For instance, you can think of this like material settings in 
// Unity
struct Material {
	sampler2D Diffuse;
	float     Shininess;
    float     Threshold;
};
// Create a uniform for the material
uniform Material u_Material;

#include "../fragments/multiple_point_lights.glsl"
#include "../fragments/frame_uniforms.glsl"
#include "../fragments/light_correction.glsl"

uniform bool toggle_diffuse = true;
uniform bool toggle_ambient = true;
uniform bool toggle_specular = true;
uniform bool spec_ramp = false;
uniform bool diff_ramp = false;

// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
void main() {
	// Get the albedo from the diffuse / albedo map
	vec4 textureColor = texture(u_Material.Diffuse, inUV);

    if (textureColor.a < u_Material.Threshold) {
        discard;
    }

	// Normalize our input normal
	vec3 normal = normalize(inNormal);

	// Use the lighting calculation that we included from our partial file
	vec3 lightAccumulation = CalcAllLightContribution(inWorldPos, normal, u_CamPos.xyz, u_Material.Shininess,toggle_diffuse, toggle_specular, toggle_ambient, spec_ramp, diff_ramp);
	lightAccumulation = LightCorrect(lightAccumulation);


	// combine for the final result
	vec3 result = lightAccumulation  * inColor * textureColor.rgb;

	frag_color = vec4(result, textureColor.a);
}