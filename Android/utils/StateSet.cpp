//
//  StateSet.c
//  cocos2dx
//
//  Created by Saul Howard on 11/25/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include "StateSet.h"
#include "Android/internal/R.h"

ANDROID_BEGIN

const vector<int> StateSet::WILD_CARD = vector<int>();
const vector<int> StateSet::NOTHING = { 0 };

int StateSet::getStateValue(string state) {
    
    if (state.compare("state_window_focused") == 0) {
        return R::attr::state_window_focused;
    } else if (state.compare("state_selected") == 0) {
        return R::attr::state_selected;
    } else if (state.compare("state_focused") == 0) {
        return R::attr::state_focused;
    } else if (state.compare("state_enabled") == 0) {
        return R::attr::state_enabled;
    } else if (state.compare("state_pressed") == 0) {
        return R::attr::state_pressed;
    } else if (state.compare("state_activated") == 0) {
        return R::attr::state_activated;
    } else if (state.compare("state_accelerated") == 0) {
        return R::attr::state_accelerated;
    } else if (state.compare("state_hovered") == 0) {
        return R::attr::state_hovered;
    } else if (state.compare("state_drag_can_accept") == 0) {
        return R::attr::state_drag_can_accept;
    } else if (state.compare("state_drag_hovered") == 0) {
        return R::attr::state_drag_hovered;
    } else if (state.compare("state_checked") == 0) {
        return R::attr::state_checked;
    }
    
    return 0;
}

ANDROID_END