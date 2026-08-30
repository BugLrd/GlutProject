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


float cloudOffset = 0.0f;
const float CLOUD_SPEED = 0.5f;

// Animation state for fire/smoke
float firePhase = 0.0f;
const float FIRE_SPEED = 0.12f;
const float SMOKE_DRIFT_SPEED = 0.25f; // smoke increment 
float smokeOffset = 0.0f;

float windForce = 0.0f;

bool Is_FireOn = true;


float rainX[MAX_RAINDROPS];
float rainY[MAX_RAINDROPS];


float userFireX[MAX_USER_FIRES];
float userFireY[MAX_USER_FIRES];
int fireCount = 0;



float debrisX[MAX_DEBRIS];
float debrisY[MAX_DEBRIS];
int debrisCount = 0;

//------------------------- Structs & Colors -------------------------
struct Color3 {
    float r, g, b;
};

const Color3 BurntBlack = { 0.05f, 0.05f, 0.05f };
const Color3 Charcoal = { 0.14f, 0.13f, 0.12f };
const Color3 CharcoalLight = { 0.26f, 0.24f, 0.22f };
const Color3 AshGray = { 0.55f, 0.53f, 0.50f };
const Color3 BrickRed = { 0.55f, 0.20f, 0.15f };
const Color3 BrickBrown = { 0.45f, 0.28f, 0.16f };
const Color3 BrickOrange = { 0.62f, 0.34f, 0.18f };
const Color3 BurntWood = { 0.22f, 0.15f, 0.09f };
const Color3 WoodBrown = { 0.42f, 0.27f, 0.14f };
const Color3 WoodBrownLight = { 0.55f, 0.38f, 0.20f };
const Color3 RoofDarkRed = { 0.42f, 0.16f, 0.14f };
const Color3 RoofGray = { 0.34f, 0.34f, 0.36f };
const Color3 DustyGreen = { 0.34f, 0.42f, 0.24f };
const Color3 DirtBrown = { 0.40f, 0.30f, 0.20f };
const Color3 EmberYellow = { 1.0f,  0.80f, 0.20f };
const Color3 EmberOrange = { 0.95f, 0.45f, 0.10f };
const Color3 EmberRed = { 0.80f, 0.15f, 0.10f };





// Lightning state
float lightningFlash = 0.0f;
float lightningTargetX = 0.0f;
float lightningTargetY = 0.0f;





void initRain() {
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        rainX[i] = rand() % WIN_W;
        rainY[i] = rand() % WIN_H;
    }
}

void updateRain() {
    if (!isRaining) return;

    for (int i = 0; i < MAX_RAINDROPS; i++) {
        rainY[i] -= 15;        // droping 
        rainX[i] += windForce; // to move with the rain 

        // Reset 
        if (rainY[i] < 0) {
            rainY[i] = WIN_H;
            rainX[i] = rand() % WIN_W;
        }
    }
}

void drawRain() {
    if (!isRaining) return;

    glColor3f(0.7f, 0.8f, 1.0f);
    glLineWidth(1.5f);

    glBegin(GL_LINES);
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        glVertex2i(rainX[i], rainY[i]);
        glVertex2i(rainX[i] - windForce, rainY[i] - 15); // boot is moved to right or left depending on wind 
    }
    glEnd();
}


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

void drawHill(float cx, float cy, float rx, float ry, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 30; i++) {
        float angle = (float)PI * i / 30.0f;
        glVertex2f(cx + rx * cosf(angle), cy + ry * sinf(angle));
    }
    glEnd();
}

