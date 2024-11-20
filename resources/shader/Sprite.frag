precision mediump float;

varying vec2 fragTexCoord;

uniform sampler2D uTexture;

void main()
{
    gl_FragColor = texture2D(uTexture, fragTexCoord);
}
