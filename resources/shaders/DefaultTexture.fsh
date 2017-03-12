precision lowp float;

uniform sampler2D texture0;
varying vec2 var_TexCoord;
void main()
{
       gl_FragColor=texture2D( texture0, var_TexCoord);
}
