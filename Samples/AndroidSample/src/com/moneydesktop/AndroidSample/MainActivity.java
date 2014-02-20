package com.moneydesktop.AndroidSample;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import org.cocos2dx.lib.Cocos2dxEditText;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;
import org.cocos2dx.lib.Cocos2dxHelper;
import org.cocos2dx.lib.Cocos2dxRenderer;
import org.cocos2dx.lib.MultisampleConfigChooser;

public class MainActivity extends Activity implements Cocos2dxHelper.Cocos2dxHelperListener {

    static final String TAG = "MainActivity";

    private Cocos2dxRenderer mRenderer;
    private Cocos2dxGLSurfaceView mGLView;

    /**
     * Called when the activity is first created.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.main);

        Cocos2dxHelper.init(this, this);

        mRenderer = new Cocos2dxRenderer();

        mGLView = (Cocos2dxGLSurfaceView) findViewById(R.id.cocos2dxView);
        mGLView.setCocos2dxEditText((Cocos2dxEditText) findViewById(R.id.textField));

        if (Build.VERSION.SDK_INT == Build.VERSION_CODES.FROYO) {
            mGLView.setEGLConfigChooser(5, 6, 5, 0, 16, 8);
        } else {
            mGLView.setEGLConfigChooser(new MultisampleConfigChooser());
        }

        mGLView.setCocos2dxRenderer(mRenderer);
        mGLView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    @Override
    public void onResume() {
        super.onResume();

        if (mGLView != null)  {
            mGLView.onResume();
        }
    }

    @Override
    public void onPause() {
        super.onPause();

        if (mGLView != null) {
            mGLView.handleOnPause();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        if (mGLView != null) {
            mGLView.onPause();
        }
    }

    @Override
    public void showDialog(String pTitle, String pMessage) {
    }

    @Override
    public void showEditTextDialog(String pTitle, String pMessage, int pInputMode, int pInputFlag, int pReturnType, int pMaxLength) {
    }

    @Override
    public void runOnGLThread(Runnable pRunnable) {
        this.mGLView.queueEvent(pRunnable);
    }

    static {
        System.loadLibrary("androidsample");
    }
}

