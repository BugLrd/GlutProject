// Glut Project
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <algorithm>
#include <math.h>

#define WIN_W 1600
#define WIN_H 1000
#define PI 3.14159265358979323846

//-------------------------------------------------------------------------------------SCENE1------------------------------------------------------------------------------------------------------
// Toggled with 'n' (night) and 'd' (day) keys
bool isNightMode = false;
// Horizontal drift offset for the clouds, advanced each frame by update()
float cloudOffset = 0.0f;
const float CLOUD_SPEED = 0.5f;

// Animation state for scene2's fire/smoke (advanced each frame by update())
float firePhase = 0.0f;
const float FIRE_SPEED = 0.12f;
const float SMOKE_DRIFT_SPEED = 0.25f;
// Drift accumulator for rising/swaying smoke puffs in Scene 2
float smokeOffset = 0.0f;

//------------------------- Basic Drawing Helpers -------------------------
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

// Helper for smooth rounded mountain domes
void drawMountainDome(float cx, float cy, float rx, float ry, float r, float g,
					  float b) {
	glColor3f(r, g, b);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx, cy);
	for (int i = 0; i <= 36; i++) {
		float angle = (float)PI * i / 36.0f; // 0 to PI (top half dome)
		glVertex2f(cx + rx * cosf(angle), cy + ry * sinf(angle));
	}
	glEnd();
}

void drawHill(float cx, float cy, float rx, float ry, float r, float g,
			  float b) {
	glColor3f(r, g, b);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx, cy);
	for (int i = 0; i <= 30; i++) {
		float angle = (float)PI * i / 30.0f; // 0 to PI (top half curve)
		glVertex2f(cx + rx * cosf(angle), cy + ry * sinf(angle));
	}
	glEnd();
}

//------------------------- Structs & Colors -------------------------
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
const HutColors HutColor5 = {{0.55f, 0.75f, 0.55f}, {0.35f, 0.45f, 0.25f},
							 {0.30f, 0.18f, 0.10f}, {0.85f, 0.75f, 0.30f},
							 {0.75f, 0.90f, 0.95f}, {0.20f, 0.25f, 0.15f},
							 {0.40f, 0.40f, 0.35f}, {0.90f, 0.90f, 0.90f}};
const HutColors HutColor6 = {{0.80f, 0.30f, 0.25f}, {0.30f, 0.30f, 0.32f},
							 {0.20f, 0.15f, 0.15f}, {0.90f, 0.80f, 0.25f},
							 {0.85f, 0.90f, 0.92f}, {0.15f, 0.15f, 0.18f},
							 {0.40f, 0.42f, 0.44f}, {0.88f, 0.88f, 0.88f}};
const HutColors HutColor7 = {{0.60f, 0.80f, 0.85f}, {0.90f, 0.90f, 0.90f},
							 {0.25f, 0.35f, 0.45f}, {0.75f, 0.75f, 0.80f},
							 {0.55f, 0.70f, 0.80f}, {0.15f, 0.20f, 0.25f},
							 {0.65f, 0.65f, 0.65f}, {0.95f, 0.95f, 0.95f}};
const HutColors HutColor8 = {{0.90f, 0.55f, 0.20f}, {0.55f, 0.20f, 0.15f},
							 {0.30f, 0.12f, 0.08f}, {0.95f, 0.90f, 0.40f},
							 {0.98f, 0.85f, 0.55f}, {0.25f, 0.10f, 0.05f},
							 {0.45f, 0.30f, 0.28f}, {0.90f, 0.85f, 0.80f}};
const HutColors HutColor9 = {{0.75f, 0.90f, 0.75f}, {0.85f, 0.40f, 0.55f},
							 {0.40f, 0.25f, 0.35f}, {0.95f, 0.85f, 0.50f},
							 {0.90f, 0.95f, 0.85f}, {0.30f, 0.20f, 0.25f},
							 {0.60f, 0.55f, 0.60f}, {0.92f, 0.92f, 0.92f}};
const HutColors HutColor10 = {{0.20f, 0.20f, 0.25f}, {0.10f, 0.10f, 0.15f},
							  {0.05f, 0.05f, 0.08f}, {0.70f, 0.70f, 0.75f},
							  {0.40f, 0.55f, 0.70f}, {0.08f, 0.08f, 0.10f},
							  {0.25f, 0.25f, 0.28f}, {0.75f, 0.75f, 0.78f}};
const HutColors HutColor11 = {{0.88f, 0.78f, 0.60f}, {0.45f, 0.25f, 0.20f},
							  {0.28f, 0.16f, 0.10f}, {0.80f, 0.65f, 0.20f},
							  {0.72f, 0.80f, 0.85f}, {0.25f, 0.18f, 0.12f},
							  {0.50f, 0.48f, 0.45f}, {0.88f, 0.88f, 0.88f}};
const HutColors HutColor12 = {{0.65f, 0.85f, 0.90f}, {0.95f, 0.60f, 0.30f},
							  {0.30f, 0.30f, 0.35f}, {0.85f, 0.80f, 0.35f},
							  {0.85f, 0.95f, 0.98f}, {0.20f, 0.22f, 0.28f},
							  {0.60f, 0.60f, 0.62f}, {0.92f, 0.92f, 0.94f}};
const HutColors HutColor13 = {{0.40f, 0.60f, 0.35f}, {0.55f, 0.25f, 0.20f},
							  {0.25f, 0.15f, 0.08f}, {0.90f, 0.75f, 0.25f},
							  {0.80f, 0.88f, 0.70f}, {0.15f, 0.18f, 0.10f},
							  {0.45f, 0.40f, 0.35f}, {0.85f, 0.85f, 0.85f}};
const HutColors HutColor14 = {{0.98f, 0.88f, 0.75f}, {0.70f, 0.40f, 0.60f},
							  {0.35f, 0.20f, 0.30f}, {0.95f, 0.90f, 0.45f},
							  {0.90f, 0.85f, 0.95f}, {0.28f, 0.20f, 0.25f},
							  {0.55f, 0.50f, 0.55f}, {0.90f, 0.90f, 0.92f}};
const HutColors HutColor15 = {{0.30f, 0.35f, 0.45f}, {0.85f, 0.85f, 0.90f},
							  {0.15f, 0.15f, 0.20f}, {0.80f, 0.70f, 0.30f},
							  {0.60f, 0.75f, 0.90f}, {0.10f, 0.10f, 0.15f},
							  {0.55f, 0.58f, 0.60f}, {0.93f, 0.93f, 0.95f}};
const HutColors HutColor16 = {{0.85f, 0.45f, 0.35f}, {0.40f, 0.55f, 0.30f},
							  {0.30f, 0.20f, 0.15f}, {0.90f, 0.85f, 0.30f},
							  {0.95f, 0.90f, 0.75f}, {0.22f, 0.18f, 0.12f},
							  {0.48f, 0.45f, 0.40f}, {0.88f, 0.86f, 0.82f}};
const HutColors HutColor17 = {{0.55f, 0.65f, 0.80f}, {0.25f, 0.25f, 0.30f},
							  {0.35f, 0.25f, 0.40f}, {0.85f, 0.80f, 0.35f},
							  {0.70f, 0.85f, 0.95f}, {0.18f, 0.15f, 0.22f},
							  {0.42f, 0.40f, 0.48f}, {0.90f, 0.90f, 0.92f}};
const HutColors HutColor18 = {{0.92f, 0.70f, 0.55f}, {0.60f, 0.35f, 0.25f},
							  {0.32f, 0.18f, 0.12f}, {0.95f, 0.88f, 0.35f},
							  {0.85f, 0.75f, 0.60f}, {0.28f, 0.16f, 0.10f},
							  {0.50f, 0.42f, 0.35f}, {0.88f, 0.84f, 0.80f}};
const HutColors HutColor19 = {{0.45f, 0.75f, 0.65f}, {0.65f, 0.30f, 0.40f},
							  {0.25f, 0.28f, 0.22f}, {0.88f, 0.82f, 0.30f},
							  {0.80f, 0.92f, 0.88f}, {0.18f, 0.22f, 0.18f},
							  {0.42f, 0.48f, 0.44f}, {0.90f, 0.92f, 0.90f}};
const HutColors HutColor20 = {{0.15f, 0.18f, 0.22f}, {0.55f, 0.15f, 0.15f},
							  {0.08f, 0.08f, 0.10f}, {0.75f, 0.72f, 0.68f},
							  {0.35f, 0.42f, 0.55f}, {0.05f, 0.05f, 0.08f},
							  {0.30f, 0.30f, 0.32f}, {0.70f, 0.70f, 0.72f}};

//---------------------------- end(hutcolors)----------------------------

