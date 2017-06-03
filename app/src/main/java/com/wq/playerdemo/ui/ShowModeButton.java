package com.wq.playerdemo.ui;

import android.content.Context;
import android.util.AttributeSet;
import android.view.View;
import android.widget.Button;

import com.wq.playerdemo.R;

/**
 * Created by qiangwang on 7/22/16.
 */
public class ShowModeButton extends Button implements View.OnClickListener {
    private State mState = State.Normal;
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

    public ShowModeButton(Context context, AttributeSet attrs) {
        super(context, attrs);
        reset();
    }

    private void changeState() {
        switch (mState) {
            case Normal:
                mState = State.Asteroid;
                setBackgroundResource(R.drawable.ic_mode_asteroid);
                break;
            case Asteroid:
                mState = State.Sphere;
                setBackgroundResource(R.drawable.ic_mode_sphere);
                break;
            /*case Sphere:
                mState = State.Other;
                setBackgroundResource(R.drawable.ic_mode_sphere_up);
                break;*/
            default:
                mState = State.Normal;
                setBackgroundResource(R.drawable.ic_mode_normal);
                break;
        }
    }

    public State getState() {
        return mState;
    }

    public void reset() {
        mState = State.Normal;
        setBackgroundResource(R.drawable.ic_mode_normal);
    }

    public enum State {
        Normal,
        Asteroid,
        Sphere,
        Other
    }
}
