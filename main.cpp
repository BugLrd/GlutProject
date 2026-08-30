

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

// ============================================================
// BOMBING ANIMATION STATE
// ============================================================
int currentScene = 1;
bool bombingStarted = false;
bool flashActive = false;
float planeX = -350.0f;
float planeY = 780.0f;
const float PLANE_SPEED = 4.0f;
const int NUM_BOMBS = 5;
float bombX[NUM_BOMBS] = {0};
float bombY[NUM_BOMBS] = {0};
bool bombDropped[NUM_BOMBS] = {false, false, false, false, false};
bool bombHit[NUM_BOMBS] = {false, false, false, false, false};
float bombDropPosition[NUM_BOMBS] = {200.0f, 500.0f, 800.0f, 1100.0f, 1400.0f};
float flashTimer = 0.0f;
float transitionTimer = 0.0f;

// Simple, non-graphic plane crash animation for Scene 4 (press P).
bool crashStarted = false;
bool crashImpacted = false;
float crashPlaneX = -140.0f;
float crashPlaneY = 760.0f;
float crashEffectTimer = 0.0f;
const float CRASH_PLANE_SPEED = 7.0f;
const float CRASH_TARGET_X = 1135.0f;
const float CRASH_TARGET_Y = 500.0f;

// Global weather state for all scenes
bool rainMode = false;
float rainOffset = 0.0f;

//------------------------- Basic Drawing Helpers -------------------------
// ============================================================
// DRAWING HELPERS
// These are primitive reusable functions.
// No Object IDs assigned.
// ============================================================
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

void drawCircle(float cx, float cy, float r, int segments = 30) {
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

// ============================================================
// C-130 CARGO PLANE
// ============================================================
void drawCargoPlane(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(scale, scale, 1);
	glColor3f(0.56f, 0.59f, 0.63f);
	glBegin(GL_POLYGON);
	glVertex2f(190, -10);
	glVertex2f(170, 15);
	glVertex2f(140, 25);
	glVertex2f(-110, 25);
	glVertex2f(-140, 15);
	glVertex2f(-175, 15);
	glVertex2f(-210, 0);
	glVertex2f(-130, -15);
	glVertex2f(140, -15);
	glVertex2f(175, -25);
	glEnd();
	glColor3f(0.80f, 0.83f, 0.87f);
	glBegin(GL_POLYGON);
	glVertex2f(190, -10);
	glVertex2f(175, -25);
	glVertex2f(140, -32);
	glVertex2f(-60, -32);
	glVertex2f(-130, -15);
	glVertex2f(140, -15);
	glEnd();
	glColor3f(0.48f, 0.52f, 0.57f);
	glBegin(GL_POLYGON);
	glVertex2f(-110, 25);
	glVertex2f(-150, 130);
	glVertex2f(-175, 130);
	glVertex2f(-175, 15);
	glVertex2f(-140, 15);
	glEnd();
	glColor3f(0.55f, 0.61f, 0.67f);
	glBegin(GL_POLYGON);
	glVertex2f(-150, 10);
	glVertex2f(-205, -5);
	glVertex2f(-190, -5);
	glVertex2f(-140, 10);
	glEnd();
	glColor3f(0.45f, 0.50f, 0.56f);
	glBegin(GL_POLYGON);
	glVertex2f(45, 25);
	glVertex2f(50, 5);
	glVertex2f(-40, 5);
	glVertex2f(-50, 25);
	glEnd();
	glColor3f(0.62f, 0.67f, 0.73f);
	glBegin(GL_POLYGON);
	glVertex2f(45, 25);
	glVertex2f(35, 38);
	glVertex2f(-35, 38);
	glVertex2f(-50, 25);
	glEnd();
	glColor3f(0.56f, 0.61f, 0.67f);
	glBegin(GL_POLYGON);
	glVertex2f(55, 5);
	glVertex2f(55, 22);
	glVertex2f(10, 22);
	glVertex2f(-5, 5);
	glEnd();
	glColor3f(0.15f, 0.15f, 0.18f);
	glBegin(GL_TRIANGLES);
	glVertex2f(55, 22);
	glVertex2f(68, 13.5f);
	glVertex2f(55, 5);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex2f(58, 13.5f);
	glVertex2f(55, 65);
	glVertex2f(52, 13.5f);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex2f(58, 13.5f);
	glVertex2f(55, -38);
	glVertex2f(52, 13.5f);
	glEnd();
	glColor3f(0.10f, 0.12f, 0.14f);
	drawCircle(118, -10, 4, 16);
	drawCircle(95, -10, 4, 16);
	drawCircle(40, -8, 3.5f, 16);
	glPopMatrix();
}

void drawBomb(float x, float y) {
	glPushMatrix();
	glTranslatef(x, y, 0);
	glColor3f(0.12f, 0.12f, 0.14f);
	drawCircle(0, 0, 13, 20);
	glColor3f(0.08f, 0.08f, 0.09f);
	drawTriangle(-9, -7, 9, -7, 0, -28);
	glColor3f(0.28f, 0.28f, 0.30f);
	drawTriangle(-10, 7, -30, 20, -5, 17);
	drawTriangle(10, 7, 30, 20, 5, 17);
	glPopMatrix();
}

void drawExplosionFlash() {
	if (flashTimer > 0.45f)
		glColor3f(1.0f, 0.90f, 0.10f);
	else if (flashTimer > 0.0f)
		glColor3f(1.0f, 0.20f, 0.02f);
	else
		glColor3f(0.15f, 0.02f, 0.01f);
	drawRect(0, 0, WIN_W, WIN_H);
}

void resetBombingAnimation() {
	bombingStarted = false;
	flashActive = false;
	planeX = -350.0f;
	planeY = 780.0f;
	flashTimer = 0;
	transitionTimer = 0;
	for (int i = 0; i < NUM_BOMBS; i++) {
		bombX[i] = bombY[i] = 0;
		bombDropped[i] = false;
		bombHit[i] = false;
	}
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
// [S1-OBJ-01] Hut Type 1
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

// [S1-OBJ-02] Hut Type 2
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

// [S1-OBJ-03] Hut Type 3
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

// [S1-OBJ-04] Hut Type 4
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

// [S1-OBJ-05] Hut Type 5
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

// [S1-OBJ-06] Pine Tree
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

// [S1-OBJ-07] Tiny Tree
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

// [S1-OBJ-08] Mountain Range
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

// [S1-OBJ-09] Rolling Hills
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

// [S1-OBJ-10] Sun
// gosun
void sun() {
	glColor3f(1.0f, 0.85f, 0.0f);
	drawCircle(520.0f, 520.0f, 40.0f, 40);
}

// [S1-OBJ-11] Cloud
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

// [S1-OBJ-12] Night Overlay
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

// [S1-OBJ-13] Ground and Path
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

// [S1-SCENE] Peaceful Village Scene
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

// [S1-RENDER] Render Scene 1
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
void drawBlob(float cx, float cy, float rx, float ry, float seed,
			  Color3 color) {
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_POLYGON);

	for (int i = 0; i < 10; i++) {
		float angle = 2.0f * (float)PI * i / 10.0f;
		// seed makes diff size
		float wobble = 1.0f + 0.25f * sinf(angle * 3.0f + seed);
		// wobble scales the distance of each vertex from the center
		glVertex2f(cx + rx * wobble * cosf(angle),
				   cy + ry * wobble * sinf(angle));
		// By multiplying each point along the perimeter is pushed outward or
		// pulled inward.
	}
	glEnd();
}

void drawCraterSmall(float x, float y, float scale) {
	// Outer
	drawBlob(x, y, 26.0f * scale, 18.0f * scale, x, DirtBrown);
	// Inner
	drawBlob(x, y, 14.0f * scale, 10.0f * scale, x + 1.0f, Charcoal);
}

// ---------------------- brick ----------------------
void drawBrick(float x, float y, float w, float h, float rotation,
			   Color3 color) {
	float hw = w * 0.5f;
	float hh = h * 0.5f;

	glPushMatrix();
	// 1. Move and rotate
	glTranslatef(x, y, 0.0f);
	glRotatef(rotation, 0.0f, 0.0f, 1.0f);

	// 2. Draw solid brick body
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_QUADS);
	glVertex2f(-hw, -hh);
	glVertex2f(hw, -hh);
	glVertex2f(hw, hh);
	glVertex2f(-hw, hh);
	glEnd();

	// 3. Draw darker border outline
	glColor3f(color.r * 0.55f, color.g * 0.55f, color.b * 0.55f);
	glLineWidth(1.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-hw, -hh);
	glVertex2f(hw, -hh);
	glVertex2f(hw, hh);
	glVertex2f(-hw, hh);
	glEnd();
	glPopMatrix();
}

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

void drawBrickPileLarge(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);
	drawBrick(-40.0f, 4.0f, 30.0f, 13.0f, -4.0f, BrickBrown);
	drawBrick(-8.0f, 3.0f, 32.0f, 13.0f, 5.0f, BrickRed);
	drawBrick(26.0f, 5.0f, 28.0f, 12.0f, -8.0f, BrickOrange);
	drawBrick(52.0f, 2.0f, 26.0f, 12.0f, 10.0f, BrickRed);
	drawBrick(-22.0f, 18.0f, 28.0f, 12.0f, 6.0f, BrickRed);
	drawBrick(6.0f, 20.0f, 26.0f, 12.0f, -5.0f, BrickBrown);
	drawBrick(34.0f, 17.0f, 24.0f, 11.0f, 9.0f, BrickOrange);
	drawBrick(-6.0f, 34.0f, 22.0f, 10.0f, -12.0f, BrickBrown);
	drawBrick(18.0f, 33.0f, 20.0f, 9.0f, 4.0f, BrickRed);
	glPopMatrix();
}

// ---------------------- mixer of brick and blob ----------------------
void drawRubblePile(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);
	drawBlob(0.0f, 6.0f, 30.0f, 14.0f, x, AshGray);
	drawBlob(-14.0f, 4.0f, 14.0f, 9.0f, x + 1.0f, CharcoalLight);
	drawBlob(16.0f, 10.0f, 12.0f, 8.0f, x + 2.0f, DirtBrown);
	drawBrick(2.0f, 14.0f, 16.0f, 8.0f, 12.0f, BrickBrown);
	drawBrick(-10.0f, 18.0f, 14.0f, 7.0f, -8.0f, BrickRed);
	glPopMatrix();
}

