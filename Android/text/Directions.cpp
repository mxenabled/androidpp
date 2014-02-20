//
//  Directions.c
//  Androidpp
//
//  Created by Saul Howard on 1/31/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "Directions.h"

ANDROID_BEGIN

Directions Directions::DIRS_ALL_LEFT_TO_RIGHT = Directions({ 0, Directions::RUN_LENGTH_MASK });
Directions Directions::DIRS_ALL_RIGHT_TO_LEFT = Directions({ 0, Directions::RUN_LENGTH_MASK | Directions::RUN_RTL_FLAG });

ANDROID_END