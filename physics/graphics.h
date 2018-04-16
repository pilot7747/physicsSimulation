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


struct window { //Класс "окно"
    int width, height;
    window(int a, int b) : width(a), height(b) {}
    window() {}
};

void printtext(int x, int y, std::string String) //Написать текст в GLUT
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

class Render { //Рендер
private:
    window w;
    void drawPoint(const atom &a); //Нарисовать молекулу
    double coordZ(const d_8& z); //Перевести из адекватных координат в GLUT'овские
    double coordX(const d_8& x); //Перевести из адекватных координат в GLUT'овские
    void drawPlane(const border& b); //Нарисовать прямоугольник
    std::vector<atom>* atoms; //Указатель на массив атомов
    std::vector<border>* planes; //Указатель на массив стенок
public:
    long double pressure; //Давление
    Render() {}
    Render(std::vector<atom>* _atoms, std::vector<border>* _planes, window _window) : atoms(_atoms), planes(_planes), w(_window) {}
    void draw();
};

void Render::draw() { //Рисовать все, что нужно
    glClear(GL_COLOR_BUFFER_BIT); //Очистка буффера
    glLoadIdentity();
    glColor4d(0.5, 0.0, 0.5, 1.0);
    for (auto& pln : *planes) { //Рисуем стенки
        drawPlane(pln);
    }
    for (auto& atm : *atoms) { //Рисуем молекулы
        drawPoint(atm);
    }
    //Пишем давление
    std::string pres = "Pressure: ";
    std::ostringstream outs;
    outs.width(10);
    outs.precision(10);
    outs << pressure;
    pres += outs.str();
    printtext(10, 10, pres);
    
    glutSwapBuffers(); //Обновляем буффер
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
        glVertex2d(coordX(b.p1.x * w.width), coordZ(b.p1.z * w.height));
        glVertex2d(coordX(b.p4.x * w.width), coordZ(b.p4.z * w.height));
    glEnd();
}

void Render::drawPoint(const atom &a) {
    glPointSize(1.0f);
    glBegin(GL_POINTS);
        glColor3f(0.0, 1, 0.0);
        glVertex2d((double)(a.x()), double(a.z()));
    glEnd();
}

#endif /* display_h */
