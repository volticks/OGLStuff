#version 330 core
layout (location = 0) in vec3 aPos;
// Currently, the second position in our vertex attributes is the colour values.
layout (location = 1) in vec3 aColour1;
// Texture coords
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColour;
// Pass tex coords to frag shader
out vec2 TexCoord;

uniform vec3 aColour;

// For our transformation
uniform mat4 transform;

// For our perspective stuff
uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat4 modelMat;

// Are we using supplied colours or vertex colours?
uniform int useUniformColours;

void main(){
	// Implicitly aPos.x/y/z
	
	//gl_Position = transform * vec4(aPos, 1.0);
	gl_Position = projMat * viewMat * modelMat * vec4(aPos, 1.0);

	//gl_Position = projMat * viewMat * vec4(aPos, 1.0);
	
	//gl_Position = vec4(aPos, 1.0);

	// Pass it along the pipeline.
	if (useUniformColours == 1)
		ourColour = aColour;
	else
		ourColour = aColour1;

	TexCoord = aTexCoord;
}