//-----------------------------HutDesigns-----------------------------
void hut1(HutColors colors) {
	glColor3f(colors.chimney.r, colors.chimney.g, colors.chimney.b);
	drawRect(350.0f, 400.0f, 390.0f, 490.0f);

	glColor3f(colors.roof.r, colors.roof.g, colors.roof.b);
	drawTriangle(140.0f, 350.0f, 460.0f, 350.0f, 300.0f, 480.0f);

	glColor3f(colors.wall.r, colors.wall.g, colors.wall.b);
	drawRect(180.0f, 150.0f, 420.0f, 350.0f);

	glColor3f(colors.door.r, colors.door.g, colors.door.b);
	drawRect(270.0f, 150.0f, 330.0f, 260.0f);

	glColor3f(colors.knob.r, colors.knob.g, colors.knob.b);
	drawCircle(320.0f, 205.0f, 4.0f, 12);

	glColor3f(colors.window.r, colors.window.g, colors.window.b);
	drawRect(210.0f, 270.0f, 260.0f, 320.0f);
	drawRect(340.0f, 270.0f, 390.0f, 320.0f);

	glColor3f(colors.windowFrame.r, colors.windowFrame.g, colors.windowFrame.b);
	glLineWidth(2.0f);

	glBegin(GL_LINES);
	glVertex2f(235.0f, 270.0f);
	glVertex2f(235.0f, 320.0f);
	glVertex2f(210.0f, 295.0f);
	glVertex2f(260.0f, 295.0f);
	glVertex2f(365.0f, 270.0f);
	glVertex2f(365.0f, 320.0f);
	glVertex2f(340.0f, 295.0f);
	glVertex2f(390.0f, 295.0f);
	glEnd();
}

void hut2(HutColors colors) {
	glColor3f(colors.wall.r, colors.wall.g, colors.wall.b);
	drawRect(190.0f, 150.0f, 410.0f, 340.0f);

	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(300.0f, 340.0f);
	for (int i = 0; i <= 20; i++) {
		float angle = (float)PI * i / 20.0f;
		glVertex2f(300.0f + 110.0f * cosf(angle), 340.0f + 40.0f * sinf(angle));
	}
	glEnd();

	glColor3f(colors.roof.r, colors.roof.g, colors.roof.b);
	drawTriangle(160.0f, 370.0f, 440.0f, 370.0f, 300.0f, 500.0f);

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

	glColor3f(colors.chimney.r, colors.chimney.g, colors.chimney.b);
	drawRect(355.0f, 420.0f, 385.0f, 470.0f);

	glColor3f(colors.door.r, colors.door.g, colors.door.b);
	drawRect(265.0f, 150.0f, 335.0f, 230.0f);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(300.0f, 230.0f);
	for (int i = 0; i <= 16; i++) {
		float angle = (float)PI * i / 16.0f;
		glVertex2f(300.0f + 35.0f * cosf(angle), 230.0f + 35.0f * sinf(angle));
	}
	glEnd();

	glColor3f(colors.knob.r, colors.knob.g, colors.knob.b);
	drawCircle(320.0f, 195.0f, 4.0f, 12);

	glColor3f(colors.window.r, colors.window.g, colors.window.b);
	drawCircle(235.0f, 280.0f, 28.0f, 24);
	drawCircle(365.0f, 280.0f, 28.0f, 24);

	glColor3f(colors.windowFrame.r, colors.windowFrame.g, colors.windowFrame.b);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glVertex2f(235.0f, 252.0f);
	glVertex2f(235.0f, 308.0f);
	glVertex2f(207.0f, 280.0f);
	glVertex2f(263.0f, 280.0f);
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

	glColor3f(colors.chimney.r, colors.chimney.g, colors.chimney.b);
	drawRect(320.0f, 400.0f, 350.0f, 460.0f);

	glColor3f(colors.roof.r * 0.85f, colors.roof.g * 0.85f,
			  colors.roof.b * 0.85f);
	drawTriangle(255.0f, 250.0f, 345.0f, 250.0f, 300.0f, 290.0f);

	glColor3f(colors.door.r, colors.door.g, colors.door.b);
	drawRect(270.0f, 150.0f, 330.0f, 250.0f);

	glColor3f(colors.knob.r, colors.knob.g, colors.knob.b);
	drawCircle(320.0f, 200.0f, 4.0f, 12);

	glColor3f(colors.window.r, colors.window.g, colors.window.b);
	drawRect(200.0f, 260.0f, 400.0f, 310.0f);

	glColor3f(colors.windowFrame.r, colors.windowFrame.g, colors.windowFrame.b);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glVertex2f(266.0f, 260.0f);
	glVertex2f(266.0f, 310.0f);
	glVertex2f(333.0f, 260.0f);
	glVertex2f(333.0f, 310.0f);
	glVertex2f(200.0f, 285.0f);
	glVertex2f(400.0f, 285.0f);
	glEnd();
}

void hut4(HutColors colors) {
	glColor3f(colors.chimney.r, colors.chimney.g, colors.chimney.b);
	drawRect(370.0f, 250.0f, 410.0f, 440.0f);

	// Chimney (drawn first so the roof overlaps it)
	glColor3f(colors.chimney.r, colors.chimney.g, colors.chimney.b);
	drawRect(370.0f, 250.0f, 410.0f, 440.0f);

	// Short base walls
	glColor3f(colors.wall.r, colors.wall.g, colors.wall.b);
	drawRect(200.0f, 150.0f, 400.0f, 200.0f);

	glColor3f(colors.roof.r, colors.roof.g, colors.roof.b);
	drawTriangle(120.0f, 200.0f, 480.0f, 200.0f, 300.0f, 480.0f);

	// Massive A-frame roof
	glColor3f(colors.roof.r, colors.roof.g, colors.roof.b);
	drawTriangle(120.0f, 200.0f, 480.0f, 200.0f, 300.0f, 480.0f);

	// Central door
	glColor3f(colors.door.r, colors.door.g, colors.door.b);
	drawRect(260.0f, 150.0f, 340.0f, 250.0f);

	// Door knob
	glColor3f(colors.knob.r, colors.knob.g, colors.knob.b);
	drawCircle(325.0f, 200.0f, 4.0f, 12);

	// Large triangular window
	glColor3f(colors.window.r, colors.window.g, colors.window.b);
	drawTriangle(240.0f, 280.0f, 360.0f, 280.0f, 300.0f, 400.0f);

	glColor3f(colors.windowFrame.r, colors.windowFrame.g, colors.windowFrame.b);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glVertex2f(300.0f, 280.0f);
	glVertex2f(300.0f, 400.0f);
	glVertex2f(240.0f, 280.0f);
	glVertex2f(360.0f, 280.0f);
	glEnd();
}

void hut5(HutColors colors) {
	glColor3f(colors.chimney.r, colors.chimney.g, colors.chimney.b);
	drawRect(155.0f, 250.0f, 185.0f, 410.0f);

	glColor3f(colors.wall.r, colors.wall.g, colors.wall.b);
	drawRect(180.0f, 150.0f, 420.0f, 320.0f);

	glColor3f(colors.roof.r, colors.roof.g, colors.roof.b);
	glBegin(GL_POLYGON);
	glVertex2f(160.0f, 320.0f);
	glVertex2f(200.0f, 420.0f);
	glVertex2f(300.0f, 470.0f);
	glVertex2f(400.0f, 420.0f);
	glVertex2f(440.0f, 320.0f);
	glEnd();

	glColor3f(colors.door.r, colors.door.g, colors.door.b);
	drawRect(240.0f, 150.0f, 360.0f, 270.0f);

	// Wide double barn doors
	glColor3f(colors.door.r, colors.door.g, colors.door.b);
	drawRect(240.0f, 150.0f, 360.0f, 270.0f);

	// Door frames and X patterns for the barn look
	glColor3f(colors.windowFrame.r, colors.windowFrame.g, colors.windowFrame.b);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glVertex2f(300.0f, 150.0f);
	glVertex2f(300.0f, 270.0f);
	glVertex2f(240.0f, 150.0f);
	glVertex2f(300.0f, 270.0f);
	glVertex2f(240.0f, 270.0f);
	glVertex2f(300.0f, 150.0f);
	glVertex2f(300.0f, 150.0f);
	glVertex2f(360.0f, 270.0f);
	glVertex2f(300.0f, 270.0f);
	glVertex2f(360.0f, 150.0f);
	glEnd();

	glColor3f(colors.window.r, colors.window.g, colors.window.b);
	drawCircle(300.0f, 370.0f, 28.0f, 20);

	glColor3f(colors.windowFrame.r, colors.windowFrame.g, colors.windowFrame.b);
	glBegin(GL_LINES);
	glVertex2f(272.0f, 370.0f);
	glVertex2f(328.0f, 370.0f);
	glVertex2f(300.0f, 342.0f);
	glVertex2f(300.0f, 398.0f);
	glEnd();
}

