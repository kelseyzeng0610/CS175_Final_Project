#version 330

//TODO: pass in the needed variables


uniform vec3 lightPosition;

in vec3 fragPosition;
in vec3 normal;
out vec4 outputColor; //this tells OpenGL that this variable is the output color

void main()
{
	
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPosition - fragPosition);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 color = (norm * 0.5) + 0.5; 


	outputColor =  diff * vec4(color,1.0);




	
}