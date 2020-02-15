#ifndef engine_h
#define engine_h
#include "atom.h"
#include "borders.h"
#include <vector>
#include <chrono>
#include <thread>

constexpr int intTimes = 200; //Количество раз, сколько нужно отработать удары об стенки
constexpr long double totalArea = 1 * 2 + 1.2 * 4; //Площадь поверхности сосуда
constexpr long double massOfmolecule = 0.0000000000000000000000000482; //Масса молекулы(это воздух если что)

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
    y = plane.p1.y;
    return diffSignes(a.y - y, b.y - y);
}



class Engine { //Движок
private:
    std::vector<atom>* atoms; //Указатель на массив молекул
    std::vector<border>* planes; //Указатель на массив стенок
    void changeCoords(); //Пересчитать координаты
    void doIntersections(); //Обработать удары об стенки

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
        a.point.y += (plane.p1.y - a.point.y) * 2;
        a.v.y *= -1;
        tmpPres += (abs(a.v.y) * 2 / dt) * massOfmolecule; //Изменение имульса
    }

public:
    long double tmpPres = 0; //Суммарная сила на стенки сосуда за время dt
    long double pressure = 0; //Давление
    Engine() {}
    Engine(std::vector<atom>* _atoms, std::vector<border>* _planes) : atoms(_atoms), planes(_planes) {}
    void startEngine(); //Запуск

};

void Engine::changeCoords() {
    for (auto &atm : *atoms) {
        atm.setCoor(point(atm.point.x + atm.v.x * dt + atm.a.x * dt * dt / 2, atm.point.y + atm.v.y * dt + atm.a.y * dt * dt / 2, atm.point.z + atm.v.z * dt + atm.a.z * dt * dt / 2)); // x_0 + vt + at^2/2
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
        changeCoords();// Пересчитываем координаты
        for (int i = 0; i < intTimes; ++i) { //Нужное кол-во раз обрабатываем столкновения со стенками
            doIntersections();
        }
        pressure = tmpPres / totalArea; //Получаем давление, деля силу на площадь
        tmpPres = 0; //Сбрасываем
        std::this_thread::sleep_for(std::chrono::milliseconds(dt_int)); //Ждем dt
    }
}


#endif /* engine_h */