void drawTinyTree(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    glColor3f(0.10f, 0.08f, 0.05f);
    drawRect(-4.0f, 0.0f, 4.0f, 15.0f);
    glColor3f(0.08f, 0.22f, 0.10f);
    drawTriangle(-25.0f, 10.0f, 0.0f, 10.0f, 0.0f, 45.0f);
    glColor3f(0.12f, 0.28f, 0.14f);
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

void drawTreeOnHillEdge(float cx, float cy, float rx, float ry, float angleDegrees, float scale) {
    float rad = angleDegrees * (float)PI / 180.0f;
    float x = cx + rx * cosf(rad);
    float y = cy + ry * sinf(rad);
    drawTinyTree(x, y, scale);
}

void drawMountains() {
    Color3 litGreen = { 0.32f, 0.50f, 0.25f };
    Color3 shadowGreen = { 0.18f, 0.30f, 0.14f };
    // Left
    glColor3f(litGreen.r, litGreen.g, litGreen.b);
    drawTriangle(50.0f, 200.0f, 300.0f, 650.0f, 300.0f, 200.0f);
    glColor3f(shadowGreen.r, shadowGreen.g, shadowGreen.b);
    drawTriangle(300.0f, 200.0f, 300.0f, 650.0f, 600.0f, 200.0f);

    // Center
    glColor3f(litGreen.r, litGreen.g, litGreen.b);
    drawTriangle(200.0f, 200.0f, 570.0f, 750.0f, 570.0f, 200.0f);
    glColor3f(shadowGreen.r, shadowGreen.g, shadowGreen.b);
    drawTriangle(570.0f, 200.0f, 570.0f, 750.0f, 1150.0f, 200.0f);

    //  Right
    glColor3f(litGreen.r, litGreen.g, litGreen.b);
    drawTriangle(800.0f, 200.0f, 1050.0f, 600.0f, 1050.0f, 200.0f);
    glColor3f(shadowGreen.r, shadowGreen.g, shadowGreen.b);
    drawTriangle(1050.0f, 200.0f, 1050.0f, 600.0f, 1450.0f, 200.0f);

}

void drawHills() {
    float h1_cx = 250.0f, h1_cy = 220.0f, h1_rx = 400.0f, h1_ry = 180.0f;
    drawHill(h1_cx, h1_cy, h1_rx, h1_ry, 0.25f, 0.32f, 0.20f);
    drawTreeOnHillEdge(h1_cx, h1_cy, h1_rx, h1_ry, 35.0f, 0.22f);
    drawTreeOnHillEdge(h1_cx, h1_cy, h1_rx, h1_ry, 55.0f, 0.25f);
    drawTreeOnHillEdge(h1_cx, h1_cy, h1_rx, h1_ry, 75.0f, 0.20f);
    drawTreeOnHillEdge(h1_cx, h1_cy, h1_rx, h1_ry, 95.0f, 0.24f);
    drawTreeOnHillEdge(h1_cx, h1_cy, h1_rx, h1_ry, 115.0f, 0.21f);
    drawTreeOnHillEdge(h1_cx, h1_cy, h1_rx, h1_ry, 135.0f, 0.23f);

    float h2_cx = 1350.0f, h2_cy = 220.0f, h2_rx = 450.0f, h2_ry = 190.0f;
    drawHill(h2_cx, h2_cy, h2_rx, h2_ry, 0.22f, 0.28f, 0.18f);
    drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 60.0f, 0.25f);
    drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 75.0f, 0.25f);
    drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 90.0f, 0.25f);
    drawTreeOnHillEdge(h2_cx, h2_cy, h2_rx, h2_ry, 105.0f, 0.25f);

    float h3_cx = 800.0f, h3_cy = 220.0f, h3_rx = 500.0f, h3_ry = 200.0f;
    drawHill(h3_cx, h3_cy, h3_rx, h3_ry, 0.26f, 0.34f, 0.21f);
    drawTreeOnHillEdge(h3_cx, h3_cy, h3_rx, h3_ry, 45.0f, 0.22f);
    drawTreeOnHillEdge(h3_cx, h3_cy, h3_rx, h3_ry, 65.0f, 0.26f);
    drawTreeOnHillEdge(h3_cx, h3_cy, h3_rx, h3_ry, 85.0f, 0.24f);
    drawTreeOnHillEdge(h3_cx, h3_cy, h3_rx, h3_ry, 105.0f, 0.21f);
    drawTreeOnHillEdge(h3_cx, h3_cy, h3_rx, h3_ry, 125.0f, 0.23f);
}

void sun() {
    glColor3f(1.0f, 0.85f, 0.0f);
    drawCircle(520.0f, 520.0f, 40.0f, 40);
}

