#ifndef borders_h
#define borders_h
#include "atom.h"
#include <vector>

enum class borderType {vertical, horizontal, ortogonal}; //Тип стенки (вертикальная, горизонтальная, перпендикулярная)

struct border {
    Point p1, p2, p3, p4; // Четыре вершины прямоугольника
    borderType type; // Тип
    vec velocity;
};

#endif /* borders_h */
