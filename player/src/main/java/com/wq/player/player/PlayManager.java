package com.wq.player.player;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.net.Uri;
import android.view.Surface;

import com.wq.player.L;

import java.io.IOException;

public class PlayManager implements IPlayer, SurfaceTexture.OnFrameAvailableListener{
    private static final String TAG = "VR_PlayManager";
    private VRMediaPlayer mMediaPlayer;

    public PlayManager(){
        mMediaPlayer = new VRMediaPlayer();
    }

    public void initMedia(Uri uri, Context context) {
        L.d(TAG, "initMedia, video uri="+uri);
        try {
            mMediaPlayer.setDataSource(context, uri);
        } catch (IOException e) {
            L.e(TAG, "", e);
        }
    }

    @Override
    public void prepareMediaSurface(SurfaceTexture surfaceTexture) {
        L.d(TAG, "prepareMediaSurface");
        Surface surface = new Surface(surfaceTexture);
        mMediaPlayer.setLooping(true);
        mMediaPlayer.setSurface(surface);
        mMediaPlayer.setOnCompletionListener(new VRMediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion() {
                if (mListener != null) mListener.onCompletion();
            }
        });
        mMediaPlayer.setOnPreparedListener(new VRMediaPlayer.OnPreparedListener() {
            @Override
            public void onPrepared() {
                if (mListener != null) mListener.onPrepareFinish();
            }
        });
        mMediaPlayer.setOnBufferingUpdateListener(new VRMediaPlayer.OnBufferingUpdateListener() {
            @Override
            public void onBufferingUpdate(int percent) {
                if (mListener != null) mListener.updateBufferProgress(percent);
            }
        });
        if (mListener != null) mListener.onPrepareStart();
        surfaceTexture.setOnFrameAvailableListener(this);
        surface.release();
        mMediaPlayer.prepareAsync();
    }

    @Override
    public void release() {
        if (mMediaPlayer != null) {
            mMediaPlayer.reset();
            mMediaPlayer.release();
            mMediaPlayer = null;
        }
    }

    @Override
    public void play() {
        if (!mMediaPlayer.isPlaying()) {
            mMediaPlayer.start();
        }
    }

    @Override
    public void stop() {
        pause();
        if (!mMediaPlayer.isPlaying()) {
            mMediaPlayer.stop();
        }
    }

    @Override
    public void pause() {
        if (mMediaPlayer.isPlaying()) {
            mMediaPlayer.pause();
        }
    }

    @Override
    public boolean isPlaying() {
        return mMediaPlayer.isPlaying();
    }

    @Override
    public void seekTo(int ms) {
        mMediaPlayer.seekTo(ms);
    }

    @Override
    public int getTotalTime() {
        return (int)mMediaPlayer.getDuration();
    }

    @Override
    public int getCurrentTime() {
        return (int)mMediaPlayer.getCurrentPosition();
    }

    @Override
    public void onFrameAvailable(SurfaceTexture mSurfaceTexture) {
        updateProgress();
        if(mListener != null) mListener.onFrameUpdate();
    }

    public float getVideoAsp(){
        return 1.0f * mMediaPlayer.getVideoWidth() / mMediaPlayer.getVideoHeight();
    }

    public interface Listener {
        void updatePlayingProgress(int time);

        void updateBufferProgress(int percent);

        void onBufferingStart();

        void onBufferingFinish();

        void onCompletion();

        void onPrepareFinish();

        void onPrepareStart();

        void onFrameUpdate();
    }

    private Listener mListener;

    @Override
    public void setListener(Listener listener) {
        mListener = listener;
    }

    private static final int UPDATE_CURRENT_TIME = 200;
    private int mCurrTime = 0;
    private void updateProgress() {
        int time = getCurrentTime();
        if (time < UPDATE_CURRENT_TIME + 10) {
            mCurrTime = 0;
        }
        if (time - mCurrTime > UPDATE_CURRENT_TIME) {
            mCurrTime = time;
            if (mListener != null) {
                mListener.updatePlayingProgress(time);
            }
        }
    }
}
