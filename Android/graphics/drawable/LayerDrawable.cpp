//
//  LayerDrawable.cpp
//  cocos2dx
//
//  Created by Saul Howard on 11/26/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include "LayerDrawable.h"

#include "Android/internal/R.h"

ANDROID_BEGIN

/**
 * Create a new layer drawable with the specified list of layers and the specified
 * constant state.
 *
 * @param layers The list of layers to add to this drawable.
 * @param state The constant drawable state.
 */
LayerDrawable::LayerDrawable(vector<shared_ptr<Drawable>> layers, LayerState state) : LayerDrawable(state, NULL) {

    int length = layers.size();
    vector<shared_ptr<ChildDrawable>> r = vector<shared_ptr<ChildDrawable>>(length);
    
    for (int i = 0; i < length; i++) {
        r[i] = make_shared<ChildDrawable>();
        r[i]->mDrawable = layers[i];
        layers[i]->setCallback(this);
        mLayerState.mChildrenChangingConfigurations |= layers[i]->getChangingConfigurations();
    }
    mLayerState.mNum = length;
    mLayerState.mChildren = r;
    
    ensurePadding();
}

LayerDrawable::LayerDrawable(LayerState state, Resources *res) {
    mLayerState = createConstantState(state, res);
    if (mLayerState.mNum > 0) {
        ensurePadding();
    }
}

LayerDrawable::LayerState LayerDrawable::createConstantState(LayerState state, Resources *res) {
    return LayerState(&state, this, res);
}

void LayerDrawable::inflate(Resources *r, CCPullParser &parser, AttributeSet *attrs) {
    Drawable::inflate(r, parser, attrs);
    
    mOpacityOverride = attrs->getAttributeIntValue(R::styleable::opacity, PixelFormats::PIXEL_FORMAT_UNKNOWN);
    
    if (parser.hasChildren()) {
        
        parser.firstChild();
        
        do {
            
            attrs->loadAttributes();
            const std::string name(parser.getName());
        
            if (name.compare("item") != 0) {
                continue;
            }
            
            string drawableRes;
            
            int left = attrs->getDimensionValue(r, R::styleable::left, 0);
            int top = attrs->getDimensionValue(r, R::styleable::top, 0);
            int right = attrs->getDimensionValue(r, R::styleable::right, 0);
            int bottom = attrs->getDimensionValue(r, R::styleable::bottom, 0);
            const char* res = attrs->getAttributeValue(R::styleable::drawable);
            if (res) {
                drawableRes = string(res);
            }
            string id = attrs->getAttributeId(R::styleable::id);
            
            shared_ptr<Drawable> dr;
            if (!drawableRes.empty()) {
                dr = r->getDrawable(drawableRes);
            } else {
                parser.firstChild();
                dr = Drawable::createFromXmlInner(r, parser);
                parser.toParent();
            }
            
            addLayer(dr, id, left, top, right, bottom);
            
        } while (parser.next());
        
        parser.toParent();
    }
    
    ensurePadding();
    onStateChange(getState());
}

/**
 * Add a new layer to this drawable. The new layer is identified by an id.
 *
 * @param layer The drawable to add as a layer->
 * @param id The id of the new layer->
 * @param left The left padding of the new layer->
 * @param top The top padding of the new layer->
 * @param right The right padding of the new layer->
 * @param bottom The bottom padding of the new layer->
 */
void LayerDrawable::addLayer(shared_ptr<Drawable> &layer, string id, int left, int top, int right, int bottom) {
    LayerState &st = mLayerState;
    int N = st.mChildren.size();
    int i = st.mNum;
    if (i >= N) {
        vector<shared_ptr<ChildDrawable>> nu = vector<shared_ptr<ChildDrawable>>(N + 10);
        if (i > 0) {
            nu = st.mChildren;
        }
        st.mChildren = nu;
    }
    
    mLayerState.mChildrenChangingConfigurations |= layer->getChangingConfigurations();
    
    shared_ptr<ChildDrawable> childDrawable = make_shared<ChildDrawable>();
    st.mChildren[i] = childDrawable;
    childDrawable->mId = id;
    childDrawable->mDrawable = layer;
    childDrawable->mInsetL = left;
    childDrawable->mInsetT = top;
    childDrawable->mInsetR = right;
    childDrawable->mInsetB = bottom;
    st.mNum++;
    
    layer->setCallback(this);
}

