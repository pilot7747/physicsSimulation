//
//  display.h
//  physics
//
//  Created by Никита on 16.04.2018.
//  Copyright © 2018 Nikita Pavlichenko. All rights reserved.
//

#ifndef display_h
#define display_h
#pragma once
#include <GLUT/GLUT.h>
#include <OpenGL/OpenGL.h>
#include <vector>
#include <cmath>
#include <string>

#include "borders.h"


struct window {
    int width, height;
    window(int a, int b) : width(a), height(b) {}
    window() {}
};

void printtext(int x, int y, std::string String)
{
    //(x,y) is from the bottom left of the window
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 600, 0, 600, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glPushAttrib(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);
    glRasterPos2i(x,y);
    for (int i=0; i<String.size(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, String[i]);
    }
    glPopAttrib();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

class Render {
private:
    window w;
    void drawPoint(const atom &a);
    double coordZ(const d_8& z);
    double coordX(const d_8& x);
    void drawPlane(const border& b);
    std::vector<atom>* atoms;
    std::vector<border>* planes;
public:
    long double pressure;
    Render() {}
    Render(std::vector<atom>* _atoms, std::vector<border>* _planes, window _window) : atoms(_atoms), planes(_planes), w(_window) {}
    void draw();
};

void Render::draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glColor4d(0.5, 0.0, 0.5, 1.0);
    for (auto& pln : *planes) {
        drawPlane(pln);
    }
    for (auto& atm : *atoms) {
        drawPoint(atm);
    }
    std::string pres = "Pressure: ";
    std::ostringstream outs;
    outs.width(10);
    outs.precision(10);
    outs << pressure;
    pres += outs.str();
    printtext(10, 10, pres);
    glutSwapBuffers();
    glutPostRedisplay();
}

double Render::coordX(const d_8& x) {
    return x / w.width;
}

double Render::coordZ(const d_8& z) {
    return z / w.height;
}

void Render::drawPlane(const border& b) {
    glBegin(GL_LINES);
        glVertex2d(coordX(b.p1.x), coordZ(b.p1.z));
        glVertex2d(coordX(b.p4.x), coordZ(b.p4.z));
    glEnd();
}

void Render::drawPoint(const atom &a) {
    glBegin(GL_POLYGON);
    for (d_8 i = 0; i < 2 * 3.14159; i += 3.14159 / 2.0)
        glVertex3d(cos(i) * 0.01 + coordX(a.x()), sin(i) * 0.01 + coordZ(a.z()), 0);
    glEnd();
}

#endif /* display_h */
