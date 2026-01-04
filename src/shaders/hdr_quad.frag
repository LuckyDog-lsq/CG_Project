#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform float exposure;
uniform float gamma;

vec3 tonemapReinhard(vec3 color) {
    return color / (color + vec3(1.0));
}

void main() {
    vec3 hdr = texture(hdrBuffer, TexCoords).rgb;
    // simple exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdr * exposure);
    // or use Reinhard: mapped = tonemapReinhard(hdr * exposure);
    mapped = pow(mapped, vec3(1.0 / gamma));
    FragColor = vec4(mapped, 1.0);
}