/**
 * Look for a layer with the given id, and returns its {@link Drawable}.
 *
 * @param id The layer ID to search for->
 * @return The {@link Drawable} of the layer that has the given id in the hierarchy or NULL.
 */
shared_ptr<Drawable> LayerDrawable::findDrawableByLayerId(string id) {
    const vector<shared_ptr<ChildDrawable>> &layers = mLayerState.mChildren;
    
    for (int i = mLayerState.mNum - 1; i >= 0; i--) {
        if (layers[i]->mId.compare(id) == 0) {
            return layers[i]->mDrawable;
        }
    }
    
    return NULL;
}

/**
 * Sets the ID of a layer->
 *
 * @param index The index of the layer which will received the ID.
 * @param id The ID to assign to the layer->
 */
void LayerDrawable::setId(int index, string id) {
    mLayerState.mChildren[index]->mId = id;
}

/**
 * Returns the number of layers contained within this.
 * @return The number of layers.
 */
int LayerDrawable::getNumberOfLayers() {
    return mLayerState.mNum;
}

/**
 * Returns the drawable at the specified layer index.
 *
 * @param index The layer index of the drawable to retrieve.
 *
 * @return The {@link android.graphics.drawable.Drawable} at the specified layer index.
 */
shared_ptr<Drawable> LayerDrawable::getDrawable(int index) {
    return mLayerState.mChildren[index]->mDrawable;
}

/**
 * Returns the id of the specified layer->
 *
 * @param index The index of the layer->
 *
 * @return The id of the layer or {@link android.view.View#NO_ID} if the layer has no id.
 */
string LayerDrawable::getId(int index) {
    return mLayerState.mChildren[index]->mId;
}

/**
 * Sets (or replaces) the {@link Drawable} for the layer with the given id.
 *
 * @param id The layer ID to search for->
 * @param drawable The replacement {@link Drawable}.
 * @return Whether the {@link Drawable} was replaced (could return false if
 *         the id was not found).
 */
bool LayerDrawable::setDrawableByLayerId(string id, shared_ptr<Drawable> &drawable) {
    const vector<shared_ptr<ChildDrawable>> &layers = mLayerState.mChildren;
    
    for (int i = mLayerState.mNum - 1; i >= 0; i--) {
        if (layers[i]->mId.compare(id) == 0) {
            if (layers[i]->mDrawable != NULL) {
                if (drawable != NULL) {
                    Rect bounds = layers[i]->mDrawable->getBounds();
                    drawable->setBounds(bounds);
                }
                layers[i]->mDrawable->setCallback(NULL);
            }
            if (drawable != NULL) {
                drawable->setCallback(this);
            }
            layers[i]->mDrawable = drawable;
            return true;
        }
    }
    
    return false;
}

/** Specify modifiers to the bounds for the drawable[index].
 left += l
 top += t;
 right -= r;
 bottom -= b;
 */
void LayerDrawable::setLayerInset(int index, int l, int t, int r, int b) {
    shared_ptr<ChildDrawable> &childDrawable = mLayerState.mChildren[index];
    childDrawable->mInsetL = l;
    childDrawable->mInsetT = t;
    childDrawable->mInsetR = r;
    childDrawable->mInsetB = b;
}

// overrides from Drawable.Callback

void LayerDrawable::invalidateDrawable(shared_ptr<Drawable> who) {
    Drawable::Callback *callback = getCallback();
    if (callback != NULL) {
        callback->invalidateDrawable(shared_from_this());
    }
}

void LayerDrawable::scheduleDrawable(Drawable * who, const mindroid::sp<Runnable> &what, uint32_t when) {
    Drawable::Callback *callback = getCallback();
    if (callback != NULL) {
        callback->scheduleDrawable(this, what, when);
    }
}

void LayerDrawable::unscheduleDrawable(Drawable * who, const mindroid::sp<Runnable> &what) {
    Drawable::Callback *callback = getCallback();
    if (callback != NULL) {
        callback->unscheduleDrawable(this, what);
    }
}

// overrides from Drawable

void LayerDrawable::draw(shared_ptr<Canvas> canvas) {
    const vector<shared_ptr<ChildDrawable>> &array = mLayerState.mChildren;
    const int N = mLayerState.mNum;
    for (int i = 0; i < N; i++) {
        array[i]->mDrawable->draw(canvas);
    }
}

