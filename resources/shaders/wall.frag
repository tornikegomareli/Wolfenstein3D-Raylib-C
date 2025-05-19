#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;
in float fragDistance;

// Output color
out vec4 FragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform float fogDensity;
uniform float darknessFactor;

// Wall rendering specific
uniform bool applyTextureOffset;
uniform float textureOffset;

void main() {
    // Sample the texture
    vec2 texCoord = fragTexCoord;
    
    // Apply texture horizontal offset if needed (for animated textures like doors)
    if (applyTextureOffset) {
        texCoord.x = fract(texCoord.x + textureOffset);
    }
    
    // Sample wall texture
    vec4 texelColor = texture(texture0, texCoord);
    
    // Apply base color
    texelColor *= colDiffuse * fragColor;
    
    // Apply distance-based darkening for depth effect
    float darkening = 1.0 - (fragDistance * darknessFactor);
    darkening = clamp(darkening, 0.2, 1.0); // Ensure walls don't get too dark
    texelColor.rgb *= darkening;
    
    // Apply simple fog effect based on distance
    float fogFactor = 1.0 / exp(fragDistance * fogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    // Fog color (match ceiling color for cohesive look)
    vec3 fogColor = vec3(0.4, 0.6, 0.8); // Sky blue, can be made a uniform later
    
    // Mix texture color with fog
    texelColor.rgb = mix(fogColor, texelColor.rgb, fogFactor);
    
    // Final fragment color
    FragColor = texelColor;
}