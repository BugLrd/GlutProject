// Glut Project
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

#define WIN_W 1600
#define WIN_H 1000
#define PI 3.14159265358979323846

void drawRect(float x1, float y1, float x2, float y2) {
	glBegin(GL_QUADS);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);
	glEnd();
}

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) {
	glBegin(GL_TRIANGLES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glVertex2f(x3, y3);
	glEnd();
}

void drawCircle(float cx, float cy, float r, int segments) {
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx, cy);
	for (int i = 0; i <= segments; i++) {
		float angle = 2.0f * (float)PI * i / segments;
		glVertex2f(cx + r * cosf(angle), cy + r * sinf(angle));
	}
	glEnd();
}

//-------------------------Structs---------------------------------------
struct Color3 {
	float r, g, b;
};

struct HutColors {
	Color3 wall;
	Color3 roof;
	Color3 door;
	Color3 knob;
	Color3 window;
	Color3 windowFrame;
	Color3 chimney;
	Color3 smoke;
};
//-------------------------End(Structs)---------------------------------------

//-------------------------HutColors---------------------------------------
const HutColors HutColor1 = {{0.85f, 0.65f, 0.45f}, {0.65f, 0.16f, 0.16f},
							 {0.40f, 0.22f, 0.10f}, {1.0f, 0.85f, 0.2f},
							 {0.68f, 0.85f, 0.90f}, {0.30f, 0.20f, 0.10f},
							 {0.55f, 0.35f, 0.30f}, {0.85f, 0.85f, 0.85f}};

const HutColors HutColor2 = {{0.93f, 0.87f, 0.70f}, {0.80f, 0.63f, 0.22f},
							 {0.35f, 0.20f, 0.08f}, {1.0f, 0.85f, 0.2f},
							 {0.68f, 0.85f, 0.90f}, {0.30f, 0.20f, 0.10f},
							 {0.50f, 0.50f, 0.50f}, {0.85f, 0.85f, 0.85f}};

const HutColors HutColor3 = {{0.96f, 0.96f, 0.94f}, {0.16f, 0.24f, 0.55f},
							 {0.70f, 0.15f, 0.15f}, {0.85f, 0.70f, 0.25f},
							 {0.90f, 0.92f, 0.60f}, {0.20f, 0.20f, 0.25f},
							 {0.45f, 0.45f, 0.48f}, {0.85f, 0.85f, 0.85f}};

const HutColors HutColor4 = {{0.95f, 0.75f, 0.80f}, {0.20f, 0.55f, 0.50f},
							 {0.35f, 0.15f, 0.35f}, {0.90f, 0.85f, 0.30f},
							 {0.95f, 0.95f, 0.90f}, {0.30f, 0.15f, 0.20f},
							 {0.55f, 0.55f, 0.55f}, {0.90f, 0.90f, 0.90f}};
//---------------------------- end(hutcolors)----------------------------

//-----------------------------HutDesigns-----------------------------
void hut1(HutColors colors) {
	// Chimney (drawn before roof so roof edge overlaps it nicely)
	glColor3f(colors.chimney.r, colors.chimney.g, colors.chimney.b);
	drawRect(350.0f, 400.0f, 390.0f, 490.0f);

	// Roof
	glColor3f(colors.roof.r, colors.roof.g, colors.roof.b);
	drawTriangle(140.0f, 350.0f, 460.0f, 350.0f, 300.0f, 480.0f);

	// Walls
	glColor3f(colors.wall.r, colors.wall.g, colors.wall.b);
	drawRect(180.0f, 150.0f, 420.0f, 350.0f);

	// Door
	glColor3f(colors.door.r, colors.door.g, colors.door.b);
	drawRect(270.0f, 150.0f, 330.0f, 260.0f);

	// Door knob
	glColor3f(colors.knob.r, colors.knob.g, colors.knob.b);
	drawCircle(320.0f, 205.0f, 4.0f, 12);

	// Left window
	glColor3f(colors.window.r, colors.window.g, colors.window.b);
	drawRect(210.0f, 270.0f, 260.0f, 320.0f);

	// Right window
	drawRect(340.0f, 270.0f, 390.0f, 320.0f);

	// Window frames (cross bars)
	glColor3f(colors.windowFrame.r, colors.windowFrame.g, colors.windowFrame.b);
	glLineWidth(2.0f);

	glBegin(GL_LINES);
	// Left window frame
	glVertex2f(235.0f, 270.0f);
	glVertex2f(235.0f, 320.0f);
	glVertex2f(210.0f, 295.0f);
	glVertex2f(260.0f, 295.0f);
	// Right window frame
	glVertex2f(365.0f, 270.0f);
	glVertex2f(365.0f, 320.0f);
	glVertex2f(340.0f, 295.0f);
	glVertex2f(390.0f, 295.0f);
	glEnd();
}

