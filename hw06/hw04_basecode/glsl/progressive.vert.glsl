#version 150
// ^ Change this to version 130 if you have compatibility issues

in vec3 vs_position;
in vec2 vs_texcoord;
out vec2 fs_texcoord;

void main( void ){

	gl_Position = vec4( vs_position, 1.0 );
	fs_texcoord = vs_texcoord;
	
}
