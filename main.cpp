#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <algorithm>

// ============================================================
// WINDOW CONSTANTS
// ============================================================

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 1000;
const float GROUND_Y = 160.0f;

// ============================================================
// BASIC STRUCTURES
// ============================================================

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

void drawRawCraneModel() {
    glLineWidth(1.6f);

    // 1. Base Foundation Slab & Legs
    glColor3f(0.16f, 0.22f, 0.28f);
    glBegin(GL_QUADS);
    glVertex2f(-22, 0); glVertex2f(22, 0); glVertex2f(22, 8); glVertex2f(-22, 8);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(-20, 8); glVertex2f(-10, 35);
    glVertex2f(20, 8);  glVertex2f(10, 35);
    glVertex2f(-20, 8); glVertex2f(10, 8);
    glVertex2f(20, 8);  glVertex2f(-10, 8);
    glEnd();

    // 2. Vertical Mast (Lattice Tower)
    glBegin(GL_LINES);
    glVertex2f(-10, 0); glVertex2f(-10, 400);
    glVertex2f(10, 0);  glVertex2f(10, 400);

    for (float y = 0; y < 400; y += 50) {
        glVertex2f(-10, y + 50); glVertex2f(10, y + 50);
        glVertex2f(-10, y);      glVertex2f(10, y + 50);
        glVertex2f(10, y);       glVertex2f(-10, y + 50);
    }
    glEnd();

    // 3. Operator Cabin
    glColor3f(0.18f, 0.24f, 0.30f);
    glBegin(GL_POLYGON);
    glVertex2f(2, 375);  glVertex2f(24, 375);
    glVertex2f(29, 387); glVertex2f(24, 397);
    glVertex2f(2, 397);
    glEnd();

    glColor3f(0.72f, 0.86f, 0.94f);
    glBegin(GL_POLYGON);
    glVertex2f(9, 382);  glVertex2f(23, 382);
    glVertex2f(26, 387); glVertex2f(23, 393);
    glVertex2f(9, 393);
    glEnd();

    // 4. Apex (Cat-Head)
    glColor3f(0.16f, 0.22f, 0.28f);
    glBegin(GL_LINES);
    glVertex2f(-10, 400); glVertex2f(0, 465);
    glVertex2f(10, 400);  glVertex2f(0, 465);
    glVertex2f(0, 400);   glVertex2f(0, 465);
    glVertex2f(-5, 432);  glVertex2f(5, 432);
    glVertex2f(-10, 400); glVertex2f(5, 432);
    glVertex2f(10, 400);  glVertex2f(-5, 432);
    glEnd();

    // 5. Horizontal Working Jib & Counter-Jib
    glBegin(GL_LINES);
    glVertex2f(-75, 416); glVertex2f(180, 416);
    glVertex2f(-75, 400); glVertex2f(180, 400);
    glVertex2f(-75, 400); glVertex2f(-75, 416);
    glVertex2f(180, 400); glVertex2f(180, 416);

    for (float x = -75; x < 180; x += 25) {
        float nx = std::min(x + 25.0f, 180.0f);
        glVertex2f(nx, 400); glVertex2f(nx, 416);
        glVertex2f(x, 400);  glVertex2f(nx, 416);
        glVertex2f(x, 416);  glVertex2f(nx, 400);
    }
    glEnd();

    // 6. Pendant Stay Cables
    glColor3f(0.10f, 0.14f, 0.18f);
    glBegin(GL_LINES);
    glVertex2f(0, 465); glVertex2f(-70, 416);
    glVertex2f(0, 465); glVertex2f(-45, 416);
    glVertex2f(0, 465); glVertex2f(90, 416);
    glVertex2f(0, 465); glVertex2f(160, 416);
    glEnd();

    // 7. Counterweight Ballast Blocks
    glColor3f(0.38f, 0.44f, 0.48f);
    glBegin(GL_QUADS);
    glVertex2f(-71, 380); glVertex2f(-39, 380);
    glVertex2f(-39, 400); glVertex2f(-71, 400);
    glEnd();

    // 8. Trolley & Suspended Steel Beam
    glColor3f(0.22f, 0.28f, 0.34f);
    glBegin(GL_QUADS);
    glVertex2f(106, 395); glVertex2f(124, 395);
    glVertex2f(124, 400); glVertex2f(106, 400);
    glEnd();

    glColor3f(0.10f, 0.14f, 0.18f);
    glBegin(GL_LINES);
    glVertex2f(112, 395); glVertex2f(112, 280);
    glVertex2f(118, 395); glVertex2f(118, 280);
    glVertex2f(115, 274); glVertex2f(92, 252);
    glVertex2f(115, 274); glVertex2f(138, 252);
    glEnd();

    glColor3f(0.50f, 0.56f, 0.60f);
    rect(87, 244, 56, 8);
    glLineWidth(1.0f);
}

