//
//  AppDelegate.h
//  AndroidppSample
//
//  Created by Saul Howard on 1/10/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#import <UIKit/UIKit.h>

@class RootViewController;

@interface AppDelegate : NSObject <UIAccelerometerDelegate, UIAlertViewDelegate, UITextFieldDelegate,UIApplicationDelegate> {
    UIWindow *window;
    RootViewController    *viewController;
}

@property (strong, nonatomic) UIWindow *window;
@property (nonatomic, retain) RootViewController *viewController;

@end