//-----------------------------End(HutDesigns)-----------------------------

//------------------------- Trees, Edge Trees &
// Environment-------------------------

// Helper to draw a customizable pine tree
void drawTree(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);

	// Tree Trunk
	glColor3f(0.35f, 0.20f, 0.10f);
	drawRect(-10.0f, 0.0f, 10.0f, 40.0f);

	// Lower Foliage
	glColor3f(0.12f, 0.38f, 0.15f); // Dark side
	drawTriangle(-60.0f, 30.0f, 0.0f, 30.0f, 0.0f, 100.0f);
	glColor3f(0.20f, 0.52f, 0.22f); // Light side
	drawTriangle(0.0f, 30.0f, 60.0f, 30.0f, 0.0f, 100.0f);

	// Middle Foliage
	glColor3f(0.12f, 0.38f, 0.15f);
	drawTriangle(-50.0f, 70.0f, 0.0f, 70.0f, 0.0f, 140.0f);
	glColor3f(0.20f, 0.52f, 0.22f);
	drawTriangle(0.0f, 70.0f, 50.0f, 70.0f, 0.0f, 140.0f);

	// Top Foliage
	glColor3f(0.12f, 0.38f, 0.15f);
	drawTriangle(-38.0f, 110.0f, 0.0f, 110.0f, 0.0f, 175.0f);
	glColor3f(0.20f, 0.52f, 0.22f);
	drawTriangle(0.0f, 110.0f, 38.0f, 110.0f, 0.0f, 175.0f);

	glPopMatrix();
}

// Tiny dark-green tree specifically for hill edge contours
void drawTinyTree(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);

	// Dark Trunk
	glColor3f(0.10f, 0.08f, 0.05f);
	drawRect(-4.0f, 0.0f, 4.0f, 15.0f);

	// Dark forest green foliage
	glColor3f(0.08f, 0.22f, 0.10f); // Shadow side
	drawTriangle(-25.0f, 10.0f, 0.0f, 10.0f, 0.0f, 45.0f);
	glColor3f(0.12f, 0.28f, 0.14f); // Lit side
	drawTriangle(0.0f, 10.0f, 25.0f, 10.0f, 0.0f, 45.0f);

	glColor3f(0.08f, 0.22f, 0.10f);
	drawTriangle(-20.0f, 30.0f, 0.0f, 30.0f, 0.0f, 65.0f);
	glColor3f(0.12f, 0.28f, 0.14f);
	drawTriangle(0.0f, 30.0f, 20.0f, 30.0f, 0.0f, 65.0f);

	glColor3f(0.08f, 0.22f, 0.10f);
	drawTriangle(-15.0f, 50.0f, 0.0f, 50.0f, 0.0f, 80.0f);
	glColor3f(0.12f, 0.28f, 0.14f);
	drawTriangle(0.0f, 50.0f, 15.0f, 50.0f, 0.0f, 80.0f);

	glPopMatrix();
}

// Automatically snaps a tiny tree exactly onto the outer curve of a hill
// ellipse
void drawTreeOnHillEdge(float cx, float cy, float rx, float ry,
						float angleDegrees, float scale) {
	float rad = angleDegrees * (float)PI / 180.0f;
	float x = cx + rx * cosf(rad);
	float y = cy + ry * sinf(rad);
	drawTinyTree(x, y, scale);
}

// Green Mountain range
void drawMountains() {
	glPushMatrix();
	glTranslatef(0.0f, -120.0f, 0.0f);

	Color3 litGreen = {0.32f, 0.58f, 0.23f};
	Color3 shadowGreen = {0.16f, 0.34f, 0.14f};

	// --- 1. LEFT MOUNTAIN ---
	glColor3f(litGreen.r, litGreen.g, litGreen.b);
	drawTriangle(50.0f, 450.0f, 300.0f, 700.0f, 300.0f, 300.0f);
	glColor3f(shadowGreen.r, shadowGreen.g, shadowGreen.b);
	drawTriangle(300.0f, 400.0f, 300.0f, 700.0f, 600.0f, 300.0f);

	// --- 2. CENTER TALL MOUNTAIN ---
	glColor3f(litGreen.r, litGreen.g, litGreen.b);
	drawTriangle(200.0f, 300.0f, 570.0f, 800.0f, 570.0f, 300.0f);
	glColor3f(shadowGreen.r, shadowGreen.g, shadowGreen.b);
	drawTriangle(570.0f, 300.0f, 570.0f, 800.0f, 1150.0f, 300.0f);

	// --- 3. RIGHT MOUNTAIN ---
	glColor3f(litGreen.r, litGreen.g, litGreen.b);
	drawTriangle(800.0f, 300.0f, 1050.0f, 600.0f, 1050.0f, 300.0f);
	glColor3f(shadowGreen.r, shadowGreen.g, shadowGreen.b);
	drawTriangle(1050.0f, 300.0f, 1050.0f, 600.0f, 1450.0f, 300.0f);

	glPopMatrix();
}

// Rolling Green Hills with small trees planted along the hill edges
void drawHills() {
	// 1. Back Left Hill
	float h1_cx = 250.0f, h1_cy = 220.0f, h1_rx = 400.0f, h1_ry = 180.0f;
	drawHill(h1_cx, h1_cy, h1_rx, h1_ry, 0.25f, 0.48f, 0.20f);

	// Small trees on Back Left Hill edge

	drawTreeOnHillEdge(h1_cx, h1_cy, h1_rx, h1_ry, 35.0f, 0.22f);

	drawTreeOnHillEdge(h1_cx, h1_cy, h1_rx, h1_ry, 55.0f, 0.25f);
	drawTreeOnHillEdge(h1_cx, h1_cy, h1_rx, h1_ry, 75.0f, 0.20f);
	drawTreeOnHillEdge(h1_cx, h1_cy, h1_rx, h1_ry, 95.0f, 0.24f);
	drawTreeOnHillEdge(h1_cx, h1_cy, h1_rx, h1_ry, 115.0f, 0.21f);
	drawTreeOnHillEdge(h1_cx, h1_cy, h1_rx, h1_ry, 135.0f, 0.23f);
	drawTreeOnHillEdge(h1_cx, h1_cy, h1_rx, h1_ry, 125.0f, 0.23f);
	drawTreeOnHillEdge(h1_cx, h1_cy, h1_rx, h1_ry, 120.0f, 0.23f);
	drawTreeOnHillEdge(h1_cx, h1_cy, h1_rx, h1_ry, 118.0f, 0.23f);
	drawTreeOnHillEdge(h1_cx, h1_cy, h1_rx, h1_ry, 112.0f, 0.23f);
	drawTreeOnHillEdge(h1_cx, h1_cy, h1_rx, h1_ry, 128.0f, 0.23f);
	drawTreeOnHillEdge(h1_cx, h1_cy, h1_rx, h1_ry, 132.0f, 0.23f);

	// 2. Back Right Hill
	float h2_cx = 1350.0f, h2_cy = 220.0f, h2_rx = 450.0f, h2_ry = 190.0f;
	drawHill(h2_cx, h2_cy, h2_rx, h2_ry, 0.22f, 0.44f, 0.18f);

	// Small trees on Back Right Hill edge
	drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 60.0f, 0.25f);
	drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 63.0f, 0.25f);
	drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 66.0f, 0.25f);

	drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 70.0f, 0.25f);
	drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 73.0f, 0.25f);
	drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 76.0f, 0.25f);
	drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 79.0f, 0.25f);
	drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 82.0f, 0.25f);
	drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 85.0f, 0.25f);
	drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 88.0f, 0.25f);
	drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 91.0f, 0.25f);
	drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 94.0f, 0.25f);
	drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 97.0f, 0.25f);
	drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 100.0f, 0.25f);
	drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 103.0f, 0.25f);
	drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 106.0f, 0.25f);
	drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 109.0f, 0.25f);
	drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 112.0f, 0.25f);
	drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 115.0f, 0.25f);

	// 3. Front Center Rolling Hill
	float h3_cx = 800.0f, h3_cy = 220.0f, h3_rx = 500.0f, h3_ry = 200.0f;
	drawHill(h3_cx, h3_cy, h3_rx, h3_ry, 0.28f, 0.52f, 0.22f);

	// Small trees on Center Hill edge
	drawTreeOnHillEdge(h3_cx, h3_cy, h3_rx, h3_ry, 45.0f, 0.22f);
	drawTreeOnHillEdge(h3_cx, h3_cy, h3_rx, h3_ry, 65.0f, 0.26f);
	drawTreeOnHillEdge(h3_cx, h3_cy, h3_rx, h3_ry, 70.0f, 0.26f);
	drawTreeOnHillEdge(h3_cx, h3_cy, h3_rx, h3_ry, 68.0f, 0.26f);
	drawTreeOnHillEdge(h3_cx, h3_cy, h3_rx, h3_ry, 72.0f, 0.26f);
	drawTreeOnHillEdge(h3_cx, h3_cy, h3_rx, h3_ry, 74.0f, 0.26f);
	drawTreeOnHillEdge(h3_cx, h3_cy, h3_rx, h3_ry, 76.0f, 0.26f);
	drawTreeOnHillEdge(h3_cx, h3_cy, h3_rx, h3_ry, 78.0f, 0.26f);
	drawTreeOnHillEdge(h3_cx, h3_cy, h3_rx, h3_ry, 80.0f, 0.26f);
	drawTreeOnHillEdge(h3_cx, h3_cy, h3_rx, h3_ry, 85.0f, 0.24f);
	drawTreeOnHillEdge(h3_cx, h3_cy, h3_rx, h3_ry, 105.0f, 0.21f);
	drawTreeOnHillEdge(h3_cx, h3_cy, h3_rx, h3_ry, 125.0f, 0.23f);
	drawTreeOnHillEdge(h3_cx, h3_cy, h3_rx, h3_ry, 145.0f, 0.20f);
}