void drawTranslatedCrane(float x, float y, float scaleX = 1.0f, float scaleY = 1.0f) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    if (scaleX != 1.0f || scaleY != 1.0f) {
        glScalef(scaleX, scaleY, 1.0f);
    }
    drawRawCraneModel();
    glPopMatrix();
}

// ============================================================
// ENVIRONMENT
// ============================================================

void drawSky() {
    setColor(0.52f, 0.75f, 0.92f);
    rect(0, GROUND_Y, WINDOW_WIDTH, WINDOW_HEIGHT - GROUND_Y);

    // Sun
    setColor(1.0f, 0.86f, 0.25f);
    circle(1450, 870, 55);

    // Clouds
    setColor(1.0f, 1.0f, 1.0f);
    circle(180, 850, 30);  circle(220, 865, 40);  circle(265, 850, 30);  rect(180, 820, 85, 40);
    circle(650, 900, 25);  circle(685, 915, 35);  circle(725, 900, 25);  rect(650, 875, 75, 35);
    circle(1100, 810, 30); circle(1140, 825, 42); circle(1185, 810, 30); rect(1100, 785, 85, 40);
}

void drawGround() {
    setColor(0.40f, 0.58f, 0.32f);
    rect(0, 0, WINDOW_WIDTH, GROUND_Y);

    setColor(0.62f, 0.62f, 0.60f);
    rect(0, 125, WINDOW_WIDTH, 35);

    setColor(0.15f, 0.15f, 0.16f);
    rect(0, 0, WINDOW_WIDTH, 125);

    setColor(0.95f, 0.82f, 0.20f);
    for (int x = 20; x < WINDOW_WIDTH; x += 120) {
        rect(x, 58, 65, 7);
    }

    setColor(0.48f, 0.48f, 0.46f);
    for (int x = 0; x < WINDOW_WIDTH; x += 80) {
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

void buildingNormal(const Building& b) {
    setColor(b.r, b.g, b.b);
    rect(b.x, b.y, b.width, b.height);
    setColor(b.r * 0.75f, b.g * 0.75f, b.b * 0.75f);
    rect(b.x - 3, b.y + b.height, b.width + 6, 8);

    float floorHeight = b.height / b.floors;
    setColor(0.78f, 0.88f, 0.92f);
    for (int floor = 0; floor < b.floors; floor++) {
        float wy = b.y + floor * floorHeight + floorHeight * 0.30f;
        int columns = (int)(b.width / 35.0f);
        if (columns < 1) columns = 1;
        float spacing = b.width / (columns + 1);

        for (int col = 0; col < columns; col++) {
            float wx = b.x + spacing * (col + 1) - 7;
            rect(wx, wy, 14, floorHeight * 0.35f);
        }
    }
}

void buildingGlass(const Building& b) {
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

void drawHalfConstructedBuilding(const Building& b, float progress) {
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
        if (columns < 1) columns = 1;
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
// 2. HEAVY STEEL SUPERSTRUCTURE (FOR THE 2 LARGEST TOWERS)
// ============================================================

void drawHeavySteelSuperstructure(const Building& b, float progress) {
    float currentHeight = b.height * progress;
    float floorHeight = b.height / b.floors;
    int completedFloors = (int)(currentHeight / floorHeight);

    // 1. Lower completed building structure
    setColor(b.r, b.g, b.b);
    rect(b.x, b.y, b.width, currentHeight);

    // Concrete floor slabs
    setColor(0.30f, 0.32f, 0.34f);
    for (int i = 0; i <= completedFloors; i++) {
        float y = b.y + i * floorHeight;
        if (y <= b.y + currentHeight) {
            rect(b.x - 4, y, b.width + 8, 5);
        }
    }

    // Windows on completed lower floors
    setColor(0.75f, 0.88f, 0.93f);
    int columns = 3;
    float colSpacing = b.width / (columns + 1);
    for (int floor = 0; floor < completedFloors; floor++) {
        float y = b.y + floor * floorHeight + 10;
        for (int c = 0; c < columns; c++) {
            float x = b.x + colSpacing * (c + 1) - 8;
            rect(x, y, 16, floorHeight * 0.40f);
        }
    }

    // 2. Heavy Steel Skeleton Superstructure (Matching Image Reference)
    float steelTopY = b.y + b.height;
    float colW = 8.0f;
    float beamH = 7.0f;

    // Steel beam colors
    const float steelR = 0.38f, steelG = 0.45f, steelB = 0.50f;
    const float darkSteelR = 0.22f, darkSteelG = 0.28f, darkSteelB = 0.32f;

    // Background exposed towers / lift core behind steel cage
    setColor(0.55f, 0.65f, 0.72f);
    rect(b.x + colSpacing * 0.6f, b.y + currentHeight, colSpacing * 0.8f, b.height * (1.0f - progress) * 0.85f);
    rect(b.x + colSpacing * 1.8f, b.y + currentHeight, colSpacing * 0.8f, b.height * (1.0f - progress) * 0.95f);

    // Structural steel bays
    float colX0 = b.x + 2;
    float colX1 = b.x + b.width * 0.33f;
    float colX2 = b.x + b.width * 0.66f;
    float colX3 = b.x + b.width - colW - 2;

    // Stored cargo pallets / construction materials on upper steel deck
    setColor(0.60f, 0.42f, 0.22f);
    rect(colX0 + 12, b.y + currentHeight + 6, 28, 20);
    rect(colX1 + 10, b.y + currentHeight + 6, 22, 14);

    // Thick Vertical Steel Columns
    setColor(steelR, steelG, steelB);
    rect(colX0, b.y + currentHeight, colW, steelTopY - (b.y + currentHeight));
    rect(colX1, b.y + currentHeight, colW, steelTopY - (b.y + currentHeight));
    rect(colX2, b.y + currentHeight, colW, steelTopY - (b.y + currentHeight) * 0.9f);
    rect(colX3, b.y + currentHeight, colW, (steelTopY - (b.y + currentHeight)) * 0.7f);

    // Thick Horizontal Steel I-Beams
    for (float y = b.y + currentHeight + floorHeight; y < steelTopY; y += floorHeight) {
        float bWidth = (y < b.y + currentHeight + floorHeight * 2) ? b.width : b.width * 0.68f;
        setColor(steelR, steelG, steelB);
        rect(b.x, y, bWidth, beamH);

        // Beam outline accent
        setColor(darkSteelR, darkSteelG, darkSteelB);
        line(b.x, y, b.x + bWidth, y);
        line(b.x, y + beamH, b.x + bWidth, y + beamH);
    }

    // Heavy Diagonal Cross-Bracing Girders (As shown in image)
    glLineWidth(4.0f);
    setColor(steelR * 0.9f, steelG * 0.9f, steelB * 0.9f);
    glBegin(GL_LINES);
    // Middle bay diagonal brace
    glVertex2f(colX1 + colW, b.y + currentHeight + beamH);
    glVertex2f(colX2, b.y + currentHeight + floorHeight);

    // Upper bay diagonal brace
    glVertex2f(colX1 + colW, b.y + currentHeight + floorHeight + beamH);
    glVertex2f(colX2, b.y + currentHeight + floorHeight * 2);

    // Top structural diagonal brace
    glVertex2f(colX1 + colW, b.y + currentHeight + floorHeight * 2 + beamH);
    glVertex2f(colX2, b.y + currentHeight + floorHeight * 3);
    glEnd();

    // Dark edge accents on columns
    glLineWidth(1.0f);
    setColor(darkSteelR, darkSteelG, darkSteelB);
    line(colX0, b.y + currentHeight, colX0, steelTopY);
    line(colX1, b.y + currentHeight, colX1, steelTopY);
    line(colX2, b.y + currentHeight, colX2, steelTopY - 30);
    line(colX3, b.y + currentHeight, colX3, steelTopY - 90);
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

void drawTree(float x, float y, float scale) {
    setColor(0.38f, 0.20f, 0.08f);
    rect(x - 6 * scale, y, 12 * scale, 45 * scale);

    setColor(0.12f, 0.48f, 0.18f);
    circle(x, y + 60 * scale, 25 * scale);
    circle(x - 20 * scale, y + 48 * scale, 20 * scale);
    circle(x + 20 * scale, y + 48 * scale, 20 * scale);
}

void drawTrees() {
    drawTree(40, 160, 1.0f);
    drawTree(155, 160, 0.85f);
    drawTree(300, 160, 0.90f);
    drawTree(390, 160, 0.75f);
    drawTree(520, 160, 0.95f);
    drawTree(675, 160, 0.85f);
    drawTree(790, 160, 0.90f);
    drawTree(1000, 160, 0.80f);
    drawTree(1090, 160, 0.95f);
    drawTree(1260, 160, 0.85f);
    drawTree(1400, 160, 0.95f);
    drawTree(1540, 160, 0.80f);
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
    Building left1 = { 45, 160, 100, 150, 0.72f, 0.43f, 0.35f, 5, 0 };
    buildingNormal(left1);

    Building left2 = { 175, 160, 110, 190, 0.45f, 0.65f, 0.75f, 6, 1 };
    buildingGlass(left2);

    Building left3 = { 310, 160, 90, 140, 0.72f, 0.58f, 0.46f, 5, 0 };
    buildingNormal(left3);

    // ========================================================
    // LARGEST BUILDING #1 (HEAVY STEEL SUPERSTRUCTURE WITH BRACES)
    // ========================================================
    Building tower1 = { 670, 160, 130, 460, 0.48f, 0.68f, 0.76f, 14, 1 };
    drawHeavySteelSuperstructure(tower1, 0.48f);

    // Small/Medium Buildings (Cleaned up with short rebar stubs)
    Building tower2 = { 420, 160, 115, 360, 0.35f, 0.55f, 0.68f, 12, 2 };
    drawHalfConstructedBuilding(tower2, 0.65f);

    Building centerLow = { 550, 160, 105, 230, 0.70f, 0.55f, 0.43f, 7, 0 };
    buildingNormal(centerLow);

    // ========================================================
    // LARGEST BUILDING #2 (MAIN CENTRAL TALL TOWER)
    // ========================================================
    Building central = { 830, 160, 145, 520, 0.45f, 0.66f, 0.76f, 17, 1 };
    drawHeavySteelSuperstructure(central, 0.38f);

    Building centerRight = { 995, 160, 115, 280, 0.72f, 0.62f, 0.52f, 9, 0 };
    buildingNormal(centerRight);

    // Medium/Small Towers (Reduced lines)
    Building rightTower = { 1130, 160, 120, 400, 0.42f, 0.61f, 0.70f, 14, 1 };
    drawHalfConstructedBuilding(rightTower, 0.55f);

    Building twin1 = { 1280, 160, 95, 420, 0.52f, 0.63f, 0.70f, 15, 1 };
    drawHalfConstructedBuilding(twin1, 0.40f);

    Building twin2 = { 1410, 160, 95, 420, 0.12f, 0.48f, 0.66f, 15, 1 };
    drawHalfConstructedBuilding(twin2, 0.25f);

    Building rightLow = { 1520, 160, 80, 240, 0.68f, 0.42f, 0.38f, 7, 0 };
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

// ============================================================
// OPENGL CALLBACKS & MAIN
// ============================================================

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawPartialCity();

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 1600, 0, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0);
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("City Construction Scene");

    glClearColor(0.52f, 0.75f, 0.92f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
