#version 330

uniform vec2 uWindowSize;
uniform vec2 uBulletPosition;
uniform vec2 uBulletSize;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

out vec2 windowSize;
out vec2 centerPos;

void main()
{
    vec2 halfWindow = uWindowSize * 0.5;
    float positionX = (uBulletPosition.x - halfWindow.x) / halfWindow.x;
    float positionY = (uBulletPosition.y - halfWindow.y) / halfWindow.y * -1.0;
    vec2 diff = (uBulletSize / uWindowSize) * 0.5;

    vec2 finalPosition = vec2(positionX, positionY) + inPosition.xy * diff;

    gl_Position = vec4(finalPosition, inPosition.z, 1.0);
    centerPos = uBulletPosition.xy;
    windowSize = uWindowSize;
}
