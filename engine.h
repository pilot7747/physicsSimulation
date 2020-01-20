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
#include <atomic>
#include <vector>
#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <random>
#include <iomanip>

constexpr int intTimes = 500; //Количество раз, сколько нужно отработать удары об стенки

unsigned concurentThreadsSupported = std::thread::hardware_concurrency();

long double NanClip(long double number) {
    return std::isnan(number) ? 0 : number;
}

using namespace std::literals;

bool diffSignes(const d_8& a, const d_8& b) { // Функция, которая проверяет разные ли знаки у двух чисел
    return (a <= 0 && b >= 0) || (a >= 0 && b <= 0);
}


class Engine { //Движок
private:
    std::vector<atom>* atoms; //Указатель на массив молекул
    std::vector<border>* planes; //Указатель на массив стенок
    void changeCoords(); //Пересчитать координаты
    void doIntersections();
    void doIntersectionsOneThread(size_t left, size_t right); //Обработать удары об стенки
    void doBumpOneThread(size_t left, size_t right);
    void doBumps(std::uniform_real_distribution<long double>& dist1, std::uniform_real_distribution<long double>& dist2, std::default_random_engine& generator); //Обработать столкновения
    void movePlanes(); //Передвинуть стенки сосуда
    void atomBumping(atom& a1, atom& a2, std::uniform_real_distribution<long double>& dist1, std::default_random_engine& generator); //Столкновение молекул
    constexpr static unsigned long long max_speed = 1;

    static int ProcessBumpImpl(long double& x, long double left, long double right) {
        int bumps = 0;
        long double size = right - left;
        if (x >= right) {
            x -= right;
            bumps = 1;
            auto bump_cnt = static_cast<long long>(std::abs(x) / size);
            long double res = x - bump_cnt * size;
            if (bump_cnt % 2 == 0) {
                x = right - res;
            } else {
                x = left + res;
            }
            bumps += bump_cnt;
        } else if (x <= left) {
            x -= left;
            bumps = 1;
            auto bump_cnt = static_cast<long long>(std::abs(x) / size);
            long double res = std::abs(x) - bump_cnt * size;
            if (bump_cnt % 2 == 0) {
                x = left + res;
            } else {
                x = right - res;
            }
            bumps += bump_cnt;
        }
        return bumps;
    }

    void ProcessBump(atom& a) {
        const border& left = (*planes)[0];
        const border& right = (*planes)[1];
        const border& top = (*planes)[2];
        const border& bottom = (*planes)[3];
        const border& background = (*planes)[4];
        const border& front = (*planes)[5];

        auto bmps = ProcessBumpImpl(a.point.x, left.p1.x, right.p1.x);
        if (bmps % 2 == 1) {
            a.v.x *= -1;
        }
        tmpPres.store(NanClip(tmpPres.load() / totalArea + (std::abs(a.v.x) * 2 / dt) / totalArea * massOfmolecule * bmps));

        bmps = ProcessBumpImpl(a.point.z, bottom.p1.z, top.p1.z);
        if (bmps % 2 == 1) {
            a.v.z *= -1;
        }
        tmpPres.store(NanClip(tmpPres.load() / totalArea + (std::abs(a.v.z) * 2 / dt) / totalArea * massOfmolecule * bmps));

        bmps = ProcessBumpImpl(a.point.y, background.p1.y, front.p1.y);
        if (bmps % 2 == 1) {
            a.v.y *= -1;
        }
        tmpPres.store(NanClip(tmpPres.load() / totalArea + (std::abs(a.v.y) * 2 / dt) / totalArea * massOfmolecule * bmps));
    }



    void PrintAtoms() {
        for (auto&& atom : *atoms) {
            std::cout << atom;
        }

    }
public:
    std::vector<unsigned long long> distribution;
    std::atomic<long double> tmpPres{0}; //Суммарная сила на стенки сосуда за время dt
    long double pressure = 0; //Давление
    long double timeLapsed = 0; //Просимулированное время
    unsigned long long bumps = 0; //Колво соударений
    Engine() {}
    Engine(std::vector<atom>* _atoms, std::vector<border>* _planes) : atoms(_atoms), planes(_planes) {}
    void startEngine(); //Запуск
    bool calculated = true;
};

inline vec Rotate(const vec& dir, const vec& x, const long double& a)
{
    vec y = x.cross(dir);
    y = y.norm();
    y *= std::sin(a);
    vec nx = x;
    nx *= std::cos(a);
    return nx + y;
}

void Engine::atomBumping(atom& a1, atom& a2, std::uniform_real_distribution<long double>& dist1, std::default_random_engine& generator) {
    if (a1.v.equal(a2.v)) { //Если скорости молекул почти сонаправлены, то считаем, что они все-таки не сталкиваются
        --bumps;
        return;
    }
    int i = 0;
    while (true) {
        if (i > 10000000) {
            --bumps;
            return;
        }
        vec v1 = a1.v; //Сохраняем сюда вектор скорости первой молекулы
        vec v2 = a2.v; //Второй молекулы
        vec sum = v1 + v2; //Суммарный импульс (масса молекул одинакова, поэтому здесь и дальше мы ее не учитываем
        d_8 energy = v1 * v1 + v2 * v2; //Удвоенная кинетическая энергия
        auto a = dist1(generator);

        vec z = v1.cross(v2);
        z = z.norm();
        vec dobavka = v2;
        v1 -= v2;
        v2 -= v2;
        dobavka += v1 / 2;
        vec t = v1 / 2;
        v1 -= t;
        v2 -= t;
        vec x = v1;
        x = x.norm();
        vec y = x.cross(z);
        y = y.norm();
        x *= std::cos(a);
        y *= std::sin(a);
        vec v1n = x + y;
        v1n *= std::sqrt(v1 * v1);
        v1n += dobavka;
        v1 = v1n;
        v2 = sum - v1;
        a1.v = v1;
        a2.v = v2;
        ++i;
        break;
    }
}

