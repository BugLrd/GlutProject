#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <algorithm>

// ============================================================
// WINDOW CONSTANTS & ANIMATION STATE
// ============================================================
const int WIN_W = 1600;
const int WIN_H = 1000;
const float PI = 3.14159265358979323846f;

bool isNightMode = false;
float cloudOffset = 0.0f;
const float CLOUD_SPEED = 0.5f;

// ============================================================
// PRIMITIVE DRAWING HELPERS
// ============================================================
void setColor(float r, float g, float b) {
    glColor3f(r, g, b);
}

void drawRect(float x1, float y1, float x2, float y2) {
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

void drawRectOutline(float x1, float y1, float x2, float y2, float lineWidth = 3.0f, float r = 0.12f, float g = 0.14f, float b = 0.18f) {
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

void drawCircle(float cx, float cy, float r, int segments = 36) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * (float)i / segments;
        glVertex2f(cx + r * cosf(angle), cy + r * sinf(angle));
    }
    glEnd();
}

void drawHill(float cx, float cy, float rx, float ry, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 30; i++) {
        float angle = PI * (float)i / 30.0f;
        glVertex2f(cx + rx * cosf(angle), cy + ry * sinf(angle));
    }
    glEnd();
}

// ============================================================
// ENVIRONMENT (SKY, SUN, MOON, CLOUDS, MOUNTAINS, HILLS)
// ============================================================
void sun() {
    glColor3f(1.0f, 0.85f, 0.0f);
    drawCircle(520.0f, 520.0f, 40.0f, 40);
}

void drawCloud(float cx, float cy, float scale) {
    glColor3f(0.85f, 0.87f, 0.90f);
    drawCircle(cx, cy - 5.0f * scale, 30.0f * scale, 30);
    drawCircle(cx + 30.0f * scale, cy, 36.0f * scale, 30);
    drawCircle(cx + 65.0f * scale, cy - 5.0f * scale, 26.0f * scale, 30);
    drawCircle(cx + 20.0f * scale, cy - 15.0f * scale, 24.0f * scale, 30);
    drawCircle(cx + 50.0f * scale, cy - 15.0f * scale, 22.0f * scale, 30);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(cx, cy, 26.0f * scale, 30);
    drawCircle(cx + 30.0f * scale, cy + 8.0f * scale, 32.0f * scale, 30);
    drawCircle(cx + 62.0f * scale, cy, 22.0f * scale, 30);
}

void drawClouds() {
    static const float baseX[] = { 120.0f, 430.0f, 700.0f, 980.0f, 1250.0f, 1480.0f };
    static const float baseY[] = { 880.0f, 930.0f, 860.0f, 910.0f, 870.0f, 920.0f };
    static const float scale[] = { 1.0f, 0.8f, 1.2f, 0.9f, 1.1f, 0.7f };
    const int numClouds = 6;

    float wrapWidth = (float)WIN_W + 400.0f;
    float dx = fmodf(cloudOffset, wrapWidth);

    for (int pass = 0; pass < 2; pass++) {
        glPushMatrix();
        glTranslatef(dx - (float)pass * wrapWidth, 0.0f, 0.0f);
        for (int i = 0; i < numClouds; i++) {
            drawCloud(baseX[i], baseY[i], scale[i]);
        }
        glPopMatrix();
    }
}

void drawMountains() {
    glPushMatrix();
    glTranslatef(0.0f, -120.0f, 0.0f);

    // Left Mountain
    glColor3f(0.32f, 0.58f, 0.23f);
    glBegin(GL_TRIANGLES);
    glVertex2f(50.0f, 450.0f); glVertex2f(300.0f, 700.0f); glVertex2f(300.0f, 300.0f);
    glEnd();
    glColor3f(0.16f, 0.34f, 0.14f);
    glBegin(GL_TRIANGLES);
    glVertex2f(300.0f, 400.0f); glVertex2f(300.0f, 700.0f); glVertex2f(600.0f, 300.0f);
    glEnd();

    // Center Mountain
    glColor3f(0.32f, 0.58f, 0.23f);
    glBegin(GL_TRIANGLES);
    glVertex2f(200.0f, 300.0f); glVertex2f(570.0f, 800.0f); glVertex2f(570.0f, 300.0f);
    glEnd();
    glColor3f(0.16f, 0.34f, 0.14f);
    glBegin(GL_TRIANGLES);
    glVertex2f(570.0f, 300.0f); glVertex2f(570.0f, 800.0f); glVertex2f(1150.0f, 300.0f);
    glEnd();

    // Right Mountain
    glColor3f(0.32f, 0.58f, 0.23f);
    glBegin(GL_TRIANGLES);
    glVertex2f(800.0f, 300.0f); glVertex2f(1050.0f, 600.0f); glVertex2f(1050.0f, 300.0f);
    glEnd();
    glColor3f(0.16f, 0.34f, 0.14f);
    glBegin(GL_TRIANGLES);
    glVertex2f(1050.0f, 300.0f); glVertex2f(1050.0f, 600.0f); glVertex2f(1450.0f, 300.0f);
    glEnd();

    glPopMatrix();
}

