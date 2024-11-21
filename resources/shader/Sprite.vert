uniform vec2 uWindowSize;

attribute vec3 inPosition;
attribute vec2 inTexCoord;

varying vec2 fragTexCoord;

void main()
{
    float max = max(uWindowSize.x, uWindowSize.y);
    float x = inPosition.x * (uWindowSize.y / max);
    float y = inPosition.y * (uWindowSize.x / max);
    gl_Position = vec4(vec2(x, y), inPosition.z, 1.0);
    fragTexCoord = inTexCoord;
}
