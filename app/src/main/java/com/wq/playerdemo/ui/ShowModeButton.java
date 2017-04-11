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
    private State mState = State.Original;
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
            case Original:
                mState = State.Front;
                setBackgroundResource(R.drawable.ic_mode_sphere_front);
                break;
            case Front:
                mState = State.FrontBack;
                setBackgroundResource(R.drawable.ic_mode_sphere_front_back);
                break;
            case FrontBack:
                mState = State.Up;
                setBackgroundResource(R.drawable.ic_mode_sphere_up);
                break;
            /*case Up:
                mState = State.Down;
                setBackgroundResource(R.drawable.ic_mode_sphere_down);
                break;
            case Down:
                mState = State.VR;
                setBackgroundResource(R.drawable.ic_mode_vr);
                break;
            case VR:
                mState = State.Plane;
                setBackgroundResource(R.drawable.ic_mode_plane);
                break;*/
            default:
                mState = State.Original;
                setBackgroundResource(R.drawable.ic_mode_original);
                break;
        }
    }

    public State getState() {
        return mState;
    }

    public void reset() {
        mState = State.Original;
        setBackgroundResource(R.drawable.ic_mode_original);
    }

    public enum State {
        Original,
        Front,
        FrontBack,
        Up,
        Down,
        VR,
        Plane
    }
}
