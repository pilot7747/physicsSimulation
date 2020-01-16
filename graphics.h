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
//#include <GLUT/GLUT.h>
//#include <OpenGL/OpenGL.h>
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
    glColor3f(1, 0, 0);
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
    void drawDist();
public:
    long double timeLapsed = 0;
    unsigned long long max_speed = 1;
    long double pressure; //Давление
    unsigned long long bumps = 0;
    unsigned long long sum_speed = 0;
    std::vector<unsigned long long> distribution;
    Render() {}
    Render(std::vector<atom>* _atoms, std::vector<border>* _planes, window _window) : atoms(_atoms), planes(_planes), w(_window) {}
    void draw();
};

void Render::drawDist() {
    constexpr size_t _disSz = 300;
    distribution.clear();
    distribution.resize(_disSz + 1);
    for (int i = 0; i < (*atoms).size(); ++i) {
        unsigned long long v = std::sqrt(((*atoms)[i].v * (*atoms)[i].v));
        if (v / 10 <= _disSz) {
            v /= 10;
            ++distribution[v];
        }
    }
    if (distribution.empty())
        return;
    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    for (int i = 0; i < distribution.size(); ++i) {
        glVertex2d(0.6 + (double(i) * 3) / (double)(w.width), -0.95);
        glVertex2d(0.6 + (double(i) * 3) / (double)(w.width), ((double)(distribution[i]) / (double)(w.height - 300)) / (double(atoms->size()) / 10000.0) - 0.95);
    }
    glEnd();
}


void Render::draw() { //Рисовать все, что нужно
    glClear(GL_COLOR_BUFFER_BIT); //Очистка буффера
    glLoadIdentity();
    glColor4d(1, 0.0, 1, 1.0);
    for (auto& pln : *planes) { //Рисуем стенки
        drawPlane(pln);
    }
    for (int i = 0; i < std::min(int((*atoms).size()), 100000); ++i) { //Рисуем молекулы
        unsigned long long v = (*atoms)[i].get_v();
        sum_speed += v * v;
        if (checkInVessel((*planes)[0], (*planes)[1], (*planes)[2], (*planes)[3], (*atoms)[i].point)) //Только если молекула внутри сосуда
            drawPoint((*atoms)[i]);
    }
    //Пишем давление
    std::string pres = "Pressure: ";
    std::ostringstream outs;
    outs.width(10);
    outs.precision(10);
    outs << pressure;
    pres += outs.str();
    printtext(10, 10, pres);
    
    std::string __bumps = "Bumps: ";
    std::ostringstream outBumps;
    outBumps << bumps;
    __bumps += outBumps.str();
    printtext(200, 10, __bumps);
    
    std::string __time = "Time lapsed: ";
    std::ostringstream outTime;
    outTime.width(3);
    outTime.precision(3);
    outTime << timeLapsed;
    __time += outTime.str();
    __time += " sec";
    printtext(10, 590, __time);
    
    long double rms_speed = std::sqrt(static_cast<long double>(sum_speed) / atoms->size());
    std::string __speed = "RMS Speed: ";
    std::ostringstream speed;
    speed.width(7);
    speed.precision(7);
    speed << rms_speed;
    __speed += speed.str();
    __speed += " m/s";
    printtext(200, 590, __speed);
    sum_speed = 0;
    
    long double temp = (rms_speed * rms_speed * massOfmolecule / 3) / k;
    std::string __temp = "T = ";
    std::ostringstream __tempS;
    __tempS.width(10);
    __tempS.precision(10);
    __tempS << temp;
    __temp += __tempS.str();
    __temp += "K";
    printtext(400, 590, __temp);
    sum_speed = 0;
    
    drawDist();
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
        unsigned long long v = a.get_v();
        glColor3f(1, std::log((long double)(v)) / 10 / 10, std::log((long double)(v)) / 10);
        glVertex2d((double)(a.x()), double(a.z()));
    glEnd();
}

#endif /* display_h */
