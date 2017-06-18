package com.wq.player;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.SurfaceTexture;
import android.net.Uri;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.FrameLayout;
import android.widget.LinearLayout;

import com.wq.player.bean.SettingsBean;
import com.wq.player.ndk.NdkPicLeft;
import com.wq.player.opengl.SensorCtrl;
import com.wq.player.opengl.VideoSurfaceView;
import com.wq.player.player.PlayManager;
import com.wq.player.security.Licence;

/**
 * Created by qiangwang on 1/16/17.
 */

public class VideoLayout extends LinearLayout implements PlayManager.Listener, VideoSurfaceView.TextureIdListener {
    private static final String TAG = "VR_VideoLayout";
    private VideoSurfaceView mLeftSurfaceView = null;
    private FrameLayout mNoLicenceLayout = null;
    private PlayState mPlayState = PlayState.UnPrepare;
    private PlayManager mPlayManager = null;
    private PlayManager.Listener mPlayerListener;
    private boolean isInitSource = false;
    private boolean isPictureMode = false;    // 图片模式、视频模式
    private ShowState mShowState = ShowState.Single;
    private SurfaceTexture mSurfaceTexture;
    private SensorCtrl mSensorCtrl;
    private int fps = 30;

    public VideoLayout(Context context) {
        super(context);
        init();
    }

    public VideoLayout(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    private void init() {
        setPlayState(PlayState.UnPrepare);

        LinearLayout.LayoutParams lp = new LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                LinearLayout.LayoutParams.MATCH_PARENT);
        lp.weight = 1;
        mNoLicenceLayout = new FrameLayout(getContext());
        mNoLicenceLayout.setVisibility(GONE);
        addView(mNoLicenceLayout, lp);

        mSensorCtrl = new SensorCtrl(getContext());
    }

    private void initSurface(boolean isPictureMode) {
        LayoutParams lp = new LayoutParams(
                LayoutParams.MATCH_PARENT,
                LayoutParams.MATCH_PARENT);
        lp.weight = 1;
        mLeftSurfaceView = new VideoSurfaceView(getContext(), isPictureMode, true);
        mLeftSurfaceView.setTextureIdListener(this);
        addView(mLeftSurfaceView, lp);
        mLeftSurfaceView.setListener(null);
        mLeftSurfaceView.setListener(new NdkPicLeft.Listener() {
            @Override
            public void onStart() {
                play();
            }

            @Override
            public void onPause() {
                pause();
            }
        });
    }

    public void setVideoSource(final String path, PlayManager.Listener listener) {
        if (isInitSource) return;

        isInitSource = true;
        mPlayManager = new PlayManager();
        isPictureMode = false;
        initSurface(false);
        removeMsg();
        setPlayState(PlayState.UnPrepare);
        setFps(42);
        mPlayerListener = listener;
        mPlayManager.initMedia(Uri.parse(path), getContext());
        L.d(TAG, "setVideoSource path=" + path);
    }

    public void setUseBitmap(PlayManager.Listener listener) {
        if (isInitSource) return;
        initSurface(true);

        isInitSource = true;
        isPictureMode = true;
        mPlayerListener = listener;
        release();
        setPlayState(PlayState.UnPrepare);
        setFps(30);
        removeMsg();
        L.d(TAG, "setUseBitmap");

        prepare(-1);
        onPrepareStart();
        onPrepareFinish();
    }

    public void updateFrame(Bitmap bmp) {
        if (isPictureMode) {
            mLeftSurfaceView.updateFrame(bmp);
        }
    }

    public void setFps(int fps) {
        if (fps < 20) {
            fps = 20;
        }
        this.fps = fps;
        L.d(TAG, "setFps fps=" + fps);
    }

    private boolean isUsePlayer() {
        return !isPictureMode && mPlayManager != null;
    }

    public void resetTransform() {
        mLeftSurfaceView.resetTransform();
    }