void drawCloud(float cx, float cy, float scale) {
    if (isRaining) {
        glColor3f(0.40f, 0.44f, 0.48f);
    }
    else {
        glColor3f(0.85f, 0.87f, 0.90f);
    }
    drawCircle(cx, cy - 5.0f * scale, 30.0f * scale, 30);
    drawCircle(cx + 30.0f * scale, cy, 36.0f * scale, 30);
    drawCircle(cx + 65.0f * scale, cy - 5.0f * scale, 26.0f * scale, 30);
    drawCircle(cx + 20.0f * scale, cy - 15.0f * scale, 24.0f * scale, 30);
    drawCircle(cx + 50.0f * scale, cy - 15.0f * scale, 22.0f * scale, 30);

    if (isRaining) {
        glColor3f(0.55f, 0.58f, 0.62f);
    }
    else {
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    drawCircle(cx, cy, 26.0f * scale, 30);
    drawCircle(cx + 30.0f * scale, cy + 8.0f * scale, 32.0f * scale, 30);
    drawCircle(cx + 62.0f * scale, cy, 22.0f * scale, 30);
}

void drawClouds() {
    static const float baseX[] = { 120.0f, 430.0f, 700.0f, 980.0f, 1250.0f, 1480.0f };
    static const float baseY[] = { 880.0f, 930.0f, 860.0f, 910.0f, 870.0f,  920.0f };
    static const float scale[] = { 1.0f, 0.8f, 1.2f, 0.9f, 1.1f, 0.7f };
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

void NightScene() {
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
        int numStars = sizeof(starPositions) / sizeof(starPositions[0]);
        for (int i = 0; i < numStars; i++) {
            glVertex2f(starPositions[i][0], starPositions[i][1]);
        }
        glEnd();

        glColor4f(0.92f, 0.92f, 0.85f, 1.0f);
        glPushMatrix();
        glTranslatef(550.0f, 250.0f, 0.0f);
        drawCircle(520.0f, 520.0f, 40.0f, 40);
        glPopMatrix();
    }

    glDisable(GL_BLEND);
}

void drawLightning() {
    if (lightningFlash <= 0.0f) return;

    // 1. Screen Flash (Semi-transparent white overlay)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1.0f, 1.0f, 1.0f, 0.4f); // screen flash 
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(WIN_W, 0.0f);
    glVertex2f(WIN_W, WIN_H);
    glVertex2f(0.0f, WIN_H);
    glEnd();

    // 2. Zig-zag Lightning Bolt (Top to Target)
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(3.0f);

    float midY1 = (WIN_H + lightningTargetY) * 0.66f;
    float midY2 = (WIN_H + lightningTargetY) * 0.33f;

    glBegin(GL_LINE_STRIP);
    glVertex2f(lightningTargetX + (rand() % 40 - 20), WIN_H);             // Top start
    glVertex2f(lightningTargetX + (rand() % 60 - 30), midY1);             // Zig
    glVertex2f(lightningTargetX + (rand() % 60 - 30), midY2);             // Zag
    glVertex2f(lightningTargetX, lightningTargetY);  // Target impact
    glEnd();

    glDisable(GL_BLEND);
}


//------------------------- Ruined Scene Elements -------------------------
void drawBlob(float cx, float cy, float rx, float ry, float seed, Color3 color) {
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 10; i++) {
        float angle = 2.0f * (float)PI * i / 10.0f;
        float wobble = 1.0f + 0.25f * sinf(angle * 3.0f + seed);
        glVertex2f(cx + rx * wobble * cosf(angle), cy + ry * wobble * sinf(angle));
    }
    glEnd();
}

void drawCraterSmall(float x, float y, float scale) {
    drawBlob(x, y, 26.0f * scale, 18.0f * scale, x, DirtBrown);
    drawBlob(x, y, 14.0f * scale, 10.0f * scale, x + 1.0f, Charcoal);
}

void drawBrick(float x, float y, float w, float h, float rotation, Color3 color) {
    float hw = w * 0.5f;
    float hh = h * 0.5f;
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_QUADS);
    glVertex2f(-hw, -hh); glVertex2f(hw, -hh);
    glVertex2f(hw, hh);   glVertex2f(-hw, hh);
    glEnd();

    glColor3f(color.r * 0.55f, color.g * 0.55f, color.b * 0.55f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-hw, -hh); glVertex2f(hw, -hh);
    glVertex2f(hw, hh);   glVertex2f(-hw, hh);
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

void drawWoodPlank(float x, float y, float length, float thickness, float rotation, Color3 color) {
    float hl = length * 0.5f;
    float ht = thickness * 0.5f;
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_QUADS);
    glVertex2f(-hl, -ht); glVertex2f(hl, -ht);
    glVertex2f(hl, ht);   glVertex2f(-hl, ht);
    glEnd();
    glPopMatrix();
}

