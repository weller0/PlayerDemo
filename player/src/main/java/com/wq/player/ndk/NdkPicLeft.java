package com.wq.player.ndk;

import android.graphics.Bitmap;

import com.wq.player.bean.SettingsBean;

/**
 * Created by qiangwang on 3/6/17.
 */

public class NdkPicLeft {
    public NdkPicLeft(SettingsBean settingsBean) {
        nativeInitApi(settingsBean);
    }

    public void setSettingsBean(SettingsBean bean) {
        nativeSetSettingsBean(bean);
    }

    public int onSurfaceCreated() {
        return nativeOnSurfaceCreated();
    }

    public void onSurfaceChanged(int width, int height) {
        nativeOnSurfaceChanged(width, height);
    }

    public void onDrawFrame(Bitmap bmp, boolean bUpdateFrameData) {
        nativeOnDrawFrame(bmp, bUpdateFrameData);
    }

    public void releaseApi() {
        nativeReleaseApi();
    }

    public void startPlayAnim() {
        nativeStartPlayAnim();
    }

    public boolean onTouch(int action, int pointCount, float x1, float y1, float x2, float y2) {
        return nativeOnTouch(action, pointCount, x1, y1, x2, y2);
    }

    public void resetTransform() {
        nativeResetTransform();
    }

    public interface Listener {
        void onStart();

        void onPause();
    }

    private Listener mListener;

    public void setListener(Listener l) {
        mListener = l;
    }

    public void mpStart() {
        if (mListener != null) mListener.onStart();
    }

    public void mpPause() {
        if (mListener != null) mListener.onPause();
    }

    private native int nativeOnSurfaceCreated();

    private native void nativeOnSurfaceChanged(int width, int height);

    private native void nativeOnDrawFrame(Bitmap bmp, boolean bUpdateFrameData);

    private native void nativeInitApi(SettingsBean bean);

    private native void nativeReleaseApi();

    private native void nativeStartPlayAnim();

    private native void nativeSetSettingsBean(SettingsBean bean);

    private native boolean nativeOnTouch(int action, int pointCount,
                                         float x1, float y1, float x2, float y2);

    private native void nativeResetTransform();

    static {
        System.loadLibrary("player_jni");
    }
}