// gosun
void sun() {
	glColor3f(1.0f, 0.85f, 0.0f);
	drawCircle(520.0f, 520.0f, 40.0f, 40);
}

// Puffy cloud made from overlapping circles (shadow puffs + bright highlight
// puffs)
void drawCloud(float cx, float cy, float scale) {
	// Soft grey base puffs give the cloud volume/shadow
	glColor3f(0.85f, 0.87f, 0.90f);
	drawCircle(cx, cy - 5.0f * scale, 30.0f * scale, 30);
	drawCircle(cx + 30.0f * scale, cy, 36.0f * scale, 30);
	drawCircle(cx + 65.0f * scale, cy - 5.0f * scale, 26.0f * scale, 30);
	drawCircle(cx + 20.0f * scale, cy - 15.0f * scale, 24.0f * scale, 30);
	drawCircle(cx + 50.0f * scale, cy - 15.0f * scale, 22.0f * scale, 30);

	// Bright white highlight puffs on top for a fluffy look
	glColor3f(1.0f, 1.0f, 1.0f);
	drawCircle(cx, cy, 26.0f * scale, 30);
	drawCircle(cx + 30.0f * scale, cy + 8.0f * scale, 32.0f * scale, 30);
	drawCircle(cx + 62.0f * scale, cy, 22.0f * scale, 30);
}

// Scatters several clouds across the sky
void drawClouds() {
	static const float baseX[] = {120.0f, 430.0f,  700.0f,
								  980.0f, 1250.0f, 1480.0f};
	static const float baseY[] = {880.0f, 930.0f, 860.0f,
								  910.0f, 870.0f, 920.0f};
	static const float scale[] = {1.0f, 0.8f, 1.2f, 0.9f, 1.1f, 0.7f};
	static const int numClouds = 6;

	float wrapWidth = WIN_W + 400.0f;
	float dx = fmodf(cloudOffset, wrapWidth);

	for (int pass = 0; pass < 2; pass++) {
		glPushMatrix();
		glTranslatef(dx - pass * wrapWidth, 0.0f, 0.0f);
		for (int i = 0; i < numClouds; i++) {
			drawCloud(baseX[i], baseY[i], scale[i]);
		}
		glPopMatrix();
	}
}

// gonight
void drawNightOverlay() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Dark blue translucent veil over the whole scene
	glColor4f(0.02f, 0.02f, 0.15f, 0.55f);
	glBegin(GL_QUADS);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(WIN_W, 0.0f);
	glVertex2f(WIN_W, WIN_H);
	glVertex2f(0.0f, WIN_H);
	glEnd();

	// Stars scattered across the sky
	glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
	glPointSize(2.5f);
	glBegin(GL_POINTS);
	float starPositions[][2] = {
		{80, 900},	 {150, 850},  {230, 920},  {310, 870},	{400, 940},
		{480, 880},	 {560, 910},  {650, 860},  {730, 930},	{820, 890},
		{900, 950},	 {980, 870},  {1060, 920}, {1150, 880}, {1230, 940},
		{1310, 860}, {1400, 910}, {1480, 870}, {60, 780},	{200, 800},
		{350, 760},	 {520, 790},  {700, 770},  {880, 800},	{1050, 760},
		{1220, 790}, {1400, 770}, {1550, 800}, {120, 700},	{950, 700}};
	int numStars = sizeof(starPositions) / sizeof(starPositions[0]);
	for (int i = 0; i < numStars; i++) {
		glVertex2f(starPositions[i][0], starPositions[i][1]);
	}
	glEnd();

	// Moon, drawn over the sun's position
	glColor4f(0.92f, 0.92f, 0.85f, 1.0f);
	glPushMatrix();
	glTranslatef(550.0f, 250.0f, 0.0f);
	drawCircle(520.0f, 520.0f, 40.0f, 40);
	glPopMatrix();

	glDisable(GL_BLEND);
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

void scene1() {
	glPushMatrix();
	glTranslatef(550.0f, 250.0f, 0.0f);
	sun();
	glPopMatrix();

	drawClouds();

	drawMountains();
	drawHills();
	drawGroundAndPath();

	// ================= BACKGROUND TREES (Shifted up onto hills behind back row
	// huts) =================
	drawTree(160.0f, 285.0f, 0.70f);
	drawTree(400.0f, 280.0f, 0.75f);
	drawTree(640.0f, 285.0f, 0.70f);
	drawTree(870.0f, 280.0f, 0.75f);
	drawTree(1110.0f, 285.0f, 0.70f);
	drawTree(1340.0f, 280.0f, 0.75f);

	// ================= FURTHEST ROW (Deep background) =================
	glPushMatrix();
	glTranslatef(50.0f, 225.0f, 0.0f);
	glScalef(0.24f, 0.27f, 1.0f);
	hut3(HutColor7);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(280.0f, 222.0f, 0.0f);
	glScalef(0.28f, 0.25f, 1.0f);
	hut1(HutColor14);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(520.0f, 220.0f, 0.0f);
	glScalef(0.25f, 0.29f, 1.0f);
	hut5(HutColor2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(760.0f, 218.0f, 0.0f);
	glScalef(0.27f, 0.24f, 1.0f);
	hut2(HutColor19);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(990.0f, 215.0f, 0.0f);
	glScalef(0.23f, 0.26f, 1.0f);
	hut4(HutColor6);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1220.0f, 212.0f, 0.0f);
	glScalef(0.29f, 0.28f, 1.0f);
	hut1(HutColor11);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1450.0f, 210.0f, 0.0f);
	glScalef(0.26f, 0.23f, 1.0f);
	hut3(HutColor4);
	glPopMatrix();

	// ================= MIDDLE ROW =================
	glPushMatrix();
	glTranslatef(-10.0f, 190.0f, 0.0f);
	glScalef(0.24f, 0.28f, 1.0f);
	hut5(HutColor16);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(200.0f, 188.0f, 0.0f);
	glScalef(0.28f, 0.26f, 1.0f);
	hut2(HutColor9);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(420.0f, 185.0f, 0.0f);
	glScalef(0.25f, 0.24f, 1.0f);
	hut4(HutColor20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(650.0f, 182.0f, 0.0f);
	glScalef(0.27f, 0.29f, 1.0f);
	hut1(HutColor5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(880.0f, 180.0f, 0.0f);
	glScalef(0.23f, 0.25f, 1.0f);
	hut3(HutColor18);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1100.0f, 178.0f, 0.0f);
	glScalef(0.26f, 0.27f, 1.0f);
	hut5(HutColor12);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1340.0f, 175.0f, 0.0f);
	glScalef(0.29f, 0.24f, 1.0f);
	hut2(HutColor3);
	glPopMatrix();

	// ================= FRONT ROW (Just behind path) =================
	glPushMatrix();
	glTranslatef(100.0f, 150.0f, 0.0f);
	glScalef(0.24f, 0.26f, 1.0f);
	hut4(HutColor15);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(340.0f, 145.0f, 0.0f);
	glScalef(0.28f, 0.23f, 1.0f);
	hut1(HutColor8);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(580.0f, 140.0f, 0.0f);
	glScalef(0.25f, 0.28f, 1.0f);
	hut3(HutColor17);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(820.0f, 135.0f, 0.0f);
	glScalef(0.27f, 0.25f, 1.0f);
	hut5(HutColor1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1050.0f, 132.0f, 0.0f);
	glScalef(0.23f, 0.29f, 1.0f);
	hut2(HutColor13);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1280.0f, 130.0f, 0.0f);
	glScalef(0.26f, 0.24f, 1.0f);
	hut4(HutColor10);
	glPopMatrix();

	// ================= FOREGROUND TREES (Moved to outer margins to frame the
	// scene) =================
	drawTree(40.0f, 100.0f, 1.2f);
	drawTree(1560.0f, 100.0f, 1.2f);
}

