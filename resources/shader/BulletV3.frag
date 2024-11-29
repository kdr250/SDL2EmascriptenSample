#version 330

in vec2 windowSize;
in vec2 centerPos;

const float radius = 20.0;

out vec4 outColor;

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;
    fragCoord.y = windowSize.y - fragCoord.y;
    float dist = distance(centerPos, fragCoord);

    if (dist > radius) {
        discard;
    }

    outColor = vec4(0.0, 1.0, 0.0, 1.0);
}
