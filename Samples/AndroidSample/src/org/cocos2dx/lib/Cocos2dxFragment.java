package org.cocos2dx.lib;

import android.app.Fragment;
import android.content.Context;
import android.os.Bundle;
import android.os.Message;

/**
 * MoneyDesktop - MoneyMobile
 * <p/>
 * User: saulhoward
 * Date: 6/7/13
 * <p/>
 * Description:
 */
public class Cocos2dxFragment extends Fragment implements Cocos2dxHelper.Cocos2dxHelperListener {

    private Cocos2dxHandler mHandler;
    private static Context sContext = null;
    private Cocos2dxGLSurfaceView mGLView;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        sContext = getActivity().getBaseContext();
        this.mHandler = new Cocos2dxHandler(getActivity());

        Cocos2dxHelper.init(getActivity(), this);
    }

    @Override
    public void showDialog(String pTitle, String pMessage) {
        Message msg = new Message();
        msg.what = Cocos2dxHandler.HANDLER_SHOW_DIALOG;
        msg.obj = new Cocos2dxHandler.DialogMessage(pTitle, pMessage);
        this.mHandler.sendMessage(msg);
    }

    @Override
    public void showEditTextDialog(String pTitle, String pMessage, int pInputMode, int pInputFlag, int pReturnType, int pMaxLength) {
        Message msg = new Message();
        msg.what = Cocos2dxHandler.HANDLER_SHOW_EDITBOX_DIALOG;
        msg.obj = new Cocos2dxHandler.EditBoxMessage(pTitle, pMessage, pInputMode, pInputFlag, pReturnType, pMaxLength);
        this.mHandler.sendMessage(msg);
    }

    @Override
    public void runOnGLThread(Runnable pRunnable) {
        this.mGLView.queueEvent(pRunnable);
    }
}
