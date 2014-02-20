/*
 * File:   TestActivity.h
 * Author: saulhoward
 *
 * Created on October 21, 2013, 10:04 AM
 */

#ifndef TESTACTIVITY_H
#define	TESTACTIVITY_H

#include "Android.h"

using namespace androidcpp;

class TestActivity : public Activity {
public:
    TestActivity(Context *context);
    virtual ~TestActivity() {}

    virtual void onCreate();

    static TestActivity *create(Context *context);
private:
    
    View *view;
};

#endif	/* TESTACTIVITY_H */