// ---------------------- Wooden Debris ----------------------
void drawWoodPlank(float x, float y, float length, float thickness,
				   float rotation, Color3 color) {
	float hl = length * 0.5f;
	float ht = thickness * 0.5f;

	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glRotatef(rotation, 0.0f, 0.0f, 1.0f);

	// 1. Plank body
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_QUADS);
	glVertex2f(-hl, -ht);
	glVertex2f(hl, -ht);
	glVertex2f(hl, ht);
	glVertex2f(-hl, ht);
	glEnd();

	glPopMatrix();
}

void drawBrokenBeam(float x, float y, float length, float thickness,
					float rotation, Color3 color) {
	float hl = length * 0.5f;
	float ht = thickness * 0.5f;

	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glRotatef(rotation, 0.0f, 0.0f, 1.0f);

	glColor3f(color.r, color.g, color.b);
	glBegin(GL_POLYGON);
	glVertex2f(-hl, -ht);
	glVertex2f(length * 0.25f, -ht);
	glVertex2f(hl, 0.0f);
	glVertex2f(length * 0.30f, ht);
	glVertex2f(-hl, ht);
	glEnd();
	glPopMatrix();
}
// --- mix if bram and plank ----
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
	glVertex2f(-w * 0.1f, h * 0.4f); // 0.4 , 0.3 for broken stucters
	glVertex2f(w * 0.15f, -h * 0.4f);
	glVertex2f(w * 0.2f, h * 0.4f);
	glEnd();
	glPopMatrix();
}

void drawRoofDebris(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);
	drawBrokenRoofPiece(-10.0f, 6.0f, 55.0f, 30.0f, 8.0f, RoofDarkRed);
	drawBrokenRoofPiece(28.0f, 4.0f, 40.0f, 24.0f, -15.0f, RoofGray);
	drawBrokenRoofPiece(5.0f, 20.0f, 35.0f, 20.0f, 30.0f, BurntWood);
	glPopMatrix();
}

// ---------------------- Broken Houses ----------------------
void drawBrokenHouse1(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);

	drawBrickPileLarge(-45.0f, 8.0f, 0.9f);
	drawBrickPileSmall(55.0f, 6.0f, 0.8f);
	drawRubblePile(10.0f, 5.0f, 0.7f);

	drawBrokenRoofPiece(30.0f, 90.0f, 150.0f, 75.0f, -25.0f, RoofDarkRed);

	glColor3f(CharcoalLight.r, CharcoalLight.g, CharcoalLight.b);
	drawRect(0.0f, 20.0f, 26.0f, 110.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(0.0f, 110.0f);
	glVertex2f(26.0f, 110.0f);
	glVertex2f(14.0f, 128.0f);
	glEnd();

	drawBrokenBeam(15.0f, 15.0f, 95.0f, 10.0f, 12.0f, WoodBrown);
	drawWoodPlank(-25.0f, 10.0f, 70.0f, 8.0f, -10.0f, BurntWood);

	glPopMatrix();
}

void NEW2drawHouse() {
	// 1. Dark foundation debris & dirt mound
	glColor3ub(75, 60, 48);
	glBegin(GL_POLYGON);
	glVertex2f(-4.0f, -6.0f);
	glVertex2f(-2.2f, -4.8f);
	glVertex2f(0.5f, -5.0f);
	glVertex2f(2.8f, -4.5f);
	glVertex2f(4.2f, -6.0f);
	glEnd();

	// 2. Broken Wooden Support Beams / Pillars
	glColor3ub(140, 90, 50);

	// Left vertical post
	glBegin(GL_POLYGON);
	glVertex2f(-3.1f, -6.0f);
	glVertex2f(-2.5f, -6.0f);
	glVertex2f(-2.4f, -2.5f);
	glVertex2f(-2.8f, -2.2f);
	glVertex2f(-3.2f, -2.7f);
	glEnd();

	// Right leaning post
	glBegin(GL_POLYGON);
	glVertex2f(2.0f, -6.0f);
	glVertex2f(2.6f, -6.0f);
	glVertex2f(3.5f, -3.2f);
	glVertex2f(3.0f, -3.0f);
	glEnd();

	// Central snapped pillar remnant
	glBegin(GL_POLYGON);
	glVertex2f(-0.4f, -6.0f);
	glVertex2f(0.2f, -6.0f);
	glVertex2f(0.4f, -4.2f);
	glVertex2f(-0.2f, -4.0f);
	glEnd();

	// 3. Broken Horizontal Framing Beams
	glColor3ub(115, 70, 35);

	// Left sagging beam segment
	glBegin(GL_POLYGON);
	glVertex2f(-3.2f, -2.4f);
	glVertex2f(-0.8f, -3.5f);
	glVertex2f(-0.7f, -3.9f);
	glVertex2f(-3.1f, -2.8f);
	glEnd();

	// Right collapsed beam cross-piece
	glBegin(GL_POLYGON);
	glVertex2f(-0.2f, -3.8f);
	glVertex2f(3.2f, -3.2f);
	glVertex2f(3.1f, -3.6f);
	glVertex2f(-0.3f, -4.2f);
	glEnd();

	// 4. Fractured Roof Sections
	glColor3ub(140, 35, 35);

	// Left roof fragment resting on post
	glBegin(GL_POLYGON);
	glVertex2f(-3.6f, -1.8f);
	glVertex2f(-1.5f, -3.0f);
	glVertex2f(-1.8f, -3.4f);
	glVertex2f(-3.8f, -2.1f);
	glEnd();

	// Center tilted peak section
	glBegin(GL_POLYGON);
	glVertex2f(-0.8f, -2.8f);
	glVertex2f(0.2f, -1.0f);
	glVertex2f(0.8f, -2.5f);
	glVertex2f(0.3f, -2.7f);
	glVertex2f(-0.2f, -1.6f);
	glEnd();

	// Fallen roof panel on right ground
	glBegin(GL_POLYGON);
	glVertex2f(1.8f, -4.8f);
	glVertex2f(3.3f, -4.2f);
	glVertex2f(3.5f, -4.6f);
	glVertex2f(2.0f, -5.2f);
	glEnd();

	// 5. Scattered Light Grey Stone Slabs / Rubble
	glColor3ub(195, 190, 180);

	// Stone 1
	glBegin(GL_POLYGON);
	glVertex2f(-4.3f, -5.7f);
	glVertex2f(-3.3f, -5.4f);
	glVertex2f(-3.1f, -6.0f);
	glVertex2f(-4.1f, -6.2f);
	glEnd();

	// Stone 2
	glBegin(GL_POLYGON);
	glVertex2f(-2.2f, -3.8f);
	glVertex2f(-1.4f, -3.6f);
	glVertex2f(-1.2f, -4.1f);
	glVertex2f(-2.0f, -4.3f);
	glEnd();

	// Stone 3
	glBegin(GL_POLYGON);
	glVertex2f(-0.8f, -5.2f);
	glVertex2f(-0.1f, -4.9f);
	glVertex2f(0.2f, -5.5f);
	glVertex2f(-0.5f, -5.7f);
	glEnd();

	// Stone 4
	glBegin(GL_POLYGON);
	glVertex2f(1.5f, -3.6f);
	glVertex2f(2.5f, -3.4f);
	glVertex2f(2.7f, -4.0f);
	glVertex2f(1.7f, -4.1f);
	glEnd();

	// Stone 5
	glBegin(GL_POLYGON);
	glVertex2f(2.4f, -5.2f);
	glVertex2f(3.6f, -4.8f);
	glVertex2f(3.8f, -5.5f);
	glVertex2f(2.6f, -5.8f);
	glEnd();

	// 6. Small Debris Chips
	glColor3ub(155, 150, 140);
	glBegin(GL_TRIANGLES);
	glVertex2f(-2.5f, -5.8f);
	glVertex2f(-2.2f, -5.5f);
	glVertex2f(-2.1f, -5.9f);

	glVertex2f(0.8f, -5.8f);
	glVertex2f(1.1f, -5.4f);
	glVertex2f(1.3f, -5.9f);

	glVertex2f(3.8f, -5.8f);
	glVertex2f(4.1f, -5.5f);
	glVertex2f(4.2f, -6.1f);
	glEnd();
}