void drawHills() {
    drawHill(250.0f, 220.0f, 400.0f, 180.0f, 0.25f, 0.48f, 0.20f);
    drawHill(1350.0f, 220.0f, 450.0f, 190.0f, 0.22f, 0.44f, 0.18f);
    drawHill(800.0f, 220.0f, 500.0f, 200.0f, 0.28f, 0.52f, 0.22f);
}

void drawNightOverlay() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.02f, 0.02f, 0.15f, 0.55f);
    drawRect(0.0f, 0.0f, (float)WIN_W, (float)WIN_H);

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

    glDisable(GL_BLEND);
}

// -----------------------------------------------------------------scene 4---------------------------------------------------







void drawWindowGrid(float startX, float startY, float totalWidth, float totalHeight,
    int floors, int cols, bool isGlass = false) {

    float floorHeight = totalHeight / (float)floors;
    float slotWidth = (totalWidth * 0.65f) / (float)cols; // per row windows size 
    float sideMargin = totalWidth * 0.175f; // left side gap
    float vertMargin = floorHeight * 0.28f; // vertical gap ( bottom gap)
    float ww = slotWidth * 0.62f;  //actual winodw hight and width
    float wh = floorHeight * 0.44f;

    if (isGlass) glColor3f(0.15f, 0.48f, 0.68f);
    else glColor3f(0.15f, 0.28f, 0.38f);

    for (int r = 0; r < floors; r++) {
        float wy = startY + (float)r * floorHeight + vertMargin; // moves the point to specific floor 
        for (int c = 0; c < cols; c++) {
            float wx = startX + sideMargin + (float)c * slotWidth; // moves coloum 
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
    glVertex2f(0.0f, 160.0f); glVertex2f(100.0f, 160.0f);
    glVertex2f(100.0f, 620.0f); glVertex2f(0.0f, 620.0f);
    glColor3f(0.65f, 0.68f, 0.72f);
    glVertex2f(-3.0f, 620.0f); glVertex2f(103.0f, 620.0f);
    glVertex2f(103.0f, 627.0f); glVertex2f(-3.0f, 627.0f);
    glEnd();

    // Outline
    drawRectOutline(0.0f, 160.0f, 100.0f, 620.0f, 3.0f);
    drawRectOutline(-3.0f, 620.0f, 103.0f, 627.0f, 3.0f);

    glColor3f(0.30f, 0.48f, 0.58f); // loop for virtical lines
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (int i = 1; i < 6; i++) {
        float xx = 100.0f * (float)i / 6.0f;
        glVertex2f(xx, 160.0f); glVertex2f(xx, 620.0f);
    }
    glEnd();

    glColor3f(0.20f, 0.40f, 0.52f);
    float fHeight = 460.0f / 22.0f;  // loop for window 
    for (int r = 0; r < 22; r++) {
        float wy = 160.0f + (float)r * fHeight + fHeight * 0.25f;
        for (int c = 0; c < 4; c++) {
            float wx = 8.0f + (float)c * (84.0f / 4.0f);
            glBegin(GL_QUADS);
            glVertex2f(wx, wy); glVertex2f(wx + 5.0f, wy);
            glVertex2f(wx + 5.0f, wy + fHeight * 0.45f); glVertex2f(wx, wy + fHeight * 0.45f);
            glEnd();
        }
    }
    glColor3f(0.15f, 0.18f, 0.20f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(50.0f, 627.0f); glVertex2f(50.0f, 697.0f);
    glEnd();
}

void drawBuildings() {
    glColor3f(0.55f, 0.67f, 0.73f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 160.0f); glVertex2f(80.0f, 160.0f);
    glVertex2f(80.0f, 380.0f); glVertex2f(0.0f, 380.0f);

    glVertex2f(90.0f, 160.0f); glVertex2f(165.0f, 160.0f);
    glVertex2f(165.0f, 420.0f); glVertex2f(90.0f, 420.0f);

    glVertex2f(1520.0f, 160.0f); glVertex2f(1600.0f, 160.0f);
    glVertex2f(1600.0f, 410.0f); glVertex2f(1520.0f, 410.0f);
    glEnd();

    drawRectOutline(0.0f, 160.0f, 80.0f, 380.0f, 2.5f, 0.25f, 0.35f, 0.40f);
    drawRectOutline(90.0f, 160.0f, 165.0f, 420.0f, 2.5f, 0.25f, 0.35f, 0.40f);
    drawRectOutline(1520.0f, 160.0f, 1600.0f, 410.0f, 2.5f, 0.25f, 0.35f, 0.40f);

    glColor3f(0.35f, 0.55f, 0.65f);
    for (float y = 190.0f; y < 380.0f; y += 28.0f) { // left most tower 
        glBegin(GL_QUADS);
        glVertex2f(15.0f, y); glVertex2f(65.0f, y);
        glVertex2f(65.0f, y + 9.0f); glVertex2f(15.0f, y + 9.0f);
    }
}

void drawCity() {
    drawBuildings();

    // --- B1 (Left) ---
    glColor3f(0.78f, 0.72f, 0.65f);
    glBegin(GL_QUADS);
    glVertex2f(45.0f, 160.0f); glVertex2f(160.0f, 160.0f);
    glVertex2f(160.0f, 335.0f); glVertex2f(45.0f, 335.0f);

    glVertex2f(54.0f, 335.0f); glVertex2f(151.0f, 335.0f);
    glVertex2f(151.0f, 372.5f); glVertex2f(54.0f, 372.5f);

    glVertex2f(65.0f, 372.5f); glVertex2f(139.0f, 372.5f);
    glVertex2f(139.0f, 410.0f); glVertex2f(65.0f, 410.0f);
    glEnd();
    drawWindowGrid(45.0f, 160.0f, 115.0f, 175.0f, 8, 3);
    drawWindowGrid(54.0f, 335.0f, 97.0f, 37.5f, 2, 2);
    drawRectOutline(45.0f, 160.0f, 160.0f, 335.0f, 3.0f);
    drawRectOutline(54.0f, 335.0f, 151.0f, 372.5f, 3.0f);
    drawRectOutline(65.0f, 372.5f, 139.0f, 410.0f, 3.0f);

    // --- B2 
    glColor3f(0.42f, 0.65f, 0.78f);
    glBegin(GL_QUADS);
    glVertex2f(175.0f, 160.0f); glVertex2f(300.0f, 160.0f);
    glVertex2f(300.0f, 470.0f); glVertex2f(175.0f, 470.0f);
    glEnd();
    glColor3f(0.10f, 0.20f, 0.28f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(206.25f, 160.0f); glVertex2f(206.25f, 470.0f);
    glVertex2f(237.50f, 160.0f); glVertex2f(237.50f, 470.0f);
    glVertex2f(268.75f, 160.0f); glVertex2f(268.75f, 470.0f);
    glEnd();
    drawWindowGrid(175.0f, 160.0f, 125.0f, 310.0f, 10, 3, true);
    drawRectOutline(175.0f, 160.0f, 300.0f, 470.0f, 3.0f);

    // --- B3 
    glColor3f(0.78f, 0.38f, 0.32f);
    glBegin(GL_QUADS);
    glVertex2f(315.0f, 160.0f); glVertex2f(415.0f, 160.0f);
    glVertex2f(415.0f, 370.0f); glVertex2f(315.0f, 370.0f);
    glEnd();
    drawWindowGrid(315.0f, 160.0f, 100.0f, 210.0f, 7, 3);
    drawRectOutline(315.0f, 160.0f, 415.0f, 370.0f, 3.0f);

    // --- B4 Spire ---
    glColor3f(0.65f, 0.75f, 0.82f);
    glBegin(GL_QUADS);
    glVertex2f(420.0f, 160.0f); glVertex2f(525.0f, 160.0f);
    glVertex2f(525.0f, 600.0f); glVertex2f(420.0f, 600.0f);
    glEnd();
    drawWindowGrid(420.0f, 160.0f, 105.0f, 440.0f, 14, 3);
    glColor3f(0.20f, 0.25f, 0.30f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(472.5f, 600.0f); glVertex2f(472.5f, 670.0f);
    glEnd();
    drawRectOutline(420.0f, 160.0f, 525.0f, 600.0f, 3.0f);

    // --- B5 Slanted-Roof Modern Skyscraper ---
    glColor3f(0.30f, 0.52f, 0.70f);
    glBegin(GL_QUADS);
    glVertex2f(545.0f, 160.0f); glVertex2f(675.0f, 160.0f);
    glVertex2f(675.0f, 520.0f); glVertex2f(545.0f, 520.0f);
    glVertex2f(545.0f, 520.0f); glVertex2f(675.0f, 555.0f);
    glVertex2f(675.0f, 540.0f); glVertex2f(545.0f, 505.0f);
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
    glVertex2f(690.0f, 160.0f); glVertex2f(825.0f, 160.0f);
    glVertex2f(825.0f, 405.0f); glVertex2f(690.0f, 405.0f);
    glEnd();
    drawWindowGrid(690.0f, 160.0f, 135.0f, 245.0f, 8, 3);
    drawRectOutline(690.0f, 160.0f, 825.0f, 405.0f, 3.0f);

    // --- B7 Central tower + Triangle  ---
    glColor3f(0.48f, 0.70f, 0.80f);
    glBegin(GL_QUADS);
    glVertex2f(840.0f, 160.0f); glVertex2f(975.0f, 160.0f);
    glVertex2f(975.0f, 660.0f); glVertex2f(840.0f, 660.0f);
    glEnd();
    glColor3f(0.62f, 0.68f, 0.73f);
    glBegin(GL_TRIANGLES);
    glVertex2f(840.0f, 660.0f); glVertex2f(975.0f, 660.0f); glVertex2f(907.5f, 725.0f);
    glEnd();
    glColor3f(0.20f, 0.25f, 0.30f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(907.5f, 725.0f); glVertex2f(907.5f, 800.0f);
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
    glVertex2f(1010.0f, 160.0f); glVertex2f(1110.0f, 160.0f);
    glVertex2f(1110.0f, 391.0f); glVertex2f(1010.0f, 391.0f);

    glVertex2f(1018.0f, 391.0f); glVertex2f(1102.0f, 391.0f);
    glVertex2f(1102.0f, 440.5f); glVertex2f(1018.0f, 440.5f);

    glVertex2f(1028.0f, 440.5f); glVertex2f(1092.0f, 440.5f);
    glVertex2f(1092.0f, 490.0f); glVertex2f(1028.0f, 490.0f);
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
    glVertex2f(1410.0f, 160.0f); glVertex2f(1535.0f, 160.0f);
    glVertex2f(1535.0f, 560.0f); glVertex2f(1410.0f, 560.0f);
    glEnd();
    glColor3f(0.55f, 0.60f, 0.65f);
    glBegin(GL_TRIANGLES);
    glVertex2f(1410.0f, 560.0f); glVertex2f(1535.0f, 560.0f); glVertex2f(1472.5f, 620.0f);
    glEnd();
    glColor3f(0.20f, 0.25f, 0.30f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(1472.5f, 620.0f); glVertex2f(1472.5f, 685.0f);
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
    glVertex2f(1530.0f, 160.0f); glVertex2f(1600.0f, 160.0f);
    glVertex2f(1600.0f, 420.0f); glVertex2f(1530.0f, 420.0f);
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


// (ROAD, TREES, STREET LIGHTS)

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

void streetLight(float x) {
    glColor3f(0.12f, 0.14f, 0.16f);
    glLineWidth(4.0f);
    glBegin(GL_LINES);
    glVertex2f(x, 160.0f); glVertex2f(x, 235.0f);
    glVertex2f(x, 235.0f); glVertex2f(x + 22.0f, 235.0f);
    glEnd();

    glColor3f(1.0f, 0.85f, 0.30f);
    drawCircle(x + 25.0f, 232.0f, 6.0f);
}

void drawStreetLights() {
    streetLight(80);   streetLight(300);  streetLight(520);  streetLight(740);
    streetLight(960);  streetLight(1140); streetLight(1360); streetLight(1580);
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
        glVertex2f(x, 95.0f); glVertex2f(x, 130.0f);
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


// SCENE COMPOSITION & CALLBACKS

void renderScene4() {
    if (isNightMode) {
        glClearColor(0.04f, 0.05f, 0.18f, 1.0f);
    }
    else {
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

    if (isNightMode) {
        drawNightOverlay();
    }
}

void display() {
    renderScene4();
    glutSwapBuffers();
}

void update(int value) {
    cloudOffset += CLOUD_SPEED;
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'n' || key == 'N') isNightMode = true;
    else if (key == 'd' || key == 'D') isNightMode = false;
    else if (key == 27) exit(0); // ESC key exits
    glutPostRedisplay();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, WIN_W, 0.0, WIN_H);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIN_W, WIN_H);
    glutCreateWindow("Scene 4 - Modern City Skyline");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}
