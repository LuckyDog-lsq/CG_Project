#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

uniform vec3 viewPos;

// 新增：支持点光与平行光
uniform int lightType;    // 0 = point, 1 = directional
uniform vec3 lightPos;
uniform vec3 lightDir;
uniform vec3 lightColor;

uniform float ambientStrength;
uniform vec3 materialSpecular;
uniform float materialShininess;

void main() {
    vec3 pos = texture(gPosition, TexCoords).rgb;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 albedo = texture(gAlbedo, TexCoords).rgb;
    float occlusion = texture(ssao, TexCoords).r;

    // ambient with ssao
    vec3 ambient = ambientStrength * albedo * occlusion;

    // 计算光方向和衰减
    vec3 L;
    float attenuation = 1.0;
    if (lightType == 0) {
        // 点光：方向从片元指向光源，包含距离衰减
        vec3 toLight = lightPos - pos;
        float dist = length(toLight);
        L = normalize(toLight);
        float constant = 1.0;
        float linear = 0.09;
        float quadratic = 0.032;
        attenuation = 1.0 / (constant + linear * dist + quadratic * dist * dist);
    } else {
        // 平行光（太阳）：lightDir 表示光线方向（从场景指向光源）
        L = normalize(-lightDir);
        attenuation = 1.0;
    }

    // diffuse
    float diff = max(dot(normal, L), 0.0);
    vec3 diffuse = diff * albedo * lightColor * attenuation;

    // specular (Blinn-Phong)
    vec3 V = normalize(viewPos - pos);
    vec3 H = normalize(L + V);
    float spec = 0.0;
    if (diff > 0.0) spec = pow(max(dot(normal, H), 0.0), materialShininess);
    vec3 specular = spec * materialSpecular * lightColor * attenuation;

    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, 1.0);
}