void renderScene1() {
	if (isNightMode) {
		glClearColor(0.04f, 0.05f, 0.18f, 1.0f);
	} else {
		glClearColor(0.58f, 0.78f, 0.92f, 1.0f);
	}
	glClear(GL_COLOR_BUFFER_BIT);
	scene1();
	if (isNightMode) {
		drawNightOverlay();
	}
}

//---------------------------END OF SCENE1-------------------------

//---------------------------SCENE2-------------------------
// ------------------- Bombed-Village Helper Drawings -------------------
//
// Scene 2 reuses Scene 1's sky/sun/cloud/mountain/hill helpers but adds a
// large library of small, reusable, parameterized drawing functions for
// destruction: burn marks, craters, bricks, rubble, wood debris, roof
// debris, metal/glass shards, broken houses, burnt trees, fire and smoke.
// Every function below accepts position/scale (and color where sensible)
// so the same visual element can be reused many times with variation.

// ---------------------- Colors ----------------------
const Color3 BurntBlack = {0.05f, 0.05f, 0.05f};
const Color3 Charcoal = {0.14f, 0.13f, 0.12f};
const Color3 CharcoalLight = {0.26f, 0.24f, 0.22f};
const Color3 AshGray = {0.55f, 0.53f, 0.50f};
const Color3 BrickRed = {0.55f, 0.20f, 0.15f};
const Color3 BrickBrown = {0.45f, 0.28f, 0.16f};
const Color3 BrickOrange = {0.62f, 0.34f, 0.18f};
const Color3 BurntWood = {0.22f, 0.15f, 0.09f};
const Color3 WoodBrown = {0.42f, 0.27f, 0.14f};
const Color3 WoodBrownLight = {0.55f, 0.38f, 0.20f};
const Color3 MetalGray = {0.45f, 0.48f, 0.50f};
const Color3 GlassBlue = {0.42f, 0.58f, 0.66f};
const Color3 RoofDarkRed = {0.42f, 0.16f, 0.14f};
const Color3 RoofGray = {0.34f, 0.34f, 0.36f};
const Color3 DustyGreen = {0.34f, 0.42f, 0.24f};
const Color3 DirtBrown = {0.40f, 0.30f, 0.20f};
const Color3 DamagedWall = {0.58f, 0.52f, 0.44f};
const Color3 DamagedWallDark = {0.40f, 0.36f, 0.30f};
const Color3 EmberYellow = {1.0f, 0.80f, 0.20f};
const Color3 EmberOrange = {0.95f, 0.45f, 0.10f};
const Color3 EmberRed = {0.80f, 0.15f, 0.10f};

// ---------------------- Generic irregular blob helper ----------------------
// Draws an organic, jagged polygon (used for burn marks, craters and rubble)
// instead of a perfect circle. `seed` shifts the wobble pattern so repeated
// calls with different seeds don't all look identical.
void drawIrregularBlob(float cx, float cy, float rx, float ry, int segments,
					   float seed, Color3 color) {
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_POLYGON);
	for (int i = 0; i < segments; i++) {
		float angle = 2.0f * (float)PI * i / segments;
		float wobble = 1.0f + 0.28f * sinf(angle * 3.0f + seed) +
					   0.15f * cosf(angle * 5.0f + seed * 1.7f);
		glVertex2f(cx + rx * wobble * cosf(angle),
				   cy + ry * wobble * sinf(angle));
	}
	glEnd();
}

// ---------------------- Burn Marks ----------------------
void drawBurnMarkSmall(float x, float y, float scale, Color3 color) {
	drawIrregularBlob(x, y, 35.0f * scale, 16.0f * scale, 10,
					  x * 0.13f + y * 0.07f, color);
}

void drawBurnMarkMedium(float x, float y, float scale, Color3 color) {
	drawIrregularBlob(x, y, 65.0f * scale, 28.0f * scale, 12,
					  x * 0.09f + y * 0.11f, color);
	Color3 inner = {color.r * 1.4f, color.g * 1.4f, color.b * 1.4f};
	drawIrregularBlob(x, y, 36.0f * scale, 15.0f * scale, 10,
					  x * 0.2f + y * 0.05f, inner);
}

void drawBurnMarkLarge(float x, float y, float scale, Color3 color) {
	drawIrregularBlob(x, y, 110.0f * scale, 46.0f * scale, 14,
					  x * 0.05f + y * 0.13f, color);
	Color3 mid = {color.r * 1.3f, color.g * 1.3f, color.b * 1.3f};
	drawIrregularBlob(x, y, 68.0f * scale, 28.0f * scale, 12,
					  x * 0.17f + y * 0.08f, mid);
	Color3 inner = {color.r * 1.6f, color.g * 1.6f, color.b * 1.6f};
	drawIrregularBlob(x, y, 34.0f * scale, 14.0f * scale, 10,
					  x * 0.24f + y * 0.02f, inner);
}

// ---------------------- Craters ----------------------
void drawCraterSmall(float x, float y, float scale) {
	// Lighter disturbed-earth rim, then a dark pit on top
	drawIrregularBlob(x, y, 26.0f * scale, 18.0f * scale, 10, x * 0.31f,
					  DirtBrown);
	drawIrregularBlob(x, y, 14.0f * scale, 10.0f * scale, 10, x * 0.19f + 3.0f,
					  Charcoal);
}

void drawCraterLarge(float x, float y, float scale) {
	Color3 rim = {DirtBrown.r * 1.15f, DirtBrown.g * 1.1f, DirtBrown.b * 1.1f};
	drawIrregularBlob(x, y, 55.0f * scale, 38.0f * scale, 14, x * 0.11f, rim);
	drawIrregularBlob(x, y, 38.0f * scale, 25.0f * scale, 12, x * 0.22f + 2.0f,
					  DirtBrown);
	drawIrregularBlob(x, y, 20.0f * scale, 12.0f * scale, 10, x * 0.33f + 4.0f,
					  BurntBlack);
}

// ---------------------- Bricks ----------------------
void drawBrick(float x, float y, float w, float h, float rotation,
			   Color3 color) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glRotatef(rotation, 0.0f, 0.0f, 1.0f);
	glColor3f(color.r, color.g, color.b);
	drawRect(-w * 0.5f, -h * 0.5f, w * 0.5f, h * 0.5f);
	glColor3f(color.r * 0.55f, color.g * 0.55f, color.b * 0.55f);
	glLineWidth(1.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-w * 0.5f, -h * 0.5f);
	glVertex2f(w * 0.5f, -h * 0.5f);
	glVertex2f(w * 0.5f, h * 0.5f);
	glVertex2f(-w * 0.5f, h * 0.5f);
	glEnd();
	glPopMatrix();
}

// A small heap of a few loosely scattered bricks
void drawBrickPileSmall(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);
	drawBrick(-14.0f, 4.0f, 26.0f, 12.0f, -6.0f, BrickRed);
	drawBrick(10.0f, 2.0f, 24.0f, 11.0f, 8.0f, BrickBrown);
	drawBrick(-2.0f, 14.0f, 22.0f, 10.0f, 3.0f, BrickOrange);
	drawBrick(20.0f, 12.0f, 20.0f, 10.0f, -10.0f, BrickRed);
	glPopMatrix();
}

// A larger, stacked pile of bricks with multiple layers
void drawBrickPileLarge(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);
	// Base layer, scattered
	drawBrick(-40.0f, 4.0f, 30.0f, 13.0f, -4.0f, BrickBrown);
	drawBrick(-8.0f, 3.0f, 32.0f, 13.0f, 5.0f, BrickRed);
	drawBrick(26.0f, 5.0f, 28.0f, 12.0f, -8.0f, BrickOrange);
	drawBrick(52.0f, 2.0f, 26.0f, 12.0f, 10.0f, BrickRed);
	// Second, partially stacked layer
	drawBrick(-22.0f, 18.0f, 28.0f, 12.0f, 6.0f, BrickRed);
	drawBrick(6.0f, 20.0f, 26.0f, 12.0f, -5.0f, BrickBrown);
	drawBrick(34.0f, 17.0f, 24.0f, 11.0f, 9.0f, BrickOrange);
	// Top, loosely scattered pieces
	drawBrick(-6.0f, 34.0f, 22.0f, 10.0f, -12.0f, BrickBrown);
	drawBrick(18.0f, 33.0f, 20.0f, 9.0f, 4.0f, BrickRed);
	glPopMatrix();
}

