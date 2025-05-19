#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragPosition;

// Output color
out vec4 FragColor;

// Input uniform values
uniform sampler2D texture0;  // Floor texture
uniform sampler2D texture1;  // Ceiling texture
uniform vec4 colDiffuse;
uniform vec3 cameraPosition;
uniform float fogDensity;
uniform float floorCeilingDarkness;

// Floor/ceiling specific uniforms
uniform bool isCeiling;     // true for ceiling, false for floor
uniform float textureScale; // how many times to repeat the texture

void main() {
    // Calculate the ray direction in world space
    vec3 rayDir = normalize(fragPosition - cameraPosition);
    
    // Calculate the distance from the camera to the floor/ceiling
    float distance;
    
    if (isCeiling) {
        // For ceiling, we need the intersection with a horizontal plane above the camera
        // y-axis in Wolfenstein is typically the up/down axis
        distance = (cameraPosition.y + 1.0) / rayDir.y; // +1.0 for ceiling height
    } else {
        // For floor, we need the intersection with a horizontal plane below the camera
        distance = cameraPosition.y / -rayDir.y;
    }
    
    // Get the exact intersection point with the floor/ceiling
    vec3 intersection = cameraPosition + rayDir * distance;
    
    // Calculate texture coordinates based on world position
    vec2 texCoord = vec2(intersection.x, intersection.z) * textureScale;
    texCoord = fract(texCoord); // Ensure wrapping
    
    // Sample either floor or ceiling texture
    vec4 texelColor;
    if (isCeiling) {
        texelColor = texture(texture1, texCoord);
    } else {
        texelColor = texture(texture0, texCoord);
    }
    
    // Apply base color
    texelColor *= colDiffuse * fragColor;
    
    // Apply distance-based darkening
    float darkening = 1.0 - (distance * floorCeilingDarkness);
    darkening = clamp(darkening, 0.2, 1.0);
    texelColor.rgb *= darkening;
    
    // Apply fog effect based on distance
    float fogFactor = 1.0 / exp(distance * fogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    // Fog color (sky blue for ceiling, darker for floor)
    vec3 fogColor;
    if (isCeiling) {
        fogColor = vec3(0.4, 0.6, 0.8); // Sky blue
    } else {
        fogColor = vec3(0.2, 0.2, 0.3); // Darker color for floor fog
    }
    
    // Mix texture color with fog
    texelColor.rgb = mix(fogColor, texelColor.rgb, fogFactor);
    
    // Final fragment color
    FragColor = texelColor;
}