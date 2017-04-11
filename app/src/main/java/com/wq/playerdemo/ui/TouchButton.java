package com.wq.playerdemo.ui;

import android.content.Context;
import android.util.AttributeSet;
import android.view.View;
import android.widget.Button;

import com.wq.playerdemo.R;

/**
 * Created by qiangwang on 7/22/16.
 */
public class TouchButton extends Button implements View.OnClickListener{
    private State mState = State.Touch;
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

    public TouchButton(Context context, AttributeSet attrs) {
        super(context, attrs);
        reset();
    }

    public void changeState() {
        switch (mState){
            case Sensor:
                mState = State.Touch;
                setBackgroundResource(R.drawable.ic_move_touch);
                break;
            default:
                mState = State.Sensor;
                setBackgroundResource(R.drawable.ic_move_sensor);
                break;
        }
    }

    public State getState(){
        return mState;
    }

    public void reset(){
        mState = State.Touch;
        setBackgroundResource(R.drawable.ic_move_touch);
    }

    public enum State {
        Touch,
        Sensor,
    }
}