void drawBrokenBeam(float x, float y, float length, float thickness, float rotation, Color3 color) {
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

void drawBrokenRoofPiece(float x, float y, float w, float h, float rotation, Color3 color) {
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

void drawRoofDebris(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    drawBrokenRoofPiece(-10.0f, 6.0f, 55.0f, 30.0f, 8.0f, RoofDarkRed);
    drawBrokenRoofPiece(28.0f, 4.0f, 40.0f, 24.0f, -15.0f, RoofGray);
    drawBrokenRoofPiece(5.0f, 20.0f, 35.0f, 20.0f, 30.0f, BurntWood);
    glPopMatrix();
}

void drawBrokenHouse1(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    drawBrickPileLarge(-45.0f, 8.0f, 0.9f);
    drawBrickPileSmall(55.0f, 6.0f, 0.8f);
    drawBlob(10.0f, 5.0f, 30.0f * 0.7f, 14.0f * 0.7f, x, AshGray);
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
    glColor3ub(75, 60, 48);
    glBegin(GL_POLYGON);
    glVertex2f(-4.0f, -6.0f); glVertex2f(-2.2f, -4.8f);
    glVertex2f(0.5f, -5.0f);  glVertex2f(2.8f, -4.5f);
    glVertex2f(4.2f, -6.0f);
    glEnd();

    glColor3ub(140, 90, 50);
    glBegin(GL_POLYGON);
    glVertex2f(-3.1f, -6.0f); glVertex2f(-2.5f, -6.0f);
    glVertex2f(-2.4f, -2.5f); glVertex2f(-2.8f, -2.2f);
    glVertex2f(-3.2f, -2.7f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(2.0f, -6.0f);  glVertex2f(2.6f, -6.0f);
    glVertex2f(3.5f, -3.2f);  glVertex2f(3.0f, -3.0f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(-0.4f, -6.0f); glVertex2f(0.2f, -6.0f);
    glVertex2f(0.4f, -4.2f);  glVertex2f(-0.2f, -4.0f);
    glEnd();

    glColor3ub(115, 70, 35);
    glBegin(GL_POLYGON);
    glVertex2f(-3.2f, -2.4f); glVertex2f(-0.8f, -3.5f);
    glVertex2f(-0.7f, -3.9f); glVertex2f(-3.1f, -2.8f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(-0.2f, -3.8f); glVertex2f(3.2f, -3.2f);
    glVertex2f(3.1f, -3.6f);  glVertex2f(-0.3f, -4.2f);
    glEnd();

    glColor3ub(140, 35, 35);
    glBegin(GL_POLYGON);
    glVertex2f(-3.6f, -1.8f); glVertex2f(-1.5f, -3.0f);
    glVertex2f(-1.8f, -3.4f); glVertex2f(-3.8f, -2.1f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(-0.8f, -2.8f); glVertex2f(0.2f, -1.0f);
    glVertex2f(0.8f, -2.5f);  glVertex2f(0.3f, -2.7f);
    glVertex2f(-0.2f, -1.6f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(1.8f, -4.8f);  glVertex2f(3.3f, -4.2f);
    glVertex2f(3.5f, -4.6f);  glVertex2f(2.0f, -5.2f);
    glEnd();

    glColor3ub(195, 190, 180);
    glBegin(GL_POLYGON); glVertex2f(-4.3f, -5.7f); glVertex2f(-3.3f, -5.4f); glVertex2f(-3.1f, -6.0f); glVertex2f(-4.1f, -6.2f); glEnd();
    glBegin(GL_POLYGON); glVertex2f(-2.2f, -3.8f); glVertex2f(-1.4f, -3.6f); glVertex2f(-1.2f, -4.1f); glVertex2f(-2.0f, -4.3f); glEnd();
    glBegin(GL_POLYGON); glVertex2f(-0.8f, -5.2f); glVertex2f(-0.1f, -4.9f); glVertex2f(0.2f, -5.5f);  glVertex2f(-0.5f, -5.7f); glEnd();
    glBegin(GL_POLYGON); glVertex2f(1.5f, -3.6f);  glVertex2f(2.5f, -3.4f);  glVertex2f(2.7f, -4.0f);  glVertex2f(1.7f, -4.1f);  glEnd();
    glBegin(GL_POLYGON); glVertex2f(2.4f, -5.2f);  glVertex2f(3.6f, -4.8f);  glVertex2f(3.8f, -5.5f);  glVertex2f(2.6f, -5.8f);  glEnd();

    glColor3ub(155, 150, 140);
    glBegin(GL_TRIANGLES);
    glVertex2f(-2.5f, -5.8f); glVertex2f(-2.2f, -5.5f); glVertex2f(-2.1f, -5.9f);
    glVertex2f(0.8f, -5.8f);  glVertex2f(1.1f, -5.4f);  glVertex2f(1.3f, -5.9f);
    glVertex2f(3.8f, -5.8f);  glVertex2f(4.1f, -5.5f);  glVertex2f(4.2f, -6.1f);
    glEnd();
}

void NEW1drawHouseBlack() {
    glColor3ub(55, 55, 62);
    glBegin(GL_POLYGON); glVertex2f(-3.0f, -1.6f); glVertex2f(-2.1f, -1.9f); glVertex2f(-1.9f, -2.8f); glVertex2f(-2.8f, -2.7f); glEnd();
    glBegin(GL_POLYGON); glVertex2f(0.9f, -1.5f);  glVertex2f(1.5f, -1.8f);  glVertex2f(1.3f, -2.8f);  glVertex2f(0.7f, -2.6f);  glEnd();
    glBegin(GL_POLYGON); glVertex2f(1.6f, -2.2f);  glVertex2f(2.3f, -2.1f);  glVertex2f(2.1f, -3.3f);  glVertex2f(1.4f, -3.1f);  glEnd();

    glColor3ub(40, 38, 48);
    glBegin(GL_POLYGON);
    glVertex2f(-1.7f, -4.8f); glVertex2f(1.7f, -4.8f);
    glVertex2f(1.6f, -3.0f);  glVertex2f(0.1f, -3.4f);
    glVertex2f(-1.6f, -3.1f);
    glEnd();

    glLineWidth(2.5f);
    glColor3ub(15, 12, 20);
    glBegin(GL_LINES);
    glVertex2f(-1.1f, -3.1f); glVertex2f(-0.8f, -4.0f);
    glVertex2f(0.8f, -3.2f);  glVertex2f(1.2f, -4.2f);
    glEnd();

    glColor3ub(75, 60, 48);
    glBegin(GL_POLYGON); glVertex2f(-0.3f, -1.3f); glVertex2f(0.1f, -1.1f); glVertex2f(0.2f, -2.4f); glVertex2f(-0.2f, -2.5f); glEnd();
    glBegin(GL_POLYGON); glVertex2f(-2.4f, -2.3f); glVertex2f(-0.5f, -2.0f); glVertex2f(-0.6f, -2.3f); glVertex2f(-2.5f, -2.6f); glEnd();

    glColor3ub(22, 20, 28);
    glBegin(GL_POLYGON); glVertex2f(-0.2f, -1.5f); glVertex2f(-3.7f, -3.7f); glVertex2f(-3.3f, -4.1f); glVertex2f(-0.4f, -2.3f); glEnd();

    glColor3ub(18, 16, 24);
    glBegin(GL_POLYGON); glVertex2f(0.3f, -1.8f);  glVertex2f(3.7f, -3.9f);  glVertex2f(3.3f, -4.3f);  glVertex2f(0.1f, -2.6f);  glEnd();

    glColor3ub(140, 175, 200);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.9f, -3.1f); glVertex2f(-0.5f, -3.1f); glVertex2f(-0.8f, -2.6f);
    glVertex2f(-0.3f, -3.0f); glVertex2f(-0.2f, -2.5f); glVertex2f(-0.4f, -2.7f);
    glVertex2f(0.2f, -3.1f);  glVertex2f(0.7f, -3.1f);  glVertex2f(0.3f, -2.4f);
    glEnd();

    glColor3ub(30, 28, 36);
    glBegin(GL_POLYGON); glVertex2f(-5.3f, -4.2f); glVertex2f(-5.0f, -3.5f); glVertex2f(-3.0f, -3.0f); glVertex2f(-3.1f, -3.5f); glEnd();
    glBegin(GL_POLYGON); glVertex2f(-3.2f, -3.1f); glVertex2f(-1.5f, -2.4f); glVertex2f(-1.7f, -3.0f); glVertex2f(-3.3f, -3.6f); glEnd();

    glColor3ub(50, 48, 56);
    glBegin(GL_POLYGON); glVertex2f(-4.6f, -4.7f); glVertex2f(-0.5f, -5.1f); glVertex2f(-0.3f, -5.6f); glVertex2f(-4.4f, -5.2f); glEnd();

    glColor3ub(30, 28, 36);
    glBegin(GL_POLYGON); glVertex2f(1.2f, -2.4f); glVertex2f(4.3f, -3.6f); glVertex2f(4.0f, -4.2f); glVertex2f(1.0f, -2.8f); glEnd();

    glColor3ub(35, 35, 40);
    glBegin(GL_POLYGON); glVertex2f(-4.2f, -5.3f); glVertex2f(-2.7f, -5.4f); glVertex2f(-2.8f, -5.9f); glVertex2f(-4.2f, -5.8f); glEnd();
    glBegin(GL_POLYGON); glVertex2f(-2.3f, -5.4f); glVertex2f(-1.1f, -5.3f); glVertex2f(-1.2f, -5.9f); glVertex2f(-2.4f, -5.9f); glEnd();
    glBegin(GL_POLYGON); glVertex2f(1.8f, -5.4f);  glVertex2f(3.2f, -5.3f);  glVertex2f(3.1f, -5.9f);  glVertex2f(1.7f, -5.9f);  glEnd();

    glColor3ub(45, 42, 50);
    glBegin(GL_POLYGON); glVertex2f(2.3f, -4.5f); glVertex2f(3.7f, -4.4f); glVertex2f(3.9f, -5.1f); glVertex2f(2.4f, -5.3f); glEnd();

    glColor3ub(230, 230, 225);
    glBegin(GL_POLYGON); glVertex2f(3.9f, -3.0f); glVertex2f(5.1f, -2.9f); glVertex2f(5.0f, -3.3f); glVertex2f(3.8f, -3.2f); glEnd();
    glBegin(GL_POLYGON); glVertex2f(4.1f, -3.7f); glVertex2f(5.3f, -4.0f); glVertex2f(5.1f, -4.3f); glVertex2f(4.0f, -4.0f); glEnd();

    glColor3ub(180, 180, 175);
    glBegin(GL_TRIANGLES);
    glVertex2f(-4.8f, -5.6f); glVertex2f(-4.5f, -5.3f); glVertex2f(-4.4f, -5.7f);
    glVertex2f(-0.2f, -5.8f); glVertex2f(0.1f, -5.4f);  glVertex2f(0.3f, -5.9f);
    glVertex2f(3.5f, -5.7f);  glVertex2f(3.8f, -5.3f);  glVertex2f(4.0f, -5.8f);
    glEnd();
}

void drawRuinedHouseNEW(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    glTranslatef(0.0f, 6.0f, 0.0f);
    NEW2drawHouse();
    glPopMatrix();
}

void drawRuinedHouseBlackNEW(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    glTranslatef(0.0f, 5.9f, 0.0f);
    NEW1drawHouseBlack();
    glPopMatrix();
}

void drawBurntTree1(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    glColor3f(BurntWood.r * 0.6f, BurntWood.g * 0.6f, BurntWood.b * 0.6f);
    drawRect(-7.0f, 0.0f, 7.0f, 130.0f);
    glColor3f(BurntBlack.r, BurntBlack.g, BurntBlack.b);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 100.0f); glVertex2f(-35.0f, 130.0f);
    glVertex2f(0.0f, 90.0f);  glVertex2f(30.0f, 115.0f);
    glVertex2f(0.0f, 70.0f);  glVertex2f(-28.0f, 95.0f);
    glVertex2f(0.0f, 55.0f);  glVertex2f(22.0f, 75.0f);
    glVertex2f(0.0f, 130.0f); glVertex2f(8.0f, 150.0f);
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
    glVertex2f(-6.0f, 140.0f); glVertex2f(6.0f, 140.0f);
    glVertex2f(14.0f, 150.0f);
    glEnd();
    glColor3f(BurntBlack.r, BurntBlack.g, BurntBlack.b);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 110.0f); glVertex2f(-25.0f, 130.0f);
    glEnd();
    glPopMatrix();
}

void drawFire(float x, float y, float scale) {
    if (isRaining) return;

    float flick = 0.9f + ((rand() % 20) / 100.0f);

    float flick2 = 0.9f + ((rand() % 20) / 100.0f); // generates random value then makes it 0.19


    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    glColor3f(EmberRed.r, EmberRed.g, EmberRed.b);
    drawTriangle(-16.0f, 0.0f, 16.0f, 0.0f, 0.0f, 55.0f * flick);
    drawTriangle(-10.0f, 0.0f, 6.0f, 0.0f, -4.0f, 40.0f * flick2); // multiples the height 
    glColor3f(EmberOrange.r, EmberOrange.g, EmberOrange.b);
    drawTriangle(-10.0f, 0.0f, 10.0f, 0.0f, 0.0f, 40.0f * flick2);
    glColor3f(EmberYellow.r, EmberYellow.g, EmberYellow.b);
    drawTriangle(-5.0f, 0.0f, 5.0f, 0.0f, 0.0f, 24.0f * flick);
    glPopMatrix();
}
void drawSmoke(float x, float y, float scale) {

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.35f, 0.35f, 0.35f, 0.25f);

    drawCircle(x, y + (10.0f * scale), 9.0f * scale, 12);


    float midX = x + (windForce * 2.0f);
    float midY = y + smokeOffset + (28.0f * scale);
    drawCircle(midX, midY, 14.0f * scale, 12); // Center
    drawCircle(midX - (10.0f * scale), midY + (2.0f * scale), 11.0f * scale, 12); //left
    drawCircle(midX + (10.0f * scale), midY + (2.0f * scale), 11.0f * scale, 12);


    float topX = x + (windForce * 4.0f);
    float topY = y + (smokeOffset * 1.5f) + (50.0f * scale);
    drawCircle(topX, topY, 18.0f * scale, 12); // Center
    drawCircle(topX - (13.0f * scale), topY + (3.0f * scale), 14.0f * scale, 12); //right
    drawCircle(topX + (13.0f * scale), topY + (3.0f * scale), 14.0f * scale, 12);

    glDisable(GL_BLEND);

}

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
    glVertex2f(0.0f, 55.0f); glVertex2f(0.0f, 105.0f);
    glVertex2f(300.0f, 65.0f); glVertex2f(300.0f, 120.0f);
    glVertex2f(650.0f, 50.0f); glVertex2f(650.0f, 100.0f);
    glVertex2f(1000.0f, 70.0f); glVertex2f(1000.0f, 125.0f);
    glVertex2f(1350.0f, 55.0f); glVertex2f(1350.0f, 105.0f);
    glVertex2f((float)WIN_W, 60.0f); glVertex2f((float)WIN_W, 110.0f);
    glEnd();

    glColor3f(DirtBrown.r * 0.85f, DirtBrown.g * 0.85f, DirtBrown.b * 0.85f);
    drawCircle(180.0f, 200.0f, 55.0f, 20);
    drawCircle(980.0f, 230.0f, 70.0f, 20);
    drawCircle(1400.0f, 170.0f, 45.0f, 20);
}


bool isNear(float x1, float y1, float x2, float y2) {
    //  click within 30 horizontally and vertically or Not 
    if (x1 >= x2 - 30.0f && x1 <= x2 + 30.0f) {
        if (y1 >= y2 - 30.0f && y1 <= y2 + 30.0f) {
            return true;
        }
    }
    return false;
}

//------------------------- Main Scene 2 Construction -------------------------
void scene2() {
    if (!isNightMode && !isRaining) {
        glPushMatrix();
        glTranslatef(550.0f, 250.0f, 0.0f);
        sun();
        glPopMatrix();
    }

    drawClouds();
    drawMountains();
    drawHills();

    drawScene2Ground();

    drawCraterSmall(280.0f, 45.0f, 1.0f);
    drawCraterSmall(1120.0f, 100.0f, 0.9f);
    drawCraterSmall(560.0f, 110.0f, 1.0f);
    drawCraterSmall(1500.0f, 60.0f, 1.0f);
    drawCraterSmall(120.0f, 55.0f, 2.0f);
    drawCraterSmall(850.0f, 30.0f, 0.8f);

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

    drawRuinedHouseNEW(260.0f, 175.0f, 12.7f);
    drawRuinedHouseBlackNEW(680.0f, 275.0f, 9.0f);
    drawRuinedHouseNEW(380.0f, 275.0f, 9.0f);
    drawRuinedHouseNEW(520.0f, 155.0f, 18.0f);
    drawBrokenHouse1(680.0f, 165.0f, 0.75f);
    drawRuinedHouseBlackNEW(1020.0f, 170.0f, 11.65f);
    drawRuinedHouseNEW(1330.0f, 160.0f, 15.7f);

    drawBrickPileLarge(500.0f, 40.0f, 1.0f);
    drawBrickPileSmall(870.0f, 35.0f, 1.0f);
    drawBrickPileSmall(1180.0f, 50.0f, 0.9f);
    drawBrick(340.0f, 30.0f, 26.0f, 12.0f, 18.0f, BrickRed);
    drawBrick(760.0f, 25.0f, 22.0f, 10.0f, -10.0f, BrickBrown);

    drawWoodDebrisPile(230.0f, 40.0f, 1.0f);
    drawWoodDebrisPile(1050.0f, 35.0f, 0.9f);
    drawWoodPlank(620.0f, 30.0f, 90.0f, 12.0f, 15.0f, WoodBrown);
    drawBrokenBeam(1350.0f, 28.0f, 80.0f, 11.0f, -25.0f, BurntWood);
    drawRoofDebris(390.0f, 40.0f, 1.0f);
    drawRoofDebris(1180.0f, 45.0f, 0.9f);

    drawRuinedHouseBlackNEW(820.0f, 115.0f, 20.0f);

    drawBurntTree1(60.0f, 90.0f, 1.1f);
    drawBurntTree2(1250.0f, 90.0f, 1.1f);


    drawFire(760.0f, 95.0f, 1.0f);
    drawFire(650.0f, 150.0f, 0.7f);
    drawFire(1380.0f, 212.0f, 0.5f);
    drawFire(500.0f, 155.0f, 0.7f);
    drawFire(380.0f, 275.0f, 0.4f);


    drawSmoke(760.0f, 95.0f, 1.0f);
    drawSmoke(650.0f, 150.0f, 0.7f);
    drawSmoke(1380.0f, 212.0f, 0.5f);
    drawSmoke(500.0f, 155.0f, 0.7f);
    drawSmoke(380.0f, 275.0f, 0.4f);

    drawSmoke(860.0f, 280.0f, 0.5f);
    drawSmoke(760.0f, 216.0f, 0.50f);
    drawSmoke(1020.0f, 170.0f, 0.65f);
    drawSmoke(520.0f, 155.0f, 0.9f);
    drawSmoke(120.0f, 222.0f, 0.95f);




    for (int i = 0; i < fireCount; i++) {
        drawFire(userFireX[i], userFireY[i], 0.8f);
        drawSmoke(userFireX[i], userFireY[i] + 30.0f, 0.8f);
    }

    for (int i = 0; i < debrisCount; i++) {
        drawBrickPileSmall(debrisX[i], debrisY[i], 0.8f);
    }
}

void renderScene2() {
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
    scene2();
    if (isNightMode) {
        NightScene();
    }
    drawRain();
    drawLightning();
}

void display() {
    renderScene2();
    glFlush();
}

void update(int value) {
    cloudOffset += CLOUD_SPEED;
    firePhase += FIRE_SPEED;
    smokeOffset += SMOKE_DRIFT_SPEED;
    if (smokeOffset > 20.0f) {
        smokeOffset = 0.0f;
    }
    updateRain();

    // Fade lightning
    if (lightningFlash > 0.0f) {
        lightningFlash -= 0.15f;
    }



    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

//------------------------- Mouse Handling -------------------------

void mouseClick(int button, int state, int x, int y) {
    if (state != GLUT_DOWN) return;

    float glX = (float)x;
    float glY = (float)(WIN_H - y);

    // left click Sky
    if (button == GLUT_LEFT_BUTTON) {
        if (glY > 400.0f) {
            // Sky Lightning
            lightningTargetX = glX;
            lightningTargetY = glY;
            lightningFlash = 1.0f;
        }
        else {
            // Ground clicked 
            bool deleted = false;
            for (int i = 0; i < debrisCount; i++) {
                if (isNear(glX, glY, debrisX[i], debrisY[i])) {
                    // Remove by shifting the last element into this slot
                    debrisX[i] = debrisX[debrisCount - 1];
                    debrisY[i] = debrisY[debrisCount - 1];
                    debrisCount--;
                    deleted = true;
                    break;
                }
            }

            // If not clicking an existing pile spawn a new one
            if (!deleted && debrisCount < MAX_DEBRIS) {  // if mothing was deleted if delete = false  then !delete is true 
                debrisX[debrisCount] = glX;
                debrisY[debrisCount] = glY;
                debrisCount++;
            }
        }
    }

    // ---------------- RIGHT CLICK: Ground = Spawn / Delete Fire ----------------
    else if (button == GLUT_RIGHT_BUTTON) {
        if (glY <= 400.0f) {
            bool deleted = false;
            for (int i = 0; i < fireCount; i++) {
                if (isNear(glX, glY, userFireX[i], userFireY[i])) {

                    userFireX[i] = userFireX[fireCount - 1];
                    userFireY[i] = userFireY[fireCount - 1];
                    fireCount--;
                    deleted = true;
                    break;
                }
            }

            // If not clicking on an existing fire, spawn a new one
            if (!deleted && fireCount < MAX_USER_FIRES) {
                userFireX[fireCount] = glX;
                userFireY[fireCount] = glY;
                fireCount++;
            }
        }
    }

    glutPostRedisplay();
}





void passiveMotion(int x, int y) {
    if (x < WIN_W / 3) {
        windForce = -1.0f;  // Mouse on the left side -> wind blows left
    }
    else if (x > (WIN_W * 2) / 3) {
        windForce = 1.0f;   // Mouse on the right side -> wind blows right
    }
    else {
        windForce = 0.0f;   // Mouse in the middle -> no wind
    }

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
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


void init() {
    initRain();

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
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseClick);
    glutPassiveMotionFunc(passiveMotion); // for  passiveMotion function call
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
