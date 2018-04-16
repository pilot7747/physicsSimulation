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

constexpr long double dt = 0.0001;
constexpr int dt_int = dt * 1000;

using d_8 = long double;

struct vec {
    d_8 x, y, z;
};

struct Point {
    d_8 x, y, z;
    vec operator-(const Point& b) const {
        vec tmp;
        tmp.x = x - b.x;
        tmp.y = y - b.y;
        tmp.z = z - b.z;
        return tmp;
    }
};

Point point(d_8 x, d_8 y, d_8 z) {
    Point p;
    p.x = x;
    p.y = y;
    p.z = z;
    return p;
}

class atom {
private:
    
public:
    Point point, prevPoint;
    vec v, a;
    d_8 x() const;
    d_8 y() const;
    d_8 z() const;
    d_8 v_x() const;
    d_8 v_y() const;
    d_8 v_z() const;
    d_8 a_x() const;
    d_8 a_y() const;
    d_8 a_z() const;
    void setCoor(const Point &p);
};

struct Segment {
    d_8 x1, y1, z1;
    d_8 x2, y2, z2;
    
    Segment(const atom &a) {
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
