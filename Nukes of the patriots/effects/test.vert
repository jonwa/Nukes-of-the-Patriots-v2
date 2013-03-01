varying vec2 pixelPos;

#define PI 3.141592653

void main()
{
   vec4 a = gl_Vertex;
   
   
   a.w *= 0.5;
   a.x -= 0.5;
   a.y -= 0.5;
   float _y = abs(a.y) - 0.5;
   float moveX = sin(_y*PI);
	a.x += _y + sin(a.y);
   gl_Position = a;
   pixelPos = vec2(a.x, a.y);
}