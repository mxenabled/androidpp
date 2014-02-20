//
//  Filter.cpp
//  Androidpp
//
//  Created by Saul Howard on 2/11/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "Filter.h"

#include "Android/text/CharSequence.h"
#include "Android/text/String.h"
#include "Android/utils/Exceptions.h"

#include <mindroid/os/Message.h>

ANDROID_BEGIN

void Filter::RequestHandler::handleMessage(const sp<Message> &msg) {
    int what = msg->what;
    sp<Message> message = NULL;
    switch (what) {
        case FILTER_TOKEN:
        {
            RequestArguments *args = (RequestArguments*) msg->obj;
            try {
                args->results = mFilter->performFiltering(args->constraint);
            } catch (Exception e) {
                args->results = make_shared<FilterResults>();
            }
            
            message = mFilter->mResultHandler->obtainMessage(what);
            message->obj = args;
            message->sendToTarget();
            
            mFilter->mLock.lock();
            if (mFilter->mThreadHandler != NULL) {
                sp<Message> finishMessage = mFilter->mThreadHandler->getHandler()->obtainMessage(FINISH_TOKEN);
                mFilter->mThreadHandler->getHandler()->sendMessageDelayed(finishMessage, 3000);
            }
            mFilter->mLock.unlock();
            break;
        }
        case FINISH_TOKEN:
        {
            mFilter->mLock.lock();
            if (mFilter->mThreadHandler != NULL) {
                mFilter->mThreadHandler->getLooper()->quit();
                mFilter->mThreadHandler = NULL;
            }
            mFilter->mLock.unlock();
            break;
        }
    }
}

void Filter::ResultsHandler::handleMessage(const sp<Message> &msg) {
    RequestArguments *args = (RequestArguments*) msg->obj;
    
    mFilter->publishResults(args->constraint, args->results);
    if (args->listener != NULL) {
        int count = args->results != NULL ? args->results->count : -1;
        args->listener->onFilterComplete(count);
    }
}

const string Filter::THREAD_NAME = "Filter";

Filter::Filter() : mCondVar(mLock) {
    mResultHandler = new ResultsHandler(this);
}

void Filter::filter(shared_ptr<CharSequence> constraint, FilterListener *listener) {
    
    AutoLock autoLock(mLock);
    
    if (mThreadHandler == NULL) {
        mThreadHandler = new LooperThread<RequestHandler>();
        mThreadHandler->getHandler()->mFilter = this;
        mThreadHandler->start();
    }
    
    const uint64_t delay = (mDelayer == NULL) ? 0 : mDelayer->getPostingDelay(constraint);
    
    sp<Message> message = mThreadHandler->getHandler()->obtainMessage(FILTER_TOKEN);
    
    RequestArguments *args = new RequestArguments();
    // make sure we use an immutable copy of the constraint, so that
    // it doesn't change while the filter operation is in progress
    args->constraint = constraint != NULL ? constraint->toString() : NULL;
    args->listener = listener;
    message->obj = (void*) args;
    
    mThreadHandler->getHandler()->removeMessages(FILTER_TOKEN);
    mThreadHandler->getHandler()->removeMessages(FINISH_TOKEN);
    mThreadHandler->getHandler()->sendMessageDelayed(message, delay);
}

shared_ptr<CharSequence> Filter::convertResultToString(shared_ptr<Object> resultValue) {
    return resultValue == NULL ? make_shared<String>() : make_shared<String>(resultValue->toString());
}

ANDROID_END