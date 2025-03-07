#version 330			

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;
												
in vec4 vCol;									
in vec2 TexCoord;		
in vec3 Normal;
in vec3 FragPos; // Fragment position
in vec4 DirectionalLightSpacePos;
												
out vec4 Colour;		

struct LightBaseData {
	vec3 Colour;
	float AmbientIntensity;
	float DiffuseIntensity;
};

struct DirectionalLightData {
	LightBaseData Base;
	vec3 Direction;
};

struct PointLightData {
	LightBaseData Base;
	vec3 Position;
	float Constant;
	float Linear;
	float Exponent;
};

struct SpotLightData{
	PointLightData Base;
	vec3 Direction;
	float Edge;
};

struct OmniShadowMap
{
	samplerCube ShadowMap;
	float FarPlane;
};

struct MaterialData {
	float SpecularIntensity;
	float Shininess;
};

uniform int PointLightCount;
uniform int SpotLightCount;

uniform DirectionalLightData DirectionalLight;
uniform PointLightData PointLights[MAX_POINT_LIGHTS];
uniform SpotLightData SpotLights[MAX_SPOT_LIGHTS];

uniform sampler2D TheTexture;
uniform sampler2D directionalShadowMap;
uniform MaterialData Material;
uniform OmniShadowMap OmniShadowMaps[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS]; // Init Point Lights First, then spot lights

uniform vec3 EyePosition; // Camera position

float CalcDirectionalShadowFactor(DirectionalLightData light)
{
	vec3 projCoords = DirectionalLightSpacePos.xyz / DirectionalLightSpacePos.w; // now coordnates would be between 1 and -1
	projCoords = (projCoords * 0.5) + 0.5; // normalize into 0-1
	
	//float closestDepth = texture(directionalShadowMap, projCoords.xy).r; // left, right, up and down
	float currentDepth = projCoords.z; // how far it is

	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(light.Direction);

	float bias = max(0.05f * (1 - dot(normal, lightDir)), 0.005);

	float shadow = 0.0;

	vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);
	for(int x = -1; x <= 1; x++)
	{
		for(int y = -1; y <= 1; y++)
		{
			// pcfDepth =  closest value
			float pcfDepth = texture(directionalShadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
			shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	//float shadow = (currentDepth - bias) > closestDepth ? 1.0 : 0.0;
	
	if(projCoords.z > 1.0)
	{
		shadow = 0.0;
	}

	return shadow;
}

float CalcOmniShadowFactor64PerPixel(PointLightData pLight, int shadowIndex)
{
	vec3 fragToLight = FragPos - pLight.Position;
	float currentDepth = length(fragToLight);

	float shadow = 0.0;
	float bias = 0.05;
	float samples = 4.0;
	float offset = 0.1;
	
	for(float x = -offset; x < offset; x += offset / (samples * 0.5))
	{
		for(float y = -offset; y < offset; y += offset / (samples * 0.5))
		{
			for(float z = -offset; z < offset; z += offset / (samples * 0.5))
			{
				// .r - get first value
				float closestDepth = texture(OmniShadowMaps[shadowIndex].ShadowMap, fragToLight + vec3(x,y,z)).r; 
				// undo divide by FarPlane from OmniShadowFrag.glsl
				closestDepth *= OmniShadowMaps[shadowIndex].FarPlane; 
				if((currentDepth - bias) > closestDepth)
				{
					shadow += 1.0f;
				}
			}
		}
	}
	shadow /= (samples * samples * samples);

	//shadow = (currentDepth - bias) > closestDepth ? 1.0f : 0.0f;
	return shadow;
}
			
vec4 CalcLightByDirection(LightBaseData BaseLight, vec3 Direction, float shadowFactor)
{
	vec4 AmbientColour = vec4(BaseLight.Colour, 1.0f) * BaseLight.AmbientIntensity;

	float DiffuseFactor = max(dot(normalize(Normal), normalize(Direction)), 0.0f); // if DiffFactor < 0, we set it to zero
	vec4 DiffuseColour = vec4(BaseLight.Colour * BaseLight.DiffuseIntensity * DiffuseFactor, 1.0f);
	vec4 SpecularColor = vec4(0,0,0,0);

	if(DiffuseFactor > 0.0f) {
		vec3 FragToEye = normalize(EyePosition - FragPos);
		vec3 ReflectedVertex = normalize(reflect(Direction,  normalize(Normal)));

		float SpecularFactor = dot(FragToEye, ReflectedVertex);
		if(SpecularFactor > 0.0f) {
			SpecularFactor = pow(SpecularFactor, Material.Shininess);
			SpecularColor = vec4(BaseLight.Colour * Material.SpecularIntensity * SpecularFactor, 1.0f);
		}
	}
	return (AmbientColour + (1.0 - shadowFactor) * (DiffuseColour + SpecularColor));
}

vec4 CalcDirectionalLight()
{
	float shadowFactor = CalcDirectionalShadowFactor(DirectionalLight);
	return CalcLightByDirection(DirectionalLight.Base, DirectionalLight.Direction, shadowFactor);
}
 
vec4 CaclSinglePointLight(PointLightData PointLight, int shadowIndex) 
{
	vec3 direction = FragPos - PointLight.Position;
	float dirDistance = length(direction);
	direction = normalize(direction);

	float shadowFactor = CalcOmniShadowFactor64PerPixel(PointLight, shadowIndex);
	 
	vec4 Colour = CalcLightByDirection(PointLight.Base, direction, shadowFactor);
	float Attenuation = PointLight.Exponent * dirDistance * dirDistance +
						PointLight.Linear * dirDistance +
						PointLight.Constant; // ax^2 + bx + c
	return (Colour/Attenuation);
}

vec4 CalcSingleSpotLight(SpotLightData SpotLight, int shadowIndex)
{
	vec3 RayDirection = normalize(FragPos - SpotLight.Base.Position);
	float SpotLightFactor = dot(RayDirection, SpotLight.Direction);

	if(SpotLightFactor > SpotLight.Edge)
	{
		vec4 Colour = CaclSinglePointLight(SpotLight.Base, shadowIndex);
		return Colour * (1.0f - (1.0f - SpotLightFactor)  * (1.0f / (1.0f - SpotLight.Edge)));
	} 
	else 
	{
		return vec4(0,0,0,0);
	}
}

vec4 CalcPointLights()
{
	vec4 TotalColour = vec4(0,0,0,0);
	for(int i = 0; i < PointLightCount; i++)
	{
		TotalColour += CaclSinglePointLight(PointLights[i], i);
	}
	return TotalColour;
}

vec4 CalcSpotLights()
{
	vec4 TotalColour = vec4(0,0,0,0);
	for(int i = 0; i < SpotLightCount; i++)
	{
		TotalColour += CalcSingleSpotLight(SpotLights[i], i + PointLightCount);
	}
	return TotalColour;
}

void main()										
{						
	vec4 FinalColour = CalcDirectionalLight();
	FinalColour += CalcPointLights();
	FinalColour += CalcSpotLights();

	Colour = texture(TheTexture, TexCoord) * FinalColour;								
}