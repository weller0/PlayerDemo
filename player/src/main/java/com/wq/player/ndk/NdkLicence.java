package com.wq.player.ndk;

public class NdkLicence {
    public String getKeyA() {
        return nativeGetKeyA();
    }

    public String getKeyH(String id) {
        return nativeGetKeyH(id);
    }

    public boolean isAllow(String hardId, String result1, String result2) {
        return nativeIsAllow(hardId, result1, result2);
    }

    private native String nativeGetKeyA();

    private native String nativeGetKeyH(String id);

    private native boolean nativeIsAllow(String hardId, String result1, String result2);

    static {
        System.loadLibrary("player_jni");
    }
}
