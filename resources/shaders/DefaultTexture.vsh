uniform mat4 Projection;
uniform mat4 Modelview;

attribute vec2 Position;
attribute vec2 TexCoord;

varying vec2 var_TexCoord;                                                         

void main( void )
{
    gl_Position = Projection * Modelview * vec4(Position.xy, vec2(1,1));
    var_TexCoord=TexCoord;
}
