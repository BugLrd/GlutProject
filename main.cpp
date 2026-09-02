#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include <vector>

#define WIN_W 1600
#define WIN_H 1000
#define PI 3.14159265358979323846
#define MAX_USER_FIRES 15
#define MAX_DEBRIS 15
#define MAX_RAINDROPS 300

bool isNightMode = false;
bool isRaining = false;

// Animation state for fire/smoke

const float FIRE_SPEED = 0.12f;
const float SMOKE_DRIFT_SPEED = 0.25f; // smoke increment 
float smokeOffset = 0.0f;
float fireY = 0.0f;
float windForce = 0.0f;

float rainX[MAX_RAINDROPS];
float rainY[MAX_RAINDROPS];

float clickFireX[MAX_USER_FIRES];
float clickFireY[MAX_USER_FIRES];
int fireCount = 0;

float debrisX[MAX_DEBRIS];
float debrisY[MAX_DEBRIS];
int debrisCount = 0;

float cloudOffset1 = 0.0f;
float cloudOffset2 = 0.0f;
float cloudOffset3 = 0.0f;
float cloudOffset4 = 0.0f;
const float CLOUD_SPEED = 0.8f;


void init_rain() {
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        rainX[i] = rand() % WIN_W; // assign random x position within window width
        rainY[i] = rand() % WIN_H; // assign random y position within window height
    }
}

void update_rain() {
    if (!isRaining) return;

    for (int i = 0; i < MAX_RAINDROPS; i++) {
        rainY[i] -= 15;        // dropping 
        rainX[i] += windForce; // to move with the rain 

        // Reset 
        if (rainY[i] < 0) {
            rainY[i] = WIN_H;
            rainX[i] = rand() % WIN_W;
        }
    }
}

void rain() {
    if (!isRaining) return;

    glColor3f(0.7f, 0.8f, 1.0f);
    glLineWidth(1.5f);

    glBegin(GL_LINES);
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        glVertex2i(rainX[i], rainY[i]);
        glVertex2i(rainX[i] - windForce, rainY[i] - 15);
    }
    glEnd();
}

