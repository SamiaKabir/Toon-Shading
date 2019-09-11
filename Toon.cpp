#include <GL/glut.h>
#include <fstream.h>
#include <math.h>

#define ImageW 600
#define ImageH 400

float framebuffer[ImageH][ImageW][3];

struct Coord3D {double x,y,z;};
struct Color {float R,G,B;};

Coord3D Light;
Coord3D SphCent[2];
double SphRad[2];
Color color;

// Normalizes the vector passed in
void normalize(double& x, double& y, double& z) {
	float temp = sqrt(x*x+y*y+z*z);
	if (temp > 0.0) {
		x /= temp;
		y /= temp;
		z /= temp;
	} else {
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}
}

// Returns dot product of two vectors
float dot(double x1, double y1, double z1, double x2, double y2, double z2) {
	return (x1*x2+y1*y2+z1*z2);
}

// Returns angle between two vectors
float angle(double x1, double y1, double z1, double x2, double y2, double z2) {
	normalize(x1,y1,z1);
	normalize(x2,y2,z2);
	return  acos(dot(x1,y1,z1,x2,y2,z2));
}

//return max between two values
float max(float a,float b)
{
float temp=0;
  if(a>b) temp=a;
  else
  temp=b;

  return temp;
}

// Get Color for a point on the surface based on diffuse or toon shading

void GetColor(Coord3D view,   // Normalized Vector pointing FROM eye TO surface
			  Coord3D normal, // Normalized Vector giving surface normal
			  Coord3D light,  // Normalized Vector pointing FROM surface TO light
			  int SphNum,     // Sphere Number (0-1)
			  float& R,       // Return these values for surface color.
			  float& G,
			  float& B) {

    float intensity;

    intensity=dot(light.x,light.y,light.z,normal.x,normal.y,normal.z);
    //sphere with toon shading

    if(SphNum==1)
    {
     //based on the intensity, the shades maps into different ranges of color instead of continuous shading
     if (intensity > 0.95)
	{

        R= color.R*1;
        G= color.G*1;
        B= color.B*1;
	}
	else if (intensity > 0.75)
	{

        R= color.R*0.8;
        G= color.G*0.8;
        B= color.B*0.8;

	}
    else if (intensity > 0.5)
	{

        R= color.R*0.6;
        G= color.G*0.6;
        B= color.B*0.6;
	}
	else if (intensity > 0.25)
    {

        R= color.R*0.4;
        G= color.G*0.4;
        B= color.B*0.4;
    }

	else

    {

        R= color.R*0.2;
        G= color.G*0.2;
        B= color.B*0.2;

    }

    //outline of the sphere or silhouette

    float view_angl=angle(view.x,view.y,view.z,normal.x,normal.y,normal.z);

    if (view_angl>1.5&&view_angl<1.83)
    {                                       //if viewing angle is ~90 degree,that indicates the edge of the object
        R=0.0;                              // so an outline is drwan at the edge by making the color values 0
	    G=0.0;
	    B=0.0;
    }
    }

  //Sphere with diffuse shading only
    else{
    R=color.R*intensity;
    G=color.G*intensity;
    B=color.B*intensity;

    }
}

// Draws the scene
void drawit(void)
{
   glDrawPixels(ImageW,ImageH,GL_RGB,GL_FLOAT,framebuffer);
   glFlush();
}


// Sets pixel x,y to the color RGB
void setFramebuffer(int x, int y, float R, float G, float B)
{
	if (R<=1.0)
		if (R>=0.0)
			framebuffer[y][x][0]=R;
		else
			framebuffer[y][x][0]=0.0;
	else
		framebuffer[y][x][0]=1.0;
	if (G<=1.0)
		if (G>=0.0)
			framebuffer[y][x][1]=G;
		else
			framebuffer[y][x][1]=0.0;
	else
		framebuffer[y][x][1]=1.0;
	if (B<=1.0)
		if (B>=0.0)
			framebuffer[y][x][2]=B;
		else
			framebuffer[y][x][2]=0.0;
	else
		framebuffer[y][x][2]=1.0;
}

void display(void)
{
	int i,j,k;
	float R, G, B;
	Coord3D refpt;
	Coord3D view;
	Coord3D normal;
	Coord3D light;
	Coord3D intpt;
	double xstep = 12.0/ImageW;
	double ystep = 8.0/ImageH;
	double t;
	double a,b,c;
	int intsphere;

	refpt.x = -6.0 + xstep/2.0;
	refpt.y = -4.0 + ystep/2.0;
	refpt.z = -10.0;

	for(i=0;i<ImageW;i++,refpt.x+=xstep) {
		for(j=0;j<ImageH;j++,refpt.y+=ystep) {
			// Compute the view vector
			view.x=refpt.x; view.y=refpt.y; view.z=refpt.z;
			normalize(view.x,view.y,view.z);

			// Find intersection with sphere (if any) - only 1 sphere can intesect.
			intsphere = -1;
			for(k=0;(k<2)&&(intsphere==-1);k++) {
				a = 1.0;  // Since normalized;
				b = 2.0*view.x*(-SphCent[k].x) + 2.0*view.y*(-SphCent[k].y) + 2.0*view.z*(-SphCent[k].z);
                c=SphCent[k].x*SphCent[k].x + SphCent[k].y*SphCent[k].y + SphCent[k].z*SphCent[k].z -
					SphRad[k]*SphRad[k];
				if ((b*b-4*a*c)>=0.0) {  // We have an intersection with that sphere
					// Want nearest of two intersections
					t = (-b - sqrt(b*b-4*a*c))/2.0;
					intsphere = k;
				}
			}

			if (intsphere != -1) { // We had an intersection with a sphere
				intpt.x=t*view.x; intpt.y=t*view.y; intpt.z=t*view.z;
				normal.x = (intpt.x-SphCent[intsphere].x)/SphRad[intsphere];
				normal.y = (intpt.y-SphCent[intsphere].y)/SphRad[intsphere];
				normal.z = (intpt.z-SphCent[intsphere].z)/SphRad[intsphere];
				normalize(normal.x,normal.y,normal.z);

				light.x = Light.x-intpt.x;
				light.y = Light.y-intpt.y;
				light.z = Light.z-intpt.z;
				normalize(light.x,light.y,light.z);
				GetColor(view,normal,light,intsphere,R,G,B);

			} else {
				R=G=B=0.7;
			}
			setFramebuffer(i,j,R,G,B);
		}
		refpt.y = -4.0 + ystep/2.0;
	}

	drawit();

}

void init(void)
{
	int i,j;

	// Initialize framebuffer to clear
	for(i=0;i<ImageH;i++) {
		for (j=0;j<ImageW;j++) {
			framebuffer[i][j][0] = 0.0;
			framebuffer[i][j][1] = 0.0;
			framebuffer[i][j][2] = 0.0;
		}
	}

	// Create Sphere data
	SphCent[0].x = -3.0;
	SphCent[0].y = 0.0;
	SphCent[0].z = -10.0;

	SphCent[1].x = 3.0;
	SphCent[1].y = 0.0;
	SphCent[1].z = -10.0;
	for(i=0;i<2;i++) SphRad[i] = 2.0;

	// Set Light Position
	Light.x = -1.0;
	Light.y = 3.0;
	Light.z = 0.0;

	// Eye is at origin, looking down -z axis, y axis is up,
	// Looks at 8x6 window centered around z = -10.

	//the original color of the balls
	color.R=0.35;
	color.G=1.0;
	color.B=0.35;

}

int main(int argc, char** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowSize(ImageW,ImageH);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Samia Kabir - 641 Assignment 4 - Toon Shading");
	init();
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}
