//
//  TextDirectionHeuristics->cpp
//  Androidpp
//
//  Created by Saul Howard on 1/20/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "TextDirectionHeuristics.h"

#include "Android/text/Character.h"
#include "Android/text/CharSequence.h"
#include "Android/text/String.h"
#include "Android/utils/Exceptions.h"

ANDROID_BEGIN

int TextDirectionHeuristics::isRtlText(int directionality) {
    switch (directionality) {
        case Character::DIRECTIONALITY_LEFT_TO_RIGHT:
            return STATE_FALSE;
        case Character::DIRECTIONALITY_RIGHT_TO_LEFT:
        case Character::DIRECTIONALITY_RIGHT_TO_LEFT_ARABIC:
            return STATE_TRUE;
        default:
            return STATE_UNKNOWN;
    }
}

int TextDirectionHeuristics::isRtlTextOrFormat(int directionality) {
    switch (directionality) {
        case Character::DIRECTIONALITY_LEFT_TO_RIGHT:
        case Character::DIRECTIONALITY_LEFT_TO_RIGHT_EMBEDDING:
        case Character::DIRECTIONALITY_LEFT_TO_RIGHT_OVERRIDE:
            return STATE_FALSE;
        case Character::DIRECTIONALITY_RIGHT_TO_LEFT:
        case Character::DIRECTIONALITY_RIGHT_TO_LEFT_ARABIC:
        case Character::DIRECTIONALITY_RIGHT_TO_LEFT_EMBEDDING:
        case Character::DIRECTIONALITY_RIGHT_TO_LEFT_OVERRIDE:
            return STATE_TRUE;
        default:
            return STATE_UNKNOWN;
    }
}

bool TextDirectionHeuristics::TextDirectionHeuristicImpl::isRtl(UnicodeString array, int start, int count) {
    return isRtl(make_shared<String>(array), start, count);
}

bool TextDirectionHeuristics::TextDirectionHeuristicImpl::isRtl(shared_ptr<CharSequence> cs, int start, int count) {
    if (cs == NULL || start < 0 || count < 0 || cs->length() - count < start) {
        throw IllegalArgumentException();
    }
    if (mAlgorithm == NULL) {
        return defaultIsRtl();
    }
    return doCheck(cs, start, count);
}

bool TextDirectionHeuristics::TextDirectionHeuristicImpl::doCheck(shared_ptr<CharSequence> cs, int start, int count) {
    switch(mAlgorithm->checkRtl(cs, start, count)) {
        case STATE_TRUE:
            return true;
        case STATE_FALSE:
            return false;
        default:
            return defaultIsRtl();
    }
}

TextDirectionHeuristics::TextDirectionHeuristicInternal::TextDirectionHeuristicInternal(bool defaultIsRtl, shared_ptr<TextDirectionAlgorithm> algorithm = NULL) :
TextDirectionHeuristicImpl(algorithm), mDefaultIsRtl(defaultIsRtl) {
}

int TextDirectionHeuristics::FirstStrong::checkRtl(shared_ptr<CharSequence> cs, int start, int count) {
    int result = STATE_UNKNOWN;
//    for (int i = start, e = start + count; i < e && result == STATE_UNKNOWN; ++i) {
//        result = isRtlTextOrFormat(Character::getDirectionality(cs->charAt(i)));
//    }
    return result;
}

shared_ptr<TextDirectionHeuristics::FirstStrong> TextDirectionHeuristics::FirstStrong::INSTANCE = make_shared<FirstStrong>();

int TextDirectionHeuristics::AnyStrong::checkRtl(shared_ptr<CharSequence> cs, int start, int count) {
    bool haveUnlookedFor = false;
    int result = STATE_TRUE;
    for (int i = start, e = start + count; i < e; ++i) {
//        result = isRtlText(Character::getDirectionality(cs->charAt(i)));
        switch (result) {
            case STATE_TRUE:
                if (mLookForRtl) {
                    return STATE_TRUE;
                }
                haveUnlookedFor = true;
                break;
            case STATE_FALSE:
                if (!mLookForRtl) {
                    return STATE_FALSE;
                }
                haveUnlookedFor = true;
                break;
            default:
                break;
        }
    }
    if (haveUnlookedFor) {
        return mLookForRtl ? STATE_FALSE : STATE_TRUE;
    }
    return STATE_UNKNOWN;
}

shared_ptr<TextDirectionHeuristics::AnyStrong> TextDirectionHeuristics::AnyStrong::INSTANCE_RTL = make_shared<AnyStrong>(true);
shared_ptr<TextDirectionHeuristics::AnyStrong> TextDirectionHeuristics::AnyStrong::INSTANCE_LTR = make_shared<AnyStrong>(false);

shared_ptr<TextDirectionHeuristics::TextDirectionHeuristicLocale> TextDirectionHeuristics::TextDirectionHeuristicLocale::INSTANCE = make_shared<TextDirectionHeuristicLocale>();

shared_ptr<TextDirectionHeuristic> TextDirectionHeuristics::LTR = make_shared<TextDirectionHeuristicInternal>(false);
shared_ptr<TextDirectionHeuristic> TextDirectionHeuristics::RTL = make_shared<TextDirectionHeuristicInternal>(true);
shared_ptr<TextDirectionHeuristic> TextDirectionHeuristics::FIRSTSTRONG_LTR = make_shared<TextDirectionHeuristicInternal>(false, TextDirectionHeuristics::FirstStrong::INSTANCE);
shared_ptr<TextDirectionHeuristic> TextDirectionHeuristics::FIRSTSTRONG_RTL = make_shared<TextDirectionHeuristicInternal>(true, TextDirectionHeuristics::FirstStrong::INSTANCE);
shared_ptr<TextDirectionHeuristic> TextDirectionHeuristics::ANYRTL_LTR = make_shared<TextDirectionHeuristicInternal>(false, TextDirectionHeuristics::AnyStrong::INSTANCE_RTL);
shared_ptr<TextDirectionHeuristic> TextDirectionHeuristics::LOCALE = TextDirectionHeuristics::TextDirectionHeuristicLocale::INSTANCE;

ANDROID_END