// --- NEW HOUSE 2: Blackened/Charred Ruined House ---
void NEW1drawHouseBlack() {
	// 1. Background Broken Wall Blocks
	glColor3ub(55, 55, 62);

	glBegin(GL_POLYGON);
	glVertex2f(-3.0f, -1.6f);
	glVertex2f(-2.1f, -1.9f);
	glVertex2f(-1.9f, -2.8f);
	glVertex2f(-2.8f, -2.7f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(0.9f, -1.5f);
	glVertex2f(1.5f, -1.8f);
	glVertex2f(1.3f, -2.8f);
	glVertex2f(0.7f, -2.6f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(1.6f, -2.2f);
	glVertex2f(2.3f, -2.1f);
	glVertex2f(2.1f, -3.3f);
	glVertex2f(1.4f, -3.1f);
	glEnd();

	// 2. Damaged Front Wall & Crooked Door
	glColor3ub(40, 38, 48);
	glBegin(GL_POLYGON);
	glVertex2f(-1.7f, -4.8f);
	glVertex2f(1.7f, -4.8f);
	glVertex2f(1.6f, -3.0f);
	glVertex2f(0.1f, -3.4f);
	glVertex2f(-1.6f, -3.1f);
	glEnd();

	glLineWidth(2.5f);
	glColor3ub(15, 12, 20);
	glBegin(GL_LINES);
	glVertex2f(-1.1f, -3.1f);
	glVertex2f(-0.8f, -4.0f);

	glVertex2f(0.8f, -3.2f);
	glVertex2f(1.2f, -4.2f);
	glEnd();

	// 3. Exposed / Splintered Internal Framing Beams
	glColor3ub(75, 60, 48);

	glBegin(GL_POLYGON);
	glVertex2f(-0.3f, -1.3f);
	glVertex2f(0.1f, -1.1f);
	glVertex2f(0.2f, -2.4f);
	glVertex2f(-0.2f, -2.5f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(-2.4f, -2.3f);
	glVertex2f(-0.5f, -2.0f);
	glVertex2f(-0.6f, -2.3f);
	glVertex2f(-2.5f, -2.6f);
	glEnd();

	// 4. Split & Cracked Roof Peak
	glColor3ub(22, 20, 28);
	glBegin(GL_POLYGON);
	glVertex2f(-0.2f, -1.5f);
	glVertex2f(-3.7f, -3.7f);
	glVertex2f(-3.3f, -4.1f);
	glVertex2f(-0.4f, -2.3f);
	glEnd();

	glColor3ub(18, 16, 24);
	glBegin(GL_POLYGON);
	glVertex2f(0.3f, -1.8f);
	glVertex2f(3.7f, -3.9f);
	glVertex2f(3.3f, -4.3f);
	glVertex2f(0.1f, -2.6f);
	glEnd();

	// 5. Shattered Window Glass Shards
	glColor3ub(140, 175, 200);

	glBegin(GL_TRIANGLES);
	glVertex2f(-0.9f, -3.1f);
	glVertex2f(-0.5f, -3.1f);
	glVertex2f(-0.8f, -2.6f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex2f(-0.3f, -3.0f);
	glVertex2f(-0.2f, -2.5f);
	glVertex2f(-0.4f, -2.7f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex2f(0.2f, -3.1f);
	glVertex2f(0.7f, -3.1f);
	glVertex2f(0.3f, -2.4f);
	glEnd();

	// 6. Long Leaning & Fractured Outer Beams
	glColor3ub(30, 28, 36);
	glBegin(GL_POLYGON);
	glVertex2f(-5.3f, -4.2f);
	glVertex2f(-5.0f, -3.5f);
	glVertex2f(-3.0f, -3.0f);
	glVertex2f(-3.1f, -3.5f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(-3.2f, -3.1f);
	glVertex2f(-1.5f, -2.4f);
	glVertex2f(-1.7f, -3.0f);
	glVertex2f(-3.3f, -3.6f);
	glEnd();

	glColor3ub(50, 48, 56);
	glBegin(GL_POLYGON);
	glVertex2f(-4.6f, -4.7f);
	glVertex2f(-0.5f, -5.1f);
	glVertex2f(-0.3f, -5.6f);
	glVertex2f(-4.4f, -5.2f);
	glEnd();

	glColor3ub(30, 28, 36);
	glBegin(GL_POLYGON);
	glVertex2f(1.2f, -2.4f);
	glVertex2f(4.3f, -3.6f);
	glVertex2f(4.0f, -4.2f);
	glVertex2f(1.0f, -2.8f);
	glEnd();

	// 7. Ground Rubble, Cracks, Slabs & Splinters
	glColor3ub(35, 35, 40);

	glBegin(GL_POLYGON);
	glVertex2f(-4.2f, -5.3f);
	glVertex2f(-2.7f, -5.4f);
	glVertex2f(-2.8f, -5.9f);
	glVertex2f(-4.2f, -5.8f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(-2.3f, -5.4f);
	glVertex2f(-1.1f, -5.3f);
	glVertex2f(-1.2f, -5.9f);
	glVertex2f(-2.4f, -5.9f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(1.8f, -5.4f);
	glVertex2f(3.2f, -5.3f);
	glVertex2f(3.1f, -5.9f);
	glVertex2f(1.7f, -5.9f);
	glEnd();

	glColor3ub(45, 42, 50);
	glBegin(GL_POLYGON);
	glVertex2f(2.3f, -4.5f);
	glVertex2f(3.7f, -4.4f);
	glVertex2f(3.9f, -5.1f);
	glVertex2f(2.4f, -5.3f);
	glEnd();

	glColor3ub(230, 230, 225);

	glBegin(GL_POLYGON);
	glVertex2f(3.9f, -3.0f);
	glVertex2f(5.1f, -2.9f);
	glVertex2f(5.0f, -3.3f);
	glVertex2f(3.8f, -3.2f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(4.1f, -3.7f);
	glVertex2f(5.3f, -4.0f);
	glVertex2f(5.1f, -4.3f);
	glVertex2f(4.0f, -4.0f);
	glEnd();

	glColor3ub(180, 180, 175);
	glBegin(GL_TRIANGLES);
	glVertex2f(-4.8f, -5.6f);
	glVertex2f(-4.5f, -5.3f);
	glVertex2f(-4.4f, -5.7f);

	glVertex2f(-0.2f, -5.8f);
	glVertex2f(0.1f, -5.4f);
	glVertex2f(0.3f, -5.9f);

	glVertex2f(3.5f, -5.7f);
	glVertex2f(3.8f, -5.3f);
	glVertex2f(4.0f, -5.8f);
	glEnd();
}

// --- Positioning Wrappers for the two new ruined houses ---
void drawRuinedHouseNEW(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);
	glTranslatef(0.0f, 6.0f, 0.0f); // Offset base origin
	NEW2drawHouse();
	glPopMatrix();
}

void drawRuinedHouseBlackNEW(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);
	glTranslatef(0.0f, 5.9f, 0.0f); // Offset base origin
	NEW1drawHouseBlack();
	glPopMatrix();
}

// ---------------------- Burnt Trees ----------------------
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

	glColor3f(0.10f, 0.14f, 0.06f);
	drawTriangle(-18.0f, 10.0f, 0.0f, 10.0f, 0.0f, 35.0f);

	glPopMatrix();
}

void drawBurntTree2(float x, float y, float scale) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);

	glColor3f(0.20f, 0.13f, 0.08f);
	drawRect(-9.0f, 0.0f, 9.0f, 60.0f);

	glColor3f(0.16f, 0.28f, 0.10f);
	drawTriangle(-45.0f, 40.0f, 0.0f, 40.0f, 0.0f, 95.0f);
	glColor3f(0.34f, 0.26f, 0.12f);
	drawTriangle(0.0f, 40.0f, 40.0f, 40.0f, 0.0f, 90.0f);

	glColor3f(0.14f, 0.22f, 0.09f);
	drawTriangle(-30.0f, 80.0f, 0.0f, 80.0f, 0.0f, 125.0f);

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

	glColor3f(0.09f, 0.13f, 0.05f);
	drawTriangle(-14.0f, 5.0f, 14.0f, 5.0f, 0.0f, 28.0f);

	glPopMatrix();
}

// ---------------------- Fire ----------------------
void drawFire(float x, float y, float scale) {
	float flick = sinf(firePhase) * 0.12f + 1.0f;
	float flick2 = cosf(firePhase * 1.3f) * 0.10f + 1.0f;

	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);

	glColor3f(EmberRed.r, EmberRed.g, EmberRed.b);
	drawTriangle(-16.0f, 0.0f, 16.0f, 0.0f, 0.0f, 55.0f * flick);
	drawTriangle(-10.0f, 0.0f, 6.0f, 0.0f, -4.0f, 40.0f * flick2);

	glColor3f(EmberOrange.r, EmberOrange.g, EmberOrange.b);
	drawTriangle(-10.0f, 0.0f, 10.0f, 0.0f, 0.0f, 40.0f * flick2);

	glColor3f(EmberYellow.r, EmberYellow.g, EmberYellow.b);
	drawTriangle(-5.0f, 0.0f, 5.0f, 0.0f, 0.0f, 24.0f * flick);

	glPopMatrix();
}

// ---------------------- Smoke ----------------------
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
void drawScene2Ground() {
	glColor3f(DustyGreen.r, DustyGreen.g, DustyGreen.b);
	glBegin(GL_QUADS);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(WIN_W, 0.0f);
	glVertex2f(WIN_W, WIN_H * 0.33f);
	glVertex2f(0.0f, WIN_H * 0.33f);
	glEnd();

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

	glColor3f(DirtBrown.r * 0.85f, DirtBrown.g * 0.85f, DirtBrown.b * 0.85f);
	drawCircle(180.0f, 200.0f, 55.0f, 20);
	drawCircle(980.0f, 230.0f, 70.0f, 20);
	drawCircle(1400.0f, 170.0f, 45.0f, 20);
}

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

// ---------------------- Scene2 Composition ----------------------
void scene2() {
	// 1. Sun
	glPushMatrix();
	glTranslatef(550.0f, 250.0f, 0.0f);
	sun();
	glPopMatrix();

	// 2. Clouds
	drawClouds();

	// 3. Mountains + background forest
	drawMountains();
	drawHills();

	// Dust haze
	drawDustHaze();

	// 4. Ground
	drawScene2Ground();

	// 5. Burn marks & craters

	drawCraterSmall(280.0f, 45.0f, 1.0f);
	drawCraterSmall(1120.0f, 100.0f, 0.9f);
	drawCraterSmall(560.0f, 110.0f, 1.0f);
	drawCraterSmall(1500.0f, 60.0f, 1.0f);
	drawCraterSmall(120.0f, 55.0f, 2.0f);

	drawCraterSmall(850.0f, 30.0f, 0.8f);

	// 6. Distant destroyed houses + burnt trees
	drawRuinedHouseBlackNEW(120.0f, 222.0f, 11.55f);
	drawBrokenHouse1(430.0f, 218.0f, 0.45f);
	drawRuinedHouseBlackNEW(760.0f, 216.0f, 0.50f);
	drawBrokenHouse1(1080.0f, 214.0f, 0.42f);
	drawRuinedHouseBlackNEW(1380.0f, 212.0f, 11.55f);

	drawBurntTree2(200.0f, 280.0f, 0.55f);
	drawBurntTree1(500.0f, 278.0f, 0.5f);
	drawRuinedHouseNEW(860.0f, 280.0f, 8.5f);
	drawBurntTree2(1180.0f, 278.0f, 0.55f);
	drawBurntTree1(1420.0f, 280.0f, 0.5f);

	// 7. Middle-ground destroyed houses (Integrated drawRuinedHouseWood here)
	drawRuinedHouseNEW(260.0f, 175.0f, 12.7f);
	drawRuinedHouseBlackNEW(680.0f, 275.0f, 9.0f);
	drawRuinedHouseNEW(380.0f, 275.0f, 9.0f);
	drawRuinedHouseNEW(520.0f, 155.0f,
					   18.0f); // Middle-ground Wooden Ruined House
	drawBrokenHouse1(680.0f, 165.0f, 0.75f);
	drawRuinedHouseBlackNEW(1020.0f, 170.0f, 11.65f);
	drawRuinedHouseNEW(1330.0f, 160.0f, 15.7f);

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

	// 12. Centerpiece ruined structure (Integrated drawRuinedHouseBlack here)
	drawRuinedHouseBlackNEW(820.0f, 115.0f,
							20.0f); // Prominent Foreground Charred House

	// 13. Foreground burnt trees framing the scene
	drawBurntTree1(60.0f, 90.0f, 1.1f);

	drawBurntTree2(1250.0f, 90.0f, 1.1f);

	// 14. Fire near the ruins
	drawFire(760.0f, 95.0f, 1.0f);
	drawFire(650.0f, 150.0f, 0.7f);
	drawFire(1380.0, 212.0f, 0.5f);
	drawFire(500.0f, 155.0f, 0.7f);
	drawFire(380.0f, 275.0f, 0.4f);

	// 15. Smoke rising from the ruins/fires
	drawSmoke(760.0f, 130.0f, 1.0f);
	drawSmoke(650.0f, 190.0f, 0.8f);
	drawSmoke(260.0f, 210.0f, 0.7f);
	drawSmoke(1330.0f, 195.0f, 0.75f);
	drawSmoke(1380.0f, 212.0f, 0.5f);
	drawSmoke(500.0f, 155.0f, 0.7f);
	drawSmoke(500.0f, 155.0f, 0.7f);
	drawSmoke(380.0f, 275.0f, 0.7f);
	drawSmoke(680.0f, 275.0f, 0.7f);
	drawSmoke(880.0f, 275.0f, 0.7f);
}

// ---------------------- Scene2 rendering ----------------------
void renderScene2() {
	glClearColor(0.45f, 0.38f, 0.32f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	scene2();
}

//===============================scene3===================================================
struct Building {
	float x;
	float y;
	float width;
	float height;
	float r, g, b;
	int floors;
	int type;
};

// ============================================================
// PRIMITIVE DRAWING HELPERS
// ============================================================

void setColor(float r, float g, float b) { glColor3f(r, g, b); }

void rect(float x, float y, float width, float height) {
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);
	glEnd();
}

void line(float x1, float y1, float x2, float y2) {
	glBegin(GL_LINES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
}

void circle(float cx, float cy, float radius, int segments = 30) {
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx, cy);
	for (int i = 0; i <= segments; i++) {
		float angle = 2.0f * (float)M_PI * i / segments;
		glVertex2f(cx + cos(angle) * radius, cy + sin(angle) * radius);
	}
	glEnd();
}

// ============================================================
// RAW-POINT CRANE MODEL (LOCAL ORIGIN 0,0)
// ============================================================

void drawCraneModel() {
	glLineWidth(1.6f);

	// 1. Base Foundation Slab & Legs
	glColor3f(0.16f, 0.22f, 0.28f);
	glBegin(GL_QUADS);
	glVertex2f(-22, 0);
	glVertex2f(22, 0);
	glVertex2f(22, 8);
	glVertex2f(-22, 8);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(-20, 8);
	glVertex2f(-10, 35);
	glVertex2f(20, 8);
	glVertex2f(10, 35);
	glVertex2f(-20, 8);
	glVertex2f(10, 8);
	glVertex2f(20, 8);
	glVertex2f(-10, 8);
	glEnd();

	// 2. Vertical Mast (Lattice Tower)
	glBegin(GL_LINES);
	glVertex2f(-10, 0);
	glVertex2f(-10, 400);
	glVertex2f(10, 0);
	glVertex2f(10, 400);

	for (float y = 0; y < 400; y += 50) {
		glVertex2f(-10, y + 50);
		glVertex2f(10, y + 50);
		glVertex2f(-10, y);
		glVertex2f(10, y + 50);
		glVertex2f(10, y);
		glVertex2f(-10, y + 50);
	}
	glEnd();

	// 3. Operator Cabin
	glColor3f(0.18f, 0.24f, 0.30f);
	glBegin(GL_POLYGON);
	glVertex2f(2, 375);
	glVertex2f(24, 375);
	glVertex2f(29, 387);
	glVertex2f(24, 397);
	glVertex2f(2, 397);
	glEnd();

	glColor3f(0.72f, 0.86f, 0.94f);
	glBegin(GL_POLYGON);
	glVertex2f(9, 382);
	glVertex2f(23, 382);
	glVertex2f(26, 387);
	glVertex2f(23, 393);
	glVertex2f(9, 393);
	glEnd();

	// 4. Apex (Cat-Head)
	glColor3f(0.16f, 0.22f, 0.28f);
	glBegin(GL_LINES);
	glVertex2f(-10, 400);
	glVertex2f(0, 465);
	glVertex2f(10, 400);
	glVertex2f(0, 465);
	glVertex2f(0, 400);
	glVertex2f(0, 465);
	glVertex2f(-5, 432);
	glVertex2f(5, 432);
	glVertex2f(-10, 400);
	glVertex2f(5, 432);
	glVertex2f(10, 400);
	glVertex2f(-5, 432);
	glEnd();

	// 5. Horizontal Working Jib & Counter-Jib
	glBegin(GL_LINES);
	glVertex2f(-75, 416);
	glVertex2f(180, 416);
	glVertex2f(-75, 400);
	glVertex2f(180, 400);
	glVertex2f(-75, 400);
	glVertex2f(-75, 416);
	glVertex2f(180, 400);
	glVertex2f(180, 416);

	for (float x = -75; x < 180; x += 25) {
		float nx = std::min(x + 25.0f, 180.0f);
		glVertex2f(nx, 400);
		glVertex2f(nx, 416);
		glVertex2f(x, 400);
		glVertex2f(nx, 416);
		glVertex2f(x, 416);
		glVertex2f(nx, 400);
	}
	glEnd();

	// 6. Pendant Stay Cables
	glColor3f(0.10f, 0.14f, 0.18f);
	glBegin(GL_LINES);
	glVertex2f(0, 465);
	glVertex2f(-70, 416);
	glVertex2f(0, 465);
	glVertex2f(-45, 416);
	glVertex2f(0, 465);
	glVertex2f(90, 416);
	glVertex2f(0, 465);
	glVertex2f(160, 416);
	glEnd();

	// 7. Counterweight Ballast Blocks
	glColor3f(0.38f, 0.44f, 0.48f);
	glBegin(GL_QUADS);
	glVertex2f(-71, 380);
	glVertex2f(-39, 380);
	glVertex2f(-39, 400);
	glVertex2f(-71, 400);
	glEnd();

	// 8. Trolley & Suspended Steel Beam
	glColor3f(0.22f, 0.28f, 0.34f);
	glBegin(GL_QUADS);
	glVertex2f(106, 395);
	glVertex2f(124, 395);
	glVertex2f(124, 400);
	glVertex2f(106, 400);
	glEnd();

	glColor3f(0.10f, 0.14f, 0.18f);
	glBegin(GL_LINES);
	glVertex2f(112, 395);
	glVertex2f(112, 280);
	glVertex2f(118, 395);
	glVertex2f(118, 280);
	glVertex2f(115, 274);
	glVertex2f(92, 252);
	glVertex2f(115, 274);
	glVertex2f(138, 252);
	glEnd();

	glColor3f(0.50f, 0.56f, 0.60f);
	rect(87, 244, 56, 8);
	glLineWidth(1.0f);
}

void drawTranslatedCrane(float x, float y, float scaleX = 1.0f,
						 float scaleY = 1.0f) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	if (scaleX != 1.0f || scaleY != 1.0f) {
		glScalef(scaleX, scaleY, 1.0f);
	}
	drawCraneModel();
	glPopMatrix();
}

// ============================================================
// ENVIRONMENT
// ============================================================

void drawSky() {
	setColor(0.52f, 0.75f, 0.92f);
	rect(0, 160, WIN_W, WIN_H - 160);

	// Sun
	setColor(1.0f, 0.86f, 0.25f);
	circle(1450, 870, 55);

	// Clouds
	setColor(1.0f, 1.0f, 1.0f);
	circle(180, 850, 30);
	circle(220, 865, 40);
	circle(265, 850, 30);
	rect(180, 820, 85, 40);
	circle(650, 900, 25);
	circle(685, 915, 35);
	circle(725, 900, 25);
	rect(650, 875, 75, 35);
	circle(1100, 810, 30);
	circle(1140, 825, 42);
	circle(1185, 810, 30);
	rect(1100, 785, 85, 40);
}

void drawGround() {
	setColor(0.40f, 0.58f, 0.32f);
	rect(0, 0, WIN_W, 160);

	setColor(0.62f, 0.62f, 0.60f);
	rect(0, 125, WIN_W, 35);

	setColor(0.15f, 0.15f, 0.16f);
	rect(0, 0, WIN_W, 125);

	setColor(0.95f, 0.82f, 0.20f);
	for (int x = 20; x < WIN_W; x += 120) {
		rect(x, 58, 65, 7);
	}

	setColor(0.48f, 0.48f, 0.46f);
	for (int x = 0; x < WIN_W; x += 80) {
		line(x, 125, x + 35, 160);
	}
}

void drawBackgroundBuildings() {
	setColor(0.48f, 0.57f, 0.63f);
	rect(0, 160, 70, 230);
	setColor(0.43f, 0.52f, 0.58f);
	rect(75, 160, 70, 270);
	setColor(0.46f, 0.56f, 0.63f);
	rect(1525, 160, 75, 270);

	setColor(0.85f, 0.90f, 0.88f);
	for (int y = 190; y < 390; y += 35) {
		rect(15, y, 40, 10);
		rect(90, y, 40, 10);
		rect(1545, y, 35, 10);
	}
}

// ============================================================
// COMPLETED BUILDINGS
// ============================================================

void buildingNormal(const Building &b) {
	setColor(b.r, b.g, b.b);
	rect(b.x, b.y, b.width, b.height);
	setColor(b.r * 0.75f, b.g * 0.75f, b.b * 0.75f);
	rect(b.x - 3, b.y + b.height, b.width + 6, 8);

	float floorHeight = b.height / b.floors;
	setColor(0.78f, 0.88f, 0.92f);
	for (int floor = 0; floor < b.floors; floor++) {
		float wy = b.y + floor * floorHeight + floorHeight * 0.30f;
		int columns = (int)(b.width / 35.0f);
		if (columns < 1)
			columns = 1;
		float spacing = b.width / (columns + 1);

		for (int col = 0; col < columns; col++) {
			float wx = b.x + spacing * (col + 1) - 7;
			rect(wx, wy, 14, floorHeight * 0.35f);
		}
	}
}

void buildingGlass(const Building &b) {
	setColor(b.r, b.g, b.b);
	rect(b.x, b.y, b.width, b.height);

	setColor(0.20f, 0.38f, 0.48f);
	int columns = (int)(b.width / 25.0f);
	for (int i = 1; i < columns; i++) {
		float x = b.x + i * b.width / columns;
		line(x, b.y, x, b.y + b.height);
	}

	for (int i = 1; i < b.floors; i++) {
		float y = b.y + i * b.height / b.floors;
		line(b.x, y, b.x + b.width, y);
	}

	setColor(0.75f, 0.90f, 0.95f);
	for (int floor = 0; floor < b.floors; floor++) {
		float y = b.y + floor * b.height / b.floors + 10;
		for (int col = 0; col < columns; col++) {
			float x = b.x + col * b.width / columns + 5;
			rect(x, y, b.width / columns - 10, b.height / b.floors - 18);
		}
	}
}

// ============================================================
// 1. STANDARD HALF-CONSTRUCTED BUILDING (SHORT REBAR STUBS)
// ============================================================

void drawHalfConstructedBuilding(const Building &b, float progress) {
	float currentHeight = b.height * progress;

	// Completed lower section
	setColor(b.r, b.g, b.b);
	rect(b.x, b.y, b.width, currentHeight);

	float floorHeight = b.height / b.floors;
	setColor(0.30f, 0.32f, 0.34f);
	int completedFloors = (int)(currentHeight / floorHeight);

	for (int i = 0; i <= completedFloors; i++) {
		float y = b.y + i * floorHeight;
		if (y <= b.y + currentHeight) {
			rect(b.x - 4, y, b.width + 8, 5);
		}
	}

	// Windows
	setColor(0.75f, 0.88f, 0.93f);
	for (int floor = 0; floor < completedFloors; floor++) {
		float y = b.y + floor * floorHeight + 10;
		int columns = (int)(b.width / 30.0f);
		if (columns < 1)
			columns = 1;
		float windowWidth = b.width / (columns + 1);

		for (int c = 0; c < columns; c++) {
			float x = b.x + windowWidth * (c + 1) - 6;
			rect(x, y, 12, floorHeight * 0.35f);
		}
	}

	// Short, reduced rebar stubs at the top slab (12px high)
	if (progress < 1.0f) {
		setColor(0.25f, 0.28f, 0.30f);
		float topY = b.y + currentHeight;
		glLineWidth(2.0f);
		for (float x = b.x + 8; x < b.x + b.width; x += 16) {
			line(x, topY, x, topY + 12);
		}
		glLineWidth(1.0f);
	}
}
// ============================================================
// 1. LEFT SUPERSTRUCTURE (SHORTER TOWER)
// ============================================================
void drawSteelSuperstructure(float x, float y, float scaleX = 1.0f,
							 float scaleY = 1.0f) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scaleX, scaleY,
			 1.0f); // Scales width and height uniformly or non-uniformly

	// --------------------------------------------------------
	// A. BACKGROUND ELEVATOR CORES (GL_POLYGON)
	// --------------------------------------------------------
	glColor3f(0.55f, 0.65f, 0.72f);

	// Left core shaft
	glBegin(GL_POLYGON);
	glVertex2f(18.0f, 0.0f);
	glVertex2f(40.0f, 0.0f);
	glVertex2f(40.0f, 230.0f);
	glVertex2f(18.0f, 230.0f);
	glEnd();

	// Right core shaft
	glBegin(GL_POLYGON);
	glVertex2f(54.0f, 0.0f);
	glVertex2f(78.0f, 0.0f);
	glVertex2f(78.0f, 255.0f);
	glVertex2f(54.0f, 255.0f);
	glEnd();

	// --------------------------------------------------------
	// B. CARGO PALLET BOXES (GL_POLYGON)
	// --------------------------------------------------------
	glColor3f(0.60f, 0.42f, 0.22f);

	// Left Box
	glBegin(GL_POLYGON);
	glVertex2f(12.0f, 4.0f);
	glVertex2f(38.0f, 4.0f);
	glVertex2f(38.0f, 22.0f);
	glVertex2f(12.0f, 22.0f);
	glEnd();

	// Right Box
	glBegin(GL_POLYGON);
	glVertex2f(48.0f, 4.0f);
	glVertex2f(70.0f, 4.0f);
	glVertex2f(70.0f, 16.0f);
	glVertex2f(48.0f, 16.0f);
	glEnd();

	// --------------------------------------------------------
	// C. VERTICAL STEEL COLUMNS (GL_QUADS)
	// --------------------------------------------------------
	glColor3f(0.38f, 0.45f, 0.50f);
	glBegin(GL_QUADS);
	// Column 1 (Left edge)
	glVertex2f(2.0f, 0.0f);
	glVertex2f(8.0f, 0.0f);
	glVertex2f(8.0f, 260.0f);
	glVertex2f(2.0f, 260.0f);

	// Column 2 (Middle)
	glVertex2f(42.0f, 0.0f);
	glVertex2f(48.0f, 0.0f);
	glVertex2f(48.0f, 260.0f);
	glVertex2f(42.0f, 260.0f);

	// Column 3 (Right mast - tallest top extension)
	glVertex2f(82.0f, 0.0f);
	glVertex2f(88.0f, 0.0f);
	glVertex2f(88.0f, 295.0f);
	glVertex2f(82.0f, 295.0f);

	// Column 4 (Right cantilever column)
	glVertex2f(118.0f, 20.0f);
	glVertex2f(124.0f, 20.0f);
	glVertex2f(124.0f, 165.0f);
	glVertex2f(118.0f, 165.0f);
	glEnd();

	// --------------------------------------------------------
	// D. HORIZONTAL STEEL BEAMS (GL_QUADS)
	// --------------------------------------------------------
	glBegin(GL_QUADS);
	// 8 Main floor beams
	for (int i = 1; i <= 8; i++) {
		float yPos = i * 28.0f;
		glVertex2f(2.0f, yPos);
		glVertex2f(88.0f, yPos);
		glVertex2f(88.0f, yPos + 6.0f);
		glVertex2f(2.0f, yPos + 6.0f);
	}

	// Extended lower cantilever beam
	glVertex2f(88.0f, 30.0f);
	glVertex2f(128.0f, 30.0f);
	glVertex2f(128.0f, 36.0f);
	glVertex2f(88.0f, 36.0f);
	glEnd();

	// --------------------------------------------------------
	// E. DIAGONAL CROSS-BRACING GIRDERS (GL_LINES)
	// --------------------------------------------------------
	glLineWidth(4.0f);
	glColor3f(0.32f, 0.38f, 0.44f);
	glBegin(GL_LINES);
	glVertex2f(48.0f, 6.0f);
	glVertex2f(82.0f, 28.0f);
	glVertex2f(48.0f, 34.0f);
	glVertex2f(82.0f, 56.0f);
	glVertex2f(48.0f, 62.0f);
	glVertex2f(82.0f, 84.0f);
	glEnd();

	// --------------------------------------------------------
	// F. OUTLINE ACCENTS (GL_LINES)
	// --------------------------------------------------------
	glLineWidth(1.0f);
	glColor3f(0.20f, 0.25f, 0.28f);
	glBegin(GL_LINES);
	glVertex2f(2.0f, 0.0f);
	glVertex2f(2.0f, 260.0f);
	glVertex2f(42.0f, 0.0f);
	glVertex2f(42.0f, 260.0f);
	glVertex2f(82.0f, 0.0f);
	glVertex2f(82.0f, 295.0f);
	glVertex2f(118.0f, 20.0f);
	glVertex2f(118.0f, 165.0f);
	glEnd();

	glPopMatrix();
}

