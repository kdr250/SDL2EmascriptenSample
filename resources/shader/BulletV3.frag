#version 330

in vec2 windowSize;
in vec2 centerPos;

const float radius = 8.0;

out vec4 outColor;

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;
    fragCoord.y = windowSize.y - fragCoord.y;
    float dist = distance(centerPos, fragCoord);

    float color = radius / dist;
    outColor = vec4(0.0, color, 0.0, color);
}