// ---------------------- Rubble ----------------------
void drawRubblePile(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);
	drawIrregularBlob(0.0f, 6.0f, 30.0f, 14.0f, 10, x * 0.1f, AshGray);
	drawIrregularBlob(-14.0f, 4.0f, 14.0f, 9.0f, 8, x * 0.2f + 1.0f,
					  CharcoalLight);
	drawIrregularBlob(16.0f, 10.0f, 12.0f, 8.0f, 8, x * 0.15f + 2.0f,
					  DirtBrown);
	drawBrick(2.0f, 14.0f, 16.0f, 8.0f, 12.0f, BrickBrown);
	drawBrick(-10.0f, 18.0f, 14.0f, 7.0f, -8.0f, BrickRed);
	glPopMatrix();
}

// ---------------------- Wooden Debris ----------------------
void drawWoodPlank(float x, float y, float length, float thickness,
				   float rotation, Color3 color) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glRotatef(rotation, 0.0f, 0.0f, 1.0f);
	glColor3f(color.r, color.g, color.b);
	drawRect(-length * 0.5f, -thickness * 0.5f, length * 0.5f,
			 thickness * 0.5f);
	glColor3f(color.r * 0.65f, color.g * 0.65f, color.b * 0.65f);
	glLineWidth(1.0f);
	glBegin(GL_LINES);
	glVertex2f(-length * 0.2f, -thickness * 0.5f);
	glVertex2f(-length * 0.2f, thickness * 0.5f);
	glVertex2f(length * 0.25f, -thickness * 0.5f);
	glVertex2f(length * 0.25f, thickness * 0.5f);
	glEnd();
	glPopMatrix();
}

// A wood beam with a jagged, snapped-off end
void drawBrokenBeam(float x, float y, float length, float thickness,
					float rotation, Color3 color) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glRotatef(rotation, 0.0f, 0.0f, 1.0f);
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_POLYGON);
	glVertex2f(-length * 0.5f, -thickness * 0.5f);
	glVertex2f(length * 0.25f, -thickness * 0.5f);
	glVertex2f(length * 0.5f, 0.0f);
	glVertex2f(length * 0.3f, thickness * 0.5f);
	glVertex2f(-length * 0.5f, thickness * 0.5f);
	glEnd();
	glColor3f(color.r * 0.5f, color.g * 0.5f, color.b * 0.5f);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-length * 0.5f, -thickness * 0.5f);
	glVertex2f(length * 0.25f, -thickness * 0.5f);
	glVertex2f(length * 0.5f, 0.0f);
	glVertex2f(length * 0.3f, thickness * 0.5f);
	glVertex2f(-length * 0.5f, thickness * 0.5f);
	glEnd();
	glPopMatrix();
}

// A crossed cluster of planks/beams (e.g. a fallen roof frame)
void drawWoodDebrisPile(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);
	drawWoodPlank(0.0f, 4.0f, 70.0f, 10.0f, 12.0f, WoodBrown);
	drawBrokenBeam(20.0f, 10.0f, 55.0f, 9.0f, -20.0f, BurntWood);
	drawWoodPlank(-15.0f, 14.0f, 50.0f, 8.0f, 35.0f, WoodBrownLight);
	drawBrokenBeam(10.0f, 20.0f, 45.0f, 8.0f, 100.0f, WoodBrown);
	glPopMatrix();
}

// ---------------------- Roof Debris ----------------------
void drawBrokenRoofPiece(float x, float y, float w, float h, float rotation,
						 Color3 color) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glRotatef(rotation, 0.0f, 0.0f, 1.0f);
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_QUADS);
	glVertex2f(-w * 0.5f, -h * 0.5f);
	glVertex2f(w * 0.5f, -h * 0.35f);
	glVertex2f(w * 0.4f, h * 0.5f);
	glVertex2f(-w * 0.45f, h * 0.4f);
	glEnd();
	glColor3f(color.r * 0.6f, color.g * 0.6f, color.b * 0.6f);
	glBegin(GL_LINES);
	glVertex2f(-w * 0.2f, -h * 0.4f);
	glVertex2f(-w * 0.1f, h * 0.4f);
	glVertex2f(w * 0.15f, -h * 0.4f);
	glVertex2f(w * 0.2f, h * 0.4f);
	glEnd();
	glPopMatrix();
}

// A cluster of broken roof shingle pieces in mixed colors/orientations
void drawRoofDebris(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);
	drawBrokenRoofPiece(-10.0f, 6.0f, 55.0f, 30.0f, 8.0f, RoofDarkRed);
	drawBrokenRoofPiece(28.0f, 4.0f, 40.0f, 24.0f, -15.0f, RoofGray);
	drawBrokenRoofPiece(5.0f, 20.0f, 35.0f, 20.0f, 30.0f, BurntWood);
	glPopMatrix();
}

// ---------------------- Metal / Glass Debris ----------------------
void drawMetalShard(float x, float y, float size, float rotation,
					Color3 color) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glRotatef(rotation, 0.0f, 0.0f, 1.0f);
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_QUADS);
	glVertex2f(-size * 0.5f, -size * 0.15f);
	glVertex2f(size * 0.5f, -size * 0.25f);
	glVertex2f(size * 0.4f, size * 0.2f);
	glVertex2f(-size * 0.35f, size * 0.15f);
	glEnd();
	glColor3f(fminf(color.r * 1.3f, 1.0f), fminf(color.g * 1.3f, 1.0f),
			  fminf(color.b * 1.3f, 1.0f));
	glLineWidth(1.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-size * 0.5f, -size * 0.15f);
	glVertex2f(size * 0.5f, -size * 0.25f);
	glVertex2f(size * 0.4f, size * 0.2f);
	glVertex2f(-size * 0.35f, size * 0.15f);
	glEnd();
	glPopMatrix();
}

// Small translucent shards of broken glass/window
void drawBrokenGlass(float x, float y, float size, float rotation) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glRotatef(rotation, 0.0f, 0.0f, 1.0f);
	glColor4f(GlassBlue.r, GlassBlue.g, GlassBlue.b, 0.55f);
	glBegin(GL_TRIANGLES);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(size, size * 0.3f);
	glVertex2f(size * 0.3f, size * 0.6f);
	glEnd();
	glColor4f(GlassBlue.r, GlassBlue.g, GlassBlue.b, 0.4f);
	glBegin(GL_TRIANGLES);
	glVertex2f(size * 0.2f, -size * 0.1f);
	glVertex2f(size * 0.7f, -size * 0.35f);
	glVertex2f(size * 0.55f, size * 0.1f);
	glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);
}

// ---------------------- Broken Houses ----------------------
// A half-collapsed cottage: one jagged wall remains, roof leans against it.
void drawBrokenHouse1(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);

	// Rubble and bricks scattered at the base
	drawBrickPileLarge(-45.0f, 8.0f, 0.9f);
	drawBrickPileSmall(55.0f, 6.0f, 0.8f);
	drawRubblePile(10.0f, 5.0f, 0.7f);

	// Remaining jagged wall section
	// glColor3f(DamagedWall.r, DamagedWall.g, DamagedWall.b);
	// glBegin(GL_POLYGON);
	// glVertex2f(-70.0f, 20.0f);
	// glVertex2f(-70.0f, 150.0f);
	// glVertex2f(-40.0f, 190.0f);
	// glVertex2f(-10.0f, 145.0f);
	// glVertex2f(-10.0f, 20.0f);
	// glEnd();

	// // Broken window hole in the wall
	// glColor3f(BurntBlack.r, BurntBlack.g, BurntBlack.b);
	// drawRect(-58.0f, 80.0f, -32.0f, 120.0f);

	// Leaning, partially collapsed roof piece resting against the wall
	drawBrokenRoofPiece(30.0f, 90.0f, 150.0f, 75.0f, -25.0f, RoofDarkRed);

	// Chimney stub with a broken top edge
	glColor3f(CharcoalLight.r, CharcoalLight.g, CharcoalLight.b);
	drawRect(0.0f, 20.0f, 26.0f, 110.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(0.0f, 110.0f);
	glVertex2f(26.0f, 110.0f);
	glVertex2f(14.0f, 128.0f);
	glEnd();

	// Wooden beams strewn across the rubble
	drawBrokenBeam(15.0f, 15.0f, 95.0f, 10.0f, 12.0f, WoodBrown);
	drawWoodPlank(-25.0f, 10.0f, 70.0f, 8.0f, -10.0f, BurntWood);

	glPopMatrix();
}