//Basic Drawing Helpers 
void rect(float x1, float y1, float x2, float y2) {
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

void triangle(float x1, float y1, float x2, float y2, float x3, float y3) {
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

void circle(float cx, float cy, float r, float red, float green, float blue) {
    glColor3f(red, green, blue);
    glBegin(GL_POLYGON);

    float pi = 3.1416f;
    for (int i = 0; i < 200; i++) {
        float A = (i * 2 * pi) / 200;
        float x = cx + r * cos(A);
        float y = cy + r * sin(A);
        glVertex2f(x, y);
    }

    glEnd();
}

void small_tree(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    glColor3f(0.10f, 0.08f, 0.05f);
    rect(-4.0f, 0.0f, 4.0f, 15.0f);
    glColor3f(0.08f, 0.22f, 0.10f);
    triangle(-25.0f, 10.0f, 0.0f, 10.0f, 0.0f, 45.0f);
    glColor3f(0.12f, 0.28f, 0.14f);
    triangle(0.0f, 10.0f, 25.0f, 10.0f, 0.0f, 45.0f);
    glColor3f(0.08f, 0.22f, 0.10f);
    triangle(-20.0f, 30.0f, 0.0f, 30.0f, 0.0f, 65.0f);
    glColor3f(0.12f, 0.28f, 0.14f);
    triangle(0.0f, 30.0f, 20.0f, 30.0f, 0.0f, 65.0f);
    glColor3f(0.08f, 0.22f, 0.10f);
    triangle(-15.0f, 50.0f, 0.0f, 50.0f, 0.0f, 80.0f);
    glColor3f(0.12f, 0.28f, 0.14f);
    triangle(0.0f, 50.0f, 15.0f, 50.0f, 0.0f, 80.0f);
    glPopMatrix();
}

void mountains() {
    // Left
    glColor3f(0.32f, 0.50f, 0.25f);
    triangle(50.0f, 200.0f, 300.0f, 650.0f, 300.0f, 200.0f);
    glColor3f(0.18f, 0.30f, 0.14f);
    triangle(300.0f, 200.0f, 300.0f, 650.0f, 600.0f, 200.0f);

    // Center
    glColor3f(0.32f, 0.50f, 0.25f);
    triangle(200.0f, 200.0f, 570.0f, 750.0f, 570.0f, 200.0f);
    glColor3f(0.18f, 0.30f, 0.14f);
    triangle(570.0f, 200.0f, 570.0f, 750.0f, 1150.0f, 200.0f);

    // Right
    glColor3f(0.32f, 0.50f, 0.25f);
    triangle(800.0f, 200.0f, 1050.0f, 600.0f, 1050.0f, 200.0f);
    glColor3f(0.18f, 0.30f, 0.14f);
    triangle(1050.0f, 200.0f, 1050.0f, 600.0f, 1450.0f, 200.0f);
}

void hills() {
    // Left 
    circle(300.0f, 180.0f, 320.0f, 0.15f, 0.35f, 0.15f);

    // Right 
    circle(1300.0f, 180.0f, 300.0f, 0.20f, 0.45f, 0.20f);

    // Center 
    circle(800.0f, 120.0f, 350.0f, 0.25f, 0.55f, 0.25f);
}

void sun() {
    circle(520.0f, 520.0f, 40.0f, 1.0f, 0.85f, 0.0f);
}

void night_scene() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.02f, 0.02f, 0.15f, 0.65f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f((float)WIN_W, 0.0f);
    glVertex2f((float)WIN_W, (float)WIN_H);
    glVertex2f(0.0f, (float)WIN_H);
    glEnd();

    if (!isRaining) {
        glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
        glPointSize(2.5f);
        glBegin(GL_POINTS);
        float starPositions[][2] = {
            {80, 900},   {150, 850},  {230, 920},  {310, 870},  {400, 940},
            {480, 880},  {560, 910},  {650, 860},  {730, 930},  {820, 890},
            {900, 950},  {980, 870},  {1060, 920}, {1150, 880}, {1230, 940},
            {1310, 860}, {1400, 910}, {1480, 870}, {60, 780},   {200, 800},
            {350, 760},  {520, 790},  {700, 770},  {880, 800},  {1050, 760},
            {1220, 790}, {1400, 770}, {1550, 800}, {120, 700},  {950, 700}
        };
        for (int i = 0; i < 30; i++) {
            glVertex2f(starPositions[i][0], starPositions[i][1]);
        }
        glEnd();
        glPushMatrix();
        glTranslatef(550.0f, 250.0f, 0.0f);
        circle(520.0f, 520.0f, 40.0f, 0.92f, 0.92f, 0.85f); // Moon
        glPopMatrix();
    }

    glDisable(GL_BLEND);
}


void fluffy_cloud(float cx, float cy, float r, bool nightmode) {
    float red, green, blue;
    if (nightmode) {
        red = 0.3f;
        green = 0.35f;
        blue = 0.4f;
    }
    else {
        red = 0.95f;
        green = 0.95f;
        blue = 0.95f;
    }
    // Left
    circle(cx - 35, cy - 5, 25, red, green, blue);
    // Center
    circle(cx, cy, 32, red, green, blue);
    // Right
    circle(cx + 35, cy - 5, 25, red, green, blue);
    // Top and left
    circle(cx - 15, cy + 22, 23, red, green, blue);
    // Top and right
    circle(cx + 15, cy + 20, 21, red, green, blue);
}


void new_house_1() {
    glColor3ub(75, 60, 48);
    glBegin(GL_POLYGON);
    glVertex2f(-4.0f, -6.0f);
    glVertex2f(-2.2f, -4.8f);
    glVertex2f(0.5f, -5.0f);
    glVertex2f(2.8f, -4.5f);
    glVertex2f(4.2f, -6.0f);
    glEnd();

    glColor3ub(140, 90, 50);
    glBegin(GL_POLYGON);
    glVertex2f(-3.1f, -6.0f);
    glVertex2f(-2.5f, -6.0f);
    glVertex2f(-2.4f, -2.5f);
    glVertex2f(-2.8f, -2.2f);
    glVertex2f(-3.2f, -2.7f);

    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(2.0f, -6.0f);
    glVertex2f(2.6f, -6.0f);
    glVertex2f(3.5f, -3.2f);
    glVertex2f(3.0f, -3.0f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(-0.4f, -6.0f);
    glVertex2f(0.2f, -6.0f);
    glVertex2f(0.4f, -4.2f);
    glVertex2f(-0.2f, -4.0f);
    glEnd();

    glColor3ub(115, 70, 35);
    glBegin(GL_POLYGON);
    glVertex2f(-3.2f, -2.4f);
    glVertex2f(-0.8f, -3.5f);
    glVertex2f(-0.7f, -3.9f);
    glVertex2f(-3.1f, -2.8f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(-0.2f, -3.8f);
    glVertex2f(3.2f, -3.2f);
    glVertex2f(3.1f, -3.6f);
    glVertex2f(-0.3f, -4.2f);
    glEnd();

    glColor3ub(140, 35, 35);
    glBegin(GL_POLYGON);
    glVertex2f(-3.6f, -1.8f);
    glVertex2f(-1.5f, -3.0f);
    glVertex2f(-1.8f, -3.4f);
    glVertex2f(-3.8f, -2.1f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-0.8f, -2.8f);
    glVertex2f(0.2f, -1.0f);
    glVertex2f(0.8f, -2.5f);
    glVertex2f(0.3f, -2.7f);
    glVertex2f(-0.2f, -1.6f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(1.8f, -4.8f);
    glVertex2f(3.3f, -4.2f);
    glVertex2f(3.5f, -4.6f);
    glVertex2f(2.0f, -5.2f);
    glEnd();

    glColor3ub(195, 190, 180);
    glBegin(GL_POLYGON);
    glVertex2f(-4.3f, -5.7f);
    glVertex2f(-3.3f, -5.4f);
    glVertex2f(-3.1f, -6.0f);
    glVertex2f(-4.1f, -6.2f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-2.2f, -3.8f);
    glVertex2f(-1.4f, -3.6f);
    glVertex2f(-1.2f, -4.1f);
    glVertex2f(-2.0f, -4.3f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-0.8f, -5.2f);
    glVertex2f(-0.1f, -4.9f);
    glVertex2f(0.2f, -5.5f);
    glVertex2f(-0.5f, -5.7f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(1.5f, -3.6f);
    glVertex2f(2.5f, -3.4f);
    glVertex2f(2.7f, -4.0f);
    glVertex2f(1.7f, -4.1f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(2.4f, -5.2f);
    glVertex2f(3.6f, -4.8f);
    glVertex2f(3.8f, -5.5f);
    glVertex2f(2.6f, -5.8f);
    glEnd();
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

void black_house() {
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

    glColor3ub(140, 175, 200);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.9f, -3.1f);
    glVertex2f(-0.5f, -3.1f);
    glVertex2f(-0.8f, -2.6f);
    glVertex2f(-0.3f, -3.0f);
    glVertex2f(-0.2f, -2.5f);
    glVertex2f(-0.4f, -2.7f);
    glVertex2f(0.2f, -3.1f);
    glVertex2f(0.7f, -3.1f);
    glVertex2f(0.3f, -2.4f);
    glEnd();

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

void translate_new_house_1(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    glTranslatef(0.0f, 6.0f, 0.0f);
    new_house_1();
    glPopMatrix();
}

void translate_black_house(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    glTranslatef(0.0f, 5.9f, 0.0f);
    black_house();
    glPopMatrix();
}

void burnt_tree_1(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    glColor3f(0.22f * 0.6f, 0.15f * 0.6f, 0.09f * 0.6f);
    rect(-7.0f, 0.0f, 7.0f, 130.0f);
    glColor3f(0.05f, 0.05f, 0.05f);
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
    triangle(-18.0f, 10.0f, 0.0f, 10.0f, 0.0f, 35.0f);
    glPopMatrix();
}

void burnt_tree_2(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    glColor3f(0.20f, 0.13f, 0.08f);
    rect(-9.0f, 0.0f, 9.0f, 60.0f);
    glColor3f(0.16f, 0.28f, 0.10f);
    triangle(-45.0f, 40.0f, 0.0f, 40.0f, 0.0f, 95.0f);
    glColor3f(0.34f, 0.26f, 0.12f);
    triangle(0.0f, 40.0f, 40.0f, 40.0f, 0.0f, 90.0f);
    glColor3f(0.14f, 0.22f, 0.09f);
    triangle(-30.0f, 80.0f, 0.0f, 80.0f, 0.0f, 125.0f);
    glColor3f(0.18f, 0.12f, 0.08f);
    rect(-6.0f, 100.0f, 6.0f, 140.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-6.0f, 140.0f);
    glVertex2f(6.0f, 140.0f);
    glVertex2f(14.0f, 150.0f);
    glEnd();
    glColor3f(0.05f, 0.05f, 0.05f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 110.0f);
    glVertex2f(-25.0f, 130.0f);
    glEnd();
    glPopMatrix();
}



void brick(float x, float y, float w, float h, float rotation, float r, float g, float b) {
    float hw = w * 0.5f;
    float hh = h * 0.5f;
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(-hw, -hh);
    glVertex2f(hw, -hh);
    glVertex2f(hw, hh);
    glVertex2f(-hw, hh);
    glEnd();

    glColor3f(r * 0.55f, g * 0.55f, b * 0.55f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-hw, -hh);
    glVertex2f(hw, -hh);
    glVertex2f(hw, hh);
    glVertex2f(-hw, hh);
    glEnd();
    glPopMatrix();
}

void blob(float cx, float cy, float rx, float ry,
          float red, float green, float blue) {

    glColor3f(red, green, blue);

    glBegin(GL_POLYGON);

    glVertex2f(cx - rx,cy);
    glVertex2f(cx - rx * 0.6f, cy + ry);
    glVertex2f(cx + rx * 0.4f, cy + ry * 0.9f);
    glVertex2f(cx + rx,       cy + ry * 0.2f);
    glVertex2f(cx + rx * 0.7f, cy - ry * 0.7f);
    glVertex2f(cx - rx * 0.2f, cy - ry);
    glVertex2f(cx - rx * 0.8f, cy - ry * 0.5f);

    glEnd();
}

void crater_small(float x, float y, float scale) {

    // Outer
    blob(x, y,26.0f * scale,18.0f * scale,0.40f, 0.30f, 0.20f);

    // Inner 
    blob(x, y,14.0f * scale,10.0f * scale,0.14f, 0.13f, 0.12f);
}

void brick_pile_small(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    brick(-14.0f, 4.0f, 26.0f, 12.0f, -6.0f, 0.55f, 0.20f, 0.15f);
    brick(10.0f, 2.0f, 24.0f, 11.0f, 8.0f, 0.45f, 0.28f, 0.16f);
    brick(-2.0f, 14.0f, 22.0f, 10.0f, 3.0f, 0.62f, 0.34f, 0.18f);
    brick(20.0f, 12.0f, 20.0f, 10.0f, -10.0f, 0.55f, 0.20f, 0.15f);
    glPopMatrix();
}

void brick_pile_large(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    brick(-40.0f, 4.0f, 30.0f, 13.0f, -4.0f, 0.45f, 0.28f, 0.16f);
    brick(-8.0f, 3.0f, 32.0f, 13.0f, 5.0f, 0.55f, 0.20f, 0.15f);
    brick(26.0f, 5.0f, 28.0f, 12.0f, -8.0f, 0.62f, 0.34f, 0.18f);
    brick(52.0f, 2.0f, 26.0f, 12.0f, 10.0f, 0.55f, 0.20f, 0.15f);
    brick(-22.0f, 18.0f, 28.0f, 12.0f, 6.0f, 0.55f, 0.20f, 0.15f);
    brick(6.0f, 20.0f, 26.0f, 12.0f, -5.0f, 0.45f, 0.28f, 0.16f);
    brick(34.0f, 17.0f, 24.0f, 11.0f, 9.0f, 0.62f, 0.34f, 0.18f);
    brick(-6.0f, 34.0f, 22.0f, 10.0f, -12.0f, 0.45f, 0.28f, 0.16f);
    brick(18.0f, 33.0f, 20.0f, 9.0f, 4.0f, 0.55f, 0.20f, 0.15f);
    glPopMatrix();
}

void wood_plank(float x, float y, float length, float thickness, float rotation, float r, float g, float b) {
    float hl = length * 0.5f;
    float ht = thickness * 0.5f;
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(-hl, -ht);
    glVertex2f(hl, -ht);
    glVertex2f(hl, ht);
    glVertex2f(-hl, ht);
    glEnd();
    glPopMatrix();
}

void broken_beam(float x, float y, float length, float thickness, float rotation, float r, float g, float b) { // pointy beam 
    float hl = length * 0.5f;
    float ht = thickness * 0.5f;
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    glVertex2f(-hl, -ht);
    glVertex2f(length * 0.25f, -ht);
    glVertex2f(hl, 0.0f);
    glVertex2f(length * 0.30f, ht);
    glVertex2f(-hl, ht);
    glEnd();
    glPopMatrix();
}

void wood_piles(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    wood_plank(0.0f, 4.0f, 70.0f, 10.0f, 12.0f, 0.42f, 0.27f, 0.14f);
    broken_beam(20.0f, 10.0f, 55.0f, 9.0f, -20.0f, 0.22f, 0.15f, 0.09f);
    wood_plank(-15.0f, 14.0f, 50.0f, 8.0f, 35.0f, 0.55f, 0.38f, 0.20f);
    broken_beam(10.0f, 20.0f, 45.0f, 8.0f, 100.0f, 0.42f, 0.27f, 0.14f);
    glPopMatrix();
}

void broken_roof_piece(float x, float y, float w, float h, float rotation, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(-w * 0.5f, -h * 0.5f);
    glVertex2f(w * 0.5f, -h * 0.35f);
    glVertex2f(w * 0.4f, h * 0.5f);
    glVertex2f(-w * 0.45f, h * 0.4f);
    glEnd();
    glPopMatrix();
}

void multiple_roof(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    broken_roof_piece(-10.0f, 6.0f, 55.0f, 30.0f, 8.0f, 0.42f, 0.16f, 0.14f);
    broken_roof_piece(28.0f, 4.0f, 40.0f, 24.0f, -15.0f, 0.34f, 0.34f, 0.36f);
    broken_roof_piece(5.0f, 20.0f, 35.0f, 20.0f, 30.0f, 0.22f, 0.15f, 0.09f);
    glPopMatrix();
}

void broken_house(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    brick_pile_large(-45.0f, 8.0f, 0.9f);
    brick_pile_small(55.0f, 6.0f, 0.8f);
    broken_roof_piece(30.0f, 90.0f, 150.0f, 75.0f, -25.0f, 0.42f, 0.16f, 0.14f);
    glColor3f(0.26f, 0.24f, 0.22f);
    rect(0.0f, 20.0f, 26.0f, 110.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0f, 110.0f);
    glVertex2f(26.0f, 110.0f);
    glVertex2f(14.0f, 128.0f);
    glEnd();
    broken_beam(15.0f, 15.0f, 95.0f, 10.0f, 12.0f, 0.42f, 0.27f, 0.14f);
    wood_plank(-25.0f, 10.0f, 70.0f, 8.0f, -10.0f, 0.22f, 0.15f, 0.09f);
    glPopMatrix();
}

void fire(float x, float y, float scale) {
    if (isRaining) return;



    glPushMatrix();

    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);

    // red
    glColor3f(0.80f, 0.15f, 0.10f);
    triangle(-16.0f, 0.0f, 16.0f, 0.0f, 0.0f, 55.0f + fireY);

    // Orange 
    glColor3f(0.95f, 0.45f, 0.10f);
    triangle(-10.0f, 0.0f, 10.0f, 0.0f, 0.0f, 40.0f + fireY);
    // Yellow 
    glColor3f(1.0f, 0.80f, 0.20f);
    triangle(-5.0f, 0.0f, 5.0f, 0.0f, 0.0f, 24.0f + fireY);

    glPopMatrix();
}

void smoke(float x, float y) {

    // Bottom
    circle(x, y + 10.0f, 9.0f, 0.35f, 0.35f, 0.35f);

    // Middle
    float midX = x + windForce;
    float midY = y + smokeOffset + 28.0f; // starts above the bottom part

    circle(midX, midY, 14.0f, 0.35f, 0.35f, 0.35f);
    circle(midX - 10.0f, midY + 2.0f, 11.0f, 0.35f, 0.35f, 0.35f);
    circle(midX + 10.0f, midY + 2.0f, 11.0f, 0.35f, 0.35f, 0.35f);

    // Top
    float topX = x + (windForce * 4.0f); // so the top is seperated from the middle part so it moves faster then the middle part
    float topY = y + (smokeOffset * 1.5f) + 50.0f;
    circle(topX, topY, 18.0f, 0.35f, 0.35f, 0.35f);
    circle(topX - 13.0f, topY + 3.0f, 14.0f, 0.35f, 0.35f, 0.35f);
    circle(topX + 13.0f, topY + 3.0f, 14.0f, 0.35f, 0.35f, 0.35f);
}
//--------------------------------------------------------------------------------------------------boom boom gap ---------------------------------------------------------------
void scene_2_ground() {
    glColor3f(0.34f, 0.42f, 0.24f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(WIN_W, 0.0f);
    glVertex2f(WIN_W, WIN_H * 0.33f);
    glVertex2f(0.0f, WIN_H * 0.33f);
    glEnd();

    glColor3f(0.40f, 0.30f, 0.20f);


    // Road 
    glBegin(GL_POLYGON);


    glVertex2f(0.0f, 105.0f);
    glVertex2f(300.0f, 120.0f);
    glVertex2f(650.0f, 100.0f);
    glVertex2f(1000.0f, 125.0f);
    glVertex2f(1350.0f, 105.0f);
    glVertex2f((float)WIN_W, 110.0f);


    glVertex2f((float)WIN_W, 60.0f);
    glVertex2f(1350.0f, 55.0f);
    glVertex2f(1000.0f, 70.0f);
    glVertex2f(650.0f, 50.0f);
    glVertex2f(300.0f, 65.0f);
    glVertex2f(0.0f, 55.0f);

    glEnd();

    circle(180.0f, 200.0f, 55.0f, 0.34f, 0.255f, 0.17f);

    circle(980.0f, 230.0f, 70.0f, 0.34f, 0.255f, 0.17f);

    circle(1400.0f, 170.0f, 45.0f, 0.34f, 0.255f, 0.17f);
}

bool is_near(float x1, float y1, float x2, float y2) {
    if (x1 >= x2 - 30.0f && x1 <= x2 + 30.0f) {// checks if the x coordinates are within 30 units of each other
        if (y1 >= y2 - 30.0f && y1 <= y2 + 30.0f) {// checks if the y coordinates are within 30 units of each other
            return true;
        }
    }
    return false;
}

void scene_2() {
    if (!isNightMode && !isRaining) {
        glPushMatrix();
        glTranslatef(550.0f, 250.0f, 0.0f);
        sun();
        glPopMatrix();
    }

    mountains();
    hills();
    scene_2_ground();
    small_tree(300.0f, 500.0f, 0.5f);
    small_tree(1300.0f, 480.0f, 0.5f);
    small_tree(800.0f, 470.0f, 0.5f);
    small_tree(22.87f, 340.00f, 0.40f);
    small_tree(116.45f, 442.13f, 0.45f);
    small_tree(300.00f, 500.00f, 0.50f);
    small_tree(483.55f, 442.13f, 0.45f);
    small_tree(625.00f, 423.11f, 0.48f);
    small_tree(800.00f, 470.00f, 0.50f);
    small_tree(975.00f, 423.11f, 0.48f);
    small_tree(1127.92f, 425.75f, 0.45f);
    small_tree(1300.00f, 480.00f, 0.50f);
    small_tree(1472.08f, 425.75f, 0.45f);
    small_tree(1559.81f, 330.00f, 0.40f);

    crater_small(280.0f, 45.0f, 1.0f);
crater_small(1120.0f, 100.0f, 0.9f);
crater_small(560.0f, 110.0f, 1.0f);
crater_small(1500.0f, 60.0f, 1.0f);
crater_small(120.0f, 55.0f, 2.0f);
crater_small(850.0f, 30.0f, 0.8f);
    translate_black_house(120.0f, 222.0f, 11.55f);
    broken_house(430.0f, 218.0f, 0.45f);
    translate_black_house(760.0f, 216.0f, 0.50f);
    broken_house(1080.0f, 214.0f, 0.42f);
    translate_black_house(1380.0f, 212.0f, 11.55f);

    burnt_tree_2(200.0f, 280.0f, 0.55f);
    burnt_tree_1(500.0f, 278.0f, 0.5f);
    translate_new_house_1(860.0f, 280.0f, 8.5f);
    burnt_tree_2(1180.0f, 278.0f, 0.55f);
    burnt_tree_1(1420.0f, 280.0f, 0.5f);

    translate_new_house_1(260.0f, 175.0f, 12.7f);
    translate_black_house(680.0f, 275.0f, 9.0f);
    translate_new_house_1(380.0f, 275.0f, 9.0f);
    translate_new_house_1(520.0f, 155.0f, 18.0f);
    broken_house(680.0f, 165.0f, 0.75f);
    translate_black_house(1020.0f, 170.0f, 11.65f);
    translate_new_house_1(1330.0f, 160.0f, 15.7f);

    brick_pile_large(500.0f, 40.0f, 1.0f);
    brick_pile_small(870.0f, 35.0f, 1.0f);
    brick_pile_small(1180.0f, 50.0f, 0.9f);
    brick(340.0f, 30.0f, 26.0f, 12.0f, 18.0f, 0.55f, 0.20f, 0.15f);
    brick(760.0f, 25.0f, 22.0f, 10.0f, -10.0f, 0.45f, 0.28f, 0.16f);
    wood_piles(230.0f, 40.0f, 1.0f);
    wood_piles(1050.0f, 35.0f, 0.9f);
    wood_plank(620.0f, 30.0f, 90.0f, 12.0f, 15.0f, 0.42f, 0.27f, 0.14f);
    broken_beam(1350.0f, 28.0f, 80.0f, 11.0f, -25.0f, 0.22f, 0.15f, 0.09f);
    multiple_roof(390.0f, 40.0f, 1.0f);
    multiple_roof(1180.0f, 45.0f, 0.9f);

    translate_black_house(820.0f, 115.0f, 20.0f);

    burnt_tree_1(60.0f, 90.0f, 1.1f);
    burnt_tree_2(1250.0f, 90.0f, 1.1f);

    fire(760.0f, 95.0f, 1.0f);
    fire(650.0f, 150.0f, 0.7f);
    fire(1380.0f, 212.0f, 0.5f);
    fire(500.0f, 155.0f, 0.7f);
    fire(380.0f, 275.0f, 0.4f);

    smoke(760.0f, 95.0f);
    smoke(650.0f, 150.0f);
    smoke(1380.0f, 212.0f);
    smoke(500.0f, 155.0f);
    smoke(380.0f, 275.0f);

    smoke(860.0f, 280.0f);
    smoke(760.0f, 216.0f);
    smoke(1020.0f, 170.0f);
    smoke(520.0f, 155.0f);
    smoke(120.0f, 222.0f);

    for (int i = 0; i < fireCount; i++) {
        fire(clickFireX[i], clickFireY[i], 0.8f);
        smoke(clickFireX[i], clickFireY[i] + 30.0f);
    }

    for (int i = 0; i < debrisCount; i++) {
        brick_pile_small(debrisX[i], debrisY[i], 0.8f);
    }


    glPushMatrix();
    glTranslatef(cloudOffset1, 0.0f, 0.0f);
    fluffy_cloud(100.0f, 880.0f, 40.0f, isNightMode);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(cloudOffset2, 0.0f, 0.0f);
    fluffy_cloud(500.0f, 920.0f, 32.0f, isNightMode);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(cloudOffset3, 0.0f, 0.0f);
    fluffy_cloud(900.0f, 860.0f, 45.0f, isNightMode);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(cloudOffset4, 0.0f, 0.0f);
    fluffy_cloud(1300.0f, 900.0f, 35.0f, isNightMode);
    glPopMatrix();
}

void render_scene_2() {
    if (isNightMode) {
        glClearColor(0.04f, 0.04f, 0.08f, 1.0f);

    }
    else if (isRaining) {
        glClearColor(0.32f, 0.30f, 0.28f, 1.0f);
    }
    else {
        glClearColor(0.45f, 0.38f, 0.32f, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT);
    scene_2();
    if (isNightMode) {
        night_scene();
    }
    rain();
}

void display_scene() {
    render_scene_2();
    glFlush();
}

void update_scene(int value) {
    smokeOffset += SMOKE_DRIFT_SPEED;
    cloudOffset1 += CLOUD_SPEED;
    cloudOffset2 += CLOUD_SPEED;
    cloudOffset3 += CLOUD_SPEED;
    cloudOffset4 += CLOUD_SPEED;


    fireY += 1.0f;

    if (fireY > 10.0f)
        fireY = 0.0f;

    if (smokeOffset > 20.0f) {
        smokeOffset = 0.0f;
    }
    if (100.0f + cloudOffset1 > WIN_W - 10.0f) {
        cloudOffset1 = -200.0f;
    }
    if (500.0f + cloudOffset2 > WIN_W - 10.0f) {
        cloudOffset2 = -600.0f;
    }
    if (900.0f + cloudOffset3 > WIN_W - 10.0f) {
        cloudOffset3 = -1000.0f;
    }
    if (1300.0f + cloudOffset4 > WIN_W - 10.0f) {
        cloudOffset4 = -1400.0f;
    }
    update_rain();
    glutPostRedisplay();
    glutTimerFunc(16, update_scene, 0);
}

void mouse_click(int button, int state, int x, int y) {
    if (state != GLUT_DOWN) return; // Only handle mouse button press events

    float glX = (float)x;
    float glY = (float)(WIN_H - y);

    // Left click Ground = Spawn / Delete Debris
    if (button == GLUT_LEFT_BUTTON) {
        if (glY <= 400.0f) {
            bool deleted = false;
            for (int i = 0; i < debrisCount; i++) {
                if (is_near(glX, glY, debrisX[i], debrisY[i])) {
                    debrisX[i] = debrisX[debrisCount - 1];
                    debrisY[i] = debrisY[debrisCount - 1];
                    debrisCount--;
                    deleted = true;
                    break;
                }
            }

            if (!deleted && debrisCount < MAX_DEBRIS) {
                debrisX[debrisCount] = glX;
                debrisY[debrisCount] = glY;
                debrisCount++;
            }
        }
    }
    // Right click Ground = Spawn / Delete Fire
    else if (button == GLUT_RIGHT_BUTTON) {
        if (glY <= 400.0f) {
            bool deleted = false;
            for (int i = 0; i < fireCount; i++) {
                if (is_near(glX, glY, clickFireX[i], clickFireY[i])) {
                    clickFireX[i] = clickFireX[fireCount - 1];
                    clickFireY[i] = clickFireY[fireCount - 1];
                    fireCount--;
                    deleted = true;
                    break;
                }
            }

            if (!deleted && fireCount < MAX_USER_FIRES) {
                clickFireX[fireCount] = glX;
                clickFireY[fireCount] = glY;
                fireCount++;
            }
        }
    }

    glutPostRedisplay();
}

void mouse_motion(int x, int y) {
    if (x < WIN_W / 3) {
        windForce = -1.0f;
    }
    else if (x > (WIN_W * 2) / 3) {
        windForce = 1.0f;
    }
    else {
        windForce = 0.0f;
    }

    glutPostRedisplay();
}

void keyboard_input(unsigned char key, int x, int y) {
    if (key == 'n' || key == 'N') {
        isNightMode = true;
        glutPostRedisplay();
    }
    else if (key == 'd' || key == 'D') {
        isNightMode = false;
        glutPostRedisplay();
    }
    else if (key == 'r' || key == 'R') {
        isRaining = !isRaining;
        glutPostRedisplay();
    }
}

void init_scene() {
    init_rain();

    glClearColor(0.45f, 0.38f, 0.32f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, WIN_W, 0.0, WIN_H);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WIN_W, WIN_H);
    glutCreateWindow("Ruined Village - Interactive");
    init_scene();
    glutDisplayFunc(display_scene);
    glutKeyboardFunc(keyboard_input);
    glutMouseFunc(mouse_click);
    glutPassiveMotionFunc(mouse_motion);
    glutTimerFunc(16, update_scene, 0);
    glutMainLoop();
    return 0;
}