void Engine::changeCoords() {
    for (auto &atm : *atoms) {
        atm.setCoor(point(atm.point.x + atm.v.x * dt + atm.a.x * dt * dt / 2, atm.point.y + atm.v.y * dt + atm.a.y * dt * dt / 2, atm.point.z + atm.v.z * dt + atm.a.z * dt * dt / 2)); // x_0 + vt + at^2/2
        atm.v.z += atm.a.z * dt;
    }
}

void Engine::doIntersectionsOneThread(size_t left, size_t right) { // Обработать пересечение перемещения молекулы со стенкой сосуда
    for (size_t i = left; i < right; ++i) {
        ProcessBump((*atoms)[i]);
    }
}

void Engine::doIntersections() { // Обработать пересечение перемещения молекулы со стенкой сосуда
    size_t blocks_size = atoms->size() / concurentThreadsSupported;
    size_t last_block = atoms->size() - (concurentThreadsSupported - 1) * (blocks_size);
    std::vector<std::thread> threads;
    for (size_t i = 0; i < concurentThreadsSupported - 1; ++i) {

        threads.emplace_back(std::thread(&Engine::doIntersectionsOneThread, this, i * blocks_size, (i + 1) * blocks_size));
    }
    threads.emplace_back(std::thread(&Engine::doIntersectionsOneThread, this, (concurentThreadsSupported - 1) * blocks_size, (concurentThreadsSupported - 1) * blocks_size + last_block));

    for (auto&& t : threads) {
        t.join();
    }
}

void Engine::doBumpOneThread(size_t left, size_t right) {
    std::uniform_real_distribution<long double> dist1(0, 2 * M_PI);
    std::default_random_engine generator;
    for (int i = left; i < right; ++i) {
        for (int j = i + 1; j < atoms->size(); ++j) {
            if (i != j && (*atoms)[i].getDistance((*atoms)[j]) < 0.001) { //Если расстояние меньше 1мм, то сталкиваем их
                ++bumps; //Увеличиваем счетчик столкновений
                atomBumping((*atoms)[i], (*atoms)[j], dist1, generator); //Запускаем функцию выше
            }
        }
    }
}

//Перебираем все пары молекул
void Engine::doBumps(std::uniform_real_distribution<long double>& dist1, std::uniform_real_distribution<long double>& dist2, std::default_random_engine& generator) {
    size_t blocks_size = atoms->size() / concurentThreadsSupported;
    size_t last_block = atoms->size() - (concurentThreadsSupported - 1) * (blocks_size);
    std::vector<std::thread> threads;
    for (size_t i = 0; i < concurentThreadsSupported - 1; ++i) {
        threads.emplace_back(std::thread(&Engine::doBumpOneThread, this, i * blocks_size, (i + 1) * blocks_size));
    }
    threads.emplace_back(std::thread(&Engine::doBumpOneThread, this, (concurentThreadsSupported - 1) * blocks_size, (concurentThreadsSupported - 1) * blocks_size + last_block));

    for (auto&& t : threads) {
        t.join();
    }
}

//Функция, которая двигает стенки сосуда, в данном примере не используется
void Engine::movePlanes() {
    d_8 speed = 0;
    for (auto& plane : *planes) {
        if (plane.v.x != 0) {
            speed = plane.v.x;
        }
        plane.p1.x += plane.v.x * dt;
        plane.p2.x += plane.v.x * dt;
        plane.p3.x += plane.v.x * dt;
        plane.p4.x += plane.v.x * dt;
        
        plane.p1.y += plane.v.y * dt;
        plane.p2.y += plane.v.y * dt;
        plane.p3.y += plane.v.y * dt;
        plane.p4.y += plane.v.y * dt;
        
        plane.p1.z += plane.v.z * dt;
        plane.p2.z += plane.v.z * dt;
        plane.p3.z += plane.v.z * dt;
        plane.p4.z += plane.v.z * dt;
    }
    totalArea -= std::abs(speed) * dt * 4;
    totV -= std::abs(speed) * dt;
}

void Engine::startEngine() { //Эта функция запускается в отдельном потоке и постоянно обрабатывает следующее состояние системы через время dt
    std::cout << atoms->size() << std::endl;
    std::cout << massOfmolecule << std::endl;
    std::cout << k << std::endl;

    std::default_random_engine generator;
    std::uniform_real_distribution<long double> dist1(0, 2 * M_PI);
    std::uniform_real_distribution<long double> dist2(-1, 1);
    while (true) {
        //PrintAtoms();
        movePlanes();
        changeCoords();// Пересчитываем координаты
        

        doIntersections();
        doBumps(dist1, dist2, generator); //Обрабатываем столкновения молекул

        pressure = tmpPres.load(); // totalArea; //Получаем давление, деля силу на площадь

        std::cout << bumps << std::endl;
        std::cout << pressure << std::endl;

        tmpPres.store(0); //Сбрасываем давление
        timeLapsed += dt;
    }
}


#endif /* engine_h */
