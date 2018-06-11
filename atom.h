//
//  atom.h
//  physics
//
//  Created by Никита on 15.04.2018.
//  Copyright © 2018 Nikita Pavlichenko. All rights reserved.
//

#ifndef atom_h
#define atom_h
#pragma once
#include <GLUT/GLUT.h>
#include <OpenGL/OpenGL.h>

constexpr long double dt = 0.01; //Время dt в секундак
constexpr int dt_int = dt * 1000; //Время dt в милисекундах

long double totalArea = 1 * 6; //Площадь поверхности сосуда
constexpr long double massOfmolecule = 0.0000000000000000000000000482; //Масса молекулы
constexpr long double k = 0.0000000000000000000000138065;

using d_8 = long double; //Заменяем long double на d_8

struct vec { //Вектор
    d_8 x = 0, y = 0, z = 0;
    vec() : x(0), y(0), z(0) {}
    explicit vec(d_8 _x, d_8 _y, d_8 _z) : x(_x), y(_y), z(_z) {}
    vec& operator-=(const vec& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    vec& operator+=(const vec& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    vec operator+(const vec& v) {
        vec tmp = *this;
        tmp.x += v.x;
        tmp.y += v.y;
        tmp.z += v.z;
        return tmp;
    }
    vec operator-(const vec& v) {
        vec tmp = *this;
        tmp.x -= v.x;
        tmp.y -= v.y;
        tmp.z -= v.z;
        return tmp;
    }
    d_8 operator*(const vec& v) {
        return x * v.x + y * v.y + z * v.z;
    }
    vec& operator*=(const d_8& a) {
        x *= a;
        y *= a;
        z *= a;
        return *this;
    }
    bool equal(const vec& v) {
        vec tmp = *this - v;
        if (tmp * tmp < 0.05) {
            return true;
        }
        return false;
    }
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
    d_8 get_v() const;
    bool operator==(const atom& atm) {
        return this == &atm;
    }
    bool operator!=(const atom& atm) {
        return this != &atm;
    }
    long double getDistance(const atom& atm) {
        //auto res = sqrtl((point.x - atm.x()) * (point.x - atm.x()) + (point.y - atm.y()) * (point.y - atm.y()) + (point.z - atm.z()) * (point.z - atm.z()));
        return sqrtl((point.x - atm.x()) * (point.x - atm.x()) + (point.y - atm.y()) * (point.y - atm.y()) + (point.z - atm.z()) * (point.z - atm.z()));
    }
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

d_8 atom::get_v() const {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

void atom::setCoor(const Point &p) {
    prevPoint = point;
    point.x = p.x;
    point.y = p.y;
    point.z = p.z;
}

#endif /* atom_h */
