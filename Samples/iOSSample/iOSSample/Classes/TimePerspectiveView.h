////
////  TimePerspectiveView.h
////  MoneyMobileX
////
////  Created by Saul Howard on 12/11/13.
////
////
//
//#ifndef __MoneyMobileX__TimePerspectiveView__
//#define __MoneyMobileX__TimePerspectiveView__
//
//#include "AndroidMacros.h"
//
//#include "UiUtils.h"
//#include "BaseTimeAdapter.h"
//#include "TimeView.h"
//
//#include "Android/view/animation/Scales.h"
//#include "Android/widget/BaseScrollView.h"
//#include "Android/graphics/Paint.h"
//#include "Android/graphics/Path.h"
//#include "Android/graphics/Point.h"
//#include "Android/graphics/Rect.h"
//#include "Android/graphics/Color.h"
//#include "Android/graphics/Gradient.h"
//#include "Android/graphics/Shader.h"
//#include "Android/graphics/Bitmap.h"
//
//#include <date_time/boost/date_time/gregorian/gregorian.hpp>
//
//#include "BaseTimeAdapter.h"
//
//#include <SkCanvas.h>
//
//#include <vector>
//#include <deque>
//
//using namespace std;
//using namespace boost::gregorian;
//
//ANDROID_BEGIN
//
//template<class T>
//class TimePerspectiveView : public BaseScrollView<BaseTimeAdapter<T>> {
//    
//private:
//    
//    static const int DISTANCE = 20000;
//    static const int VIEW_DISTANCE = 3500;
//    static const int PADDING = 20;
//    static const int STROKE_WIDTH = 2;
//    static const int FONT_SIZE = 60;
//    static const int NOW_FONT_SIZE = 40;
//    
//    static const int LAYOUT_MODE_FRONT = 0;
//    static const int LAYOUT_MODE_BACK = -1;
//    
////    static const dateComparator mdateComparator = dateComparator.getDateOnlyInstance();
////    static const dateFormatter mFormat = dateFormat.forPattern("MMMM yyyy");
//    
//    struct TimeViewInfo {
//        
//        TimeView *view;
//        date date;
//        void *item;
//        
//        TimeViewInfo(TimeView *view, class date date, void *item) {
//            this->view = view;
//            this->date = date;
//            this->item = item;
//        }
//    };
//    
//    class AdapterDataSetObserver : public DataSetObserver {
//        
//    public:
//        
//        AdapterDataSetObserver(TimePerspectiveView<T> view) {
//            mView = view;
//        }
//        
//        virtual void onChanged() {
//            mView->updateViews();
//        }
//        
//        virtual void onInvalidated() {
//            mView->updateViews();
//        }
//        
//    private:
//        
//        TimePerspectiveView<T> *mView;
//    };
//    
//    class OnTimeListScrollListener {
//    public:
//        virtual void onScrollFinished() = 0;
//    };
//    
//    int mPadding = 0;
//    
//    int mDistance = 0;
//    int mVisibleDistance = 0;
//    
//    /** The ratio of milliseconds to pixels */
//    float mRatio = 0.0f;
//    long mYearDistance = 0;
//    
//    Path mRoadPath;
//    Bitmap mBackground;
//    
//    int mScrollPosition = 0;
//    int mFirstYearTick = 0;
//    int mFirstYearNumber = 0;
//    
//    int mFirstViewPosition = 0;
//    int mLastViewPosition = 0;
//    int mSelectedIndex = -1;
//    void *mSelectedItem = NULL;
//    
//    date mStart, mVisibleStart, mVisibleEnd;
//    
//    deque<TimeViewInfo> mTimeViewInfos;
//    
//    /** A list of cached (re-usable) item views */
//    const deque<TimeView*> mCachedItemViews;
//    
//    BaseTimeAdapter<T> *mAdapter = NULL;
//    AdapterDataSetObserver *mAdapterDataSetObserver = NULL;
//    
//    Paint mLinePaint;
//    Paint mBgLinePaint;
//    Paint mRoadPaint;
//    Paint mTextPaint;
//    Paint mDatePaint;
//    Paint mShadowPaint;
//    Rect mTextBounds;
//    float mFontSize, mNowFontSize;
//    
//    String mNowString;
//    
//    vector<int> mEyePosition;
//    LinearScale mLinearScale;
//    
////    ObjectAnimator mScrollAnimator;
////    ObjectAnimator mChildAnimator;
//    bool mAnimatorCancelled, mChildAnimatorCancelled;
//    
//    float mAnimationPercent = 0;
//    Point mCenter;
//    
//    typename AdapterView<T>::OnItemClickListener mOnItemClickListener;
//    TimeView *mClickedView = NULL;
//    
//    Point mFocusPoint;
//    
//    bool mTransitionClickedItems = true;
//    bool mUpdating = false;
//    
//    vector<TimeView*> mDeletedViews;
//    vector<TimeView*> mAddedViews;
//    float mDeleteScale = 1.0f;
//    float mAddScale = 0.0f;
//    
//    bool mDisableShadows = false;
//    
//    OnTimeListScrollListener *mOnTimeListScrollListener = NULL;
//    
//    TimeView *getTimeChildAt(int index) {
//        return (TimeView*) ViewGroup::getChildAt(index);
//    }
//    
//    /**
//     * Initializes the view
//     */
//    void init() {
//        
//        View::setWillNotDraw(false);
//        
//        mDistance = (int) UiUtils::getDynamicPixels(View::getContext(), DISTANCE);
//        mVisibleDistance = (int) UiUtils::getDynamicPixels(View::getContext(), VIEW_DISTANCE);
//        float mStrokeWidth = UiUtils::getDynamicPixels(View::getContext(), STROKE_WIDTH);
//        mPadding = (int) UiUtils::getDynamicPixels(View::getContext(), PADDING);
//        
//        mLinePaint = Paint(Paint::Flags::kAntiAlias_Flag);
//        mLinePaint.setColor(Color::WHITE);
//        mLinePaint.setStrokeWidth(mStrokeWidth);
//        mBgLinePaint = Paint(Paint::Flags::kAntiAlias_Flag);
//        mBgLinePaint.setColor(Color::WHITE);
//        mBgLinePaint.setStrokeWidth(mStrokeWidth / 2.0f);
//        mBgLinePaint.setAlpha(160);
//        
////        mFontSize = FontUtils.getFontSize(FONT_SIZE);
////        mNowFontSize = FontUtils.getFontSize(NOW_FONT_SIZE);
//        
//        mTextPaint = Paint(Paint::Flags::kAntiAlias_Flag);
//        mTextPaint.setColor(View::getResources()->getColor("gray7"));
////        mTextPaint.setTypeface(FontUtils.getFont(FontUtils.SECONDARY_ITALIC));
//        mTextPaint.setTextSize(mFontSize);
//        
//        mDatePaint = Paint(Paint::Flags::kAntiAlias_Flag);
//        mDatePaint.setColor(Color::WHITE);
////        mDatePaint.setTypeface(FontUtils.getFont(FontUtils.SECONDARY_SEMI_BOLD_ITALIC));
//        mDatePaint.setTextSize(mNowFontSize);
////        mDatePaint.setShadowLayer(mStrokeWidth, mStrokeWidth / 2, mStrokeWidth / 2, View::getResources()->getColor("gray7"));
//        
//        Gradient *gradient = Gradient::createRadial(200, 200, 200, View::getResources().getColor("gray7"),
//                                                      View::getResources().getColor("transparent"), Shader::TileMode::kClamp_TileMode);
//        mShadowPaint = Paint(Paint::Flags::kAntiAlias_Flag);
//        mShadowPaint.setDither(true);
//        mShadowPaint.setShader(gradient);
//        
//        mRoadPaint = Paint(Paint::Flags::kAntiAlias_Flag);
//        mRoadPaint.setColor(View::getResources().getColor("gray2"));
//        
//        vector<vector<float>> range = {{-0.025f, 0.0f}, {0.0f, 0.5f}, {0.5f, 1.0f}, {1.0f, 1.5f}};
//        vector<vector<float>> domain = {{0.0f, 240.0f}, {240.0f, 200.0f}, {200.0f, 0.0f}, {0.0f, 0.0f}};
//        mLinearScale = new LinearScale(range, domain, new LinearInterpolator());
//    }
//    
//    void setDeleteScale(float scale) {
//        mDeleteScale = scale;
//        View::invalidate();
//    }
//    
//    void setAddScale(float scale) {
//        mAddScale = scale;
//        View::invalidate();
//    }
//    
//    /**
//     * Providing a buffer of 2 years before and after
//     * the first and last dates
//     */
//    void setupDateRange() {
//        
//        date first = day_clock::local_day();
//        date last = first;
//        
//        if (mAdapter->getCount() > 0) {
//            first = mAdapter->getDate(0);
//        }
//        
//        bool init = (mStart == NULL);
//        
//        mStart = first.minusYears(2);
//        add_month mf(1);
//        
//        if (mAdapter->getCount() > 0) {
//            last = mAdapter->getDate(mAdapter->getCount() - 1).plusYears(2);
//        }
//        
//        // Calculate the total amount of time the view covers in ms
//        long totalTime = last.getMillis() - mStart.getMillis();
//        
//        // Determine the ms / px ratio
//        mRatio = totalTime / (mDistance - mVisibleDistance);
//        
//        // Calculate the distance 1 year is in pixels
//        mYearDistance = (long) (1000L * 60L * 60L * 24L * 365.25f / mRatio);
//        
//        // If the view is just starting scroll to the current date
//        if (init) {
//            const int z = getZPosition(day_clock::local_day());
//            View::postDelayed([this] (int& z) { scrollToPosition(z, 4000, false); }, 1500);
//        }
//    }
//    
//    void createBackground() {
//        
//        vector<int> left = getProjectedCoordinates(0, View::getHeight(), mVisibleDistance);
//        vector<int> right = getProjectedCoordinates(View::getWidth(), View::getHeight(), mVisibleDistance);
//        
//        int topInterval = (right[0] - left[0]) / 4;
//        int bottomInterval = View::getWidth() / 4;
//        
//        mRoadPath.moveTo(0, View::getHeight());
//        mRoadPath.lineTo(left[0], left[1]);
//        mRoadPath.lineTo(right[0], right[1]);
//        mRoadPath.lineTo(View::getWidth(), View::getHeight());
//        mRoadPath.close();
//        
//        if (mBackground == NULL) {
//            mBackground = Bitmap::createBitmap(View::getWidth(), View::getHeight(), Bitmap::Config::kARGB_8888_Config);
//        }
//        
//        mRoadPaint.setShader(Gradient::createLinear(0, View::getHeight(), 0, left[1],
//                                                             View::getResources().getColor("gray3"), View::getResources().getColor("gray2"),
//                                                             Shader::TileMode::kMirror_TileMode));
//        mBgLinePaint.setShader(Gradient::createLinear(0, View::getHeight(), 0, left[1],
//                                                               View::getResources().getColor("gray0"), View::getResources().getColor("gray11"),
//                                                               Shader::TileMode::kMirror_TileMode));
//        Paint bg = Paint();
//        bg.setShader(Gradient::createLinear(0, View::getHeight(), 0, left[1],
//                                                     View::getResources().getColor("gray01"), View::getResources().getColor("gray11"),
//                                                     Shader::TileMode::kMirror_TileMode));
//        
//        SkCanvas *canvas = new SkCanvas(mBackground);
//        canvas->drawRect(0, left[1], View::getWidth(), View::getHeight(), bg);
//        canvas->drawPath(mRoadPath, mRoadPaint);
//        for (int i = 1; i < 4; i++) {
//            canvas->drawLine(bottomInterval * i, View::getHeight(), left[0] + topInterval * i, left[1], mBgLinePaint);
//        }
//        
//        delete canvas;
//        
//        mBgLinePaint.setShader(NULL);
//    }
//    
//    /**
//     * Updates the visible date range based on our current scroll position
//     */
//    void updateVisibleDateRange() {
//        mVisibleStart = getDate(mScrollPosition - (int) ((float) mYearDistance / 4.0f)).withTimeAtStartOfDay();
//        mVisibleEnd = getDate(mScrollPosition + mVisibleDistance).withTimeAtStartOfDay();
//        
//        date start = mVisibleStart.plusYears(1).withMonthOfYear(1).withDayOfMonth(1);
//        mFirstYearTick = getZPosition(start);
//        mFirstYearNumber = start.getYear();
//    }
//    
//    /**
//     * Position all views based on their current z position in 3D space.
//     * Also adjust the alpha of the view based on its z position.
//     */
//    void positionViews() {
//        
//        for (int index = 0; index < ViewGroup::getChildCount(); index++) {
//            
//            TimeView *child = getTimeChildAt(index);
//            
//            TimeViewInfo timeViewInfo = mTimeViewInfos.get(index);
//            int z = getZPosition(timeViewInfo.date) - mScrollPosition;
//            
//            float percent = 1.0f;
//            if (z <= mVisibleDistance) percent = (float) (z % mVisibleDistance) / (float) mVisibleDistance;
//            const int alpha = (int) mLinearScale.scale(percent);
//            
//            child->setViewAlpha(alpha);
//            
//            measureChild(child, z);
//            
//            const int width = child->getMeasuredWidth();
//            const int height = child->getMeasuredHeight();
//            const vector<int> position = getProjectedCoordinates(0, View::getHeight(), z);
//            
//            child->layout(0, position[1] - height, width, position[1]);
//        }
//    }
//    
//    /**
//     * Runs through all views that should be displayed based on our adapter and updates
//     * the views representing each item.  Also delete/add views that were not previously
//     * represented based on changes to the adapter data.
//     */
//    void updateViews() {
//        
//        if (mUpdating) return;
//        
//        mUpdating = true;
//        
//        deque<TimeViewInfo> tempTimeViewInfos;
//        setupDateRange();
//        updateVisibleDateRange();
//        
//        mLastViewPosition = -1;
//        
//        // Find the last position for our visible date range
//        for (int position = mAdapter->getCount() - 1; position >= 0; position--) {
//            if (mdateComparator.compare(mAdapter->getDate(position), mVisibleEnd) < 0) {
//                mLastViewPosition = mFirstViewPosition = position;
//                break;
//            }
//        }
//        
//        for (int position = mLastViewPosition; position >= 0
//             && mdateComparator.compare(mAdapter->getDate(position), mVisibleStart) > 0; position--) {
//            
//            TimeView *timeView = NULL;
//            
//            // Check if the adapter item is represented by a view currently in the hierarchy
//            // so it can be updated with the new data
//            for (int index = 0; index < mTimeViewInfos.size(); index++) {
//                
//                TimeViewInfo timeViewInfo = mTimeViewInfos.get(index);
//                
//                if (timeViewInfo.item.equals(mAdapter->getItem(position))) {
//                    timeViewInfo.date = mAdapter->getDate(position);
//                    timeView = mAdapter->getView(position, timeViewInfo.view, this, true);
//                    tempTimeViewInfos.addLast(mTimeViewInfos.remove(index));
//                    mFirstViewPosition--;
//                    break;
//                }
//            }
//            
//            // Add new views that are now in our visible range
//            if (timeView == NULL) {
//                timeView = mAdapter->getView(position, getCachedView(), this);
//                mAddedViews.push_back(timeView);
//                TimeViewInfo timeViewInfo = addAndMeasureChild(timeView, mAdapter->getDate(position), mAdapter->getItem(position), LAYOUT_MODE_BACK);
//                tempTimeViewInfos.addLast(timeViewInfo);
//            }
//            
//            ViewGroup::bringChildToFront(timeView);
//        }
//        
//        // Views that are no longer in visible range need to be removed
//        for (TimeViewInfo timeViewInfo : mTimeViewInfos) {
//            mDeletedViews.push_back(timeViewInfo.view);
//            timeViewInfo.view.setDeleted();
//            if (mAdapter->getPosition(timeViewInfo.item) == -1) {
//                mFirstViewPosition++;
//            }
//        }
//        
//        // Update our ViewInfo list
//        mTimeViewInfos = tempTimeViewInfos;
//        
//        // If there are not any deleted views to animate simply finish the update
//        if (!animateDeletedViews()) {
//            finishUpdate();
//        }
//    }
//    
//    /**
//     * Position items and redraw
//     */
//    void finishUpdate() {
//        
//        positionViews();
//        View::invalidate();
//        
//        // If there are not any new views to animate scroll to the currently selected view
//        if (!animateAddedViews()) {
//            updateSelection();
//        }
//    }
//    
//    /**
//     * Scroll to the selected item after the data set has changed and the view
//     * has been updated
//     */
//    void updateSelection() {
//        
//        if (mSelectedIndex != -1) {
//            mSelectedIndex = mAdapter->getPosition(mSelectedItem);
//            if (mSelectedIndex != -1) {
//                setSelection(mSelectedIndex, false);
//            }
//        }
//        
//        mUpdating = false;
//    }
//    
//    bool animateDeletedViews() {
//        
//        if (mDeletedViews.size() == 0) return false;
//        
////        android.animation.ObjectAnimator animator = android.animation.ObjectAnimator.ofFloat(this, "deleteScale", 1f, 0f);
////        animator.setDuration(1000);
////        animator.addListener(new android.animation.Animator.AnimatorListener() {
////            @Override
////            void onAnimationStart(android.animation.Animator animator) {
////            }
////            
////            @Override
////            void onAnimationEnd(android.animation.Animator animator) {
////                
////                for (TimeView view : mDeletedViews) {
////                    mCachedItemViews.addLast(view);
////                    removeViewInLayout(view);
////                }
////                
////                mDeleteScale = 1f;
////                mDeletedViews.clear();
////                
////                finishUpdate();
////            }
////            
////            @Override
////            void onAnimationCancel(android.animation.Animator animator) {
////            }
////            
////            @Override
////            void onAnimationRepeat(android.animation.Animator animator) {
////            }
////        });
////        animator.start();
//        
//        return true;
//    }
//    
//    bool animateAddedViews() {
//        
//        if (mAddedViews.size() == 0) return false;
//        
////        android.animation.ObjectAnimator animator = android.animation.ObjectAnimator.ofFloat(this, "addScale", 0f, 1f);
////        animator.setDuration(1000);
////        animator.addListener(new android.animation.Animator.AnimatorListener() {
////            @Override
////            void onAnimationStart(android.animation.Animator animator) {
////            }
////            
////            @Override
////            void onAnimationEnd(android.animation.Animator animator) {
////                mAddScale = 0f;
////                mAddedViews.clear();
////                updateSelection();
////            }
////            
////            @Override
////            void onAnimationCancel(android.animation.Animator animator) {
////            }
////            
////            @Override
////            void onAnimationRepeat(android.animation.Animator animator) {
////            }
////        });
////        animator.start();
//        
//        return true;
//    }
//    
//    /**
//     * Loops through each view to see if it still resides
//     * within the current date range we are presenting
//     */
//    void removeNonVisibleViews(date visibleStart, date visibleEnd) {
//        
//        if (ViewGroup::getChildCount() == 0) return;
//        
//        TimeViewInfo timeViewInfo;
//        
//        if (mLastViewPosition != 0 && mTimeViewInfos.size() > 0) {
//            
//            timeViewInfo = mTimeViewInfos.front();
//            
//            while (timeViewInfo != NULL && mdateComparator.compare(timeViewInfo.date, visibleEnd) > 0) {
//                
//                mTimeViewInfos.pop_front();
//                removeViewInLayout(timeViewInfo.view);
//                mCachedItemViews.push_back(timeViewInfo.view);
//                
//                mLastViewPosition--;
//                
//                if (mTimeViewInfos.size() > 0) {
//                    timeViewInfo = mTimeViewInfos.front();
//                } else {
//                    timeViewInfo = NULL;
//                }
//            }
//        }
//        
//        if (mFirstViewPosition != mAdapter->getCount() - 1 && mTimeViewInfos.size() > 0) {
//            
//            timeViewInfo = mTimeViewInfos.back();
//            
//            while (timeViewInfo != NULL && mdateComparator.compare(timeViewInfo.date, visibleStart) < 0) {
//                
//                mTimeViewInfos.pop_back();
//                removeViewInLayout(timeViewInfo.view);
//                mCachedItemViews.push_back(timeViewInfo.view);
//                
//                mFirstViewPosition++;
//                
//                if (mTimeViewInfos.size() > 0) {
//                    timeViewInfo = mTimeViewInfos.back();
//                } else {
//                    timeViewInfo = NULL;
//                }
//            }
//        }
//    }
//    
//    void fillList(date visibleStart, date visibleEnd) {
//        date lastDate = visibleStart;
//        if (mTimeViewInfos.size() > 0) {
//            lastDate = mTimeViewInfos.getFirst().date;
//        }
//        addViewsAfterDate(lastDate, visibleEnd);
//        
//        date firstDate = visibleEnd;
//        if (mTimeViewInfos.size() > 0) {
//            firstDate = mTimeViewInfos.getLast().date;
//        }
//        
//        addViewsBeforeDate(firstDate, visibleStart);
//    }
//    
//    void addViewsAfterDate(class date date, class date visibleEnd) {
//        
//        while (mLastViewPosition < mAdapter->getCount() - 1
//               && mdateComparator.compare(mAdapter->getDate(mLastViewPosition + 1), date) >= 0
//               && mdateComparator.compare(mAdapter->getDate(mLastViewPosition + 1), visibleEnd) <= 0) {
//            mLastViewPosition++;
//            const TimeView *newLastChild = mAdapter->getView(mLastViewPosition, getCachedView(), this);
//            TimeViewInfo timeViewInfo
//            = addAndMeasureChild(newLastChild, mAdapter->getDate(mLastViewPosition), mAdapter->getItem(mLastViewPosition), LAYOUT_MODE_FRONT);
//            mTimeViewInfos.addFirst(timeViewInfo);
//        }
//    }
//    
//    void addViewsBeforeDate(class date date, class date visibleStart) {
//        
//        while (mFirstViewPosition > 0
//               && mdateComparator.compare(mAdapter->getDate(mFirstViewPosition - 1), date) <= 0
//               && mdateComparator.compare(mAdapter->getDate(mFirstViewPosition - 1), visibleStart) >= 0) {
//            mFirstViewPosition--;
//            const TimeView *newFirstChild = mAdapter->getView(mFirstViewPosition, getCachedView(), this);
//            TimeViewInfo timeViewInfo
//            = addAndMeasureChild(newFirstChild, mAdapter->getDate(mFirstViewPosition), mAdapter->getItem(mFirstViewPosition), LAYOUT_MODE_BACK);
//            mTimeViewInfos.addLast(timeViewInfo);
//        }
//    }
//    
//    TimeViewInfo addAndMeasureChild(TimeView *child, const class date date, const void *item, const int index) {
//        
//        LayoutParams *params = child->getLayoutParams();
//        if (params == NULL) {
//            params = new LayoutParams(LayoutParams::MATCH_PARENT, LayoutParams::WRAP_CONTENT);
//        }
//        
//        TimeViewInfo timeViewInfo = TimeViewInfo(child, date, item);
//        
//        ViewGroup::addViewInLayout(child, index, params, true);
//        
//        const int z = getZPosition(date) - mScrollPosition;
//        
//        measureChild(child, z);
//        
//        return timeViewInfo;
//    }
//    
//    void measureChild(View *child, const int z) {
//        
//        const int width = getProjectedWidth(View::getMeasuredWidth(), z);
//        const int height = getProjectedHeight(View::getMeasuredHeight(), z);
//        
//        const int padding = (int) ((float) (View::getWidth() - width) / 2.0f);
//        child->measure(View::MeasureSpec::EXACTLY | View::getWidth(), View::MeasureSpec::EXACTLY | height);
//        child->setPadding(padding, 0, padding, 0);
//    }
//    
//    int getZPosition(class date date) {
//        float millis = date.getMillis() - mStart.getMillis();
//        return (int) (millis / mRatio);
//    }
//    
//    date getDate(int zPosition) {
//        long millis = (long) (zPosition * mRatio) + mStart.getMillis();
//        return new date(millis);
//    }
//    
//    void scrollToPosition(const int position, bool click) {
//        int distance = Math.abs(position - mScrollPosition);
//        float percent = (float) distance / (float) mVisibleDistance;
//        long duration = 750 + (long) (750f * percent);
//        scrollToPosition(position, distance == 0 ? 0 : duration, click);
//    }
//    
//    void scrollToPosition(const int position, long duration, const bool click) {
//        
//        cancelScroll();
//        
//        mScrollAnimator = ObjectAnimator.ofInt(this, "scrollPosition", mScrollPosition, position);
//        mScrollAnimator.setDuration(duration);
//        mScrollAnimator.addListener(new Animator.AnimatorListener() {
//            @Override
//            void onAnimationStart(Animator animator) {
//                mAnimatorCancelled = false;
//            }
//            
//            @Override
//            void onAnimationEnd(Animator animator) {
//                
//                if (!mAnimatorCancelled && mTransitionClickedItems && click) {
//                    animateSelection(true);
//                } else if (!mAnimatorCancelled && click) {
//                    clickItem();
//                    mSelectedIndex = -1;
//                    mClickedView = NULL;
//                }
//                
//                if (mOnTimeListScrollListener != NULL) {
//                    mOnTimeListScrollListener.onScrollFinished();
//                }
//            }
//            
//            @Override
//            void onAnimationCancel(Animator animator) {
//                mAnimatorCancelled = true;
//            }
//            
//            @Override
//            void onAnimationRepeat(Animator animator) {
//            }
//        });
//        mScrollAnimator.start();
//    }
//    
//    void animateSelection(const bool in) {
//        
//        if (in) mClickedView = getTimeChildAt(mLastViewPosition - mSelectedIndex);
//        
////        mChildAnimator = ObjectAnimator.ofFloat(this, "animationPercent", in ? 0f : 1f, in ? 1f : 0f);
////        mChildAnimator.addListener(new Animator.AnimatorListener() {
////            @Override
////            void onAnimationStart(Animator animator) {
////                mChildAnimatorCancelled = false;
////            }
////            
////            @Override
////            void onAnimationEnd(Animator animator) {
////                if (!mChildAnimatorCancelled && in) {
////                    clickItem();
////                } else if (!in) {
////                    mSelectedIndex = -1;
////                    mClickedView = NULL;
////                }
////            }
////            
////            @Override
////            void onAnimationCancel(Animator animator) {
////                mChildAnimatorCancelled = true;
////            }
////            
////            @Override
////            void onAnimationRepeat(Animator animator) {
////            }
////        });
////        mChildAnimator.setDuration(500);
////        mChildAnimator.start();
//    }
//    
//    void clickItem() {
//        if (mOnItemClickListener != NULL) {
//            mOnItemClickListener.onItemClick(TimePerspectiveView.this, mClickedView, mSelectedIndex, mSelectedIndex);
//        }
//    }
//    
//    void setAnimationPercent(float animationPercent) {
//        mAnimationPercent = animationPercent;
//        invalidate();
//    }
//    
//    /**
//     * Checks if there is a cached view that can be used
//     *
//     * @return A cached view or, if none was found, NULL
//     */
//    TimeView getCachedView() {
//        if (mCachedItemViews.size() != 0) {
//            return mCachedItemViews.removeFirst();
//        }
//        return NULL;
//    }
//    
//    void drawShadow(Canvas *canvas, float x, float y, float radius, int alpha) {
//        
//        if (mDisableShadows || radius <= 0 || alpha == 0) return;
//        
//        const Gradient *gradient = Gradient::createRadial(x, y + radius, radius, View::getResources()->getColor("gray5"),
//                                                          View::getResources()->getColor("transparent"), Shader::TileMode::kClamp_TileMode);
//        mShadowPaint.setDither(true);
//        mShadowPaint.setShader(gradient);
//        mShadowPaint.setAlpha(alpha);
//        canvas->save();
//        canvas->scale(1f, 0.2f, x, y + radius);
//        canvas->drawCircle(x, y + radius, radius, mShadowPaint);
//        canvas->restore();
//    }
//    
//    vector<int> getProjectedCoordinates(int x, int y, int z) {
//        
//        if (mEyePosition[2] + z == 0) z--;
//        
//        int updateX = (mEyePosition[2] * (x - mEyePosition[0])) / (mEyePosition[2] + z) + mEyePosition[0];
//        int updateY = (mEyePosition[2] * (y - mEyePosition[1])) / (mEyePosition[2] + z) + mEyePosition[1];
//        
//        return vector<int>({updateX, updateY});
//    }
//    
//    int getProjectedWidth(int width, int z) {
//        return getProjectedCoordinates(width, 0, z)[0] - getProjectedCoordinates(0, 0, z)[0];
//    }
//    
//    int getProjectedHeight(int height, int z) {
//        return getProjectedCoordinates(0, height, z)[1] - getProjectedCoordinates(0, 0, z)[1];
//    }
//
//public:
//    
//    void setOnTimeListScrollListener(OnTimeListScrollListener mOnTimeListScrollListener) {
//        this.mOnTimeListScrollListener = mOnTimeListScrollListener;
//    }
//    
//    void setOnItemClickListener(typename AdapterView<T>::OnItemClickListener mOnItemClickListener) {
//        this.mOnItemClickListener = mOnItemClickListener;
//    }
//    
//    void setTransitionClickedItems(bool mTransitionClickedItems) {
//        this.mTransitionClickedItems = mTransitionClickedItems;
//    }
//    
//    void setDisableShadows(bool mDisableShadows) {
//        this.mDisableShadows = mDisableShadows;
//    }
//    
//    TimePerspectiveView(Context *context) : TimePerspectiveView(context, NULL) {
//    }
//    
//    TimePerspectiveView(Context *context, AttributeSet *attrs) : BaseScrollView<BaseTimeAdapter<T> >(context, attrs) {
//        init();
//    }
//    
//    /**
//     * Sets the scroll position of the view
//     *
//     * @param position the scroll position of the view
//     */
//    void setScrollPosition(int position) {
//        
//        if (mChildAnimator != NULL && mChildAnimator.isRunning()) return;
//        
//        mScrollPosition = position;
//        
//        if (mScrollPosition < 0) {
//            mScrollPosition = 0;
//        } else if (mScrollPosition + mVisibleDistance > mDistance) {
//            mScrollPosition = mDistance - mVisibleDistance;
//        }
//        
//        requestLayout();
//    }
//    
//    virtual BaseTimeAdapter<T> getAdapter() {
//        return mAdapter;
//    }
//    
//    virtual void setAdapter(BaseTimeAdapter<T> *baseTimeAdapter) {
//        
//        if (baseTimeAdapter != NULL && mAdapterDataSetObserver != NULL) {
//            mAdapter->unregisterDataSetObserver(mAdapterDataSetObserver);
//        }
//        
//        mAdapter = baseTimeAdapter;
//        
//        if (mAdapter != NULL) {
//            mAdapterDataSetObserver = new AdapterDataSetObserver();
//            mAdapter->registerDataSetObserver(mAdapterDataSetObserver);
//        }
//        
//        setupDateRange();
//        requestLayout();
//    }
//    
//    date getCurrentDate() {
//        return getDate(mScrollPosition);
//    }
//    
//    bool showingDate(class date date) {
//        long time = date.getMillis();
//        return mVisibleStart.getMillis() <= time && mVisibleEnd.getMillis() >= time;
//    }
//    
//    virtual View *getSelectedView() {
//        return mClickedView;
//    }
//    
//    virtual void setSelection(int position) {
//        setSelection(position, true);
//        mSelectedItem = mAdapter->getItem(position);
//    }
//    
//    void setSelection(int position, bool click) {
//        
//        if (mAdapter == NULL || position == -1) return;
//        
//        mSelectedIndex = position;
//        
//        int z = getZPosition(mAdapter->getDate(position));
//        scrollToPosition(z, click);
//    }
//    
//    void unselect() {
//        if (mClickedView != NULL) {
//            animateSelection(false);
//        }
//    }
//    
//    void scrollToDate(class date date) {
//        int z = getZPosition(date);
//        scrollToPosition(z, false);
//    }
//    
//    void scrollBy(int pixels) {
//        setScrollPosition(mScrollPosition + pixels);
//    }
//    
//protected:
//    
//    virtual void onDetachedFromWindow() {
//        BaseScrollView<T>::onDetachedFromWindow();
//        
//        if (mBackground != NULL && !mBackground.isRecycled()) {
//            mBackground.recycle();
//            mBackground = NULL;
//        }
//    }
//    
//    /**
//     * Scrolls the list. Takes care of updating rotation (if enabled) and
//     * snapping
//     *
//     * @param scrolledDistance The distance to scroll
//     *
//     * @return Whether the scrolled view is at its max or min scroll amount
//     */
//    virtual bool scrollList(int scrolledDistance) {
//        
//        if (scrolledDistance == 0) return false;
//        
//        if (mClickedView != NULL || (mChildAnimator != NULL && mChildAnimator.isRunning())) return true;
//        
//        if (mScrollAnimator != NULL && mScrollAnimator.isRunning()) mScrollAnimator.cancel();
//        
//        setScrollPosition(mScrollPosition + scrolledDistance);
//        
//        return mScrollPosition == (mDistance - mVisibleDistance) || mScrollPosition == 0;
//    }
//    
//    /**
//     * Calls the item click listener for the child with at the specified
//     * coordinates
//     *
//     * @param x The x-coordinate
//     * @param y The y-coordinate
//     */
//    virtual void clickChildAt(int x, int y) {
//        
//        if (mUpdating || (mChildAnimator != NULL && mChildAnimator.isRunning())) return;
//        
//        if (mScrollAnimator != NULL && mScrollAnimator.isRunning()) mScrollAnimator.cancel();
//        
//        for (int i = getChildCount() - 1; i >= 0; i--) {
//            
//            TimeView view = getTimeChildAt(i);
//            if (view != NULL && view.isTouchingView(x, y)) {
//                mSelectedIndex = mLastViewPosition - i;
//                mSelectedItem = mTimeViewInfos.get(i).item;
//                scrollToPosition(getZPosition(mTimeViewInfos.get(i).date), true);
//                return;
//            }
//        }
//    }
//    
//    virtual void onLayout(bool changed, int l, int t, int r, int b) {
//        BaseScrollView<T>::onLayout(changed, l, t, r, b);
//        
//        if (mUpdating) return;
//        
//        // Setup the eye position so we can calculate our perspective
//        // Also update the background image based on that perspective
//        if (changed || mEyePosition == NULL) {
//            mCenter = new Point(getWidth() / 2, getHeight() / 2);
//            mEyePosition = new int[] {getWidth() / 2, getHeight() / 6, (int) UiUtils.getDynamicPixels(getContext(), 400)};
//            mFocusPoint = new Point(getWidth() * 3 / 10, getHeight() / 2);
//            createBackground();
//        }
//        
//        // if we don't have an adapter, we don't need to do anything
//        if (mAdapter == NULL) {
//            return;
//        }
//        
//        mNowString = String.format("%s", mFormat.print(getCurrentDate()));
//        updateVisibleDateRange();
//        
//        // Update children views by removing non-visible views and adding visible views
//        if (getChildCount() == 0) {
//            if (mScrollPosition == 0) {
//                mLastViewPosition = -1;
//                mFirstViewPosition = 0;
//                addViewsAfterDate(mVisibleStart, mVisibleEnd);
//            } else {
//                mLastViewPosition = mAdapter->getCount() - 1;
//                mFirstViewPosition = mAdapter->getCount();
//                addViewsBeforeDate(mVisibleEnd, mVisibleStart);
//            }
//        } else {
//            removeNonVisibleViews(mVisibleStart, mVisibleEnd);
//            fillList(mVisibleStart, mVisibleEnd);
//        }
//        
//        positionViews();
//        invalidate();
//    }
//    
//    virtual void onDraw(Canvas *canvas) {
//        BaseScrollView<T>::onDraw(canvas);
//        
//        canvas->drawBitmap(mBackground, 0, 0, NULL);
//        
//        const int max = mVisibleDistance;
//        int distance = mFirstYearTick  - mScrollPosition;
//        const int interval = (int) mYearDistance;
//        
//        int count = 0;
//        
//        while (distance <= max) {
//            
//            const int[] pointStart = getProjectedCoordinates(0, getHeight(), distance);
//            const int[] pointEnd = getProjectedCoordinates(getWidth(), getHeight(), distance);
//            
//            const float percent2 = (float) (pointEnd[0] - pointStart[0]) / (float) getWidth();
//            
//            const float percent = ((float) distance % (float) mVisibleDistance) / (float) mVisibleDistance;
//            const int alpha = 255 - (int) (255.0f * percent);
//            
//            mLinePaint.setAlpha(alpha);
//            mBgLinePaint.setAlpha(alpha);
//            mTextPaint.setAlpha(alpha);
//            
//            canvas->drawLine(0, pointStart[1], getWidth(), pointEnd[1], mBgLinePaint);
//            canvas->drawLine(pointStart[0], pointStart[1], pointEnd[0], pointEnd[1], mLinePaint);
//            
//            const String year = "" + (mFirstYearNumber + count);
//            mTextPaint.setTextSize(mFontSize);
//            mTextPaint.getTextBounds(year, 0, year.length(), mTextBounds);
//            
//            const float x = pointStart[0] - mTextBounds.width();
//            const float y = pointStart[1];
//            
//            canvas->save();
//            canvas->scale(percent2, percent2, pointStart[0], y);
//            canvas->drawText(year, x, y, mTextPaint);
//            canvas->restore();
//            
//            distance += interval;
//            count++;
//        }
//    }
//    
//    virtual void dispatchDraw(Canvas *canvas) {
//        BaseScrollView<T>::dispatchDraw(canvas);
//        
//        mDatePaint.getTextBounds(mNowString, 0, mNowString.length(), mTextBounds);
//        canvas->drawText(mNowString, mCenter.x - mTextBounds.exactCenterX(), getHeight() - mPadding, mDatePaint);
//    }
//    
//    virtual bool drawChild(Canvas *canvas, sp<View> child, long drawingTime) {
//        return drawTimeChild(canvas, (TimeView) child, drawingTime);
//    }
//    
//    bool drawTimeChild(Canvas *canvas, const TimeView child, long drawingTime) {
//        
//        const int bubbleX = child.getViewCenterX();
//        const int bubbleY = child.getViewCenterY();
//        const float radius = child.getRadius();
//        
//        const bool deleted = mDeletedViews.contains(child);
//        const bool added = mAddedViews.contains(child);
//        
//        if (child != mClickedView && !deleted && !added) {
//            drawShadow(canvas, bubbleX, bubbleY, radius, child.getViewAlpha());
//            return super.drawChild(canvas, child, drawingTime);
//        }
//        
//        canvas->save();
//        
//        float scale = 1f;
//        
//        if (child == mClickedView) {
//            const float xOffset = (mFocusPoint.x - bubbleX) * mAnimationPercent;
//            const float yOffset = (mFocusPoint.y - bubbleY) * mAnimationPercent;
//            scale = 1.0f + (1.0f - child.getRadiusPercent()) * mAnimationPercent;
//            
//            canvas->translate(xOffset, yOffset);
//        }
//        
//        scale *= deleted ? mDeleteScale : added ? mAddScale : 1f;
//        
//        canvas->scale(scale, scale, bubbleX, bubbleY);
//        drawShadow(canvas, bubbleX, bubbleY, radius, child.getViewAlpha());
//        bool result = super.drawChild(canvas, child, drawingTime);
//        canvas->restore();
//        
//        
//        return result;
//    }
//};
//
//ANDROID_END
//
//#endif /* defined(__MoneyMobileX__TimePerspectiveView__) */
