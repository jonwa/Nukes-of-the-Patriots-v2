uniform sampler2D texture;
varying vec2 pixelPos;
uniform vec2 mousePos;

#define PI 3.141592653

/*
void main()
{
	vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
	
	float dist = sqrt( (mousePos.x - gl_FragCoord.x)*(mousePos.x - gl_FragCoord.x) + (mousePos.y - gl_FragCoord.y)*(mousePos.y - gl_FragCoord.y) );
	dist -= 100;
	dist = max(dist, 0.0);
	dist /= 200;
	float alpha = 1 - min(dist, 1.0);
	gl_FragColor = vec4(pixel.r, pixel.g, pixel.b, pixel.a * alpha);
}
*/
/*
void main()
{
	float x = gl_TexCoord[0].x;
	float y = gl_TexCoord[0].y;
	
	float _y = gl_FragCoord.y/768;
	float moveX = 1 - (mousePos.x / 1024);
	x += sin(_y*PI)*moveX;
	if(x < 0.0 || x > 1.0 || y < 0.0 || y > 1.0)
		discard;
	vec4 pixel = texture2D(texture, vec2(x, y));
	gl_FragColor = vec4(pixel.r, pixel.g, pixel.b, pixel.a);
}
*/

void main()
{
	float offset = (mousePos.x/1024)*0.003;
	vec4 pixel = texture2D(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y));
	pixel += texture2D(texture, vec2(gl_TexCoord[0].x + offset*-2, gl_TexCoord[0].y));
	pixel += texture2D(texture, vec2(gl_TexCoord[0].x + offset*-1, gl_TexCoord[0].y));
	pixel += texture2D(texture, vec2(gl_TexCoord[0].x + offset, gl_TexCoord[0].y));
	pixel += texture2D(texture, vec2(gl_TexCoord[0].x + offset*2, gl_TexCoord[0].y));
	
	pixel += texture2D(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + offset*-2));
	pixel += texture2D(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + offset*-1));
	pixel += texture2D(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + offset));
	pixel += texture2D(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + offset*2));
	gl_FragColor = vec4(pixel.r/9, pixel.g/9, pixel.b/9, pixel.a/9);
}