//
//  Filter.h
//  Androidpp
//
//  Created by Saul Howard on 2/11/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__Filter__
#define __Androidpp__Filter__

#include "AndroidMacros.h"

#include "Android/utils/Object.h"

#include <mindroid/os/Handler.h>
#include <mindroid/os/LooperThread.h>
#include <mindroid/os/Lock.h>
#include <mindroid/os/CondVar.h>

#include <memory>
#include <string>

using namespace mindroid;
using namespace std;

ANDROID_BEGIN

class CharSequence;

/**
 * <p>A filter constrains data with a filtering pattern.</p>
 *
 * <p>Filters are usually created by {@link android.widget.Filterable}
 * classes.</p>
 *
 * <p>Filtering operations performed by calling {@link #filter(CharSequence)} or
 * {@link #filter(CharSequence, android.widget.Filter.FilterListener)} are
 * performed asynchronously. When these methods are called, a filtering request
 * is posted in a request queue and processed later. Any call to one of these
 * methods will cancel any previous non-executed filtering request.</p>
 *
 * @see android.widget.Filterable
 */
class Filter {
    
public:
    
    /**
     * @hide
     */
    class Delayer {
        
    public:
        
        /**
         * @param constraint The constraint passed to {@link Filter#filter(CharSequence)}
         * @return The delay that should be used for
         *         {@link Handler#sendMessageDelayed(android.os.Message, long)}
         */
        virtual uint64_t getPostingDelay(shared_ptr<CharSequence> constraint) = 0;
    };
    
    /**
     * <p>Listener used to receive a notification upon completion of a filtering
     * operation.</p>
     */
    class FilterListener {
        
    public:
        
        /**
         * <p>Notifies the end of a filtering operation.</p>
         *
         * @param count the number of values computed by the filter
         */
        virtual void onFilterComplete(int count) = 0;
    };
    
protected:
    
    /**
     * <p>Holds the results of a filtering operation. The results are the values
     * computed by the filtering operation and the number of these values.</p>
     */
    class FilterResults : public Object {
    public:
        
        FilterResults() {
            // nothing to see here
        }
        
        /**
         * <p>Contains all the values computed by the filtering operation.</p>
         */
        shared_ptr<Object> values;
        
        /**
         * <p>Contains the number of values computed by the filtering
         * operation.</p>
         */
        int count;
    };
    
private:
    
    /**
     * <p>Worker thread handler. When a new filtering request is posted from
     * {@link android.widget.Filter#filter(CharSequence, android.widget.Filter.FilterListener)},
     * it is sent to this handler.</p>
     */
    class RequestHandler : public Handler {
    
    public:
        
        Filter *mFilter = NULL;
        
        /**
         * <p>Handles filtering requests by calling
         * {@link Filter#performFiltering} and then sending a message
         * with the results to the results handler.</p>
         *
         * @param msg the filtering request
         */
        void handleMessage(const sp<Message> &message);
    };
    
    /**
     * <p>Handles the results of a filtering operation. The results are
     * handled in the UI thread.</p>
     */
    class ResultsHandler : public Handler {
        
    private:
        
        Filter *mFilter = NULL;
        
    public:
        
        ResultsHandler(Filter *filter) : Handler() {
            mFilter = filter;
        }
        
        /**
         * <p>Messages received from the request handler are processed in the
         * UI thread. The processing involves calling
         * {@link Filter#publishResults(CharSequence,
         * android.widget.Filter.FilterResults)}
         * to post the results back in the UI and then notifying the listener,
         * if any.</p>
         *
         * @param msg the filtering results
         */
        void handleMessage(const sp<Message> &message);
    };
    
    /**
     * <p>Holds the arguments of a filtering request as well as the results
     * of the request.</p>
     */
    class RequestArguments {
        
    public:
        
        /**
         * <p>The constraint used to filter the data.</p>
         */
        shared_ptr<CharSequence> constraint;
        
        /**
         * <p>The listener to notify upon completion. Can be NULL.</p>
         */
        FilterListener *listener = NULL;
        
