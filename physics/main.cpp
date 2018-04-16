//
//  main.cpp
//  physics
//
//  Created by Никита on 15.04.2018.
//  Copyright © 2018 Nikita Pavlichenko. All rights reserved.
//
#pragma once
#include <iostream>
#include <GLUT/GLUT.h>
#include <OpenGL/OpenGL.h>
#include <cmath>
#include <vector>
#include <thread>
#include <sstream>
#include "atom.h"
#include "borders.h"
#include "engine.h"
#include "graphics.h"

window _window(600, 600);

constexpr int maxX = 400;
constexpr int maxY = 400;
constexpr int maxZ = 400;

std::vector<atom> atoms;
std::vector<border> planes;
Render render(&atoms, &planes, _window);
Engine engine(&atoms, &planes);

void Idle() {
    render.pressure = engine.pressure;
    render.draw();
}

void Display() {
    render.draw();
}

void InitGlut(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(600, 600);
    
    //W_WIDTH = 1000;
    //W_HEIGHT = 1000;
    glutInitWindowPosition(100, 200);
    glutCreateWindow("Физика");
    glutDisplayFunc(Display);
    glutIdleFunc(Idle);
    //glutReshapeFunc(WindowReshape);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
}

void InitializeObjects(size_t size) {
    border left;
    left.p1 = point(-250, 0, -250);
    left.p2 = point(-250, 600, -250);
    left.p3 = point(-250, 0, 250);
    left.p4 = point(-250, 600, 250);
    left.type = borderType::vertical;
    
    border right;
    right.p1 = point(250, 0, -250);
    right.p2 = point(250, 600, -250);
    right.p3 = point(250, 0, 250);
    right.p4 = point(250, 600, 250);
    right.type = borderType::vertical;
    
    border up;
    up.p1 = point(-250, 0, 250);
    up.p2 = point(250, 600, 250);
    up.p3 = point(-250, 0, 250);
    up.p4 = point(250, 600, 250);
    up.type = borderType::horizontal;
    
    border down;
    down.p1 = point(-250, 0, -250);
    down.p2 = point(250, 600, -250);
    down.p3 = point(-250, 0, -250);
    down.p4 = point(250, 600, -250);
    down.type = borderType::horizontal;
    
    /*border background;
    background.p1 = point(-250, 0, -250);
    background.p2 = point(-250, 0, 250);
    background.p3 = point(250, 0, 250);
    background.p4 = point(250, 0, -250);
    background.type = borderType::ortogonal;
    
    border front;
    front.p1 = point(-250, 600, -250);
    front.p2 = point(-250, 600, 250);
    front.p3 = point(250, 600, 250);
    front.p4 = point(250, 600, -250);
    front.type = borderType::ortogonal;*/
    
    planes.push_back(left);
    planes.push_back(right);
    planes.push_back(up);
    planes.push_back(down);
    //planes.push_back(background);
    //planes.push_back(front);
    
    for (int i = 0; i < size; ++i) {
        atom a;
        Point p = point((rand() % maxX) - 240, (rand() % maxY) - 240, (rand() % maxZ) - 240);
        a.setCoor(p);
        a.prevPoint = p;
        a.v.x = rand() % 482000;
        a.v.y = rand() % 482000;
        a.v.z = rand() % 482000;
        atoms.push_back(a);
    }
}

void startThread() {
    engine.startEngine();
}

int main(int argc, char *argv[]) {
    srand(3);
    InitializeObjects(100000);
    _window.width = 600;
    _window.height = 600;
    InitGlut(argc, argv);
    std::thread thread(startThread);
    thread.detach();
    glutMainLoop();
    return 0;
}