// ============================================================
// ACCESSORIES & TREES
// ============================================================

void drawMaterials(float x, float y) {
	setColor(0.72f, 0.40f, 0.20f);
	rect(x, y, 45, 12);
	rect(x + 5, y + 12, 45, 12);
	rect(x - 5, y + 24, 45, 12);

	setColor(0.35f, 0.37f, 0.38f);
	for (int i = 0; i < 5; i++) {
		line(x + i * 8, y + 40, x + i * 8 + 20, y + 40);
	}
}

void tree(float x, float y, float scale) {
	glColor3f(0.38f, 0.23f, 0.10f);
	drawRect(x - 6.0f * scale, y, x + 6.0f * scale, y + 40.0f * scale);

	glColor3f(0.12f, 0.50f, 0.18f);
	drawCircle(x, y + 65.0f * scale, 27.0f * scale);
	drawCircle(x - 20.0f * scale, y + 52.0f * scale, 22.0f * scale);
	drawCircle(x + 20.0f * scale, y + 52.0f * scale, 22.0f * scale);
	drawCircle(x, y + 87.0f * scale, 20.0f * scale);
}

void drawTrees() {
	tree(35, 160, 1.0f);
	tree(145, 160, 0.85f);
	tree(285, 160, 0.90f);
	tree(390, 160, 0.75f);
	tree(525, 160, 0.95f);
	tree(660, 160, 0.80f);
	tree(790, 160, 0.90f);
	tree(1000, 160, 0.75f);
	tree(1090, 160, 0.95f);
	tree(1250, 160, 0.80f);
	tree(1400, 160, 0.95f);
	tree(1530, 160, 0.85f);
	tree(1580, 160, 0.75f);
}

