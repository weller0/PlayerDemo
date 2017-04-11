package com.wq.player.player;

import tv.danmaku.ijk.media.player.IMediaPlayer;
import tv.danmaku.ijk.media.player.IjkMediaPlayer;

/**
 * Created by arena on 2017/2/23.
 */

public class VRMediaPlayer extends IjkMediaPlayer {

    public VRMediaPlayer(){
        super();

        native_setLogLevel(IjkMediaPlayer.IJK_LOG_WARN);

        setOption(IjkMediaPlayer.OPT_CATEGORY_PLAYER, "mediacodec", 1);
        //setOption(IjkMediaPlayer.OPT_CATEGORY_PLAYER, "mediacodec", 0);
        setOption(IjkMediaPlayer.OPT_CATEGORY_PLAYER, "mediacodec-auto-rotate", 1);
        //setOption(IjkMediaPlayer.OPT_CATEGORY_PLAYER, "mediacodec-auto-rotate", 0);
        setOption(IjkMediaPlayer.OPT_CATEGORY_PLAYER, "mediacodec-handle-resolution-change", 1);
        //setOption(IjkMediaPlayer.OPT_CATEGORY_PLAYER, "mediacodec-handle-resolution-change", 0);

        setOption(IjkMediaPlayer.OPT_CATEGORY_PLAYER, "opensles", 1);
        //setOption(IjkMediaPlayer.OPT_CATEGORY_PLAYER, "opensles", 0);

        setOption(IjkMediaPlayer.OPT_CATEGORY_PLAYER, "overlay-format", IjkMediaPlayer.SDL_FCC_RV32);
        setOption(IjkMediaPlayer.OPT_CATEGORY_PLAYER, "framedrop", 1);
        setOption(IjkMediaPlayer.OPT_CATEGORY_PLAYER, "start-on-prepared", 0);

        setOption(IjkMediaPlayer.OPT_CATEGORY_FORMAT, "http-detect-range-support", 0);

        setOption(IjkMediaPlayer.OPT_CATEGORY_CODEC, "skip_loop_filter", 48);
    }
    
    private OnPreparedListener mOnPreparedListener;
    private OnCompletionListener mOnCompletionListener;
    private OnBufferingUpdateListener mOnBufferingUpdateListener;
    public interface OnPreparedListener {
        void onPrepared();
    }
    public interface OnCompletionListener {
        void onCompletion();
    }

    public interface OnBufferingUpdateListener {
        void onBufferingUpdate(int percent);
    }

    public void setOnBufferingUpdateListener(OnBufferingUpdateListener listener) {
        mOnBufferingUpdateListener = listener;
        super.setOnBufferingUpdateListener(new IMediaPlayer.OnBufferingUpdateListener() {
            @Override
            public void onBufferingUpdate(IMediaPlayer mp, int percent) {
                if(mOnBufferingUpdateListener != null) mOnBufferingUpdateListener.onBufferingUpdate(percent);
            }
        });
    }

    public void setOnCompletionListener(OnCompletionListener listener) {
        mOnCompletionListener = listener;
        super.setOnCompletionListener(new IMediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion(IMediaPlayer mp) {
                if(mOnCompletionListener != null) mOnCompletionListener.onCompletion();
            }
        });
    }

    public void setOnPreparedListener(OnPreparedListener listener) {
        mOnPreparedListener = listener;
        super.setOnPreparedListener(new IjkMediaPlayer.OnPreparedListener() {
            @Override
            public void onPrepared(IMediaPlayer mp) {
                if(mOnPreparedListener != null) mOnPreparedListener.onPrepared();
            }
        });
    }
}
