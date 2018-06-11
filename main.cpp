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
//#include "/usr/local/Cellar/glfw/3.2.1/include/GLFW/glfw3.h"
#include <OpenGL/OpenGL.h>
#include <cmath>
#include <vector>
#include <thread>
#include <sstream>
#include "atom.h"
#include "borders.h"
#include "engine.h"
#include "graphics.h"
//#include <omp.h>

constexpr int windowSize = 800;
constexpr unsigned long long maximumSpeed = 1000;

window _window(windowSize, windowSize);

constexpr int maxX = windowSize - 200;
constexpr int maxY = windowSize - 200;
constexpr int maxZ = windowSize - 200;

std::vector<atom> atoms; //Массив малекул
std::vector<border> planes; //Массив стенок сосуда

Render render(&atoms, &planes, _window); //Рендер
Engine engine(&atoms, &planes); //Движок

//Функция, которую постоянно запускает GLUT
void Idle() {
    render.pressure = engine.pressure; //Предаем давление в рендер
    render.bumps = engine.bumps;
    render.timeLapsed = engine.timeLapsed;
    //if (engine.calculated) {
    //    render.distribution = engine.distribution;
    //}
    render.draw(); //Отрисовываем
}

//Отрисовка
void Display() {
    render.draw();
}

//Запуск GLUT
void InitGlut(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 200);
    glutCreateWindow("Physics");
    glutDisplayFunc(Display);
    glutIdleFunc(Idle);
    //glutReshapeFunc(WindowReshape);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
}

//Генерируем объекты
void InitializeObjects(size_t size) {
    border left; //Левая стенка
    left.p1 = point(-0.5, -0.5, -0.5);
    left.p2 = point(-0.5, 0.5, -0.5);
    left.p3 = point(-0.5, -0.5, 0.5);
    left.p4 = point(-0.5, 0.5, 0.5);
    left.type = borderType::vertical;
    //left.v.x = 0.001;
    
    border right; //Правая
    right.p1 = point(0.5, -0.5, -0.5);
    right.p2 = point(0.5, 0.5, -0.5);
    right.p3 = point(0.5, -0.5, 0.5);
    right.p4 = point(0.5, 0.5, 0.5);
    right.type = borderType::vertical;
    //right.v.x = -0.001;
    
    border up; //Верхняя
    up.p1 = point(-0.5, -0.5, 0.5);
    up.p2 = point(0.5, 0.5, 0.5);
    up.p3 = point(-0.5, -0.5, 0.5);
    up.p4 = point(0.5, 0.5, 0.5);
    up.type = borderType::horizontal;
    //up.v.z = -0.001;
    
    border down; //Нижняя
    down.p1 = point(-0.5, -0.5, -0.5);
    down.p2 = point(0.5, 0.5, -0.5);
    down.p3 = point(-0.5, -0.5, -0.5);
    down.p4 = point(0.5, 0.5, -0.5);
    down.type = borderType::horizontal;
    //down.v.z = 0.001;
    
    border background; //Задняя
    background.p1 = point(-0.5, -0.5, -0.5);
    background.p2 = point(-0.5, -0.5, 0.5);
    background.p3 = point(0.5, -0.5, 0.5);
    background.p4 = point(0.5, -0.5, -0.5);
    background.type = borderType::ortogonal;
    //background.v.y = 0.001;
    
    border front; //Передняя
    front.p1 = point(-0.5, 0.5, -0.5);
    front.p2 = point(-0.5, 0.5, 0.5);
    front.p3 = point(0.5, 0.5, 0.5);
    front.p4 = point(0.5, 0.5, -0.5);
    front.type = borderType::ortogonal;
    //front.v.y = -0.001;
    
    planes.push_back(left);
    planes.push_back(right);
    planes.push_back(up);
    planes.push_back(down);
    planes.push_back(background);
    planes.push_back(front);
    
    //Генерим молекулы
    for (int i = 0; i < size; ++i) {
        atom a;
        Point p = point((rand() % maxX) - 240, (rand() % maxY) - 240, (rand() % maxZ) - 240);
        p.x /= 500.0;
        p.y /= 500.0;
        p.z /= 500.0;
        a.setCoor(p);
        a.prevPoint = p;
        a.v.x = 1000;//rand() % maximumSpeed + 1;
        if (rand() % 2)
            a.v.x *= -1;
        a.v.y = 0;//rand() % maximumSpeed + 1;
        if (rand() % 2)
            a.v.y *= -1;
        a.v.z = 0;//rand() % maximumSpeed + 1;
        if (rand() % 2)
            a.v.z *= -1;
        a.a.z = -9.8;
        atoms.push_back(a);
    }
}
//Стартуем
void startThread() {
    engine.startEngine();
}

int main(int argc, char *argv[]) {
    render.max_speed = static_cast<unsigned long long>((long double)(maximumSpeed) * 1.73205);
    srand(4);
    InitializeObjects(20000); //Создаем объекты
    _window.width = 800;
    _window.height = 800;
    InitGlut(argc, argv);
    std::thread thread(startThread); //Стартуем поток движка
    thread.detach();
    glutMainLoop();
    return 0;
}