void drawBarrier(float x, float y) {
	setColor(0.95f, 0.65f, 0.05f);
	rect(x, y, 75, 8);
	setColor(0.25f, 0.25f, 0.25f);
	rect(x + 5, y - 25, 7, 25);
	rect(x + 63, y - 25, 7, 25);
	setColor(0.15f, 0.15f, 0.15f);
	line(x + 5, y, x + 25, y + 8);
	line(x + 30, y, x + 50, y + 8);
	line(x + 55, y, x + 70, y + 8);
}

void drawConstructionSign(float x, float y) {
	setColor(0.25f, 0.25f, 0.25f);
	rect(x, y, 6, 70);
	rect(x + 74, y, 6, 70);
	setColor(0.95f, 0.65f, 0.05f);
	rect(x - 10, y + 50, 100, 45);
	setColor(0.15f, 0.15f, 0.15f);
	rect(x + 35, y + 65, 10, 20);
	circle(x + 40, y + 58, 5);
}

// ============================================================
// MAIN SCENE
// ============================================================

void drawPartialCity() {
	drawSky();
	drawGround();
	drawBackgroundBuildings();

	// Established Completed Buildings
	Building left1 = {45, 160, 100, 150, 0.72f, 0.43f, 0.35f, 5, 0};
	buildingNormal(left1);

	Building left2 = {175, 160, 110, 190, 0.45f, 0.65f, 0.75f, 6, 1};
	buildingGlass(left2);

	Building left3 = {310, 160, 90, 140, 0.72f, 0.58f, 0.46f, 5, 0};
	buildingNormal(left3);

	// ========================================================
	// LARGEST BUILDING #1 (HEAVY STEEL SUPERSTRUCTURE WITH BRACES)
	// ========================================================
	Building tower1 = {670, 160, 140, 150, 0.48f, 0.68f, 0.76f, 5, 1};
	buildingNormal(tower1);
	drawSteelSuperstructure(tower1.x + 5.0f, tower1.y + tower1.height, 0.90f,
							0.78f);

	// Small/Medium Buildings (Cleaned up with short rebar stubs)
	Building tower2 = {420, 160, 115, 360, 0.35f, 0.55f, 0.68f, 12, 2};
	drawHalfConstructedBuilding(tower2, 0.65f);

	Building centerLow = {550, 160, 105, 230, 0.70f, 0.55f, 0.43f, 7, 0};
	buildingNormal(centerLow);

	// ========================================================
	// LARGEST BUILDING #2 (MAIN CENTRAL TALL TOWER)
	// ========================================================
	Building central = {830, 160, 140, 200, 0.45f, 0.66f, 0.76f, 6, 1};
	buildingNormal(central);
	drawSteelSuperstructure(central.x + 5.0f, central.y + central.height, 1.05f,
							1.0f);

	Building centerRight = {995, 160, 115, 280, 0.72f, 0.62f, 0.52f, 9, 0};
	buildingNormal(centerRight);

	// Medium/Small Towers (Reduced lines)
	Building rightTower = {1130, 160, 120, 400, 0.42f, 0.61f, 0.70f, 14, 1};
	drawHalfConstructedBuilding(rightTower, 0.55f);

	Building twin1 = {1280, 160, 95, 420, 0.52f, 0.63f, 0.70f, 15, 1};
	drawHalfConstructedBuilding(twin1, 0.40f);

	Building twin2 = {1410, 160, 95, 420, 0.12f, 0.48f, 0.66f, 15, 1};
	drawHalfConstructedBuilding(twin2, 0.25f);

	Building rightLow = {1520, 160, 80, 240, 0.68f, 0.42f, 0.38f, 7, 0};
	buildingNormal(rightLow);

	// ========================================================
	// CRANES (POSITIONED OVER THE ACTIVE CONSTRUCTION SITES)
	// ========================================================
	drawTranslatedCrane(520.0f, 160.0f, 1.05f, 1.25f);
	drawTranslatedCrane(820.0f, 160.0f, 1.15f, 1.40f);
	drawTranslatedCrane(1270.0f, 160.0f, -1.0f, 1.20f);

	// Accessories
	drawMaterials(360, 165);
	drawMaterials(680, 165);
	drawMaterials(1060, 165);
	drawMaterials(1260, 165);

	drawBarrier(420, 170);
	drawBarrier(720, 170);
	drawBarrier(1030, 170);
	drawBarrier(1320, 170);

	drawConstructionSign(285, 165);
	drawConstructionSign(1080, 165);

	drawTrees();
}