        /**
         * <p>The results of the filtering operation.</p>
         */
        shared_ptr<FilterResults> results;
    };
    
private:
    
    static const string THREAD_NAME;
    static const int FILTER_TOKEN = 0xD0D0F00D;
    static const int FINISH_TOKEN = 0xDEADBEEF;
    
    sp<LooperThread<RequestHandler>> mThreadHandler = NULL;
    sp<Handler> mResultHandler = NULL;
    
    shared_ptr<Delayer> mDelayer;
    
	Lock mLock;
	CondVar mCondVar;
    
public:
    
    /**
     * <p>Creates a new asynchronous filter.</p>
     */
    Filter();
    
    /**
     * Provide an interface that decides how long to delay the message for a given query.  Useful
     * for heuristics such as posting a delay for the delete key to avoid doing any work while the
     * user holds down the delete key.
     *
     * @param delayer The delayer.
     * @hide
     */
    void setDelayer(shared_ptr<Delayer> delayer) {
        AutoLock autoLock(mLock);
        mDelayer = delayer;
    }
    
    /**
     * <p>Starts an asynchronous filtering operation. Calling this method
     * cancels all previous non-executed filtering requests and posts a new
     * filtering request that will be executed later.</p>
     *
     * @param constraint the constraint used to filter the data
     *
     * @see #filter(CharSequence, android.widget.Filter.FilterListener)
     */
    void filter(shared_ptr<CharSequence> constraint) {
        filter(constraint, NULL);
    }
    
    /**
     * <p>Starts an asynchronous filtering operation. Calling this method
     * cancels all previous non-executed filtering requests and posts a new
     * filtering request that will be executed later.</p>
     *
     * <p>Upon completion, the listener is notified.</p>
     *
     * @param constraint the constraint used to filter the data
     * @param listener a listener notified upon completion of the operation
     *
     * @see #filter(CharSequence)
     * @see #performFiltering(CharSequence)
     * @see #publishResults(CharSequence, android.widget.Filter.FilterResults)
     */
    void filter(shared_ptr<CharSequence> constraint, FilterListener *listener);
    
protected:
    
    /**
     * <p>Invoked in a worker thread to filter the data according to the
     * constraint. Subclasses must implement this method to perform the
     * filtering operation. Results computed by the filtering operation
     * must be returned as a {@link android.widget.Filter.FilterResults} that
     * will then be published in the UI thread through
     * {@link #publishResults(CharSequence,
     * android.widget.Filter.FilterResults)}.</p>
     *
     * <p><strong>Contract:</strong> When the constraint is NULL, the original
     * data must be restored.</p>
     *
     * @param constraint the constraint used to filter the data
     * @return the results of the filtering operation
     *
     * @see #filter(CharSequence, android.widget.Filter.FilterListener)
     * @see #publishResults(CharSequence, android.widget.Filter.FilterResults)
     * @see android.widget.Filter.FilterResults
     */
    virtual shared_ptr<FilterResults> performFiltering(shared_ptr<CharSequence> constraint) = 0;
    
    /**
     * <p>Invoked in the UI thread to publish the filtering results in the
     * user interface. Subclasses must implement this method to display the
     * results computed in {@link #performFiltering}.</p>
     *
     * @param constraint the constraint used to filter the data
     * @param results the results of the filtering operation
     *
     * @see #filter(CharSequence, android.widget.Filter.FilterListener)
     * @see #performFiltering(CharSequence)
     * @see android.widget.Filter.FilterResults
     */
    virtual void publishResults(shared_ptr<CharSequence> constraint, shared_ptr<FilterResults> results) = 0;
    
public:
    
    /**
     * <p>Converts a value from the filtered set into a CharSequence. Subclasses
     * should override this method to convert their results. The default
     * implementation returns an empty String for NULL values or the default
     * String representation of the value.</p>
     *
     * @param resultValue the value to convert to a CharSequence
     * @return a CharSequence representing the value
     */
    shared_ptr<CharSequence> convertResultToString(shared_ptr<Object> resultValue);
};

ANDROID_END

#endif /* defined(__Androidpp__Filter__) */
