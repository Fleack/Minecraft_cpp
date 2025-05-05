in vec2 v_UV;
in vec3 v_Normal;
in vec3 v_FragPos;

uniform sampler2D u_Texture;

// Lightning
uniform vec3 u_LightDir;
uniform vec3 u_LightColor;
uniform vec3 u_AmbientColor;

// Fog
uniform vec3 u_CameraPos;
uniform vec3 u_FogColor;
uniform float u_FogDistance;

out vec4 FragColor;

void main()
{
    vec3 normal = normalize(v_Normal);
    vec3 lightDir = normalize(-u_LightDir);
    vec3 viewDir = normalize(u_CameraPos - v_FragPos);

    // Lambert diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    diff = pow(diff, 1.5);

    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
    vec3 specular = spec * vec3(0.1);

    // Lightning
    vec3 baseColor = texture(u_Texture, v_UV).rgb;
    vec3 ambient = u_AmbientColor * baseColor;
    vec3 diffuse = diff * u_LightColor * baseColor;

    // Fog
    float dist = length(v_FragPos - u_CameraPos);
    float fogDensity = 1.0 / max(u_FogDistance, 0.01);
    float fogFactor = exp(-pow(dist * fogDensity, 2.0));
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 color = (ambient + diffuse + specular);
    vec3 finalColor = mix(u_FogColor, color, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}
