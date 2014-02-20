Android++
=========

**Based off Android 4.3 r1**

A mobile cross platform framework based on Android and written in C++.  This is still very much in the alpha state so not every feature you would find in Android will be working here.  The sample projects show general project setup and demo most of the features we are currently supporting.  The sample projects are setup in such a way that we are maximizing code reuse.  For example, all of the C++ code for the Android project is referencing code in the iOS project.  Therefore you really only need to write your code in one place, the only caveat is all resources (images, layout files, etc.) must be duplicated on a per platform basis.

#Getting Started
This will help you get things up and running on the platforms we currently support.

##iOS
You will first need to build the Android++ static library by opening the Androidpp xcode project.

_Note_: You may have to remove the static libraries (libcurl.a, libstatic-libpng.a, libwebp.a, libwebsockets.a) and add them back in so they are referenced correctly for your local environment.

Once the static library is built you can add it to the sample iOS project found in the Samples directory.  From there you should be able to build and run the sample project.

_Note_: Currently you can only build for devices as we haven't built complete support for the simulator architecture

##Android
If you open a terminal window and move to the android sample project home directory (/Androidpp/Samples/AndroidSample) you can compile all the source code for both the framework and sample project by executing the build_native.sh script (./build_native.sh).  Once that code is compiled you should be able to open the Android project (in IntelliJ) and run it on a device or the simulator.

When creating your own project I would suggest setting it up in the same way we have it here as the code in the iOS sample project is shared with the Android project.  The only thing that is really duplicated is resource files (everything in the res directory)