int LayerDrawable::getChangingConfigurations() {
    return Drawable::getChangingConfigurations()
    | mLayerState.mChangingConfigurations
    | mLayerState.mChildrenChangingConfigurations;
}

bool LayerDrawable::getPadding(Rect &padding) {
    // Arbitrarily get the padding from the first image.
    // Technically we should maybe do something more intelligent,
    // like take the max padding of all the images.
    padding.fLeft = 0;
    padding.fTop = 0;
    padding.fRight = 0;
    padding.fBottom = 0;
    const vector<shared_ptr<ChildDrawable>> &array = mLayerState.mChildren;
    const int N = mLayerState.mNum;
    for (int i = 0; i < N; i++) {
        reapplyPadding(i, array[i]);
        padding.fLeft += mPaddingL[i];
        padding.fTop += mPaddingT[i];
        padding.fRight += mPaddingR[i];
        padding.fBottom += mPaddingB[i];
    }
    return true;
}

bool LayerDrawable::setVisible(bool visible, bool restart) {
    bool changed = Drawable::setVisible(visible, restart);
    const vector<shared_ptr<ChildDrawable>> &array = mLayerState.mChildren;
    const int N = mLayerState.mNum;
    for (int i = 0; i < N; i++) {
        array[i]->mDrawable->setVisible(visible, restart);
    }
    return changed;
}

void LayerDrawable::setDither(bool dither) {
    const vector<shared_ptr<ChildDrawable>> &array = mLayerState.mChildren;
    const int N = mLayerState.mNum;
    for (int i = 0; i < N; i++) {
        array[i]->mDrawable->setDither(dither);
    }
}

void LayerDrawable::setAlpha(int alpha) {
    const vector<shared_ptr<ChildDrawable>> &array = mLayerState.mChildren;
    const int N = mLayerState.mNum;
    for (int i = 0; i < N; i++) {
        array[i]->mDrawable->setAlpha(alpha);
    }
}

void LayerDrawable::setColorFilter(ColorFilter *cf) {
    const vector<shared_ptr<ChildDrawable>> &array = mLayerState.mChildren;

    const int N = mLayerState.mNum;
    for (int i = 0; i < N; i++) {
        array[i]->mDrawable->setColorFilter(cf);
    }
}

/**
 * Sets the opacity of this drawable directly, instead of collecting the states from
 * the layers
 *
 * @param opacity The opacity to use, or {@link PixelFormat#UNKNOWN PixelFormat.UNKNOWN}
 * for the default behavior
 *
 * @see PixelFormat#UNKNOWN
 * @see PixelFormat#TRANSLUCENT
 * @see PixelFormat#TRANSPARENT
 * @see PixelFormat#OPAQUE
 */
void LayerDrawable::setOpacity(int opacity) {
    mOpacityOverride = opacity;
}

int LayerDrawable::getOpacity() {
    if (mOpacityOverride != PixelFormats::PIXEL_FORMAT_UNKNOWN) {
        return mOpacityOverride;
    }
    return mLayerState.getOpacity();
}

bool LayerDrawable::isStateful() {
    return mLayerState.isStateful();
}

bool LayerDrawable::onStateChange(vector<int> state) {
    const vector<shared_ptr<ChildDrawable>> &array = mLayerState.mChildren;
    const int N = mLayerState.mNum;
    bool paddingChanged = false;
    bool changed = false;
    for (int i = 0; i < N; i++) {
        const shared_ptr<ChildDrawable> &r = array[i];
        if (r->mDrawable->setState(state)) {
            changed = true;
        }
        if (reapplyPadding(i, r)) {
            paddingChanged = true;
        }
    }
    if (paddingChanged) {
        onBoundsChange(getBounds());
    }
    return changed;
}

bool LayerDrawable::onLevelChange(int level) {
    const vector<shared_ptr<ChildDrawable>> &array = mLayerState.mChildren;
    const int N = mLayerState.mNum;
    bool paddingChanged = false;
    bool changed = false;
    for (int i = 0; i < N; i++) {
        const shared_ptr<ChildDrawable> &r = array[i];
        if (r->mDrawable->setLevel(level)) {
            changed = true;
        }
        if (reapplyPadding(i, r)) {
            paddingChanged = true;
        }
    }
    if (paddingChanged) {
        onBoundsChange(getBounds());
    }
    return changed;
}

