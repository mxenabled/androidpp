//
//  Selection.cpp
//  Androidpp
//
//  Created by Saul Howard on 1/16/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "Selection.h"

#include "Android/text/CharSequence.h"
#include "Android/text/Layout.h"
#include "Android/text/Spannable.h"
#include "Android/text/Spanned.h"

#include <memory>
#include <algorithm>

using namespace std;

ANDROID_BEGIN

const Object Selection::SELECTION_START = START();
const Object Selection::SELECTION_END = END();

int Selection::getSelectionStart(shared_ptr<CharSequence> text) {
    
    shared_ptr<Spanned> spanned = dynamic_pointer_cast<Spanned>(text);
    if (spanned != NULL) {
        return spanned->getSpanStart(make_shared<Object>(SELECTION_START));
    } else {
        return -1;
    }
}

int Selection::getSelectionEnd(shared_ptr<CharSequence> text) {
    
    shared_ptr<Spanned> spanned = dynamic_pointer_cast<Spanned>(text);
    if (spanned != NULL) {
        return spanned->getSpanStart(make_shared<Object>(SELECTION_END));
    } else {
        return -1;
    }
}


// private int pin(int value, int min, int max) {
//     return value < min ? 0 : (value > max ? max : value);
// }

void Selection::setSelection(shared_ptr<Spannable> text, int start, int stop) {
    // int len = text->length();
    // start = pin(start, 0, len);  XXX remove unless we really need it
    // stop = pin(stop, 0, len);
    
    int ostart = getSelectionStart(text);
    int oend = getSelectionEnd(text);
    
    if (ostart != start || oend != stop) {
        text->setSpan(make_shared<Object>(SELECTION_START), start, start,
                     Spanned::SPAN_POINT_POINT|Spanned::SPAN_INTERMEDIATE);
        text->setSpan(make_shared<Object>(SELECTION_END), stop, stop,
                     Spanned::SPAN_POINT_POINT);
    }
}

void Selection::setSelection(shared_ptr<Spannable> text, int index) {
    setSelection(text, index, index);
}

void Selection::selectAll(shared_ptr<Spannable> text) {
    setSelection(text, 0, text->length());
}

void Selection::extendSelection(shared_ptr<Spannable> text, int index) {
    if (text->getSpanStart(make_shared<Object>(SELECTION_END)) != index)
        text->setSpan(make_shared<Object>(SELECTION_END), index, index, Spanned::SPAN_POINT_POINT);
}

void Selection::removeSelection(shared_ptr<Spannable> text) {
    text->removeSpan(make_shared<Object>(SELECTION_START));
    text->removeSpan(make_shared<Object>(SELECTION_END));
}


bool Selection::moveUp(shared_ptr<Spannable> text, shared_ptr<Layout> layout) {
    int start = getSelectionStart(text);
    int end = getSelectionEnd(text);
    
    if (start != end) {
        int mins = min(start, end);
        int maxs = max(start, end);
        
        setSelection(text, mins);
        
        if (mins == 0 && maxs == text->length()) {
            return false;
        }
        
        return true;
    } else {
        int line = layout->getLineForOffset(end);
        
        if (line > 0) {
            int move;
            
            if (layout->getParagraphDirection(line) ==
                layout->getParagraphDirection(line - 1)) {
                float h = layout->getPrimaryHorizontal(end);
                move = layout->getOffsetForHorizontal(line - 1, h);
            } else {
                move = layout->getLineStart(line - 1);
            }
            
            setSelection(text, move);
            return true;
        }
    }
    
    return false;
}

bool Selection::moveDown(shared_ptr<Spannable> text, shared_ptr<Layout> layout) {
    int start = getSelectionStart(text);
    int end = getSelectionEnd(text);
    
    if (start != end) {
        int mins = min(start, end);
        int maxs = max(start, end);
        
        setSelection(text, maxs);
        
        if (mins == 0 && maxs == text->length()) {
            return false;
        }
        
        return true;
    } else {
        int line = layout->getLineForOffset(end);
        
        if (line < layout->getLineCount() - 1) {
            int move;
            
            if (layout->getParagraphDirection(line) ==
                layout->getParagraphDirection(line + 1)) {
                float h = layout->getPrimaryHorizontal(end);
                move = layout->getOffsetForHorizontal(line + 1, h);
            } else {
                move = layout->getLineStart(line + 1);
            }
            
            setSelection(text, move);
            return true;
        }
    }
    
    return false;
}

bool Selection::moveLeft(shared_ptr<Spannable> text, shared_ptr<Layout> layout) {
    int start = getSelectionStart(text);
    int end = getSelectionEnd(text);
    
    if (start != end) {
        setSelection(text, chooseHorizontal(layout, -1, start, end));
        return true;
    } else {
        int to = layout->getOffsetToLeftOf(end);
        
        if (to != end) {
            setSelection(text, to);
            return true;
        }
    }
    
    return false;
}

bool Selection::moveRight(shared_ptr<Spannable> text, shared_ptr<Layout> layout) {
    int start = getSelectionStart(text);
    int end = getSelectionEnd(text);
    
    if (start != end) {
        setSelection(text, chooseHorizontal(layout, 1, start, end));
        return true;
    } else {
        int to = layout->getOffsetToRightOf(end);
        
        if (to != end) {
            setSelection(text, to);
            return true;
        }
    }
    
    return false;
}

