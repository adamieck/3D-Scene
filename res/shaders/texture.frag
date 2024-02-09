#version 410 core

in vec4 TEPosition;
in vec4 Position;

//layout(location = 0) out vec4 fragColor;
out vec4 fragColor;

//uniform vec4 _Color;
uniform sampler2D _Tex;
//uniform sampler2D _NormalMap;

in vec3 Normal;



void main()
{
    vec4 texColor = texture(_Tex, TEPosition.xy);

    fragColor =  texColor; //* light;
}