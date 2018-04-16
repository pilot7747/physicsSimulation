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

enum class borderType {vertical, horizontal, ortogonal}; //Тип стенки (вертикальная, горизонтальная, перпендикулярная)

struct border {
    Point p1, p2, p3, p4; // Четыре вершины прямоугольника
    borderType type; // Тип
};

#endif /* borders_h */
