//
//  borders.h
//  physics
//
//  Created by Никита on 16.04.2018.
//  Copyright © 2018 Nikita Pavlichenko. All rights reserved.
//

#ifndef borders_h
#define borders_h
#include "atom.h"
#include <vector>

enum class borderType {vertical, horizontal, ortogonal}; //Тип стенки (вертикальная, горизонтальная, перпендикулярная)

struct border {
    Point p1, p2, p3, p4; // Четыре вершины прямоугольника
    borderType type; // Тип
    vec v;
};


bool checkInVessel(border& left, border& right, border& up, border& down, Point& p) { //Находится ли точка внутри сосуда
    if (p.x < left.p1.x)
        return false;
    if (p.x > right.p1.x)
        return false;
    if (p.z > up.p1.z)
        return false;
    if (p.z < down.p1.z)
        return false;
    return true;
}

Point getCenter(border& left, border& right, border& up, border& down) { //Центр сосуда
    auto l = left.p1.x;
    auto r = right.p1.x;
    auto u = up.p1.z;
    auto d = down.p1.z;
    return point(l + (r - l) / 2, 0, d + (u - d) / 2);
}
#endif /* borders_h */
