#version 330 core
out vec4 FragColor;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NR_POINT_LIGHTS 1

in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform bool reflection;
uniform bool refraction;
uniform float refractiveIndex;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gReflection;
uniform samplerCube skybox;

float alpha_cutoff = 0.001;
// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
void main()
{
	// retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
	float Shininess = texture(gPosition, TexCoords).a;
	vec3 Reflection = texture(gReflection, TexCoords).rgb;
    vec3 viewDir = normalize(viewPos - FragPos);

	if(refraction)
	{
		float ratio = 1.00 / refractiveIndex;
		vec3 I = -viewDir;
		vec3 R = refract(I, Normal, ratio);
		FragColor = vec4(texture(skybox, R).rgb, 1.0);
		return;
	}

    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
    
	vec3 lightDir = normalize(-dirLight.direction);
    // diffuse shading
    float diff = max(dot(Normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
    // combine results
    vec3 ambient = dirLight.ambient * Albedo;
    vec3 diffuse = dirLight.diffuse * diff * Albedo;
    vec3 specular = dirLight.specular * spec * Specular;
    vec3 result = (ambient + diffuse + specular);

    // phase 2: point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		lightDir = normalize(pointLights[i].position - FragPos);
		// diffuse shading
		diff = max(dot(Normal, lightDir), 0.0);
		// specular shading
		reflectDir = reflect(-lightDir, Normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
		// attenuation
		float distance = length(pointLights[i].position - FragPos);
		float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * (distance * distance));
		// combine results
		ambient = pointLights[i].ambient * Albedo;
		diffuse = pointLights[i].diffuse * diff * Albedo;
		specular = pointLights[i].specular * spec * Specular;
		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;
		result += (ambient + diffuse + specular);
	}

    // phase 3: spot light
	lightDir = normalize(spotLight.position - FragPos);
    // diffuse shading
    diff = max(dot(Normal, lightDir), 0.0);
    // specular shading
    reflectDir = reflect(-lightDir, Normal);
    spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
    // attenuation
    float distance = length(spotLight.position - FragPos);
    float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-spotLight.direction));
    float epsilon = spotLight.cutOff - spotLight.outerCutOff;
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    ambient = spotLight.ambient * Albedo;
    diffuse = spotLight.diffuse * diff * Albedo;
    specular = spotLight.specular * spec * Specular;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    result += (ambient + diffuse + specular);

    if(reflection)
		if(Reflection.r > 0.0)
		{
			vec3 I = -viewDir;
			vec3 R = reflect(I, Normal);
			result = mix(result, texture(skybox, R).rgb, Reflection.r);
		}

    FragColor = vec4(result, 1.0);
}