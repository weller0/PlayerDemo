package com.wq.player.ndk;

public class NdkLicence {
    public String getEncodeA() {
        return nativeGetEncodeA();
    }

    public String getEncodeH(String id) {
        return nativeGetEncodeH(id);
    }

    public boolean isAllow(String hardId, String result1, String result2) {
        return nativeIsAllow(hardId, result1, result2);
    }

    public boolean hasLicence(){
        return nativeHasLicence();
    }

    private native String nativeGetEncodeA();

    private native String nativeGetEncodeH(String id);

    private native boolean nativeIsAllow(String hardId, String result1, String result2);

    private native boolean nativeHasLicence();

    static {
        System.loadLibrary("player_jni");
    }
}
