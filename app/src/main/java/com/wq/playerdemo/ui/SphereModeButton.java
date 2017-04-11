package com.wq.playerdemo.ui;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.Button;

import com.wq.playerdemo.R;

/**
 * Created by qiangwang on 7/22/16.
 */
public class SphereModeButton extends Button {
    private State mState = State.Front;

    public SphereModeButton(Context context, AttributeSet attrs) {
        super(context, attrs);
        setBackgroundResource(R.drawable.ic_mode_sphere_front);
    }

    public void changeState() {
        switch (mState){
            case Front:
                mState = State.Up;
                setBackgroundResource(R.drawable.ic_mode_sphere_up);
                break;
            case Up:
                mState = State.Down;
                setBackgroundResource(R.drawable.ic_mode_sphere_down);
                break;
            default:
                mState = State.Front;
                setBackgroundResource(R.drawable.ic_mode_sphere_front);
                break;
        }
    }

    public void reset(){
        mState = State.Front;
        setBackgroundResource(R.drawable.ic_mode_sphere_front);
    }

    public State getState(){
        return mState;
    }

    public enum State {
        Front,
        Up,
        Down
    }
}
