#version 410 core

in vec4 TEPosition;
in vec4 Position;

layout(location = 0) out vec4 fragColor;

uniform vec4 _Color;
uniform sampler2D _Tex;
uniform sampler2D _NormalMap;

uniform vec3 LightPosition;
uniform vec3 LightColor;
uniform float Kd;
uniform float Ks;
uniform float m;
uniform int hasTexture;
uniform int hasNormals;
uniform int hasLight;
uniform int hasReflectors;
uniform float reflectorAlpha;

in vec3 Normal;

vec3 Lambert( vec3 pos, vec3 norm)
{
    vec3 L = normalize(vec3(LightPosition) - pos);
    float LDotN = max( dot(norm, L), 0.0 );
    vec3 diffuse = Kd * LightColor * LDotN;

    vec3 R = 2*LDotN*norm - L;
    float VDotR = max( dot(vec3(0,0,1), R) ,0.0);
    diffuse += Ks * LightColor * pow(VDotR, m);

    return diffuse;
}

vec3 Reflector( vec3 pos, vec3 norm)
{
    vec3 ref1Color = vec3(1.0,0.0,0.0);
    vec3 ref2Color = vec3(0.0,1.0,0.0);
    vec3 ref3Color = vec3(0.0,0.0,1.0);

    vec3 ref1Pos = vec3(0.0, 0.0, m);
    vec3 ref2Pos = vec3(1.0, 0.0, m);
    vec3 ref3Pos = vec3(1.0/2.0, 1.0, m);

    vec3 target = vec3(1.0/2.0, 1.0/2.0, 0);

    vec3 SpotDir1 =  normalize(ref1Pos - target);
    vec3 SpotDir2 = normalize(ref2Pos - target);
    vec3 SpotDir3 = normalize(ref3Pos - target);

    vec3 L, R, diffuse;
    float LDotN, VDotR;
    int k = 20;
    vec3 Il;

    // 1
    L = normalize(vec3(ref1Pos) - pos);
    LDotN = max( dot(norm, L), 0.0 );
    Il = ref1Color * pow(dot(L, SpotDir1), k);
    diffuse = Kd * Il * LDotN;

    R = 2*LDotN*norm - L;
    VDotR = max( dot(vec3(0,0,1), R) ,0.0);
    Il = ref1Color * pow(dot(L, SpotDir1), k);
    diffuse += Ks * Il * pow(VDotR, m) * pow(max(0.0,cos(reflectorAlpha)), 20);

    // 2
    L = normalize(vec3(ref2Pos) - pos);
    LDotN = max( dot(norm, L), 0.0 );
    Il = ref2Color * pow(dot(L, SpotDir2), k);

    diffuse += Kd * Il * LDotN;

    R = 2*LDotN*norm - L;
    VDotR = max( dot(vec3(0,0,1), R) ,0.0);
    Il = ref2Color * pow(dot(L, SpotDir2), k);

    diffuse += Ks * Il * pow(VDotR, m) * pow(max(0.0,cos(reflectorAlpha)), 20);

    // 3
    L = normalize(vec3(ref3Pos) - pos);
    LDotN = max( dot(norm, L), 0.0 );
    Il = ref3Color * pow(dot(L, SpotDir3), k);

    diffuse += Kd * Il * LDotN;

    R = 2*LDotN*norm - L;
    VDotR = max( dot(vec3(0,0,1), R) ,0.0);
    Il = ref3Color * pow(dot(L, SpotDir3), k);

    diffuse += Ks * Il * pow(VDotR, m) * pow(max(0.0,cos(reflectorAlpha)), 20);

    return diffuse;
}

void main()
{
    vec4 texColor;
    if (hasTexture == 1)
    {
	    texColor = texture(_Tex, TEPosition.xy);
    }
    else
    {
        texColor = _Color;
    }

    vec3 normal;
    if(hasNormals == 1)
    {
        vec3 normalTex = normalize(texture(_NormalMap, TEPosition.xy).xyz * 2.0f - 1.0f); // normal from tex

        vec3 N = vec3(0.0, 0.0, 1.0);
        vec3 B = vec3(0.0, 1.0, 0.0);
        vec3 T = normalize( cross(B, N) );
        mat3 TBN = mat3(T, B, N);

        normal = TBN * normalTex;
    }
    else
    {
        normal = Normal;
    }

    // Light
    vec4 light = vec4(0.0, 0.0, 0.0, 0.0);
    if(hasLight == 1)
    {
        light += vec4( Lambert( TEPosition.xyz, normal ), 1.0);
    }

    if(hasReflectors == 1)
    {
        light += vec4(Reflector(TEPosition.xyz, normal), 1.0);
    }

    fragColor =  texColor * light;
}