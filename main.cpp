// Glut Project - Green Mountains with Hills, Shadows, and Trees
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

#define WIN_W 1600
#define WIN_H 1000
#define PI 3.14159265358979323846

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

// Helper for smooth rolling hill curves (semi-ellipse)
void drawHill(float cx, float cy, float rx, float ry, float r, float g, float b) {
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
    Color3 wall, roof, door, knob, window, windowFrame, chimney, smoke;
};

const HutColors HutColor1 = {
    {0.85f, 0.65f, 0.45f}, {0.65f, 0.16f, 0.16f},
    {0.40f, 0.22f, 0.10f}, {1.0f, 0.85f, 0.2f},
    {0.68f, 0.85f, 0.90f}, {0.30f, 0.20f, 0.10f},
    {0.55f, 0.35f, 0.30f}, {0.85f, 0.85f, 0.85f}
};

//------------------------- Trees, Mountains & Hills -------------------------

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

// Green Mountain range (Lowered by 120 units)
void drawMountains() {
    glPushMatrix();
    glTranslatef(0.0f, -120.0f, 0.0f); // Adjust this Y offset to move higher/lower

    Color3 litGreen = { 0.32f, 0.58f, 0.23f }; // Light facing slope
    Color3 shadowGreen = { 0.16f, 0.34f, 0.14f }; // Shadow slope

    // --- 1. LEFT MOUNTAIN ---
    glColor3f(litGreen.r, litGreen.g, litGreen.b);
    drawTriangle(50.0f, 300.0f, 330.0f, 720.0f, 330.0f, 300.0f);
    glColor3f(shadowGreen.r, shadowGreen.g, shadowGreen.b);
    drawTriangle(330.0f, 300.0f, 330.0f, 720.0f, 600.0f, 300.0f);

    // --- 2. CENTER TALL MOUNTAIN ---
    glColor3f(litGreen.r, litGreen.g, litGreen.b);
    drawTriangle(280.0f, 300.0f, 570.0f, 860.0f, 570.0f, 300.0f);
    glColor3f(shadowGreen.r, shadowGreen.g, shadowGreen.b);
    drawTriangle(570.0f, 300.0f, 570.0f, 860.0f, 1150.0f, 300.0f);

    // --- 3. RIGHT MOUNTAIN ---
    glColor3f(litGreen.r, litGreen.g, litGreen.b);
    drawTriangle(820.0f, 300.0f, 1050.0f, 650.0f, 1050.0f, 300.0f);
    glColor3f(shadowGreen.r, shadowGreen.g, shadowGreen.b);
    drawTriangle(1050.0f, 300.0f, 1050.0f, 650.0f, 1450.0f, 300.0f);

    glPopMatrix();
}

// Rolling Green Hills
void drawHills() {
    // Back Left Hill
    drawHill(250.0f, 250.0f, 400.0f, 180.0f, 0.25f, 0.48f, 0.20f);

    // Back Right Hill
    drawHill(1350.0f, 250.0f, 450.0f, 190.0f, 0.22f, 0.44f, 0.18f);

    // Front Center Rolling Hill
    drawHill(800.0f, 230.0f, 500.0f, 200.0f, 0.28f, 0.52f, 0.22f);
}

//------------------------- Hut & Environment -------------------------
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
    glVertex2f(235.0f, 270.0f); glVertex2f(235.0f, 320.0f);
    glVertex2f(210.0f, 295.0f); glVertex2f(260.0f, 295.0f);
    glVertex2f(365.0f, 270.0f); glVertex2f(365.0f, 320.0f);
    glVertex2f(340.0f, 295.0f); glVertex2f(390.0f, 295.0f);
    glEnd();
}

void sun() {
    glColor3f(1.0f, 0.85f, 0.0f);
    drawCircle(820.0f, 880.0f, 50.0f, 40);
}

void drawGroundAndPath() {
    // Ground
    glColor3f(0.28f, 0.52f, 0.22f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(WIN_W, 0.0f);
    glVertex2f(WIN_W, WIN_H * 0.33f);
    glVertex2f(0.0f, WIN_H * 0.33f);
    glEnd();

    // Path
    glColor3f(0.55f, 0.42f, 0.28f);
    glBegin(GL_QUAD_STRIP);
    glVertex2f(0.0f, 60.0f);   glVertex2f(0.0f, 100.0f);
    glVertex2f(400.0f, 70.0f);  glVertex2f(400.0f, 110.0f);
    glVertex2f(800.0f, 55.0f);  glVertex2f(800.0f, 95.0f);
    glVertex2f(1200.0f, 75.0f); glVertex2f(1200.0f, 115.0f);
    glVertex2f(1600.0f, 60.0f); glVertex2f(1600.0f, 100.0f);
    glEnd();
}

//------------------------- Display & Main -------------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // 1. Sun
    sun();

    // 2. High Mountains
    drawMountains();

    // 3. Rolling Hills (Overlapping mountain base)
    drawHills();

    // 4. Midground Trees
    drawTree(120.0f, 290.0f, 0.65f);
    drawTree(220.0f, 285.0f, 0.80f);
    drawTree(720.0f, 290.0f, 0.75f);
    drawTree(1180.0f, 285.0f, 0.85f);
    drawTree(1350.0f, 290.0f, 0.70f);

    // 5. Ground and Path
    drawGroundAndPath();

    // 6. Foreground Trees
    drawTree(80.0f, 120.0f, 1.1f);
    drawTree(1480.0f, 100.0f, 1.2f);


    glFlush();
}

void init() {
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, WIN_W, 0.0, WIN_H);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WIN_W, WIN_H);
    glutCreateWindow("Green Mountains & Hills Scene");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
