#ifdef GL_ES
precision mediump float;
#endif

varying vec2 windowSize;
varying vec2 centerPos;

const float radius = 5.0;

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;
    fragCoord.y = windowSize.y - fragCoord.y;
    float dist = distance(centerPos, fragCoord);

    float color = pow(radius / dist, 2.0);
    gl_FragColor = vec4(0.0, color, 0.0, 1.0);
}