//==============================scene4===================================================

void drawRectOutline(float x1, float y1, float x2, float y2,
					 float lineWidth = 3.0f, float r = 0.12f, float g = 0.14f,
					 float b = 0.18f) {
	glColor3f(r, g, b);
	glLineWidth(lineWidth);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);
	glEnd();
	glLineWidth(1.0f);
}

void drawWindowGrid(float startX, float startY, float totalWidth,
					float totalHeight, int floors, int cols,
					bool isGlass = false) {

	float floorHeight = totalHeight / (float)floors;
	float slotWidth =
		(totalWidth * 0.65f) / (float)cols; // per row windows size
	float sideMargin = totalWidth * 0.175f; // left side gap
	float vertMargin = floorHeight * 0.28f; // vertical gap ( bottom gap)
	float ww = slotWidth * 0.62f;			// actual winodw hight and width
	float wh = floorHeight * 0.44f;

	if (isGlass)
		glColor3f(0.15f, 0.48f, 0.68f);
	else
		glColor3f(0.15f, 0.28f, 0.38f);

	for (int r = 0; r < floors; r++) {
		float wy = startY + (float)r * floorHeight +
				   vertMargin; // moves the point to specific floor
		for (int c = 0; c < cols; c++) {
			float wx =
				startX + sideMargin + (float)c * slotWidth; // moves coloum
			glBegin(GL_QUADS);
			glVertex2f(wx, wy);
			glVertex2f(wx + ww, wy);
			glVertex2f(wx + ww, wy + wh);
			glVertex2f(wx, wy + wh);
			glEnd();
		}
	}
}
//-------------------------twin tower----

void drawTwinTower() {
	glColor3f(0.74f, 0.77f, 0.80f);
	glBegin(GL_QUADS);
	glVertex2f(0.0f, 160.0f);
	glVertex2f(100.0f, 160.0f);
	glVertex2f(100.0f, 620.0f);
	glVertex2f(0.0f, 620.0f);
	glColor3f(0.65f, 0.68f, 0.72f);
	glVertex2f(-3.0f, 620.0f);
	glVertex2f(103.0f, 620.0f);
	glVertex2f(103.0f, 627.0f);
	glVertex2f(-3.0f, 627.0f);
	glEnd();

	// Outline
	drawRectOutline(0.0f, 160.0f, 100.0f, 620.0f, 3.0f);
	drawRectOutline(-3.0f, 620.0f, 103.0f, 627.0f, 3.0f);

	glColor3f(0.30f, 0.48f, 0.58f); // loop for virtical lines
	glLineWidth(1.0f);
	glBegin(GL_LINES);
	for (int i = 1; i < 6; i++) {
		float xx = 100.0f * (float)i / 6.0f;
		glVertex2f(xx, 160.0f);
		glVertex2f(xx, 620.0f);
	}
	glEnd();

	glColor3f(0.20f, 0.40f, 0.52f);
	float fHeight = 460.0f / 22.0f; // loop for window
	for (int r = 0; r < 22; r++) {
		float wy = 160.0f + (float)r * fHeight + fHeight * 0.25f;
		for (int c = 0; c < 4; c++) {
			float wx = 8.0f + (float)c * (84.0f / 4.0f);
			glBegin(GL_QUADS);
			glVertex2f(wx, wy);
			glVertex2f(wx + 5.0f, wy);
			glVertex2f(wx + 5.0f, wy + fHeight * 0.45f);
			glVertex2f(wx, wy + fHeight * 0.45f);
			glEnd();
		}
	}
	glColor3f(0.15f, 0.18f, 0.20f);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glVertex2f(50.0f, 627.0f);
	glVertex2f(50.0f, 697.0f);
	glEnd();
}

void drawBuildings() {
	glColor3f(0.55f, 0.67f, 0.73f);
	glBegin(GL_QUADS);
	glVertex2f(0.0f, 160.0f);
	glVertex2f(80.0f, 160.0f);
	glVertex2f(80.0f, 380.0f);
	glVertex2f(0.0f, 380.0f);

	glVertex2f(90.0f, 160.0f);
	glVertex2f(165.0f, 160.0f);
	glVertex2f(165.0f, 420.0f);
	glVertex2f(90.0f, 420.0f);

	glVertex2f(1520.0f, 160.0f);
	glVertex2f(1600.0f, 160.0f);
	glVertex2f(1600.0f, 410.0f);
	glVertex2f(1520.0f, 410.0f);
	glEnd();

	drawRectOutline(0.0f, 160.0f, 80.0f, 380.0f, 2.5f, 0.25f, 0.35f, 0.40f);
	drawRectOutline(90.0f, 160.0f, 165.0f, 420.0f, 2.5f, 0.25f, 0.35f, 0.40f);
	drawRectOutline(1520.0f, 160.0f, 1600.0f, 410.0f, 2.5f, 0.25f, 0.35f,
					0.40f);

	glColor3f(0.35f, 0.55f, 0.65f);
	for (float y = 190.0f; y < 380.0f; y += 28.0f) { // left most tower
		glBegin(GL_QUADS);
		glVertex2f(15.0f, y);
		glVertex2f(65.0f, y);
		glVertex2f(65.0f, y + 9.0f);
		glVertex2f(15.0f, y + 9.0f);
	}
}

