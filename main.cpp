#include <GL/glut.h>
#include <math.h>

#define WIN_W 1600
#define WIN_H 1000
#define PI 3.14159265358979323846

// -----------------------------------------------------------------------------
// Global Variables & Animation State
// -----------------------------------------------------------------------------
float planeX = -300.0f;
float planeY = 750.0f;
const float PLANE_SPEED = 3.5f;
const float GROUND_Y = 220.0f;

// -----------------------------------------------------------------------------
// Drawing Helpers
// -----------------------------------------------------------------------------
void drawCircle(float cx, float cy, float r, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * (float)PI * i / segments;
        glVertex2f(cx + r * cosf(angle), cy + r * sinf(angle));
    }
    glEnd();
}

void drawRect(float x1, float y1, float x2, float y2) {
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

// Ground and Sky Environment
void drawEnvironment() {
    // Sky background
    glClearColor(0.52f, 0.72f, 0.88f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Green Grass Ground
    glColor3f(0.30f, 0.55f, 0.22f);
    drawRect(0.0f, 0.0f, WIN_W, GROUND_Y);

    // Earth Dirt Layer
    glColor3f(0.40f, 0.30f, 0.20f);
    drawRect(0.0f, 0.0f, WIN_W, GROUND_Y - 20.0f);
}

// -----------------------------------------------------------------------------
// C-130 Military Transport Aircraft
// -----------------------------------------------------------------------------
void drawCargoPlane(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);

    // 1. Upper Fuselage (Light Grey-Blue)
    glColor3f(0.56f, 0.59f, 0.63f);
    glBegin(GL_POLYGON);
    glVertex2f(190.0f, -10.0f);
    glVertex2f(170.0f, 15.0f);
    glVertex2f(140.0f, 25.0f);
    glVertex2f(-110.0f, 25.0f);
    glVertex2f(-140.0f, 15.0f);
    glVertex2f(-175.0f, 15.0f);
    glVertex2f(-210.0f, 0.0f);
    glVertex2f(-130.0f, -15.0f);
    glVertex2f(140.0f, -15.0f);
    glVertex2f(175.0f, -25.0f);
    glEnd();

    // Lower Belly Strip (Light Gray Tone)
    glColor3f(0.80f, 0.83f, 0.87f);
    glBegin(GL_POLYGON);
    glVertex2f(190.0f, -10.0f);
    glVertex2f(175.0f, -25.0f);
    glVertex2f(140.0f, -32.0f);
    glVertex2f(-60.0f, -32.0f);
    glVertex2f(-130.0f, -15.0f);
    glVertex2f(140.0f, -15.0f);
    glEnd();

    // Dark Triangular Base Fillet Patch (shadow tone, matches fuselage hue)
    glColor3f(0.42f, 0.47f, 0.53f);
    glBegin(GL_POLYGON);
    glVertex2f(-115.0f, 25.0f);
    glVertex2f(-140.0f, 60.0f);
    glVertex2f(-138.0f, 25.0f);
    glEnd();

    // Outlines
    glColor3f(0.12f, 0.14f, 0.16f);
    glLineWidth(1.8f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(190.0f, -10.0f);
    glVertex2f(170.0f, 15.0f);
    glVertex2f(140.0f, 25.0f);
    glVertex2f(-110.0f, 25.0f);
    glVertex2f(-140.0f, 15.0f);
    glVertex2f(-175.0f, 15.0f);
    glVertex2f(-210.0f, 0.0f);
    glVertex2f(-130.0f, -32.0f);
    glVertex2f(-60.0f, -32.0f);
    glVertex2f(140.0f, -32.0f);
    glVertex2f(175.0f, -25.0f);
    glEnd();

    // 1b. Vertical Tail Fin (separate shape/shade so it reads as a fin, not a wash)
    glColor3f(0.48f, 0.52f, 0.57f);
    glBegin(GL_POLYGON);
    glVertex2f(-110.0f, 25.0f);
    glVertex2f(-150.0f, 130.0f);
    glVertex2f(-175.0f, 130.0f);
    glVertex2f(-175.0f, 15.0f);
    glVertex2f(-140.0f, 15.0f);
    glEnd();

    // Dark anti-glare cap near the fin tip (typical on real C-130 tails)
    glColor3f(0.14f, 0.16f, 0.18f);
    glBegin(GL_POLYGON);
    glVertex2f(-153.0f, 95.0f);
    glVertex2f(-158.0f, 125.0f);
    glVertex2f(-170.0f, 125.0f);
    glVertex2f(-168.0f, 95.0f);
    glEnd();

    // Fin outline for definition against the sky
    glColor3f(0.12f, 0.14f, 0.16f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-110.0f, 25.0f);
    glVertex2f(-150.0f, 130.0f);
    glVertex2f(-175.0f, 130.0f);
    glVertex2f(-175.0f, 15.0f);
    glVertex2f(-140.0f, 15.0f);
    glEnd();

    // Belly Line
    glBegin(GL_LINES);
    glVertex2f(190.0f, -10.0f);
    glVertex2f(-130.0f, -15.0f);
    glEnd();

    // 2. Horizontal Tail Stabilizer
    glColor3f(0.55f, 0.61f, 0.67f);
    glBegin(GL_POLYGON);
    glVertex2f(-150.0f, 10.0f);
    glVertex2f(-205.0f, -5.0f);
    glVertex2f(-190.0f, -5.0f);
    glVertex2f(-140.0f, 10.0f);
    glEnd();

    glColor3f(0.12f, 0.14f, 0.16f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-150.0f, 10.0f);
    glVertex2f(-205.0f, -5.0f);
    glVertex2f(-190.0f, -5.0f);
    glVertex2f(-140.0f, 10.0f);
    glEnd();

    // 3. Wing Shadow & Engine Nacelle
    glColor3f(0.45f, 0.50f, 0.56f);
    glBegin(GL_POLYGON);
    glVertex2f(45.0f, 25.0f);
    glVertex2f(50.0f, 5.0f);
    glVertex2f(-40.0f, 5.0f);
    glVertex2f(-50.0f, 25.0f);
    glEnd();

    glColor3f(0.62f, 0.67f, 0.73f);
    glBegin(GL_POLYGON);
    glVertex2f(45.0f, 25.0f);
    glVertex2f(35.0f, 38.0f);
    glVertex2f(-35.0f, 38.0f);
    glVertex2f(-50.0f, 25.0f);
    glEnd();

    glColor3f(0.12f, 0.14f, 0.16f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(45.0f, 25.0f);
    glVertex2f(35.0f, 38.0f);
    glVertex2f(-35.0f, 38.0f);
    glVertex2f(-50.0f, 25.0f);
    glEnd();

    // Engine Nacelle Housing
    glColor3f(0.56f, 0.61f, 0.67f);
    glBegin(GL_POLYGON);
    glVertex2f(55.0f, 5.0f);
    glVertex2f(55.0f, 22.0f);
    glVertex2f(10.0f, 22.0f);
    glVertex2f(-5.0f, 5.0f);
    glEnd();

    glColor3f(0.12f, 0.14f, 0.16f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(55.0f, 5.0f);
    glVertex2f(55.0f, 22.0f);
    glVertex2f(10.0f, 22.0f);
    glVertex2f(-5.0f, 5.0f);
    glEnd();

    // Spinner Hub & Propellers
    glColor3f(0.15f, 0.15f, 0.18f);
    glBegin(GL_POLYGON);
    glVertex2f(55.0f, 22.0f);
    glVertex2f(68.0f, 13.5f);
    glVertex2f(55.0f, 5.0f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(58.0f, 13.5f);
    glVertex2f(55.0f, 65.0f);
    glVertex2f(52.0f, 13.5f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(58.0f, 13.5f);
    glVertex2f(55.0f, -38.0f);
    glVertex2f(52.0f, 13.5f);
    glEnd();

    // 4. Wheel Sponson (Belly Pod)
    glColor3f(0.52f, 0.57f, 0.63f);
    glBegin(GL_POLYGON);
    glVertex2f(55.0f, -12.0f);
    glVertex2f(55.0f, -38.0f);
    glVertex2f(-55.0f, -38.0f);
    glVertex2f(-65.0f, -12.0f);
    glEnd();

    glColor3f(0.22f, 0.25f, 0.28f);
    glBegin(GL_POLYGON);
    glVertex2f(55.0f, -28.0f);
    glVertex2f(55.0f, -38.0f);
    glVertex2f(-55.0f, -38.0f);
    glVertex2f(-65.0f, -28.0f);
    glEnd();

    glColor3f(0.12f, 0.14f, 0.16f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(55.0f, -12.0f);
    glVertex2f(55.0f, -38.0f);
    glVertex2f(-55.0f, -38.0f);
    glVertex2f(-65.0f, -12.0f);
    glEnd();

    // 5. Windows & Details
    glColor3f(0.10f, 0.12f, 0.14f);
    glBegin(GL_POLYGON);
    glVertex2f(150.0f, 8.0f);
    glVertex2f(165.0f, 8.0f);
    glVertex2f(158.0f, 18.0f);
    glVertex2f(148.0f, 18.0f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(150.0f, -2.0f);
    glVertex2f(165.0f, -2.0f);
    glVertex2f(165.0f, 6.0f);
    glVertex2f(150.0f, 6.0f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(140.0f, 8.0f);
    glVertex2f(146.0f, 8.0f);
    glVertex2f(145.0f, 18.0f);
    glVertex2f(139.0f, 18.0f);
    glEnd();

    drawCircle(118.0f, -10.0f, 4.0f, 16);
    drawCircle(95.0f, -10.0f, 4.0f, 16);
    drawCircle(40.0f, -8.0f, 3.5f, 16);

    // Door and Markings
    glColor3f(0.15f, 0.18f, 0.20f);
    glLineWidth(1.2f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(132.0f, -28.0f);
    glVertex2f(120.0f, -28.0f);
    glVertex2f(120.0f, 0.0f);
    glVertex2f(132.0f, 0.0f);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex2f(-30.0f, -25.0f);
    glVertex2f(-45.0f, -25.0f);
    glVertex2f(-45.0f, 5.0f);
    glVertex2f(-30.0f, 5.0f);
    glEnd();

    glColor3f(0.92f, 0.94f, 0.96f);
    glBegin(GL_QUADS);
    glVertex2f(110.0f, 8.0f);
    glVertex2f(85.0f, 8.0f);
    glVertex2f(85.0f, 11.0f);
    glVertex2f(110.0f, 11.0f);

    glVertex2f(-10.0f, 5.0f);
    glVertex2f(-25.0f, 5.0f);
    glVertex2f(-25.0f, 8.0f);
    glVertex2f(-10.0f, 8.0f);
    glEnd();

    glColor3f(0.70f, 0.72f, 0.75f);
    drawRect(-168.0f, 75.0f, -164.0f, 110.0f);
    drawRect(-155.0f, 85.0f, -145.0f, 92.0f);

    glPopMatrix();
}

// -----------------------------------------------------------------------------
// GLUT Callbacks
// -----------------------------------------------------------------------------
void display() {
    drawEnvironment();
    drawCargoPlane(planeX, planeY, 0.8f);
    glFlush();
}

void update(int value) {
    // Move Cargo Plane Forward, wrap around when off-screen
    planeX += PLANE_SPEED;
    if (planeX > WIN_W + 350.0f) {
        planeX = -350.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // ~60 FPS
}

void init() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, WIN_W, 0.0, WIN_H);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WIN_W, WIN_H);
    glutCreateWindow("C-130 Cargo Plane - Fly By");
    init();

    glutDisplayFunc(display);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
