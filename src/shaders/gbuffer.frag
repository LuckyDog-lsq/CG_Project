#version 330 core
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec3 gAlbedo;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 fallbackColor;
uniform bool useAlbedoTexture;
uniform sampler2D albedoTex;

void main() {
    gPosition = FragPos;
    gNormal = normalize(Normal);
    vec3 albedo = fallbackColor;
    if(useAlbedoTexture) {
        albedo = texture(albedoTex, TexCoords).rgb;
    }
    gAlbedo = albedo;
}