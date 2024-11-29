#version 330

in vec2 windowSize;
in vec2 centerPos;

const float radius = 5.0;

out vec4 outColor;

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;
    fragCoord.y = windowSize.y - fragCoord.y;
    float dist = distance(centerPos, fragCoord);

    float color = pow(radius / dist, 2.0);
    outColor = vec4(0.0, color, 0.0, 1.0);
}