void hut2(HutColors colors) {
	// Rounded cottage walls, topped with an arched crown
	// that blends into the conical roof.
	glColor3f(colors.wall.r, colors.wall.g, colors.wall.b);
	drawRect(190.0f, 150.0f, 410.0f, 340.0f);

	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(300.0f, 340.0f);
	for (int i = 0; i <= 20; i++) {
		float angle = (float)PI * i / 20.0f; // 0 to PI, top half only
		glVertex2f(300.0f + 110.0f * cosf(angle), 340.0f + 40.0f * sinf(angle));
	}
	glEnd();

	// Conical thatched roof
	glColor3f(colors.roof.r, colors.roof.g, colors.roof.b);
	drawTriangle(160.0f, 370.0f, 440.0f, 370.0f, 300.0f, 500.0f);

	// Layered thatch lines for texture (a shaded variant of the roof color)
	glColor3f(colors.roof.r * 0.75f, colors.roof.g * 0.75f,
			  colors.roof.b * 0.75f);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	for (int i = 1; i <= 4; i++) {
		float t = i / 5.0f;
		float y = 370.0f + t * (500.0f - 370.0f);
		float halfWidth = (1.0f - t) * 140.0f;
		glVertex2f(300.0f - halfWidth, y);
		glVertex2f(300.0f + halfWidth, y);
	}
	glEnd();

	// Round chimney with a bit of smoke
	glColor3f(colors.chimney.r, colors.chimney.g, colors.chimney.b);
	drawRect(355.0f, 420.0f, 385.0f, 470.0f);
	// glColor3f(colors.smoke.r, colors.smoke.g, colors.smoke.b);
	// drawCircle(370.0f, 485.0f, 10.0f, 16);
	// drawCircle(382.0f, 505.0f, 13.0f, 16);
	// drawCircle(368.0f, 522.0f, 9.0f, 16);

	// Arched wooden door
	glColor3f(colors.door.r, colors.door.g, colors.door.b);
	drawRect(265.0f, 150.0f, 335.0f, 230.0f);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(300.0f, 230.0f);
	for (int i = 0; i <= 16; i++) {
		float angle = (float)PI * i / 16.0f;
		glVertex2f(300.0f + 35.0f * cosf(angle), 230.0f + 35.0f * sinf(angle));
	}
	glEnd();

	// Door knob
	glColor3f(colors.knob.r, colors.knob.g, colors.knob.b);
	drawCircle(320.0f, 195.0f, 4.0f, 12);

	// Round porthole windows with a cross frame
	glColor3f(colors.window.r, colors.window.g, colors.window.b);
	drawCircle(235.0f, 280.0f, 28.0f, 24);
	drawCircle(365.0f, 280.0f, 28.0f, 24);

	glColor3f(colors.windowFrame.r, colors.windowFrame.g, colors.windowFrame.b);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	// Left window frame
	glVertex2f(235.0f, 252.0f);
	glVertex2f(235.0f, 308.0f);
	glVertex2f(207.0f, 280.0f);
	glVertex2f(263.0f, 280.0f);
	// Right window frame
	glVertex2f(365.0f, 252.0f);
	glVertex2f(365.0f, 308.0f);
	glVertex2f(337.0f, 280.0f);
	glVertex2f(393.0f, 280.0f);
	glEnd();
}

