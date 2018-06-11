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

constexpr int intTimes = 500; //Количество раз, сколько нужно отработать удары об стенки


using namespace std::literals;

bool diffSignes(const d_8& a, const d_8& b) { // Функция, которая проверяет разные ли знаки у двух чисел
    if ((a <= 0 && b >= 0) || (a >= 0 && b <= 0))
        return true;
    return false;
}

bool intersects(const border& plane, const Point& a, const Point& b) { //Проверка пересечения отрезка и плоскости (переделать!)
    long double x, y, z;
    if (plane.type == borderType::horizontal) {
        z = plane.p1.z;
        return diffSignes(a.z - z, b.z - z);
    }
    if (plane.type == borderType::vertical) {
        x = plane.p1.x;
        return diffSignes(a.x - x, b.x - x);
    }
    if (plane.type == borderType::ortogonal) {
        y = plane.p1.y;
        return diffSignes(a.y - y, b.y - y);
    }
    return false;
}


class Engine { //Движок
private:
    std::vector<atom>* atoms; //Указатель на массив молекул
    std::vector<border>* planes; //Указатель на массив стенок
    void changeCoords(); //Пересчитать координаты
    void doIntersections(); //Обработать удары об стенки
    void doBumps(); //Обработать столкновения
    void movePlanes();
    void calcDistribution();
    void atomBumping(atom& a1, atom& a2);
    constexpr static unsigned long long max_speed = 1;
    
    void mirror(atom &a, const border &plane) { // Отразить относительно плоскости (можно, наверное, переделать)
        if (plane.type == borderType::horizontal) {
            a.point.z += (plane.p1.z - a.point.z) * 2;
            a.v.z *= -1;
            tmpPres += (abs(a.v.z) * 2 / dt) * massOfmolecule; //Изменение имульса
        }
        if (plane.type == borderType::vertical) {
            a.point.x += (plane.p1.x - a.point.x) * 2;
            a.v.x *= -1;
            tmpPres += (abs(a.v.x) * 2 / dt) * massOfmolecule; //Изменение имульса
        }
        if (plane.type == borderType::ortogonal) {
            a.point.y += (plane.p1.y - a.point.y) * 2;
            a.v.y *= -1;
            tmpPres += (abs(a.v.y) * 2 / dt) * massOfmolecule; //Изменение имульса
        }
        a.v += plane.v;
    }
    
public:
    std::vector<unsigned long long> distribution;
    long double tmpPres = 0; //Суммарная сила на стенки сосуда за время dt
    long double pressure = 0; //Давление
    long double timeLapsed = 0;
    unsigned long long bumps = 0; //Колво соударений
    Engine() {}
    Engine(std::vector<atom>* _atoms, std::vector<border>* _planes) : atoms(_atoms), planes(_planes) {}
    void startEngine(); //Запуск
    bool calculated = true;
};

void Engine::atomBumping(atom& a1, atom& a2) {
    if (a1.v.equal(a2.v)) {
        --bumps;
        return;
    }
    while (true) {
        if (rand() % 2) {
            std::swap(a1, a2);
        }
        vec v1 = a1.v;
        vec v2 = a2.v;
        vec sum = v1 + v2;
        d_8 energy = v1 * v1 + v2 * v2;
        v1.x = v1.x * cos(rand());
        v1.y = v1.y * cos(rand());
        v1.z = v1.z * cos(rand());
        auto D = (v1 * sum) * (v1 * sum) - 2 * (v1 * v1) * (sum * sum - energy);
        if (D < 0) {
            continue;
        } else {
            auto alpha = (v1 * sum - std::sqrt(D)) / (2 * (v1 * v1));
            v1 *= alpha;
            v2 = sum - v1;
            auto _tmp = v1 * v1 + v2 * v2;
            a1.v = v1;
            a2.v = v2;
            break;
        }
    }
}

void Engine::changeCoords() {
    for (auto &atm : *atoms) {
        atm.setCoor(point(atm.point.x + atm.v.x * dt + atm.a.x * dt * dt / 2, atm.point.y + atm.v.y * dt + atm.a.y * dt * dt / 2, atm.point.z + atm.v.z * dt + atm.a.z * dt * dt / 2)); // x_0 + vt + at^2/2
        atm.v.z += atm.a.z * dt;
    }
}

void Engine::doIntersections() {
    for (auto &atm : *atoms) {
        for (auto &plane : *planes) {
            if (intersects(plane, getCenter((*planes)[0], (*planes)[1], (*planes)[2], (*planes)[3]), atm.point)) {
                mirror(atm, plane);
            }
        }
    }
}

void Engine::doBumps() {
    for (auto& atm1 : *atoms) {
        for (auto& atm2 : *atoms) {
            if (atm1 != atm2 && atm1.getDistance(atm2) < 0.0005) {
                ++bumps;
                atomBumping(atm1, atm2);
            }
        }
    }
}

void Engine::movePlanes() {
    for (auto& plane : *planes) {
        plane.p1.x += plane.v.x;
        plane.p2.x += plane.v.x;
        plane.p3.x += plane.v.x;
        plane.p4.x += plane.v.x;
        
        plane.p1.y += plane.v.y;
        plane.p2.y += plane.v.y;
        plane.p3.y += plane.v.y;
        plane.p4.y += plane.v.y;
        
        plane.p1.z += plane.v.z;
        plane.p2.z += plane.v.z;
        plane.p3.z += plane.v.z;
        plane.p4.z += plane.v.z;
    }
}

void Engine::calcDistribution() {
    
}

void Engine::startEngine() {
    while (true) {
        movePlanes();
        changeCoords();// Пересчитываем координаты
        for (int i = 0; i < intTimes; ++i) { //Нужное кол-во раз обрабатываем столкновения со стенками
            doIntersections();
        }
        doBumps();
        pressure = tmpPres / totalArea; //Получаем давление, деля силу на площадь
        tmpPres = 0; //Сбрасываем
        std::this_thread::sleep_for(std::chrono::milliseconds(dt_int)); //Ждем dt
        timeLapsed += dt;
        
        //calculated = false;
        //calcDistribution();
        //calculated = true;
    }
}


#endif /* engine_h */
