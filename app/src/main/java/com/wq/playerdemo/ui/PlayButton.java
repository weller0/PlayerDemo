package com.wq.playerdemo.ui;

import android.content.Context;
import android.util.AttributeSet;
import android.view.View;
import android.widget.Button;

import com.wq.playerdemo.R;

/**
 * Created by qiangwang on 7/22/16.
 */
public class PlayButton extends Button implements View.OnClickListener{
    private State mState = State.Playing;
    private OnClickListener mOnClickListener;

    @Override
    public void setOnClickListener(OnClickListener listener) {
        super.setOnClickListener(this);
        mOnClickListener = listener;
    }

    @Override
    public void onClick(View v) {
        changeState();
        if (mOnClickListener != null) mOnClickListener.onClick(v);
    }

    public PlayButton(Context context, AttributeSet attrs) {
        super(context, attrs);
        reset();
    }

    public void changeState() {
        switch (mState){
            case Playing:
                mState = State.Pause;
                setBackgroundResource(R.drawable.ic_video_play);
                break;
            default:
                mState = State.Playing;
                setBackgroundResource(R.drawable.ic_video_pause);
                break;
        }
    }

    public State getState(){
        return mState;
    }

    public void reset(){
        mState = State.Playing;
        setBackgroundResource(R.drawable.ic_video_pause);
    }

    public enum State {
        Playing,
        Pause,
    }
}