void LayerDrawable::onBoundsChange(Rect bounds) {
    
    const vector<shared_ptr<ChildDrawable>> &array = mLayerState.mChildren;
    const int N = mLayerState.mNum;
    int padL = 0, padT = 0, padR = 0, padB = 0;
    
    for (int i = 0; i < N; i++) {
        const shared_ptr<ChildDrawable> &r = array[i];
        r->mDrawable->setBounds(bounds.left() + r->mInsetL + padL,
                              bounds.top() + r->mInsetT + padT,
                              bounds.right() - r->mInsetR - padR,
                              bounds.bottom() - r->mInsetB - padB);
        padL += mPaddingL[i];
        padR += mPaddingR[i];
        padT += mPaddingT[i];
        padB += mPaddingB[i];
    }
}

int LayerDrawable::getIntrinsicWidth() {
    
    int width = -1;
    const vector<shared_ptr<ChildDrawable>> &array = mLayerState.mChildren;
    const int N = mLayerState.mNum;
    int padL = 0, padR = 0;
    
    for (int i = 0; i < N; i++) {
        const shared_ptr<ChildDrawable> &r = array[i];
        int w = r->mDrawable->getIntrinsicWidth() + r->mInsetL + r->mInsetR + padL + padR;
        if (w > width) {
            width = w;
        }
        padL += mPaddingL[i];
        padR += mPaddingR[i];
    }
    
    return width;
}

int LayerDrawable::getIntrinsicHeight() {
    
    int height = -1;
    const vector<shared_ptr<ChildDrawable>> &array = mLayerState.mChildren;
    const int N = mLayerState.mNum;
    int padT = 0, padB = 0;
    
    for (int i = 0; i < N; i++) {
        const shared_ptr<ChildDrawable> &r = array[i];
        int h = r->mDrawable->getIntrinsicHeight() + r->mInsetT + r->mInsetB + + padT + padB;
        if (h > height) {
            height = h;
        }
        padT += mPaddingT[i];
        padB += mPaddingB[i];
    }
    
    return height;
}

bool LayerDrawable::reapplyPadding(int i, shared_ptr<ChildDrawable> r) {
    
    Rect &rect = mTmpRect;
    r->mDrawable->getPadding(rect);
    
    if (rect.left() != mPaddingL[i] || rect.top() != mPaddingT[i] ||
        rect.right() != mPaddingR[i] || rect.bottom() != mPaddingB[i]) {
        mPaddingL[i] = rect.left();
        mPaddingT[i] = rect.top();
        mPaddingR[i] = rect.right();
        mPaddingB[i] = rect.bottom();
        return true;
    }
    
    return false;
}

void LayerDrawable::ensurePadding() {
    const int N = mLayerState.mNum;
    if (mPaddingL.size() >= N) {
        return;
    }
    mPaddingL = vector<int>(N);
    mPaddingT = vector<int>(N);
    mPaddingR = vector<int>(N);
    mPaddingB = vector<int>(N);
}

Drawable::ConstantState *LayerDrawable::getConstantState() {
    if (mLayerState.canConstantState()) {
        mLayerState.mChangingConfigurations = getChangingConfigurations();
        return &mLayerState;
    }
    return NULL;
}

shared_ptr<Drawable> LayerDrawable::mutate() {
    if (!mMutated && Drawable::mutate() == shared_from_this()) {
        mLayerState = createConstantState(mLayerState, NULL);
        const vector<shared_ptr<ChildDrawable>> &array = mLayerState.mChildren;
        const int N = mLayerState.mNum;
        for (int i = 0; i < N; i++) {
            array[i]->mDrawable->mutate();
        }
        mMutated = true;
    }
    return shared_from_this();
}

void LayerDrawable::setLayoutDirection(int layoutDirection) {
    const vector<shared_ptr<ChildDrawable>> &array = mLayerState.mChildren;
    const int N = mLayerState.mNum;
    for (int i = 0; i < N; i++) {
        array[i]->mDrawable->setLayoutDirection(layoutDirection);
    }
    Drawable::setLayoutDirection(layoutDirection);
}

ANDROID_END