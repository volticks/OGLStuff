#version 330 core
out vec4 FragColor;

// Uniform == global variable, accessable from anywhere in the shader pipeline, can also be accessed from c++, see colourGradient function.

in vec3 ourColour;
in vec2 TexCoord;

// For sampling tex colours
// GL_TEXTURE0
uniform sampler2D texture1;
// GL_TEXTURE1
uniform sampler2D texture2;
// Checks if we actually wanna be texturing
uniform int doTexturing;

void main()
{
    //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    //FragColor = vec4(ourColour, 1.0);
    if (doTexturing == 1)
        FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2)/* + vec4(ourColour, 1.0)*/;
    else 
        FragColor = vec4(ourColour, 1.0);
    //FragColor = texture(texture2, TexCoord) + vec4(ourColour, 1.0);

}
