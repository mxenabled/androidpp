//
//  LayerDrawable.h
//  cocos2dx
//
//  Created by Saul Howard on 11/26/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef __cocos2dx__LayerDrawable__
#define __cocos2dx__LayerDrawable__

#include "AndroidMacros.h"

#include "Android/graphics/drawable/Drawable.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

class LayerDrawable : public Drawable, public Drawable::Callback {
    
protected:
    
    class ChildDrawable {
    public:
        ChildDrawable() {}
        shared_ptr<Drawable> mDrawable;
        int mInsetL = 0, mInsetT = 0, mInsetR = 0, mInsetB = 0;
        string mId;
    };
    
    class LayerState : public ConstantState {
        
    public:
        int mNum = 0;
        vector<shared_ptr<ChildDrawable>> mChildren;
        
        int mChangingConfigurations = 0;
        int mChildrenChangingConfigurations = 0;
        
        bool mHaveOpacity = false;
        int mOpacity = 0;
        
        bool mHaveStateful = false;
        bool mStateful = 0;
        
        bool mCheckedConstantState = false;
        bool mCanConstantState = false;
        
        LayerState() {}
        
        LayerState(LayerState *orig, LayerDrawable *owner, Resources *res) {
            
            if (orig) {
                const vector<shared_ptr<ChildDrawable>> &origChildDrawable = orig->mChildren;
                const int N = orig->mNum;
                
                mNum = N;
                mChildren = vector<shared_ptr<ChildDrawable>>(N);
                
                mChangingConfigurations = orig->mChangingConfigurations;
                mChildrenChangingConfigurations = orig->mChildrenChangingConfigurations;
                
                for (int i = 0; i < N; i++) {
                    shared_ptr<ChildDrawable> &r = mChildren[i] = make_shared<ChildDrawable>();
                    const shared_ptr<ChildDrawable> &org = origChildDrawable[i];
                    if (res) {
                        r->mDrawable = org->mDrawable->getConstantState()->newDrawable(res);
                    } else {
                        r->mDrawable = org->mDrawable->getConstantState()->newDrawable();
                    }
                    r->mDrawable->setCallback(owner);
                    r->mDrawable->setLayoutDirection(org->mDrawable->getLayoutDirection());
                    r->mInsetL = org->mInsetL;
                    r->mInsetT = org->mInsetT;
                    r->mInsetR = org->mInsetR;
                    r->mInsetB = org->mInsetB;
                    r->mId = org->mId;
                }
                
                mHaveOpacity = orig->mHaveOpacity;
                mOpacity = orig->mOpacity;
                mHaveStateful = orig->mHaveStateful;
                mStateful = orig->mStateful;
                mCheckedConstantState = mCanConstantState = true;
            }
        }
        
        virtual shared_ptr<Drawable> newDrawable() {
            return make_shared<LayerDrawable>(*this, (Resources*) NULL);
        }
        
        virtual shared_ptr<Drawable> newDrawable(Resources *res) {
            return make_shared<LayerDrawable>(*this, res);
        }
        
        virtual int getChangingConfigurations() {
            return mChangingConfigurations;
        }
        
        const int getOpacity() {
            if (mHaveOpacity) {
                return mOpacity;
            }
            
            const int N = mNum;
            int op = N > 0 ? mChildren[0]->mDrawable->getOpacity() : PixelFormats::PIXEL_FORMAT_TRANSPARENT;
            for (int i = 1; i < N; i++) {
                op = Drawable::resolveOpacity(op, mChildren[i]->mDrawable->getOpacity());
            }
            mOpacity = op;
            mHaveOpacity = true;
            return op;
        }
        
        const bool isStateful() {
            if (mHaveStateful) {
                return mStateful;
            }
            
            bool stateful = false;
            const int N = mNum;
            for (int i = 0; i < N; i++) {
                if (mChildren[i]->mDrawable->isStateful()) {
                    stateful = true;
                    break;
                }
            }
            
            mStateful = stateful;
            mHaveStateful = true;
            return stateful;
        }
        
        bool canConstantState() {
            if (!mCheckedConstantState && !mChildren.empty()) {
                mCanConstantState = true;
                const int N = mNum;
                for (int i = 0; i < N; i++) {
                    if (mChildren[i]->mDrawable->getConstantState() == NULL) {
                        mCanConstantState = false;
                        break;
                    }
                }
                mCheckedConstantState = true;
            }
            
            return mCanConstantState;
        }
    };
    
    LayerDrawable(vector<shared_ptr<Drawable>> layers, LayerState state);
    
    LayerState mLayerState;
    
private:
    
    int mOpacityOverride = PixelFormats::PIXEL_FORMAT_UNKNOWN;
    vector<int> mPaddingL;
    vector<int> mPaddingT;
    vector<int> mPaddingR;
    vector<int> mPaddingB;
    
    Rect mTmpRect;
    bool mMutated = false;
    
    void addLayer(shared_ptr<Drawable> &layer, string id, int left, int top, int right, int bottom);
    virtual LayerState createConstantState(LayerState state, Resources *res);
    void ensurePadding();
    bool reapplyPadding(int i, shared_ptr<ChildDrawable> r);
    
public:
    
    LayerDrawable() {}
    LayerDrawable(LayerState state, Resources *res);
    
    /**
     * Create a new layer drawable with the list of specified layers.
     *
     * @param layers A list of drawables to use as layers in this new drawable.
     */
    LayerDrawable(vector<shared_ptr<Drawable>> layers) : LayerDrawable(layers, LayerState()) {
    }
    
    shared_ptr<Drawable> findDrawableByLayerId(string id);
    
    virtual int getChangingConfigurations();
    virtual ConstantState *getConstantState();
    shared_ptr<Drawable> getDrawable(int index);
    string getId(int index);
    virtual int getIntrinsicHeight();
    virtual int getIntrinsicWidth();
    int getNumberOfLayers();
    virtual int getOpacity();
    virtual bool getPadding(Rect &padding);
    
    virtual void draw(shared_ptr<Canvas> canvas);
    virtual void invalidateDrawable(shared_ptr<Drawable> who);
    virtual void inflate(Resources *r, CCPullParser &parser, AttributeSet *attrs);
    virtual shared_ptr<Drawable> mutate();
    
    virtual bool isStateful();
    
    virtual void scheduleDrawable(Drawable *who, const mindroid::sp<Runnable> &what, uint32_t when);
    
    virtual void setAlpha(int alpha);
    virtual void setColorFilter(ColorFilter *cf);
    virtual void setDither(bool dither);
    bool setDrawableByLayerId(string id, shared_ptr<Drawable> &drawable);
    void setId(int index, string id);
    void setLayerInset(int index, int l, int t, int r, int b);
    virtual void setLayoutDirection(int layoutDirection);
    void setOpacity(int opacity);
    virtual bool setVisible(bool visible, bool restart);
    
    virtual void unscheduleDrawable(Drawable *who, const mindroid::sp<Runnable> &what);
    
protected:
    
    virtual void onBoundsChange(Rect bounds);
    virtual bool onLevelChange(int level);
    virtual bool onStateChange(vector<int> state);
};

ANDROID_END

#endif /* defined(__cocos2dx__LayerDrawable__) */
