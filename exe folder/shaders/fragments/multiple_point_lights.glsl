/*
 * This is a partial file that allows shaders to add fragment shading 
 * to their final output. Defines a common Light structure, uniform buffer
 * and light parameters that can be shared between all lighting enabled
 * shaders
 * 
 * Usage:
 * vec3 normal = normalize(inNormal);
 * vec3 lighting = CalculateAllLightContribution(inWorldPos, normal, u_CamPos);
*/
// The maximum number of lights the shader supports, increasing this will lower performance!
#define MAX_LIGHTS 8

// Represents a single light source
struct Light {
	vec4  Position;
	// Stores color in RBG and attenuation in w
	vec4  ColorAttenuation;
};

// Our uniform buffer that will store all our lighting data
// so that it can be shared between shaders
layout (std140, binding = 2) uniform b_LightBlock {
    // Stores ambient light color in rgb, and number
	// of lights in w, allowing for easier struct packing
	// on the C++ side
    vec4  AmbientColAndNumLights;

    // Our array of all lights
    Light Lights[MAX_LIGHTS];

    // The rotation of the skybox/environment map
	mat3  EnvironmentRotation;
};

// Uniform for our environment map / skybox, bound to slot 0 by default
uniform layout(binding=15) samplerCube s_EnvironmentMap;

// Samples the environment map at a given direction. Will apply environment
// rotation to the input
// @param normal The direction to sample
// @returns The RGB color that was sampled from the environment map
vec3 SampleEnvironmentMap(vec3 normal) {
	vec3 transformed = EnvironmentRotation * normal;
	return texture(s_EnvironmentMap, transformed).rgb;
}

float diffuseFunction(float inVal)
{
	//function representing desired behaviour
	return ((sin(5*inVal))/2 + 0.5);
}
float specularFunction(float inVal)
{
	//function representing desired behaviour
	return ((cos(6*inVal))/2 + 0.5);
}


// Calculates the contribution the given point light has 
// for the current fragment
// @param worldPos  The fragment's position in world space
// @param normal    The fragment's normal (normalized)
// @param viewDir   Direction between camera and fragment
// @param Light     The light to caluclate the contribution for
// @param shininess The specular power for the fragment, between 0 and 1
vec3 CalcPointLightContribution(vec3 worldPos, vec3 normal, vec3 viewDir, Light light, float shininess, bool d, bool s, bool a, bool specRamp, bool diffRamp) {
	// Get the direction to the light in world space
	vec3 toLight = light.Position.xyz - worldPos;
	// Get distance between fragment and light
	float dist = length(toLight);
	// Normalize toLight for other calculations
	toLight = normalize(toLight);

	// Halfway vector between light normal and direction to camera
	vec3 halfDir     = normalize(toLight + viewDir);

	// Calculate our specular power
	float specPower;
	if (s)
	{
		
		if (specRamp)
		{
			float newspec;
			newspec = specularFunction(dot(normal, halfDir));
			specPower  = pow(max(newspec, 0.0), pow(256, shininess));
		}
		else
		{
			specPower  = pow(max(dot(normal, halfDir), 0.0), pow(256, shininess));
		}
	}
	else
	{
		specPower = 0.0;
	}
	// Calculate specular color
	vec3 specularOut = specPower * light.ColorAttenuation.rgb;

	float diffuseFactor;
	if (d)
	// Calculate diffuse factor
	{
		if (diffRamp)
		{
			float newdiff;
			newdiff = diffuseFunction(dot(normal, toLight));
			diffuseFactor = max(newdiff, 0);
		}
		else
		{
			diffuseFactor = max(dot(normal, toLight), 0);
		}
		
	}
	else
	{
		diffuseFactor = 0.0;
	}

	
	// Calculate diffuse color
	vec3  diffuseOut = diffuseFactor * light.ColorAttenuation.rgb;
	

	float attenuation;
	
	// We'll use a modified distance squared attenuation factor to keep it simple
	// We add the one to prevent divide by zero errors
	attenuation = clamp(1.0 / (1.0 + light.ColorAttenuation.w * pow(dist, 2)), 0, 1);
	

	return (diffuseOut + specularOut) * attenuation;
}

/*
 * Calculates the lighting contribution for all lights in the scene
 * for a given fragment
 * @param worldPos The fragment's position in world space
 * @param normal The normalized surface normal for the fragment
 * @param camPos The camera's position in world space
*/
vec3 CalcAllLightContribution(vec3 worldPos, vec3 normal, vec3 camPos, float shininess, bool d, bool s, bool a, bool sr, bool dr) {
    
	vec3 lightAccumulation;
	if (a)
	// Will accumulate the contributions of all lights on this fragment
		{
			lightAccumulation = AmbientColAndNumLights.rgb;
		}
		else
		{
			lightAccumulation = vec3(0.0, 0.0, 0.0);
		}

		// Direction between camera and fragment will be shared for all lights
		vec3 viewDir  = normalize(camPos - worldPos);
	
		// Iterate over all lights
		for(int ix = 0; ix < AmbientColAndNumLights.w && ix < MAX_LIGHTS; ix++) {
			// Additive lighting model
			lightAccumulation += CalcPointLightContribution(worldPos, normal, viewDir, Lights[ix], shininess, d, s, a, sr, dr);
		}

	return lightAccumulation;
}