void hut3(HutColors colors) {
	// A square, flat-roofed adobe-style hut with a stepped roofline
	// and a small awning over the door.

	// Base walls (wide, squat rectangle)
	glColor3f(colors.wall.r, colors.wall.g, colors.wall.b);
	drawRect(170.0f, 150.0f, 430.0f, 340.0f);

	// Stepped parapet roofline (flat roof look) drawn as a few offset
	// rectangles
	glColor3f(colors.roof.r, colors.roof.g, colors.roof.b);
	drawRect(150.0f, 340.0f, 450.0f, 365.0f);
	drawRect(190.0f, 365.0f, 410.0f, 385.0f);
	drawRect(230.0f, 385.0f, 370.0f, 400.0f);

	// Chimney, offset to one side, sitting on the top step
	glColor3f(colors.chimney.r, colors.chimney.g, colors.chimney.b);
	drawRect(320.0f, 400.0f, 350.0f, 460.0f);

	// Awning over the door (triangular canopy)
	glColor3f(colors.roof.r * 0.85f, colors.roof.g * 0.85f,
			  colors.roof.b * 0.85f);
	drawTriangle(255.0f, 250.0f, 345.0f, 250.0f, 300.0f, 290.0f);

	// Door
	glColor3f(colors.door.r, colors.door.g, colors.door.b);
	drawRect(270.0f, 150.0f, 330.0f, 250.0f);

	// Door knob
	glColor3f(colors.knob.r, colors.knob.g, colors.knob.b);
	drawCircle(320.0f, 200.0f, 4.0f, 12);

	// Single wide window with multiple panes
	glColor3f(colors.window.r, colors.window.g, colors.window.b);
	drawRect(200.0f, 260.0f, 400.0f, 310.0f);

	// Window frame (grid of panes)
	glColor3f(colors.windowFrame.r, colors.windowFrame.g, colors.windowFrame.b);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	// Vertical dividers
	glVertex2f(266.0f, 260.0f);
	glVertex2f(266.0f, 310.0f);
	glVertex2f(333.0f, 260.0f);
	glVertex2f(333.0f, 310.0f);
	// Horizontal divider
	glVertex2f(200.0f, 285.0f);
	glVertex2f(400.0f, 285.0f);
	glEnd();
}

void hut4(HutColors colors) {}

//-----------------------------End(HutDesigns)-----------------------------

void sun() {
	glColor3f(1.0f, 0.85f, 0.0f);
	drawCircle(520.0f, 520.0f, 40.0f, 40);
}

void drawGroundAndPath() {
	glColor3f(0.30f, 0.55f, 0.25f);
	glBegin(GL_QUADS);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(WIN_W, 0.0f);
	glVertex2f(WIN_W, WIN_H * 0.33);
	glVertex2f(0.0f, WIN_H * 0.33);
	glEnd();

	glColor3f(0.55f, 0.42f, 0.28f);
	glBegin(GL_QUAD_STRIP);
	glVertex2f(0.0f * WIN_W / 1000.0f, 60.0f * WIN_H / 1000.0f);
	glVertex2f(0.0f * WIN_W / 1000.0f, 100.0f * WIN_H / 1000.0f);
	glVertex2f(250.0f * WIN_W / 1000.0f, 70.0f * WIN_H / 1000.0f);
	glVertex2f(250.0f * WIN_W / 1000.0f, 110.0f * WIN_H / 1000.0f);
	glVertex2f(500.0f * WIN_W / 1000.0f, 55.0f * WIN_H / 1000.0f);
	glVertex2f(500.0f * WIN_W / 1000.0f, 95.0f * WIN_H / 1000.0f);
	glVertex2f(750.0f * WIN_W / 1000.0f, 75.0f * WIN_H / 1000.0f);
	glVertex2f(750.0f * WIN_W / 1000.0f, 115.0f * WIN_H / 1000.0f);
	glVertex2f(1000.0f * WIN_W / 1000.0f, 60.0f * WIN_H / 1000.0f);
	glVertex2f(1000.0f * WIN_W / 1000.0f, 100.0f * WIN_H / 1000.0f);
	glEnd();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();
	glTranslatef(280.0f, 380.0f, 0.0f);
	glScalef(0.26f, 0.26f, 1.0f);
	hut4(HutColor1); // default palette
	glPopMatrix();

	drawGroundAndPath();
	glFlush();
}

void init() {
	glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, WIN_W, 0.0, WIN_H);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(WIN_W, WIN_H);
	glutCreateWindow("GlutWindow");
	init();
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}
