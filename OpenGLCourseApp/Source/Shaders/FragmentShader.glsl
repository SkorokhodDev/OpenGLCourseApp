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

uniform vec3 EyePosition; // Camera position

float CalcDirectionalShadowFactor(DirectionalLightData light)
{
	vec3 projCoords = DirectionalLightSpacePos.xyz / DirectionalLightSpacePos.w; // now coordnates would be between 1 and -1
	projCoords = (projCoords * 0.5) + 0.5; // normalize into 0-1
	
	float closestDepth = texture(directionalShadowMap, projCoords.xy).r; // left, right, up and down
	float currentDepth = projCoords.z; // how far it is
	
	float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
	return shadow;
}
			
vec4 CalcLightByDirection(LightBaseData BaseLight, vec3 Direction, float shadowFactor)
{
	vec4 AmbientColour = vec4(BaseLight.Colour, 1.0f) * BaseLight.AmbientIntensity;

	float DiffuseFactor = max(dot(normalize(Normal), normalize(Direction)), 0.0f); // if DiffFactor < 0, we set it to zero
	vec4 DiffuseColour = vec4(BaseLight.Colour, 1.0f) * BaseLight.DiffuseIntensity * DiffuseFactor;
	
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

vec4 CaclSinglePointLight(PointLightData PointLight) 
{
	vec3 direction = FragPos - PointLight.Position;
	float dirDistance = length(direction);

	direction = normalize(direction);
	vec4 Colour = CalcLightByDirection(PointLight.Base, direction, 0.0f);
	float Attenuation = PointLight.Exponent * dirDistance * dirDistance +
						PointLight.Linear * dirDistance +
						PointLight.Constant; // ax^2 + bx + c
	return (Colour/Attenuation);
}

vec4 CalcSingleSpotLight(SpotLightData SpotLight)
{
	vec3 RayDirection = normalize(FragPos - SpotLight.Base.Position);
	float SpotLightFactor = dot(RayDirection, SpotLight.Direction);

	if(SpotLightFactor > SpotLight.Edge)
	{
		vec4 Colour = CaclSinglePointLight(SpotLight.Base);
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
		TotalColour += CaclSinglePointLight(PointLights[i]);
	}
	return TotalColour;
}

vec4 CalcSpotLights()
{
	vec4 TotalColour = vec4(0,0,0,0);
	for(int i = 0; i < SpotLightCount; i++)
	{
		TotalColour += CalcSingleSpotLight(SpotLights[i]);
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