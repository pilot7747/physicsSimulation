//
//  main.cpp
//  physics
//
//  Created by Никита on 15.04.2018.
//  Copyright © 2018 Nikita Pavlichenko. All rights reserved.
//
#pragma once
#include <iostream>

//#include <GLUT/GLUT.h>
//#include <OpenGL/OpenGL.h>

#include <cmath>
#include <vector>
#include <thread>
#include <sstream>
#include <random>
#include "atom.h"
#include "borders.h"
#include "engine.h"
//#include <omp.h>

constexpr int windowSize = 800;
unsigned long long maximumSpeed = 890;

//window _window(windowSize, windowSize);

constexpr int maxX = windowSize - 200;
constexpr int maxY = windowSize - 200;
constexpr int maxZ = windowSize - 200;

std::vector<atom> atoms; //Массив малекул
std::vector<border> planes; //Массив стенок сосуда

Engine engine(&atoms, &planes); //Движок


void setVexel(double size, double speed = 0, bool piston = false) {
    size /= 2;
    border left; //Левая стенка
    left.p1 = point(-size, -size, -size);
    left.p2 = point(-size, size, -size);
    left.p3 = point(-size, -size, size);
    left.p4 = point(-size, size, size);
    left.type = borderType::vertical;
    if (!piston)
        left.v.x = speed;
    
    border right; //Правая
    right.p1 = point(size, -size, -size);
    right.p2 = point(size, size, -size);
    right.p3 = point(size, -size, size);
    right.p4 = point(size, size, size);
    right.type = borderType::vertical;
    right.v.x = -speed;
    
    border up; //Верхняя
    up.p1 = point(-size, -size, size);
    up.p2 = point(size, size, size);
    up.p3 = point(-size, -size, size);
    up.p4 = point(size, size, size);
    up.type = borderType::horizontal;
    if (!piston)
        up.v.z = -speed;
    
    border down; //Нижняя
    down.p1 = point(-size, -size, -size);
    down.p2 = point(size, size, -size);
    down.p3 = point(-size, -size, -size);
    down.p4 = point(size, size, -size);
    down.type = borderType::horizontal;
    if (!piston)
        down.v.z = speed;
    
    border background; //Задняя
    background.p1 = point(-size, -size, -size);
    background.p2 = point(-size, -size, size);
    background.p3 = point(size, -size, size);
    background.p4 = point(size, -size, -size);
    background.type = borderType::ortogonal;
    if (!piston)
        background.v.y = speed;
    
    border front; //Передняя
    front.p1 = point(-size, size, -size);
    front.p2 = point(-size, size, size);
    front.p3 = point(size, size, size);
    front.p4 = point(size, size, -size);
    front.type = borderType::ortogonal;
    if (!piston)
        front.v.y = -speed;
    
    planes.push_back(left);
    planes.push_back(right);
    planes.push_back(up);
    planes.push_back(down);
    planes.push_back(background);
    planes.push_back(front);
    
    size *= 2;
    totalArea = size * size * 6;
}

vec SphereUniform() {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator (seed);
    std::uniform_real_distribution<double> uniform01(0.0, 1.0);

    vec res;
    double theta = 2 * M_PI * uniform01(generator);
    double phi = acos(1 - 2 * uniform01(generator));
    res.x = sin(phi) * cos(theta);
    res.y = sin(phi) * sin(theta);
    res.z = cos(phi);
    return res;
}

//Генерируем объекты
void InitializeObjects(size_t size, const std::string& mode = "constant", bool gravity=false) {
    
    double vexel_size = 1; // Cорона куба
    setVexel(vexel_size);
    
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(-vexel_size + 0.001, vexel_size - 0.001);
    std::uniform_real_distribution<double> angle_distribution;
    
    //Генерим молекулы
    for (int i = 0; i < size; ++i) {
        atom a;
        Point p = point(distribution(generator), distribution(generator), distribution(generator));
        a.setCoor(p);
        a.prevPoint = p;

        if (mode == "constant") {
            a.v = SphereUniform();
            a.v *= static_cast<long double>(maximumSpeed);
        }

        if (gravity) {
            a.a.z = -9.8;
        }
        atoms.push_back(a);
    }
}

void startThread() {
    engine.startEngine();
}

int main(int argc, char *argv[]) {
    std::ios_base::sync_with_stdio(false);
    srand(4);
    maximumSpeed = atoi(argv[2]);
    InitializeObjects(atoi(argv[1])); //Создаем объекты
    startThread();
    return 0;
}
