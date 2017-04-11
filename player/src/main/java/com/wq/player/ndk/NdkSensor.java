package com.wq.player.ndk;

/**
 * Created by qiangwang on 3/6/17.
 */

public class NdkSensor {
    public void onSensor(float x, float y, float z, long timestamp) {
        nativeOnSensor(x, y, z, timestamp);
    }

    private native void nativeOnSensor(float x, float y, float z, long timestamp);

    static {
        System.loadLibrary("player_jni");
    }
}
