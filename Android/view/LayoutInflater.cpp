/*
 * File:   LayoutInflater.cpp
 * Author: saulhoward
 *
 * Created on October 7, 2013, 5:08 PM
 */

#include "LayoutInflater.h"

ANDROID_BEGIN

static LayoutInflater *s_SharedInflater = NULL;

LayoutInflater* LayoutInflater::from(Context *context)
{
    if (!s_SharedInflater) {
        s_SharedInflater = new LayoutInflater(context);
    }

    return s_SharedInflater;
}

LayoutInflater::LayoutInflater(Context *context) {
    mContext = context;
}

LayoutInflater::~LayoutInflater(void) {
}

shared_ptr<View> LayoutInflater::inflate(const char *resource, shared_ptr<ViewGroup> root) {
    return inflate(resource, root, root != NULL);
}

shared_ptr<View> LayoutInflater::inflate(const char *resource, shared_ptr<ViewGroup> root, bool attachToRoot) {

    string res(resource);
    size_t found = res.find("@layout/");
    if (found != string::npos) {
        res = "res/" + res.substr(8, res.length()) + ".xml";
    }
    
    CCPullParser parser = CCPullParser();

    if (parser.init(res.c_str())) {
        return inflate(parser, root, attachToRoot);
    } else {
        return NULL;
    }
}

shared_ptr<View> LayoutInflater::inflate(CCPullParser &parser, shared_ptr<ViewGroup> root, bool attachToRoot) {

    AttributeSet *attrs = (AttributeSet*) &parser;
    shared_ptr<View> result = root;

    const char* name = parser.getName();

    shared_ptr<View> temp = createViewFromTag(root, name, attrs);

    if (temp == NULL) return result;

    LayoutParams *params = NULL;

    if (root != NULL) {
        params = root->generateLayoutParams(attrs);
        if (!attachToRoot) {
            temp->setLayoutParams(params);
        }
    }

    rInflate(parser, temp, attrs, true);

    if (root != NULL && attachToRoot) {
        root->addView(temp, params);
    }

    if (root == NULL || !attachToRoot) {
        result = temp;
    }

    return result;
}

shared_ptr<View> LayoutInflater::createViewFromTag(shared_ptr<View> parent, const char *name, AttributeSet *attrs) {

    shared_ptr<View> view;

    if (name) {
        view = createView(name, attrs);
    }

    return view;
}

shared_ptr<View> LayoutInflater::onCreateView(View* parent, const char* name, AttributeSet* attrs) {
    return onCreateView(name, attrs);
}

shared_ptr<View> LayoutInflater::onCreateView(const char* name, AttributeSet* attrs) {
    return createView(name, attrs);
}

shared_ptr<View> LayoutInflater::createView(const char* name, AttributeSet* attrs) {
    return View::createInstance(name, mContext, attrs);
}


void LayoutInflater::rInflate(CCPullParser &parser, shared_ptr<View> parent, AttributeSet *attrs, bool finishInflate) {

    if (!parser.hasChildren()) {
        return;
    }

    parser.firstChild();

    do {
        const char* name = parser.getName();

        shared_ptr<View> view = createViewFromTag(parent, name, attrs);

        if (view != NULL) {

            shared_ptr<ViewGroup> viewGroup = static_pointer_cast<ViewGroup>(parent);
            LayoutParams *params = viewGroup->generateLayoutParams(attrs);

            rInflate(parser, view, attrs, true);

            viewGroup->addView(view, params);
        }

    } while (parser.next());

    parser.toParent();
    
    if (finishInflate) parent->onFinishInflate();
}

ANDROID_END