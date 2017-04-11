package com.wq.playerdemo;

import android.os.Handler;
import android.os.Message;

public class PlayerBitmapActivity extends BasePlayerActivity {
    private int fps = 30;
    @Override
    protected void setSource() {
        super.setSource();
        mVideoLayout.setUseBitmap(null);
        ImageUtil.init(this);
    }
    @Override
    protected void onResume() {
        super.onResume();
        refreshFrame();
    }

    @Override
    protected void onPause() {
        super.onPause();
        removeMsg();
    }

    int bmpCount = 0;
    int[] id = {
            R.drawable.img_1,
            R.drawable.img_2,
            R.drawable.img_3,
            R.drawable.img_4,
            R.drawable.img_5,
    };
    private static final int MSG_PAUSE_REFRESH_FRAME = 1;
    Handler mRefreshHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case MSG_PAUSE_REFRESH_FRAME:
                    mVideoLayout.updateFrame(ImageUtil.bitmap[bmpCount]);
                    //mVideoLayout.updateFrame(BitmapFactory.decodeResource(getResources(), id[bmpCount]));
                    bmpCount++;
                    bmpCount = bmpCount % ImageUtil.bitmap.length;
                    refreshFrame();
                    break;
            }
        }
    };

    private void refreshFrame() {
        mRefreshHandler.sendEmptyMessageDelayed(MSG_PAUSE_REFRESH_FRAME, (long) (1000.0 / fps));
    }

    private void removeMsg() {
        mRefreshHandler.removeMessages(MSG_PAUSE_REFRESH_FRAME);
    }
}