void drawCity() {
	drawBuildings();

	// --- B1 (Left) ---
	glColor3f(0.78f, 0.72f, 0.65f);
	glBegin(GL_QUADS);
	glVertex2f(45.0f, 160.0f);
	glVertex2f(160.0f, 160.0f);
	glVertex2f(160.0f, 335.0f);
	glVertex2f(45.0f, 335.0f);

	glVertex2f(54.0f, 335.0f);
	glVertex2f(151.0f, 335.0f);
	glVertex2f(151.0f, 372.5f);
	glVertex2f(54.0f, 372.5f);

	glVertex2f(65.0f, 372.5f);
	glVertex2f(139.0f, 372.5f);
	glVertex2f(139.0f, 410.0f);
	glVertex2f(65.0f, 410.0f);
	glEnd();
	drawWindowGrid(45.0f, 160.0f, 115.0f, 175.0f, 8, 3);
	drawWindowGrid(54.0f, 335.0f, 97.0f, 37.5f, 2, 2);
	drawRectOutline(45.0f, 160.0f, 160.0f, 335.0f, 3.0f);
	drawRectOutline(54.0f, 335.0f, 151.0f, 372.5f, 3.0f);
	drawRectOutline(65.0f, 372.5f, 139.0f, 410.0f, 3.0f);

	// --- B2
	glColor3f(0.42f, 0.65f, 0.78f);
	glBegin(GL_QUADS);
	glVertex2f(175.0f, 160.0f);
	glVertex2f(300.0f, 160.0f);
	glVertex2f(300.0f, 470.0f);
	glVertex2f(175.0f, 470.0f);
	glEnd();
	glColor3f(0.10f, 0.20f, 0.28f);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glVertex2f(206.25f, 160.0f);
	glVertex2f(206.25f, 470.0f);
	glVertex2f(237.50f, 160.0f);
	glVertex2f(237.50f, 470.0f);
	glVertex2f(268.75f, 160.0f);
	glVertex2f(268.75f, 470.0f);
	glEnd();
	drawWindowGrid(175.0f, 160.0f, 125.0f, 310.0f, 10, 3, true);
	drawRectOutline(175.0f, 160.0f, 300.0f, 470.0f, 3.0f);

	// --- B3
	glColor3f(0.78f, 0.38f, 0.32f);
	glBegin(GL_QUADS);
	glVertex2f(315.0f, 160.0f);
	glVertex2f(415.0f, 160.0f);
	glVertex2f(415.0f, 370.0f);
	glVertex2f(315.0f, 370.0f);
	glEnd();
	drawWindowGrid(315.0f, 160.0f, 100.0f, 210.0f, 7, 3);
	drawRectOutline(315.0f, 160.0f, 415.0f, 370.0f, 3.0f);

	// --- B4 Spire ---
	glColor3f(0.65f, 0.75f, 0.82f);
	glBegin(GL_QUADS);
	glVertex2f(420.0f, 160.0f);
	glVertex2f(525.0f, 160.0f);
	glVertex2f(525.0f, 600.0f);
	glVertex2f(420.0f, 600.0f);
	glEnd();
	drawWindowGrid(420.0f, 160.0f, 105.0f, 440.0f, 14, 3);
	glColor3f(0.20f, 0.25f, 0.30f);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glVertex2f(472.5f, 600.0f);
	glVertex2f(472.5f, 670.0f);
	glEnd();
	drawRectOutline(420.0f, 160.0f, 525.0f, 600.0f, 3.0f);

	// --- B5 Slanted-Roof Modern Skyscraper ---
	glColor3f(0.30f, 0.52f, 0.70f);
	glBegin(GL_QUADS);
	glVertex2f(545.0f, 160.0f);
	glVertex2f(675.0f, 160.0f);
	glVertex2f(675.0f, 520.0f);
	glVertex2f(545.0f, 520.0f);
	glVertex2f(545.0f, 520.0f);
	glVertex2f(675.0f, 555.0f);
	glVertex2f(675.0f, 540.0f);
	glVertex2f(545.0f, 505.0f);
	glEnd();
	drawWindowGrid(545.0f, 160.0f, 130.0f, 355.0f, 13, 3, true);
	glColor3f(0.12f, 0.14f, 0.18f);
	glLineWidth(3.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2f(545.0f, 160.0f);
	glVertex2f(675.0f, 160.0f);
	glVertex2f(675.0f, 555.0f);
	glVertex2f(545.0f, 521.0f);
	glEnd();

	// --- B6 Mid Center Building ---
	glColor3f(0.72f, 0.62f, 0.52f);
	glBegin(GL_QUADS);
	glVertex2f(690.0f, 160.0f);
	glVertex2f(825.0f, 160.0f);
	glVertex2f(825.0f, 405.0f);
	glVertex2f(690.0f, 405.0f);
	glEnd();
	drawWindowGrid(690.0f, 160.0f, 135.0f, 245.0f, 8, 3);
	drawRectOutline(690.0f, 160.0f, 825.0f, 405.0f, 3.0f);

	// --- B7 Central tower + Triangle  ---
	glColor3f(0.48f, 0.70f, 0.80f);
	glBegin(GL_QUADS);
	glVertex2f(840.0f, 160.0f);
	glVertex2f(975.0f, 160.0f);
	glVertex2f(975.0f, 660.0f);
	glVertex2f(840.0f, 660.0f);
	glEnd();
	glColor3f(0.62f, 0.68f, 0.73f);
	glBegin(GL_TRIANGLES);
	glVertex2f(840.0f, 660.0f);
	glVertex2f(975.0f, 660.0f);
	glVertex2f(907.5f, 725.0f);
	glEnd();
	glColor3f(0.20f, 0.25f, 0.30f);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glVertex2f(907.5f, 725.0f);
	glVertex2f(907.5f, 800.0f);
	glEnd();
	drawWindowGrid(840.0f, 160.0f, 135.0f, 500.0f, 17, 3, true);
	drawRectOutline(840.0f, 160.0f, 975.0f, 660.0f, 3.0f);
	glColor3f(0.12f, 0.14f, 0.18f);
	glLineWidth(3.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2f(840.0f, 660.0f);
	glVertex2f(975.0f, 660.0f);
	glVertex2f(907.5f, 725.0f);
	glEnd();

	// --- B8 Stepped Tower
	glColor3f(0.70f, 0.67f, 0.60f);
	glBegin(GL_QUADS);
	glVertex2f(1010.0f, 160.0f);
	glVertex2f(1110.0f, 160.0f);
	glVertex2f(1110.0f, 391.0f);
	glVertex2f(1010.0f, 391.0f);

	glVertex2f(1018.0f, 391.0f);
	glVertex2f(1102.0f, 391.0f);
	glVertex2f(1102.0f, 440.5f);
	glVertex2f(1018.0f, 440.5f);

	glVertex2f(1028.0f, 440.5f);
	glVertex2f(1092.0f, 440.5f);
	glVertex2f(1092.0f, 490.0f);
	glVertex2f(1028.0f, 490.0f);
	glEnd();
	drawWindowGrid(1010.0f, 160.0f, 100.0f, 231.0f, 11, 3);
	drawRectOutline(1010.0f, 160.0f, 1110.0f, 391.0f, 3.0f);
	drawRectOutline(1018.0f, 391.0f, 1102.0f, 440.5f, 3.0f);
	drawRectOutline(1028.0f, 440.5f, 1092.0f, 490.0f, 3.0f);

	glPushMatrix();
	glTranslatef(1135.0f, 0.0f, 0.0f);
	drawTwinTower();
	glPopMatrix();

	// Tower 2
	glPushMatrix();
	glTranslatef(1280.0f, 0.0f, 0.0f);
	drawTwinTower();
	glPopMatrix();

	// --- B9 Right  Triangular Cap ---
	glColor3f(0.40f, 0.60f, 0.68f);
	glBegin(GL_QUADS);
	glVertex2f(1410.0f, 160.0f);
	glVertex2f(1535.0f, 160.0f);
	glVertex2f(1535.0f, 560.0f);
	glVertex2f(1410.0f, 560.0f);
	glEnd();
	glColor3f(0.55f, 0.60f, 0.65f);
	glBegin(GL_TRIANGLES);
	glVertex2f(1410.0f, 560.0f);
	glVertex2f(1535.0f, 560.0f);
	glVertex2f(1472.5f, 620.0f);
	glEnd();
	glColor3f(0.20f, 0.25f, 0.30f);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glVertex2f(1472.5f, 620.0f);
	glVertex2f(1472.5f, 685.0f);
	glEnd();
	drawWindowGrid(1410.0f, 160.0f, 125.0f, 400.0f, 14, 3, true);
	drawRectOutline(1410.0f, 160.0f, 1535.0f, 560.0f, 3.0f);
	glColor3f(0.12f, 0.14f, 0.18f);
	glLineWidth(3.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2f(1410.0f, 560.0f);
	glVertex2f(1535.0f, 560.0f);
	glVertex2f(1472.5f, 620.0f);
	glEnd();

	// --- B10 Right Tower ---
	glColor3f(0.68f, 0.38f, 0.35f);
	glBegin(GL_QUADS);
	glVertex2f(1530.0f, 160.0f);
	glVertex2f(1600.0f, 160.0f);
	glVertex2f(1600.0f, 420.0f);
	glVertex2f(1530.0f, 420.0f);
	glEnd();
	drawWindowGrid(1530.0f, 160.0f, 70.0f, 260.0f, 9, 2);
	drawRectOutline(1530.0f, 160.0f, 1600.0f, 420.0f, 3.0f);

	// ---  Low buildings ---
	glColor3f(0.82f, 0.70f, 0.55f);
	drawRect(55.0f, 160.0f, 130.0f, 320.0f);
	drawWindowGrid(55.0f, 160.0f, 75.0f, 160.0f, 5, 2);
	drawRectOutline(55.0f, 160.0f, 130.0f, 320.0f, 3.0f);

	glColor3f(0.70f, 0.42f, 0.40f);
	drawRect(250.0f, 160.0f, 330.0f, 350.0f);
	drawWindowGrid(250.0f, 160.0f, 80.0f, 190.0f, 6, 2);
	drawRectOutline(250.0f, 160.0f, 330.0f, 350.0f, 3.0f);

	glColor3f(0.76f, 0.65f, 0.50f);
	drawRect(610.0f, 160.0f, 695.0f, 330.0f);
	drawWindowGrid(610.0f, 160.0f, 85.0f, 170.0f, 5, 2);
	drawRectOutline(610.0f, 160.0f, 695.0f, 330.0f, 3.0f);

	glColor3f(0.50f, 0.66f, 0.72f);
	drawRect(970.0f, 160.0f, 1060.0f, 345.0f);
	drawWindowGrid(970.0f, 160.0f, 90.0f, 185.0f, 6, 2, true);
	drawRectOutline(970.0f, 160.0f, 1060.0f, 345.0f, 3.0f);

	glColor3f(0.80f, 0.48f, 0.45f);
	drawRect(1080.0f, 160.0f, 1160.0f, 320.0f);
	drawWindowGrid(1080.0f, 160.0f, 80.0f, 160.0f, 5, 2);
	drawRectOutline(1080.0f, 160.0f, 1160.0f, 320.0f, 3.0f);

	glColor3f(0.70f, 0.55f, 0.50f);
	drawRect(1370.0f, 160.0f, 1440.0f, 305.0f);
	drawWindowGrid(1370.0f, 160.0f, 70.0f, 145.0f, 5, 2);
	drawRectOutline(1370.0f, 160.0f, 1440.0f, 305.0f, 3.0f);
}

void streetLight(float x) {
	glColor3f(0.12f, 0.14f, 0.16f);
	glLineWidth(4.0f);
	glBegin(GL_LINES);
	glVertex2f(x, 160.0f);
	glVertex2f(x, 235.0f);
	glVertex2f(x, 235.0f);
	glVertex2f(x + 22.0f, 235.0f);
	glEnd();

	glColor3f(1.0f, 0.85f, 0.30f);
	drawCircle(x + 25.0f, 232.0f, 6.0f);
}

void drawStreetLights() {
	streetLight(80);
	streetLight(300);
	streetLight(520);
	streetLight(740);
	streetLight(960);
	streetLight(1140);
	streetLight(1360);
	streetLight(1580);
}

void drawCrashPlane(float x, float y) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glRotatef(-12.0f, 0.0f, 0.0f, 1.0f);

	// Fuselage and nose.
	glColor3f(0.88f, 0.90f, 0.92f);
	drawRect(-65.0f, -10.0f, 50.0f, 10.0f);
	drawTriangle(50.0f, -10.0f, 76.0f, 0.0f, 50.0f, 10.0f);

	// Wings and tail.
	glColor3f(0.55f, 0.60f, 0.68f);
	drawTriangle(-15.0f, 5.0f, 25.0f, 5.0f, -25.0f, 42.0f);
	drawTriangle(-15.0f, -5.0f, 25.0f, -5.0f, -25.0f, -42.0f);
	drawTriangle(-58.0f, 7.0f, -42.0f, 7.0f, -62.0f, 28.0f);

	// Cockpit window.
	glColor3f(0.12f, 0.25f, 0.35f);
	drawCircle(51.0f, 2.0f, 4.0f, 12);
	glPopMatrix();
}

void drawCrashEffect() {
	if (!crashImpacted)
		return;

	// A short fire flash followed by simple rising smoke.
	if (crashEffectTimer < 0.7f) {
		glColor3f(1.0f, 0.35f, 0.03f);
		drawCircle(CRASH_TARGET_X, CRASH_TARGET_Y, 30.0f, 24);
		glColor3f(1.0f, 0.82f, 0.08f);
		drawCircle(CRASH_TARGET_X - 7.0f, CRASH_TARGET_Y, 16.0f, 20);
	}

	float rise = std::min(crashEffectTimer * 22.0f, 115.0f);
	glColor3f(0.20f, 0.20f, 0.22f);
	drawCircle(CRASH_TARGET_X, CRASH_TARGET_Y + 18.0f + rise, 22.0f, 20);
	drawCircle(CRASH_TARGET_X + 14.0f, CRASH_TARGET_Y + 48.0f + rise * 0.75f,
			   27.0f, 20);
	glColor3f(0.32f, 0.32f, 0.34f);
	drawCircle(CRASH_TARGET_X - 10.0f, CRASH_TARGET_Y + 78.0f + rise * 0.5f,
			   31.0f, 20);
}

void resetCrashAnimation() {
	crashStarted = false;
	crashImpacted = false;
	crashPlaneX = -140.0f;
	crashPlaneY = 760.0f;
	crashEffectTimer = 0.0f;
}

void startCrashAnimation() {
	resetCrashAnimation();
	crashStarted = true;
}

void drawRoad() {
	// Grass buffer
	glColor3f(0.25f, 0.55f, 0.25f);
	drawRect(0.0f, 130.0f, (float)WIN_W, 160.0f);

	// Sidewalk
	glColor3f(0.70f, 0.70f, 0.70f);
	drawRect(0.0f, 95.0f, (float)WIN_W, 130.0f);

	glColor3f(0.55f, 0.55f, 0.55f);
	glLineWidth(1.0f);
	glBegin(GL_LINES);
	for (float x = 0.0f; x <= (float)WIN_W; x += 40.0f) {
		glVertex2f(x, 95.0f);
		glVertex2f(x, 130.0f);
	}
	glEnd();

	// road
	glColor3f(0.12f, 0.13f, 0.15f);
	drawRect(0.0f, 0.0f, (float)WIN_W, 95.0f);

	// Yellow
	glColor3f(0.95f, 0.85f, 0.20f);
	drawRect(0.0f, 45.0f, (float)WIN_W, 49.0f);

	// White
	glColor3f(0.95f, 0.95f, 0.95f);
	for (float x = 0.0f; x < (float)WIN_W; x += 100.0f) {
		drawRect(x, 72.0f, x + 55.0f, 76.0f);
		drawRect(x, 18.0f, x + 55.0f, 22.0f);
	}
}

// ============================================================
// GLOBAL WEATHER EFFECTS
// ============================================================
void drawRain() {
	if (!rainMode)
		return;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.65f, 0.78f, 0.95f, 0.70f);
	glLineWidth(2.0f);

	glBegin(GL_LINES);
	for (int x = -100; x < WIN_W + 100; x += 35) {
		for (int y = -100; y < WIN_H + 100; y += 70) {
			float yy = fmodf((float)y - rainOffset, (float)(WIN_H + 120));
			if (yy < -60.0f)
				yy += WIN_H + 120.0f;
			glVertex2f((float)x, yy);
			glVertex2f((float)x - 12.0f, yy - 28.0f);
		}
	}
	glEnd();

	glDisable(GL_BLEND);
}

// ---------------------- Scene3 rendering ----------------------
void renderScene3() {
	if (isNightMode)
		glClearColor(0.04f, 0.05f, 0.18f, 1.0f);
	else
		glClearColor(0.52f, 0.78f, 0.96f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT);
	drawPartialCity();

	if (isNightMode)
		drawNightOverlay();
}

// SCENE COMPOSITION & CALLBACKS

void renderScene4() {
	if (isNightMode) {
		glClearColor(0.04f, 0.05f, 0.18f, 1.0f);
	} else {
		glClearColor(0.52f, 0.78f, 0.96f, 1.0f);
	}
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();
	glTranslatef(550.0f, 250.0f, 0.0f);
	sun();
	glPopMatrix();

	drawClouds();
	drawMountains();
	drawHills();

	drawCity();
	drawStreetLights();
	drawRoad();
	drawTrees();

	if (crashStarted && !crashImpacted)
		drawCrashPlane(crashPlaneX, crashPlaneY);
	drawCrashEffect();

	if (isNightMode) {
		drawNightOverlay();
	}
}

void display() {
	if (currentScene == 1) {
		if (isNightMode)
			glClearColor(0.04f, 0.05f, 0.18f, 1.0f);
		else
			glClearColor(0.58f, 0.78f, 0.92f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT);
		scene1();

		if (bombingStarted) {
			drawCargoPlane(planeX, planeY, 0.8f);
			for (int i = 0; i < NUM_BOMBS; i++)
				if (bombDropped[i] && !bombHit[i])
					drawBomb(bombX[i], bombY[i]);

			if (flashActive)
				drawExplosionFlash();
		}

		if (isNightMode)
			drawNightOverlay();
	} else if (currentScene == 2) {
		renderScene2();
		if (isNightMode)
			drawNightOverlay();
	} else if (currentScene == 3) {
		renderScene3();
	} else if (currentScene == 4) {
		renderScene4();
	}

	// Rain is drawn last so it appears over every scene.
	drawRain();
	glFlush();
}

void update(int value) {
	cloudOffset += CLOUD_SPEED;
	firePhase += FIRE_SPEED;
	smokeOffset += SMOKE_DRIFT_SPEED;

	if (rainMode) {
		rainOffset += 18.0f;
		if (rainOffset > WIN_H + 120.0f)
			rainOffset = 0.0f;
	}

	if (crashStarted && currentScene == 4) {
		if (!crashImpacted) {
			crashPlaneX += CRASH_PLANE_SPEED;
			crashPlaneY -= 1.45f;

			// The plane's nose reaches the left face of the first tower.
			if (crashPlaneX + 76.0f >= CRASH_TARGET_X) {
				crashImpacted = true;
				crashEffectTimer = 0.0f;
			}
		} else if (crashEffectTimer < 8.0f) {
			crashEffectTimer += 0.016f;
		}
	}

	if (bombingStarted && currentScene == 1) {
		planeX += PLANE_SPEED;

		for (int i = 0; i < NUM_BOMBS; i++) {
			if (!bombDropped[i] && planeX >= bombDropPosition[i]) {
				bombDropped[i] = true;
				bombX[i] = planeX + 20.0f;
				bombY[i] = planeY - 45.0f;
			}

			if (bombDropped[i] && !bombHit[i]) {
				bombY[i] -= 9.0f;
				if (bombY[i] <= 160.0f) {
					bombY[i] = 160.0f;
					bombHit[i] = true;
					flashActive = true;
					flashTimer = 1.2f;
				}
			}
		}

		if (flashActive) {
			flashTimer -= 0.016f;
			if (flashTimer <= 0.0f) {
				flashTimer = 0.0f;
				flashActive = false;
			}
		}

		bool allBombsHit = true;
		for (int i = 0; i < NUM_BOMBS; i++)
			if (!bombHit[i])
				allBombsHit = false;

		if (allBombsHit && !flashActive) {
			transitionTimer += 0.056f;
			if (transitionTimer > 0.5f) {
				bombingStarted = false;
				currentScene = 2;
			}
		}
	}

	glutPostRedisplay();
	glutTimerFunc(16, update, 0);
}

// Direct scene switching helper
void switchScene(int scene) {
	if (scene < 1)
		scene = 1;
	if (scene > 4)
		scene = 4;

	// Reset scene-specific animations when leaving or re-entering their scene.
	if (scene != currentScene || scene == 1)
		resetBombingAnimation();
	if (scene != currentScene || scene == 4)
		resetCrashAnimation();

	currentScene = scene;
	glutPostRedisplay();
}

// Mouse click starts the bombing only in Scene 1.
void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && currentScene == 1 &&
		!bombingStarted) {
		resetBombingAnimation();
		bombingStarted = true;
	}

	glutPostRedisplay();
}

// Arrow keys move through scenes.
void specialKeys(int key, int x, int y) {
	if (key == GLUT_KEY_RIGHT)
		switchScene(currentScene == 4 ? 1 : currentScene + 1);
	else if (key == GLUT_KEY_LEFT)
		switchScene(currentScene == 1 ? 4 : currentScene - 1);
}

// D = day, N = night, R = rain, P = Scene 4 plane animation, 1-4 = scene.
void keyboard(unsigned char key, int x, int y) {
	if ((key == 'p' || key == 'P') && currentScene == 4)
		startCrashAnimation();
	else if (key == 'd' || key == 'D')
		isNightMode = false;
	else if (key == 'n' || key == 'N')
		isNightMode = true;
	else if (key == 'r' || key == 'R')
		rainMode = !rainMode;
	else if (key >= '1' && key <= '4')
		switchScene(key - '0');

	glutPostRedisplay();
}

void init() {
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
	glutSpecialFunc(specialKeys);
	glutMouseFunc(mouse);
	glutTimerFunc(16, update, 0);
	glutMainLoop();
	return 0;
}
