package com.wq.player.bean;

import android.util.Log;

/**
 * Created by qiangwang on 3/7/17.
 */

public class SettingsBean {
    private boolean isLeft;
    private boolean isUseBitmap;
    private int mShowMode;
    private int mCtrlStyle;
    private int mResolutionRatio;

    private String mAppPath;

    public String getAppPath() {
        return mAppPath;
    }

    public void setAppPath(String appPath) {
        Log.e("qqqq", "path="+appPath);
        mAppPath = appPath;
    }

    public SettingsBean(){
        isLeft = true;
        mShowMode = SM_ORIGINAL;
        mCtrlStyle = CS_DRAG;
        mResolutionRatio = RR_4K;
    }

    public String toString(){
        return "isLeft="+isLeft+", isUseBitmap="+isUseBitmap+", mShowMode="+mShowMode
                +", mCtrlStyle="+mCtrlStyle+", mResolutionRatio="+mResolutionRatio;
    }

    public boolean isUseBitmap() {
        return isUseBitmap;
    }

    public void setUseBitmap(boolean isUse) {
        isUseBitmap = isUse;
    }

    public boolean isLeft() {
        return isLeft;
    }

    public void setLeft(boolean left) {
        isLeft = left;
    }

    public int getShowMode() {
        return mShowMode;
    }

    public void setShowMode(int mShowMode) {
        this.mShowMode = mShowMode;
    }

    public int getCtrlStyle() {
        return mCtrlStyle;
    }

    public void setCtrlStyle(int mCtrlStyle) {
        this.mCtrlStyle = mCtrlStyle;
    }

    public int getResolutionRatio() {
        return mResolutionRatio;
    }

    public void setResolutionRatio(int mResolutionRatio) {
        this.mResolutionRatio = mResolutionRatio;
    }

    private final static int SM_BASE = -2;
    public final static int SM_NULL = SM_BASE + 1;
    public final static int SM_ORIGINAL = SM_BASE + 2;
    public final static int SM_SPHERE_FRONT = SM_BASE + 3;
    public final static int SM_SPHERE_FRONT_BACK = SM_BASE + 4;
    public final static int SM_SPHERE_UP = SM_BASE + 5;
    public final static int SM_SPHERE_DOWN = SM_BASE + 6;
    public final static int SM_SPHERE_VR = SM_BASE + 7;
    public final static int SM_CYLINDER_UP_DOWN = SM_BASE + 8;
    public final static int SM_PLANE_UP_DOWN = SM_BASE + 9;
    public final static int SM_MAX = SM_BASE + 10;

    private final static int CS_BASE = -2;
    public final static int CS_NULL = CS_BASE + 1;
    public final static int CS_DRAG = CS_BASE + 2;
    public final static int CS_DRAG_ZOOM = CS_BASE + 3;
    public final static int CS_SENSOR = CS_BASE + 4;
    public final static int CS_MAX = CS_BASE + 5;

    private final static int RR_BASE = -2;
    public final static int RR_NULL = RR_BASE + 1;
    public final static int RR_4K = RR_BASE + 2;
    public final static int RR_1080P = RR_BASE + 3;
    public final static int RR_720P = RR_BASE + 4;
    public final static int RR_MAX = RR_BASE + 5;
}