// A wider corner ruin: two adjoining walls survive as a jagged skyline,
// with exposed beams and larger rubble/metal/glass debris.
void drawBrokenHouse2(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);

	// Large base rubble
	drawBrickPileLarge(-25.0f, 8.0f, 1.1f);
	drawRubblePile(55.0f, 6.0f, 1.0f);
	drawWoodDebrisPile(-75.0f, 5.0f, 0.85f);

	// // Jagged broken wall silhouette (remains of two adjoining walls)
	// glColor3f(DamagedWallDark.r, DamagedWallDark.g, DamagedWallDark.b);
	// glBegin(GL_POLYGON);
	// glVertex2f(-95.0f, 20.0f);
	// glVertex2f(-95.0f, 190.0f);
	// glVertex2f(-65.0f, 225.0f);
	// glVertex2f(-40.0f, 165.0f);
	// glVertex2f(-10.0f, 205.0f);
	// glVertex2f(20.0f, 135.0f);
	// glVertex2f(50.0f, 180.0f);
	// glVertex2f(85.0f, 150.0f);
	// glVertex2f(95.0f, 20.0f);
	// glEnd();

	// Broken window holes
	// glColor3f(BurntBlack.r, BurntBlack.g, BurntBlack.b);
	// drawRect(-85.0f, 70.0f, -60.0f, 110.0f);
	// drawRect(0.0f, 60.0f, 22.0f, 95.0f);

	// Exposed wooden support beams sticking out from the broken top
	drawWoodPlank(-55.0f, 175.0f, 55.0f, 8.0f, -30.0f, WoodBrownLight);
	drawBrokenBeam(35.0f, 155.0f, 50.0f, 8.0f, 40.0f, WoodBrown);

	// Metal and glass debris near the base
	drawMetalShard(65.0f, 16.0f, 34.0f, 20.0f, MetalGray);
	drawBrokenGlass(75.0f, 22.0f, 18.0f, -12.0f);

	glPopMatrix();
}

// A simple, cheaper ruin (rubble mound + wall stub) for background/small use
void drawBrokenHouse3(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);

	// Low mound of rubble
	drawIrregularBlob(0.0f, 14.0f, 65.0f, 26.0f, 10, 2.3f, AshGray);
	drawIrregularBlob(-10.0f, 10.0f, 30.0f, 14.0f, 8, 4.1f, DirtBrown);

	// Short stub of a wall
	glColor3f(DamagedWallDark.r, DamagedWallDark.g, DamagedWallDark.b);
	glBegin(GL_POLYGON);
	glVertex2f(-15.0f, 15.0f);
	glVertex2f(-15.0f, 60.0f);
	glVertex2f(5.0f, 78.0f);
	glVertex2f(25.0f, 55.0f);
	glVertex2f(25.0f, 15.0f);
	glEnd();

	// Flat roof fragment lying atop the mound
	drawBrokenRoofPiece(20.0f, 45.0f, 55.0f, 24.0f, 12.0f, RoofGray);

	// A couple of loose bricks
	drawBrick(-35.0f, 12.0f, 16.0f, 8.0f, 12.0f, BrickBrown);
	drawBrick(38.0f, 10.0f, 14.0f, 7.0f, -8.0f, BrickRed);

	glPopMatrix();
}

// ---------------------- Burnt Trees (3 distinct designs)
// ---------------------- Mostly dead pine: dark trunk, a few bare branches,
// almost no foliage.
void drawBurntTree1(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);

	glColor3f(BurntWood.r * 0.6f, BurntWood.g * 0.6f, BurntWood.b * 0.6f);
	drawRect(-7.0f, 0.0f, 7.0f, 130.0f);

	glColor3f(BurntBlack.r, BurntBlack.g, BurntBlack.b);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 100.0f);
	glVertex2f(-35.0f, 130.0f);
	glVertex2f(0.0f, 90.0f);
	glVertex2f(30.0f, 115.0f);
	glVertex2f(0.0f, 70.0f);
	glVertex2f(-28.0f, 95.0f);
	glVertex2f(0.0f, 55.0f);
	glVertex2f(22.0f, 75.0f);
	glVertex2f(0.0f, 130.0f);
	glVertex2f(8.0f, 150.0f);
	glEnd();

	// A couple of sparse burnt foliage tufts near the base
	glColor3f(0.10f, 0.14f, 0.06f);
	drawTriangle(-18.0f, 10.0f, 0.0f, 10.0f, 0.0f, 35.0f);

	glPopMatrix();
}

// Partially burned pine: dark trunk, patchy brown/green foliage, broken top.
void drawBurntTree2(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);

	glColor3f(0.20f, 0.13f, 0.08f);
	drawRect(-9.0f, 0.0f, 9.0f, 60.0f);

	// Lower remaining foliage (patchy dark green/brown)
	glColor3f(0.16f, 0.28f, 0.10f);
	drawTriangle(-45.0f, 40.0f, 0.0f, 40.0f, 0.0f, 95.0f);
	glColor3f(0.34f, 0.26f, 0.12f);
	drawTriangle(0.0f, 40.0f, 40.0f, 40.0f, 0.0f, 90.0f);

	// Middle sparse foliage
	glColor3f(0.14f, 0.22f, 0.09f);
	drawTriangle(-30.0f, 80.0f, 0.0f, 80.0f, 0.0f, 125.0f);

	// Broken top: a snapped trunk stub instead of a pointed crown
	glColor3f(0.18f, 0.12f, 0.08f);
	drawRect(-6.0f, 100.0f, 6.0f, 140.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(-6.0f, 140.0f);
	glVertex2f(6.0f, 140.0f);
	glVertex2f(14.0f, 150.0f);
	glEnd();

	glColor3f(BurntBlack.r, BurntBlack.g, BurntBlack.b);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 110.0f);
	glVertex2f(-25.0f, 130.0f);
	glEnd();

	glPopMatrix();
}

// Heavily destroyed tree: mostly black trunk, broken branch stubs, and a
// small amount of burnt foliage clinging near the bottom.
void drawBurntTree3(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);

	glColor3f(BurntBlack.r, BurntBlack.g, BurntBlack.b);
	drawRect(-6.0f, 0.0f, 6.0f, 85.0f);

	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 60.0f);
	glVertex2f(-20.0f, 72.0f);
	glVertex2f(0.0f, 45.0f);
	glVertex2f(18.0f, 55.0f);
	glVertex2f(0.0f, 30.0f);
	glVertex2f(-15.0f, 40.0f);
	glVertex2f(0.0f, 85.0f);
	glVertex2f(6.0f, 100.0f);
	glEnd();

	// Small burnt foliage clump near the bottom only
	glColor3f(0.09f, 0.13f, 0.05f);
	drawTriangle(-14.0f, 5.0f, 14.0f, 5.0f, 0.0f, 28.0f);

	glPopMatrix();
}

// ---------------------- Fire ----------------------
// Layered flame using firePhase for a subtle flicker (no complex animation).
void drawFire(float x, float y, float scale) {
	float flick = sinf(firePhase) * 0.12f + 1.0f;
	float flick2 = cosf(firePhase * 1.3f) * 0.10f + 1.0f;

	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);

	// Outer red flame
	glColor3f(EmberRed.r, EmberRed.g, EmberRed.b);
	drawTriangle(-16.0f, 0.0f, 16.0f, 0.0f, 0.0f, 55.0f * flick);
	drawTriangle(-10.0f, 0.0f, 6.0f, 0.0f, -4.0f, 40.0f * flick2);

	// Middle orange flame
	glColor3f(EmberOrange.r, EmberOrange.g, EmberOrange.b);
	drawTriangle(-10.0f, 0.0f, 10.0f, 0.0f, 0.0f, 40.0f * flick2);

	// Inner yellow flame
	glColor3f(EmberYellow.r, EmberYellow.g, EmberYellow.b);
	drawTriangle(-5.0f, 0.0f, 5.0f, 0.0f, 0.0f, 24.0f * flick);

	glPopMatrix();
}

// ---------------------- Smoke ----------------------
// Overlapping translucent puffs that rise and gently sway using smokeOffset.
void drawSmoke(float x, float y, float scale) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (int i = 0; i < 4; i++) {
		float t = (float)i / 4.0f;
		float rise = fmodf(smokeOffset * 0.6f + t * 60.0f, 140.0f);
		float sway = sinf(smokeOffset * 0.04f + i * 1.7f) * 10.0f;
		float puffY = y + rise * scale;
		float puffX = x + (sway + t * 6.0f) * scale;
		float alpha = 0.30f * (1.0f - rise / 140.0f);
		float radius = (16.0f + i * 6.0f) * scale;
		glColor4f(0.30f, 0.30f, 0.30f, alpha);
		drawCircle(puffX, puffY, radius, 16);
	}

	glDisable(GL_BLEND);
}

