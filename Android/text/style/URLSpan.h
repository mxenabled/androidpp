//
//  URLSpan.h
//  Androidpp
//
//  Created by Saul Howard on 1/22/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_URLSpan_h
#define Androidpp_URLSpan_h

#include "AndroidMacros.h"

#include "Android/content/Context.h"
#include "Android/text/style/ClickableSpan.h"
#include "Android/view/View.h"

#include <string>

using namespace std;

ANDROID_BEGIN

class URLSpan : public ClickableSpan{
    
private:
    
    string mURL;
    
public:
    
    URLSpan(string url) {
        mURL = url;
    }
    
//    public URLSpan(Parcel src) {
//        mURL = src.readString();
//    }
//    
//    public int getSpanTypeId() {
//        return TextUtils.URL_SPAN;
//    }
//    
//    public int describeContents() {
//        return 0;
//    }
//    
//    public void writeToParcel(Parcel dest, int flags) {
//        dest.writeString(mURL);
//    }
    
    string getURL() {
        return mURL;
    }
    
    void onClick(View *widget) {
//        Uri uri = Uri.parse(getURL());
//        Context *context = widget->getContext();
//        Intent intent = new Intent(Intent.ACTION_VIEW, uri);
//        intent.putExtra(Browser.EXTRA_APPLICATION_ID, context.getPackageName());
//        context.startActivity(intent);
    }
};

ANDROID_END

#endif
