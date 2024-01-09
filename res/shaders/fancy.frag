#version 410 core
in vec2 fragCoord;
out vec4 fragColor;
uniform float iTime;
float sdHexagon(in vec2 p, in float r)
{
    const vec3 k = vec3(-0.866025404, 0.5, 0.577350269);
    p = abs(p);
    p -= 2.0 * min(dot(k.xy, p), 0.0) * k.xy;
    p -= vec2(clamp(p.x, -k.z * r, k.z * r), r);
    return length(p) * sign(p.y);
}

vec3 pallette(float t)
{
    vec3 a = vec3(0.691, 1.078, 1.258);
    vec3 b = vec3(0.358, 0.668, 0.848);
    vec3 c = vec3(-1.882, -3.142, -1.172);
    vec3 d = vec3(-0.682, -0.422, 1.138);

    return a + b * cos(6.28318 * (c * t + d));
}

void main()
{
    vec2 uv = fragCoord;
    vec2 uv0 = uv;

    vec3 finalColor = vec3(0.0);

    for (float i = 0.0; i < 2.0; i++)
    {
        uv = fract(uv * 1.5) - 0.5;

        float d = sdHexagon(uv, 0.7) * exp(-sdHexagon(uv0, 1.0));
        vec3 col = pallette(sdHexagon(uv0, 0.4) + iTime * 0.4);

        d = sin(d * 8.0 + iTime * 2.0) / 8.0;
        d = smoothstep(0.0, 0.1, abs(d));

        d = pow(0.15 / d, 1.1);

        finalColor += col * d;
    }

    fragColor = vec4(finalColor, 1.0);
}