#version 330 core
out vec4 FragColor;

//in vec2 TexCoords;
in vec4 pass;

uniform sampler2D texture_diffuse1;

void main()
{    
	if(pass.y < -0.9) FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	else FragColor = vec4(0.1, 0.5, 0.1, 1.0);
	if(pass.y > 0.0) FragColor = vec4(0.0, 0.0, 1.0, 1.0);
   // FragColor = pass;//= texture(texture_diffuse1, TexCoords);	
}