// ---------------------- Ground ----------------------
// Damaged ground: muted grass, a scorched dirt swath instead of a tidy path,
// and a few darker trampled patches. Burn marks/craters/debris are layered
// on top of this in scene2().
void drawScene2Ground() {
	glColor3f(DustyGreen.r, DustyGreen.g, DustyGreen.b);
	glBegin(GL_QUADS);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(WIN_W, 0.0f);
	glVertex2f(WIN_W, WIN_H * 0.33f);
	glVertex2f(0.0f, WIN_H * 0.33f);
	glEnd();

	// Broad dirt/scorched swath replacing the peaceful path
	glColor3f(DirtBrown.r, DirtBrown.g, DirtBrown.b);
	glBegin(GL_QUAD_STRIP);
	glVertex2f(0.0f, 55.0f);
	glVertex2f(0.0f, 105.0f);
	glVertex2f(300.0f, 65.0f);
	glVertex2f(300.0f, 120.0f);
	glVertex2f(650.0f, 50.0f);
	glVertex2f(650.0f, 100.0f);
	glVertex2f(1000.0f, 70.0f);
	glVertex2f(1000.0f, 125.0f);
	glVertex2f(1350.0f, 55.0f);
	glVertex2f(1350.0f, 105.0f);
	glVertex2f((float)WIN_W, 60.0f);
	glVertex2f((float)WIN_W, 110.0f);
	glEnd();

	// Darker, trampled/damaged patches scattered on the grass
	glColor3f(DirtBrown.r * 0.85f, DirtBrown.g * 0.85f, DirtBrown.b * 0.85f);
	drawCircle(180.0f, 200.0f, 55.0f, 20);
	drawCircle(980.0f, 230.0f, 70.0f, 20);
	drawCircle(1400.0f, 170.0f, 45.0f, 20);
}

// A translucent brownish veil laid over the background (mountains/hills/sky)
// to give Scene 2 a hazy, dust-choked atmosphere without altering Scene 1's
// drawMountains()/drawHills() colors directly.
void drawDustHaze() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.45f, 0.40f, 0.32f, 0.28f);
	glBegin(GL_QUADS);
	glVertex2f(0.0f, 250.0f);
	glVertex2f((float)WIN_W, 250.0f);
	glVertex2f((float)WIN_W, (float)WIN_H);
	glVertex2f(0.0f, (float)WIN_H);
	glEnd();
	glDisable(GL_BLEND);
}

// ---------------------- Scene2 composition ----------------------
void scene2() {
	// 1. Sun (dimmed/hazed by the dust overlay drawn right after it)
	glPushMatrix();
	glTranslatef(550.0f, 250.0f, 0.0f);
	sun();
	glPopMatrix();

	// 2. Clouds
	drawClouds();

	// 3. Mountains + background forest, reused from Scene 1
	drawMountains();
	drawHills();

	// Dust haze desaturates the background so it reads as devastated/smoky
	drawDustHaze();

	// 4. Ground (damaged grass + scorched dirt swath)
	drawScene2Ground();

	// 5. Burn marks / craters directly on the ground
	drawBurnMarkLarge(430.0f, 90.0f, 1.0f, Charcoal);
	drawBurnMarkLarge(950.0f, 70.0f, 1.1f, BurntBlack);
	drawBurnMarkMedium(180.0f, 60.0f, 0.9f, Charcoal);
	drawBurnMarkMedium(1250.0f, 55.0f, 1.0f, CharcoalLight);
	drawBurnMarkSmall(700.0f, 40.0f, 1.0f, Charcoal);
	drawBurnMarkSmall(1450.0f, 90.0f, 0.9f, BurntBlack);
	drawCraterSmall(280.0f, 45.0f, 1.0f);
	drawCraterSmall(1120.0f, 100.0f, 0.9f);
	drawCraterLarge(560.0f, 110.0f, 1.0f);
	drawCraterSmall(850.0f, 30.0f, 0.8f);

	// 6. Distant destroyed houses + burnt trees (small, background row)
	drawBrokenHouse3(120.0f, 222.0f, 0.55f);
	drawBrokenHouse1(430.0f, 218.0f, 0.45f);
	drawBrokenHouse3(760.0f, 216.0f, 0.50f);
	drawBrokenHouse1(1080.0f, 214.0f, 0.42f);
	drawBrokenHouse3(1380.0f, 212.0f, 0.55f);

	drawBurntTree2(200.0f, 280.0f, 0.55f);
	drawBurntTree1(500.0f, 278.0f, 0.5f);
	drawBurntTree3(860.0f, 280.0f, 0.5f);
	drawBurntTree2(1180.0f, 278.0f, 0.55f);
	drawBurntTree1(1420.0f, 280.0f, 0.5f);

	// 7. Middle-ground destroyed houses
	drawBrokenHouse2(260.0f, 175.0f, 0.7f);
	drawBrokenHouse1(650.0f, 165.0f, 0.75f);
	drawBrokenHouse3(1020.0f, 170.0f, 0.65f);
	drawBrokenHouse2(1330.0f, 160.0f, 0.7f);

	// 8. Rubble scattered across the ground
	drawRubblePile(120.0f, 55.0f, 1.0f);
	drawRubblePile(1500.0f, 60.0f, 1.0f);
	drawRubblePile(1000.0f, 45.0f, 0.8f);

	// 9. Bricks / brick piles
	drawBrickPileLarge(500.0f, 40.0f, 1.0f);
	drawBrickPileSmall(870.0f, 35.0f, 1.0f);
	drawBrickPileSmall(1180.0f, 50.0f, 0.9f);
	drawBrick(340.0f, 30.0f, 26.0f, 12.0f, 18.0f, BrickRed);
	drawBrick(760.0f, 25.0f, 22.0f, 10.0f, -10.0f, BrickBrown);

	// 10. Broken wood / roof debris
	drawWoodDebrisPile(230.0f, 40.0f, 1.0f);
	drawWoodDebrisPile(1050.0f, 35.0f, 0.9f);
	drawWoodPlank(620.0f, 30.0f, 90.0f, 12.0f, 15.0f, WoodBrown);
	drawBrokenBeam(1350.0f, 28.0f, 80.0f, 11.0f, -25.0f, BurntWood);
	drawRoofDebris(390.0f, 40.0f, 1.0f);
	drawRoofDebris(1180.0f, 45.0f, 0.9f);

	// 11. Metal / glass debris
	drawMetalShard(680.0f, 32.0f, 34.0f, 20.0f, MetalGray);
	drawBrokenGlass(710.0f, 40.0f, 22.0f, -15.0f);
	drawMetalShard(1420.0f, 35.0f, 30.0f, -10.0f, MetalGray);

	// 12. Large, prominent destroyed structure in the foreground
	// drawBrokenHouse2(800.0f, 100.0f, 1.15f);

	// 13. Foreground burnt trees framing the scene
	drawBurntTree1(60.0f, 90.0f, 1.1f);
	drawBurntTree3(340.0f, 85.0f, 1.0f);
	drawBurntTree2(1250.0f, 90.0f, 1.1f);
	drawBurntTree3(1540.0f, 85.0f, 1.05f);

	// 14. Fire near the ruins
	drawFire(760.0f, 95.0f, 1.0f);
	drawFire(650.0f, 150.0f, 0.7f);

	// 15. Smoke rising from the ruins/fires
	drawSmoke(760.0f, 130.0f, 1.0f);
	drawSmoke(650.0f, 190.0f, 0.8f);
	drawSmoke(260.0f, 210.0f, 0.7f);
	drawSmoke(1330.0f, 195.0f, 0.75f);
}

// ---------------------- Scene2 rendering ----------------------
void renderScene2() {
	// Dusty, smoke-dimmed sky for the bombed scene
	glClearColor(0.45f, 0.38f, 0.32f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	scene2();
}

void display() {
	renderScene2();
	glFlush();
};

void update(int value) {
	cloudOffset += CLOUD_SPEED;
	firePhase += FIRE_SPEED;
	smokeOffset += SMOKE_DRIFT_SPEED;
	glutPostRedisplay();
	glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
	if (key == 'n' || key == 'N') {
		isNightMode = true;
		glutPostRedisplay();
	} else if (key == 'd' || key == 'D') {
		isNightMode = false;
		glutPostRedisplay();
	}
}

void init() {
	// Soft sunny sky blue matching the photo
	glClearColor(0.58f, 0.78f, 0.92f, 1.0f);
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
	glutKeyboardFunc(keyboard);
	glutTimerFunc(16, update, 0);
	glutMainLoop();
	return 0;
}
