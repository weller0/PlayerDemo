package com.wq.playerdemo;

import android.os.Bundle;

public class PlayerVideoActivity extends BasePlayerActivity {
    @Override
    protected void setSource() {
        super.setSource();
        Bundle bundle = getIntent().getExtras();
        String fileName = bundle.getString("file_name");
        mVideoLayout.setVideoSource(fileName, this);
    }
}
