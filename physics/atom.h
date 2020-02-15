#ifndef atom_h
#define atom_h
#pragma once
#include <GLUT/GLUT.h>
#include <OpenGL/OpenGL.h>

constexpr long double dt = 0.01; //Время dt в секундак
constexpr int dt_int = dt * 1000; //Время dt в милисекундах

using d_8 = long double; //Заменяем long double на d_8

struct vec { //Вектор
    d_8 x = 0, y = 0, z = 0;
};

struct Point { //Точка
    d_8 x, y, z;
    vec operator-(const Point& b) const {
        vec tmp;
        tmp.x = x - b.x;
        tmp.y = y - b.y;
        tmp.z = z - b.z;
        return tmp;
    }
};

Point point(d_8 x, d_8 y, d_8 z) { //Создать точку по трем координатам (переделать на конструктор)
    Point p;
    p.x = x;
    p.y = y;
    p.z = z;
    return p;
}

class atom { //Молекула
private:

public:
    Point point, prevPoint; //Текущая и предыдущая точки
    vec v, a; //Вектор скорости и ускорения

    //Функции для получения проекций
    d_8 x() const;
    d_8 y() const;
    d_8 z() const;
    d_8 v_x() const;
    d_8 v_y() const;
    d_8 v_z() const;
    d_8 a_x() const;
    d_8 a_y() const;
    d_8 a_z() const;

    void setCoor(const Point &p); //Установить новые координаты
};

struct Segment { //Отрезок
    d_8 x1, y1, z1;
    d_8 x2, y2, z2;

    Segment(const atom &a) { //Конструктор от текцщего и предыдущего положения молекулы
        x1 = a.prevPoint.x;
        y1 = a.prevPoint.y;
        z1 = a.prevPoint.z;
        x2 = a.point.x;
        y2 = a.point.y;
        z2 = a.point.z;
    }
};

d_8 atom::x() const {
    return point.x;
}

d_8 atom::y() const {
    return point.y;
}

d_8 atom::z() const {
    return point.z;
}

d_8 atom::v_x() const {
    return (point - prevPoint).x / dt;
}

d_8 atom::v_y() const {
    return (point - prevPoint).y / dt;
}

d_8 atom::v_z() const {
    return (point - prevPoint).z / dt;
}

void atom::setCoor(const Point &p) {
    point.x = p.x;
    point.y = p.y;
    point.z = p.z;
}

#endif /* atom_h */
