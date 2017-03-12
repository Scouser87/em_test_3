uniform mat4 Projection;
uniform mat4 Modelview;

attribute vec2 Position;

void main( void )
{
    gl_Position = Projection * Modelview * vec4(Position.xy, vec2(1,1));
}
