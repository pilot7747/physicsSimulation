//
//  engine.h
//  physics
//
//  Created by Никита on 16.04.2018.
//  Copyright © 2018 Nikita Pavlichenko. All rights reserved.
//

#ifndef engine_h
#define engine_h
#include "atom.h"
#include "borders.h"
#include <vector>
#include <chrono>
#include <thread>

constexpr int intTimes = 20;

using namespace std::literals;

bool intersects(const border& plane, const Point& a, const Point& b) {
    int x, y, z;
    if (plane.type == borderType::horizontal) {
        z = plane.p1.z;
        return ((a.z - z) * (b.z - z)) <= 0;
    }
    if (plane.type == borderType::vertical) {
        x = plane.p1.x;
        return ((a.x - x) * (b.x - x)) <= 0;
    }
    y = plane.p1.y;
    return ((a.y - y) * (b.y - y)) <= 0;
}



class Engine {
private:
    std::vector<atom>* atoms;
    std::vector<border>* planes;
    void changeCoords();
    void doIntersections();
    
    void mirror(atom &a, const border &plane) {
        if (plane.type == borderType::horizontal) {
            a.point.z += (plane.p1.z - a.point.z) * 2;
            a.v.z *= -1;
            tmpPres += (abs(a.v.z) * 2 / dt) * 0.0000000000000000000000000482;
        }
        if (plane.type == borderType::vertical) {
            a.point.x += (plane.p1.x - a.point.x) * 2;
            a.v.x *= -1;
            tmpPres += (abs(a.v.x) * 2 / dt) * 0.0000000000000000000000000482;
        }
        a.point.y += (plane.p1.y - a.point.y) * 2;
        a.v.y *= -1;
        tmpPres += (abs(a.v.y) * 2 / dt) * 0.0000000000000000000000000482;
    }
    
public:
    long double tmpPres = 0;
    long double pressure = 0;
    Engine() {}
    Engine(std::vector<atom>* _atoms, std::vector<border>* _planes) : atoms(_atoms), planes(_planes) {}
    void startEngine();
    
};

void Engine::changeCoords() {
    for (auto &atm : *atoms) {
        atm.setCoor(point(atm.point.x + atm.v.x * dt + atm.a.x * dt * dt / 2, atm.point.y + atm.v.y * dt + atm.a.y * dt * dt / 2, atm.point.z + atm.v.z * dt + atm.a.z * dt * dt / 2));
    }
}

void Engine::doIntersections() {
    for (auto &atm : *atoms) {
        for (auto &plane : *planes) {
            if (intersects(plane, atm.prevPoint, atm.point)) {
                mirror(atm, plane);
            }
        }
    }
}

void Engine::startEngine() {
    while (true) {
        changeCoords();
        for (int i = 0; i < intTimes; ++i) {
            doIntersections();
        }
        pressure = tmpPres;
        tmpPres = 0;
        std::this_thread::sleep_for(std::chrono::milliseconds(dt_int));
    }
}


#endif /* engine_h */
