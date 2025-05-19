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
out vec3 fragPosition;

// Floor/ceiling specific uniforms
uniform vec3 cameraPosition;

void main() {
    // Pass texture coordinates to fragment shader
    fragTexCoord = vertexTexCoord;
    
    // Pass vertex color to fragment shader
    fragColor = vertexColor;
    
    // Pass vertex position to fragment shader for proper texture mapping
    fragPosition = vertexPosition;
    
    // Apply MVP transformation to vertex
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}