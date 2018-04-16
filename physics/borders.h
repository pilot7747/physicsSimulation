//  borders.h
//  physics
//
//  Created by Никита on 16.04.2018.
//  Copyright © 2018 Nikita Pavlichenko. All rights reserved.
//

#ifndef borders_h
#define borders_h
#include "atom.h"

enum class borderType {vertical, horizontal, ortogonal};

struct border {
    Point p1, p2, p3, p4;
    borderType type;
};

#endif /* borders_h */
