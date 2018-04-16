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

constexpr int windowSize = 600;
constexpr int maximumSpeed = 2;

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
    glutInitWindowSize(600, 600);
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
    left.p1 = point(-0.5, 0, -0.5);
    left.p2 = point(-0.5, 1.2, -0.5);
    left.p3 = point(-0.5, 0, 0.5);
    left.p4 = point(-0.5, 1.2, 0.5);
    left.type = borderType::vertical;
    
    border right; //Правая
    right.p1 = point(0.5, 0, -0.5);
    right.p2 = point(0.5, 1.2, -0.5);
    right.p3 = point(0.5, 0, 0.5);
    right.p4 = point(0.5, 1.2, 0.5);
    right.type = borderType::vertical;
    
    border up; //Верхняя
    up.p1 = point(-0.5, 0, 0.5);
    up.p2 = point(0.5, 1.2, 0.5);
    up.p3 = point(-0.5, 0, 0.5);
    up.p4 = point(0.5, 1.2, 0.5);
    up.type = borderType::horizontal;
    
    border down; //Нижняя
    down.p1 = point(-0.5, 0, -0.5);
    down.p2 = point(0.5, 1.2, -0.5);
    down.p3 = point(-0.5, 0, -0.5);
    down.p4 = point(0.5, 1.2, -0.5);
    down.type = borderType::horizontal;
    
    border background; //Задняя
    background.p1 = point(-0.5, 0, -0.5);
    background.p2 = point(-0.5, 0, 0.5);
    background.p3 = point(0.5, 0, 0.5);
    background.p4 = point(0.5, 0, -0.5);
    background.type = borderType::ortogonal;
    
    border front; //Передняя
    front.p1 = point(-0.5, 1.2, -0.5);
    front.p2 = point(-0.5, 1.2, 0.5);
    front.p3 = point(0.5, 1.2, 0.5);
    front.p4 = point(0.5, 1.2, -0.5);
    front.type = borderType::ortogonal;
    
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
        p.y /= 600.0;
        p.z /= 500.0;
        a.setCoor(p);
        a.prevPoint = p;
        a.v.x = rand() % maximumSpeed + 1;
        if (rand() % 2)
            a.v.x *= -1;
        a.v.y = rand() % maximumSpeed + 1;
        if (rand() % 2)
            a.v.y *= -1;
        a.v.z = rand() % maximumSpeed + 1;
        if (rand() % 2)
            a.v.z *= -1;
        atoms.push_back(a);
    }
}
//Стартуем
void startThread() {
    engine.startEngine();
}

int main(int argc, char *argv[]) {
    srand(4);
    InitializeObjects(10000); //Создаем объекты
    _window.width = 600;
    _window.height = 600;
    InitGlut(argc, argv);
    std::thread thread(startThread); //Стартуем поток движка
    thread.detach();
    glutMainLoop();
    return 0;
}