    public void setMode(int sm, int rr, int cs) {
        if (cs == SettingsBean.CS_SENSOR) {
            mSensorCtrl.registerListener();
        } else {
            mSensorCtrl.unRegisterListener();
        }
        mLeftSurfaceView.setMode(sm, rr, cs);
        setShowState(sm);
    }

    private void setShowState(ShowState state) {
        switch (state) {
            case Single:
                setOrientation(HORIZONTAL);
                mLeftSurfaceView.setVisibility(VISIBLE);
                mNoLicenceLayout.setVisibility(GONE);
                break;
            case LeftAndRight:
                setOrientation(HORIZONTAL);
                mLeftSurfaceView.setVisibility(VISIBLE);
                mNoLicenceLayout.setVisibility(GONE);
                break;
            case UpAndDown:
                setOrientation(VERTICAL);
                mLeftSurfaceView.setVisibility(VISIBLE);
                mNoLicenceLayout.setVisibility(GONE);
                break;
            case NoLicence:
                setOrientation(VERTICAL);
                mLeftSurfaceView.setVisibility(GONE);
                mNoLicenceLayout.setVisibility(VISIBLE);
                break;
            default:
                setOrientation(HORIZONTAL);
                mLeftSurfaceView.setVisibility(VISIBLE);
                mNoLicenceLayout.setVisibility(GONE);
                break;
        }
        mShowState = state;
        L.d(TAG, "setShowState : " + mShowState);
    }

    private void setShowState(int sm) {
        if (mShowState != ShowState.NoLicence) {
            setShowState(ShowState.Single);
//            switch (sm) {
//                case SettingsBean.SM_ORIGINAL:
//                case SettingsBean.SM_SPHERE_FRONT:
//                case SettingsBean.SM_SPHERE_UP:
//                case SettingsBean.SM_SPHERE_DOWN:
//                case SettingsBean.SM_PLANE_UP_DOWN:
//                case SettingsBean.SM_CYLINDER_UP_DOWN:
//                    setShowState(ShowState.Single);
//                    break;
//                case SettingsBean.SM_SPHERE_VR:
//                case SettingsBean.SM_SPHERE_FRONT_BACK:
//                    setShowState(ShowState.LeftAndRight);
//                    break;
//
//            }
        }
    }

    private void addListener() {
        if (!isUsePlayer()) return;
        mPlayManager.setListener(this);
    }

    private void removeListener() {
        if (!isUsePlayer()) return;
        mPlayManager.setListener(null);
    }

    public void onResume() {
        L.d(TAG, "onResume");
        addListener();
        mLeftSurfaceView.resume();
        play();
    }

    public void onPause() {
        L.d(TAG, "onPause");
        removeListener();
        mLeftSurfaceView.pause();
        pause();
        //removeMsg();
    }

    public void onDestroy() {
        L.d(TAG, "onDestroy");
        mLeftSurfaceView.destroy();
        removeView(mLeftSurfaceView);
        removeView(mNoLicenceLayout);
        release();
    }

    public FrameLayout getNoLicenceLayout() {
        return mNoLicenceLayout;
    }

    public boolean onTouch(MotionEvent event) {
        return mLeftSurfaceView.onTouch(event);
    }

    private void setPlayState(PlayState state) {
        mPlayState = state;
        L.d(TAG, "setPlayState : " + mPlayState);
    }

    private PlayState getPlayState() {
        return mPlayState;
    }

    @Override
    public void onTextureId(int id) {
        prepare(id);
    }

    @Override
    public void updatePlayingProgress(int time) {
        if (mPlayerListener != null) mPlayerListener.updatePlayingProgress(time);
    }

    @Override
    public void updateBufferProgress(int percent) {
        if (mPlayerListener != null) mPlayerListener.updateBufferProgress(percent);
    }

    @Override
    public void onBufferingStart() {
        if (mPlayerListener != null) mPlayerListener.onBufferingStart();
    }

    @Override
    public void onBufferingFinish() {
        if (mPlayerListener != null) mPlayerListener.onBufferingFinish();
    }

