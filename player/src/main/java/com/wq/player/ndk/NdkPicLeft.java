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

    public void onDrawFrame(Bitmap bmp, float asp) {
        nativeOnDrawFrame(bmp, asp);
    }

    public void releaseApi() {
        nativeReleaseApi();
    }

    public boolean onTouch(int action, int pointCount, float x1, float y1, float x2, float y2) {
        return nativeOnTouch(action, pointCount, x1, y1, x2, y2);
    }

    public void resetTransform() {
        nativeResetTransform();
    }

    private native int nativeOnSurfaceCreated();

    private native void nativeOnSurfaceChanged(int width, int height);

    private native void nativeOnDrawFrame(Bitmap bmp, float asp);

    private native void nativeInitApi(SettingsBean bean);

    private native void nativeReleaseApi();

    private native void nativeSetSettingsBean(SettingsBean bean);

    private native boolean nativeOnTouch(int action, int pointCount,
                                         float x1, float y1, float x2, float y2);

    private native void nativeResetTransform();

    static {
        System.loadLibrary("player_jni");
    }
}
