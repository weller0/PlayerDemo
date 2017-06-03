package com.wq.playerdemo.ui;

import android.content.Context;
import android.util.AttributeSet;
import android.view.View;
import android.widget.Button;

import com.wq.playerdemo.R;

/**
 * Created by qiangwang on 7/22/16.
 */
public class ResolutionButton extends Button implements View.OnClickListener {
    private State mState = State.P4K;
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

    public ResolutionButton(Context context, AttributeSet attrs) {
        super(context, attrs);
        reset();
    }

    public void changeState() {
        switch (mState) {
            case P4K:
                mState = State.P720;
                setBackgroundResource(R.drawable.ic_resolution_hd);
                break;
            default:
                mState = State.P4K;
                setBackgroundResource(R.drawable.ic_resolution_2k);
                break;
        }
    }

    public State getState() {
        return mState;
    }

    public void reset() {
        mState = State.P4K;
        setBackgroundResource(R.drawable.ic_resolution_2k);
    }

    public enum State {
        P4K,
        P720
    }
}
