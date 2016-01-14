#version 150
// ^ Change this to version 130 if you have compatibility issues

in vec2 fs_texcoord;
out vec4 color;
uniform sampler2D mytexture;

void main( void ){
	
        color = texture( mytexture, fs_texcoord );
	
}
