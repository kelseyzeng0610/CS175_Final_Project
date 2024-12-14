#version 330



//TODO: pass in the needed variables

uniform mat4 myModelviewMatrix;
uniform mat4 myProjectionMatrix;


in vec3 myPosition;
in vec3 myNormal;



out vec3 normal;
out vec3 fragPosition;


//TODO: pass out the needed variables



void main() {
	gl_Position = myProjectionMatrix * myModelviewMatrix * vec4(myPosition, 1.0);
	normal = normalize(mat3(myModelviewMatrix) * myNormal);
	// normal = myNormal;
	fragPosition = vec3(myModelviewMatrix * vec4(myPosition, 1.0));
	


	
}





