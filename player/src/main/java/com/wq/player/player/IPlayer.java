package com.wq.player.player;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.net.Uri;

/**
 * Created by qiangwang on 7/22/16.
 */
public interface IPlayer {
    void initMedia(Uri uri, Context context);

    void play();

    void pause();

    void stop();

    void release();

    boolean isPlaying();

    void seekTo(int ms);

    int getTotalTime();

    int getCurrentTime();

    void setListener(PlayManager.Listener listener);

    void prepareMediaSurface(SurfaceTexture surfaceTexture);
}
