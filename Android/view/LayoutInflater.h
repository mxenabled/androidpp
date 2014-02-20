/*
 * File:   LayoutInflater.h
 * Author: saulhoward
 *
 * Created on October 7, 2013, 5:08 PM
 */

#ifndef LAYOUTINFLATER_H
#define	LAYOUTINFLATER_H

#include "AndroidMacros.h"

#include "Android/view/View.h"
#include "Android/view/ViewGroup.h"
#include "Android/utils/AttributeSet.h"
#include "Android/utils/CCPullParser.h"
#include "Android/content/Context.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

class LayoutInflater {

    shared_ptr<View> createViewFromTag(shared_ptr<View> parent, const char *name, AttributeSet *attrs);

public:
    LayoutInflater() {}
    LayoutInflater(Context *context);
    ~LayoutInflater(void);

    shared_ptr<View> inflate(const char *resource, shared_ptr<ViewGroup> root);
    shared_ptr<View> inflate(const char *resource, shared_ptr<ViewGroup> root, bool attachToRoot);
    shared_ptr<View> inflate(CCPullParser &parser, shared_ptr<ViewGroup> root, bool attachToRoot);

    shared_ptr<View> createView(const char *name, AttributeSet *attrs);

    static LayoutInflater* from(Context *context);

protected:

    shared_ptr<View> onCreateView(const char *name, AttributeSet *attrs);
    shared_ptr<View> onCreateView(View *parent, const char *name, AttributeSet *attrs);

private:

    Context *mContext;

    void rInflate(CCPullParser &parser, shared_ptr<View> parent, AttributeSet *attrs, bool finishLayout);
};

ANDROID_END

#endif	/* LAYOUTINFLATER_H */

