#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;
out float fragDistance;

// Wall rendering specific
uniform float wallHeight;
uniform float distanceFactor;

void main() {
    // Pass texture coordinates to fragment shader
    fragTexCoord = vertexTexCoord;
    
    // Pass wall distance information to fragment shader
    // This will be used for distance fog/shading
    fragDistance = vertexPosition.z;
    
    // Pass vertex color to fragment shader
    fragColor = vertexColor;
    
    // Calculate final position with wall height scaling
    vec4 position = vec4(vertexPosition.x, vertexPosition.y * wallHeight, vertexPosition.z, 1.0);
    
    // Apply projection and view transformations
    gl_Position = mvp * position;
}