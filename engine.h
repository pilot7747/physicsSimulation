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

class Engine { //Движок
private:
    size_t iterations = 0;
    int move_step = 20;
    std::vector<atom>* atoms; //Указатель на массив молекул
    std::vector<border>* planes; //Указатель на массив стенок
    void changeCoords(); //Пересчитать координаты
    void doIntersections();
    void doBumps(); //Обработать столкновения
    void movePlanes(); //Передвинуть стенки сосуда (для адиабаты например )
    void atomBumping(atom& a1, atom& a2); //Столкновение молекул
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

    template <typename T> int sgn(T val) {
        return (T(0) < val) - (val < T(0));
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
	tmpPres += std::abs(a.v.x) * bmps;
	if (iterations % move_step == 0) {
        long double speedX = std::abs(left.velocity.x);
	    a.v.x += sgn(a.v.x) * bmps * speedX * 2;
	    tmpPres += speedX * 2 * bmps;
	}

        bmps = ProcessBumpImpl(a.point.z, bottom.p1.z, top.p1.z);
        if (bmps % 2 == 1) {
            a.v.z *= -1;
        }

        tmpPres += std::abs(a.v.z) * bmps;
	if (iterations % move_step == 0) {
        long double speedZ = std::abs(top.velocity.z);
	    a.v.z += sgn(a.v.z) * bmps * speedZ * 2;
	    tmpPres += speedZ * 2 * bmps;
	}

        bmps = ProcessBumpImpl(a.point.y, background.p1.y, front.p1.y);
        if (bmps % 2 == 1) {
            a.v.y *= -1;
        }
        tmpPres += std::abs(a.v.y) * bmps;
	if (iterations % move_step == 0) {
        long double speedY = std::abs(front.velocity.y);
	    a.v.y += sgn(a.v.y) * bmps * speedY * 2;
	    tmpPres += speedY * 2 * bmps;
	}
    }

    void PrintAtoms() {
        for (auto&& atom : *atoms) {
            std::cout << atom;
        }

    }
public:
    std::vector<unsigned long long> distribution;
    long double tmpPres = 0; //Суммарная сила на стенки сосуда за время dt
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

void Engine::atomBumping(atom& a1, atom& a2) {
    std::uniform_real_distribution<long double> dist1(0, 2 * M_PI);
    std::default_random_engine generator;
    if (a1.v.equal(a2.v)) { //Если скорости молекул почти сонаправлены, то считаем, что они все-таки не сталкиваются
        --bumps;
        return;
    }
        vec v1 = a1.v; //Сохраняем сюда вектор скорости первой молекулы
        vec v2 = a2.v; //Второй молекулы
        vec sum = v1 + v2; //Суммарный импульс (масса молекул одинакова, поэтому здесь и дальше мы ее не учитываем
        d_8 energy = v1 * v1 + v2 * v2; //Удвоенная кинетическая энергия
        auto a = dist1(generator) * M_PI;
        if (rand() % 2 == 0) {
            a = 2 * M_PI - a;
        }
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
}

void Engine::changeCoords() {
    for (auto &atm : *atoms) {
        atm.setCoor(point(atm.point.x + atm.v.x * dt + atm.a.x * dt * dt / 2, atm.point.y + atm.v.y * dt + atm.a.y * dt * dt / 2, atm.point.z + atm.v.z * dt + atm.a.z * dt * dt / 2)); // x_0 + vt + at^2/2
        atm.v.z += atm.a.z * dt;
    }
}

void Engine::doIntersections() { // Обработать пересечение перемещения молекулы со стенкой сосуда
    std::vector<atom>& atoms_cp = *atoms;
    // #pragma omp target teams distribute parallel for map(from:atoms_cp)
    for (size_t i = 0; i < atoms_cp.size(); ++i) {
        ProcessBump(atoms_cp[i]);
    }
}

//Перебираем все пары молекул
void Engine::doBumps() {
    std::uniform_real_distribution<long double> dist1(0, 2 * M_PI);
    std::default_random_engine generator;
    auto& atoms_cp = *atoms;
    #pragma omp target teams distribute parallel for map(from:atoms_cp)
    for (int i = 0; i < atoms_cp.size(); ++i) {
        for (int j = i + 1; j < atoms_cp.size(); ++j) {
            if (i != j && atoms_cp[i].getDistance(atoms_cp[j]) < 0.003) { //Если расстояние меньше 1мм, то сталкиваем их
                ++bumps; //Увеличиваем счетчик столкновений
                atomBumping(atoms_cp[i], atoms_cp[j]); //Запускаем функцию выше
            }
        }
    }
}

//Функция, которая двигает стенки сосуда, в данном примере не используется
void Engine::movePlanes() {
    d_8 speed = 0;
    for (auto& plane : *planes) {
        if (plane.velocity.x != 0) {
            speed = plane.velocity.x;
        }
        plane.p1.x += plane.velocity.x * dt;
        plane.p2.x += plane.velocity.x * dt;
        plane.p3.x += plane.velocity.x * dt;
        plane.p4.x += plane.velocity.x * dt;

        plane.p1.y += plane.velocity.y * dt;
        plane.p2.y += plane.velocity.y * dt;
        plane.p3.y += plane.velocity.y * dt;
        plane.p4.y += plane.velocity.y * dt;

        plane.p1.z += plane.velocity.z * dt;
        plane.p2.z += plane.velocity.z * dt;
        plane.p3.z += plane.velocity.z * dt;
        plane.p4.z += plane.velocity.z * dt;
    }
    totV = (1 - 2 * std::abs(speed) * dt) * (1 - 2 * std::abs(speed) * dt) * (1 - 2 * std::abs(speed) * dt);
    totalArea = (1 - 2 * std::abs(speed) * dt) * (1 - 2 * std::abs(speed) * dt) * 6; // сжимаем с двух сторон
}

void Engine::startEngine() { //Эта функция запускается в отдельном потоке и постоянно обрабатывает следующее состояние системы через время dt
    std::ios_base::sync_with_stdio(false);
    std::cout << atoms->size() << std::endl;
    std::cout << massOfmolecule << std::endl;
    std::cout << k << std::endl;

    while (true) {
	++iterations;
        PrintAtoms();
	if (iterations % move_step == 0) {
            movePlanes();
	}
        changeCoords();// Пересчитываем координаты

        doIntersections();
        doBumps(); //Обрабатываем столкновения молекул

        pressure = tmpPres / totalArea * 2 * massOfmolecule / dt; //Получаем давление, деля силу на площадь

        std::cout << bumps << std::endl;
        std::cout << pressure << std::endl;

        tmpPres = 0; //Сбрасываем давление
        timeLapsed += dt;
    }
}


#endif /* engine_h */