bool Selection::extendUp(shared_ptr<Spannable> text, shared_ptr<Layout> layout) {
    int end = getSelectionEnd(text);
    int line = layout->getLineForOffset(end);
    
    if (line > 0) {
        int move;
        
        if (layout->getParagraphDirection(line) ==
            layout->getParagraphDirection(line - 1)) {
            float h = layout->getPrimaryHorizontal(end);
            move = layout->getOffsetForHorizontal(line - 1, h);
        } else {
            move = layout->getLineStart(line - 1);
        }
        
        extendSelection(text, move);
        return true;
    } else if (end != 0) {
        extendSelection(text, 0);
        return true;
    }
    
    return true;
}

bool Selection::extendDown(shared_ptr<Spannable> text, shared_ptr<Layout> layout) {
    int end = getSelectionEnd(text);
    int line = layout->getLineForOffset(end);
    
    if (line < layout->getLineCount() - 1) {
        int move;
        
        if (layout->getParagraphDirection(line) ==
            layout->getParagraphDirection(line + 1)) {
            float h = layout->getPrimaryHorizontal(end);
            move = layout->getOffsetForHorizontal(line + 1, h);
        } else {
            move = layout->getLineStart(line + 1);
        }
        
        extendSelection(text, move);
        return true;
    } else if (end != text->length()) {
        extendSelection(text, text->length());
        return true;
    }
    
    return true;
}

bool Selection::extendLeft(shared_ptr<Spannable> text, shared_ptr<Layout> layout) {
    int end = getSelectionEnd(text);
    int to = layout->getOffsetToLeftOf(end);
    
    if (to != end) {
        extendSelection(text, to);
        return true;
    }
    
    return true;
}

bool Selection::extendRight(shared_ptr<Spannable> text, shared_ptr<Layout> layout) {
    int end = getSelectionEnd(text);
    int to = layout->getOffsetToRightOf(end);
    
    if (to != end) {
        extendSelection(text, to);
        return true;
    }
    
    return true;
}

bool Selection::extendToLeftEdge(shared_ptr<Spannable> text, shared_ptr<Layout> layout) {
    int where = findEdge(text, layout, -1);
    extendSelection(text, where);
    return true;
}

bool Selection::extendToRightEdge(shared_ptr<Spannable> text, shared_ptr<Layout> layout) {
    int where = findEdge(text, layout, 1);
    extendSelection(text, where);
    return true;
}

bool Selection::moveToLeftEdge(shared_ptr<Spannable> text, shared_ptr<Layout> layout) {
    int where = findEdge(text, layout, -1);
    setSelection(text, where);
    return true;
}

bool Selection::moveToRightEdge(shared_ptr<Spannable> text, shared_ptr<Layout> layout) {
    int where = findEdge(text, layout, 1);
    setSelection(text, where);
    return true;
}


bool Selection::moveToPreceding(shared_ptr<Spannable> text, shared_ptr<PositionIterator> iter, bool extendSelections) {
    const int offset = iter->preceding(getSelectionEnd(text));
    if (offset != PositionIterator::DONE) {
        if (extendSelections) {
            extendSelection(text, offset);
        } else {
            setSelection(text, offset);
        }
    }
    return true;
}

bool Selection::moveToFollowing(shared_ptr<Spannable> text, shared_ptr<PositionIterator> iter, bool extendSelections) {
    const int offset = iter->following(getSelectionEnd(text));
    if (offset != PositionIterator::DONE) {
        if (extendSelections) {
            extendSelection(text, offset);
        } else {
            setSelection(text, offset);
        }
    }
    return true;
}

int Selection::findEdge(shared_ptr<Spannable> text, shared_ptr<Layout> layout, int dir) {
    int pt = getSelectionEnd(text);
    int line = layout->getLineForOffset(pt);
    int pdir = layout->getParagraphDirection(line);
    
    if (dir * pdir < 0) {
        return layout->getLineStart(line);
    } else {
        int end = layout->getLineEnd(line);
        
        if (line == layout->getLineCount() - 1)
            return end;
        else
            return end - 1;
    }
}

int Selection::chooseHorizontal(shared_ptr<Layout> layout, int direction,
                                    int off1, int off2) {
    int line1 = layout->getLineForOffset(off1);
    int line2 = layout->getLineForOffset(off2);
    
    if (line1 == line2) {
        // same line, so it goes by pure physical direction
        
        float h1 = layout->getPrimaryHorizontal(off1);
        float h2 = layout->getPrimaryHorizontal(off2);
        
        if (direction < 0) {
            // to left
            
            if (h1 < h2)
                return off1;
            else
                return off2;
        } else {
            // to right
            
            if (h1 > h2)
                return off1;
            else
                return off2;
        }
    } else {
        // different line, so which line is "left" and which is "right"
        // depends upon the directionality of the text
        
        // This only checks at one end, but it's not clear what the
        // right thing to do is if the ends don't agree.  Even if it
        // is wrong it should still not be too bad.
        int line = layout->getLineForOffset(off1);
        int textdir = layout->getParagraphDirection(line);
        
        if (textdir == direction)
            return max(off1, off2);
        else
            return min(off1, off2);
    }
}

ANDROID_END