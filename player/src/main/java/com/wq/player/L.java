package com.wq.player;

import android.util.Log;

/**
 * Created by arena on 2017/1/11.
 */

public class L {
    public static final boolean LOG_DEBUG = true;

    public static int d(String tag, String msg) {
        if(!LOG_DEBUG) return -1;
        return Log.d(tag, msg);
    }

    public static int w(String tag, String msg) {
        //if(!LOG_DEBUG) return -1;
        return Log.w(tag, msg);
    }

    public static int e(String tag, String msg) {
        //if(!LOG_DEBUG) return -1;
        return Log.e(tag, msg);
    }

    public static int e(String tag, String msg, Throwable tr) {
        //if(!LOG_DEBUG) return -1;
        return Log.e(tag, msg, tr);
    }
}
