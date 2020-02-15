#ifndef borders_h
#define borders_h
#include "atom.h"
#include <vector>

enum class borderType {vertical, horizontal, ortogonal}; //Тип стенки (вертикальная, горизонтальная, перпендикулярная)

struct border {
    Point p1, p2, p3, p4; // Четыре вершины прямоугольника
    borderType type; // Тип
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
#endif /* borders_h */