    @Override
    public void onCompletion() {
        L.d(TAG, "PlayManager onCompletion");
        if (mPlayerListener != null) mPlayerListener.onCompletion();
    }

    @Override
    public void onPrepareStart() {
        L.d(TAG, "PlayManager onPrepareStart");
        if (mPlayerListener != null) mPlayerListener.onPrepareStart();
    }

    @Override
    public void onPrepareFinish() {
        L.d(TAG, "PlayManager onPrepareFinish");
        setPlayState(PlayState.Prepared);
        play();
        refreshFrame();
        mFrameCount = 0;
        if (mPlayerListener != null) mPlayerListener.onPrepareFinish();
    }

    int mFrameCount = 0;

    @Override
    public void onFrameUpdate() {
        if (mFrameCount == 2) {
            mFrameCount++;
            mLeftSurfaceView.startPlayAnim();
        } else if (mFrameCount < 2) {
            mFrameCount++;
        }
        mLeftSurfaceView.updateFrameData();
        if (mPlayerListener != null) mPlayerListener.onFrameUpdate();
    }

    private void prepare(int textureId) {
        if (getPlayState() == PlayState.UnPrepare) {
            Licence.getInstance().setLicence(new Licence.Listener() {
                @Override
                public void onSuccess() {
                    L.d(TAG, "Licence is allow !!!");
                }

                @Override
                public void onNetError() {
                    L.d(TAG, "Your net is wrong, please check it !!!");
                    setShowState(ShowState.NoLicence);
                }

                @Override
                public void onHardIDError() {
                    L.d(TAG, "Your licence is wrong, please check it !!!");
                    setShowState(ShowState.NoLicence);
                }
            });
            setPlayState(PlayState.Preparing);
            if (isUsePlayer()) {
                addListener();
                mSurfaceTexture = new SurfaceTexture(textureId);
                mLeftSurfaceView.setSurfaceTexture(mSurfaceTexture);
                mPlayManager.prepareMediaSurface(mSurfaceTexture);
            }
        }
    }

    private void release() {
        if (getPlayState() != PlayState.UnPrepare) {
            setPlayState(PlayState.Release);
            removeMsg();
            if (isUsePlayer()) {
                mPlayManager.setListener(null);
                mPlayManager.stop();
                mPlayManager.release();
                mPlayManager = null;
            }
        }
    }

    public void play() {
        if (getPlayState() == PlayState.Prepared || getPlayState() == PlayState.Pause) {
            setPlayState(PlayState.Playing);
            //removeMsg();
            if (isUsePlayer()) {
                mPlayManager.play();
            }
        }
    }

    public void pause() {
        if (getPlayState() == PlayState.Playing) {
            setPlayState(PlayState.Pause);
            //removeMsg();
            if (isUsePlayer()) {
                mPlayManager.pause();
            }
        }
    }

    public boolean isPlaying() {
        return getPlayState() == PlayState.Playing;
    }

    public void seekTo(int ms) {
        if (!isUsePlayer()) return;
        mPlayManager.seekTo(ms);
    }

    public int getTotalTime() {
        if (!isUsePlayer()) return 0;
        return mPlayManager.getTotalTime();
    }

    public int getCurrentTime() {
        if (!isUsePlayer()) return 0;
        return mPlayManager.getCurrentTime();
    }

    private enum PlayState {
        UnPrepare,
        Preparing,
        Prepared,
        Playing,
        Pause,
        Release,
    }

    private enum ShowState {
        Single,
        LeftAndRight,
        UpAndDown,
        NoLicence
    }

    private static final int MSG_REFRESH_FRAME = 1;
    Handler mRefreshHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case MSG_REFRESH_FRAME:
                    mLeftSurfaceView.requestRender();
                    refreshFrame();
                    break;
            }
        }
    };

    private void refreshFrame() {
        mRefreshHandler.sendEmptyMessageDelayed(MSG_REFRESH_FRAME, (long) (1000.0 / fps));
    }

    private void removeMsg() {
        mRefreshHandler.removeMessages(MSG_REFRESH_FRAME);
    }
}
