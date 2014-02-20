//
//  Selection.h
//  Androidpp
//
//  Created by Saul Howard on 1/16/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__Selection__
#define __Androidpp__Selection__

#include "AndroidMacros.h"

#include "Android/text/NoCopySpan.h"
#include "Android/utils/Object.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

class CharSequence;
class Spannable;
class Layout;
class PositionIterator;

/**
 * Utility class for manipulating cursors and selections in CharSequences.
 * A cursor is a selection where the start and end are at the same offset.
 */
class Selection {
    
private:
    
    Selection() { /* cannot be instantiated */ }
    
public:
    
    /*
     * Retrieving the selection
     */
    
    /**
     * Return the offset of the selection anchor or cursor, or -1 if
     * there is no selection or cursor.
     */
    static int getSelectionStart(shared_ptr<CharSequence> text);
    
    /**
     * Return the offset of the selection edge or cursor, or -1 if
     * there is no selection or cursor.
     */
    static int getSelectionEnd(shared_ptr<CharSequence> text);
    
    /*
     * Setting the selection
     */
    
    // static int pin(int value, int min, int max) {
    //     return value < min ? 0 : (value > max ? max : value);
    // }
    
    /**
     * Set the selection anchor to <code>start</code> and the selection edge
     * to <code>stop</code>.
     */
    static void setSelection(shared_ptr<Spannable> text, int start, int stop);
    
    /**
     * Move the cursor to offset <code>index</code>.
     */
    static void setSelection(shared_ptr<Spannable> text, int index);
    
    /**
     * Select the entire text.
     */
    static void selectAll(shared_ptr<Spannable> text);
    
    /**
     * Move the selection edge to offset <code>index</code>.
     */
    static void extendSelection(shared_ptr<Spannable> text, int index);
    
    /**
     * Remove the selection or cursor, if any, from the text.
     */
    static void removeSelection(shared_ptr<Spannable> text);
    
    /*
     * Moving the selection within the layout
     */
    
    /**
     * Move the cursor to the buffer offset physically above the current
     * offset, or return false if the cursor is already on the top line.
     */
    static bool moveUp(shared_ptr<Spannable> text, shared_ptr<Layout> layout);
    
    /**
     * Move the cursor to the buffer offset physically below the current
     * offset, or return false if the cursor is already on the bottom line.
     */
    static bool moveDown(shared_ptr<Spannable> text, shared_ptr<Layout> layout);
    
    /**
     * Move the cursor to the buffer offset physically to the left of
     * the current offset, or return false if the cursor is already
     * at the left edge of the line and there is not another line to move it to.
     */
    static bool moveLeft(shared_ptr<Spannable> text, shared_ptr<Layout> layout);
    
    /**
     * Move the cursor to the buffer offset physically to the right of
     * the current offset, or return false if the cursor is already at
     * at the right edge of the line and there is not another line
     * to move it to.
     */
    static bool moveRight(shared_ptr<Spannable> text, shared_ptr<Layout> layout);
    
    /**
     * Move the selection end to the buffer offset physically above
     * the current selection end.
     */
    static bool extendUp(shared_ptr<Spannable> text, shared_ptr<Layout> layout);
    
    /**
     * Move the selection end to the buffer offset physically below
     * the current selection end.
     */
    static bool extendDown(shared_ptr<Spannable> text, shared_ptr<Layout> layout);
    
    /**
     * Move the selection end to the buffer offset physically to the left of
     * the current selection end.
     */
    static bool extendLeft(shared_ptr<Spannable> text, shared_ptr<Layout> layout);
    
    /**
     * Move the selection end to the buffer offset physically to the right of
     * the current selection end.
     */
    static bool extendRight(shared_ptr<Spannable> text, shared_ptr<Layout> layout);
    
    static bool extendToLeftEdge(shared_ptr<Spannable> text, shared_ptr<Layout> layout);
    
    static bool extendToRightEdge(shared_ptr<Spannable> text, shared_ptr<Layout> layout);
    
    static bool moveToLeftEdge(shared_ptr<Spannable> text, shared_ptr<Layout> layout);
    
    static bool moveToRightEdge(shared_ptr<Spannable> text, shared_ptr<Layout> layout);
    
    /** {@hide} */
    class PositionIterator {
    public:
        static const int DONE = -1;
        
        virtual int preceding(int position) = 0;
        virtual int following(int position) = 0;
    };
    
    /** {@hide} */
    static bool moveToPreceding(shared_ptr<Spannable> text, shared_ptr<PositionIterator> iter, bool extendSelection);
    
    /** {@hide} */
    static bool moveToFollowing(shared_ptr<Spannable> text, shared_ptr<PositionIterator> iter, bool extendSelection);
    
private:
    
    static int findEdge(shared_ptr<Spannable> text, shared_ptr<Layout> layout, int dir);
    
    static int chooseHorizontal(shared_ptr<Layout> layout, int direction,
                                        int off1, int off2);
    
    class START : public NoCopySpan, public Object { };
    class END : public NoCopySpan, public Object { };
    
public:
    
    /*
     * Public constants
     */
    
    static const Object SELECTION_START;
    static const Object SELECTION_END;
};

ANDROID_END

#endif /* defined(__Androidpp__Selection__) */
