package com.wq.player.security;

import com.wq.player.L;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

/**
 * Created by qiangwang on 1/11/17.
 */

public class HttpReturnData {
    private static final String TAG = "VR_HttpReturnData";
    private static final String RESULT_CODE = "result";
    private static final String RESULT_MSG = "msg";
    private static final String RESULT_CONTENT = "content";
    private static final String RESULT_CONTENT_HARD_ID = "hardID";
    private static final String RESULT_CONTENT_MCRPTY = "mcrpty";
    public static final int RESULT_CODE_SUCCESS = 0;
    public static final int RESULT_CODE_PARAMS_ERROR = 100;
    public static final int RESULT_CODE_HARD_ID_ERROR = 101;

    private int mReturnCode = -1;
    private String mReturnMsg = null;
    private String mAESHardId = null;
    private String mAESMcrpty = null;

    public String getHardId() {
        return mHardId;
    }

    public void setHardId(String mHardId) {
        this.mHardId = mHardId;
    }

    private String mHardId = null;

    public int getReturnCode() {
        return mReturnCode;
    }

    public void setReturnCode(int mReturnCode) {
        this.mReturnCode = mReturnCode;
    }

    public String getAESHardId() {
        return mAESHardId;
    }

    public void setAESHardId(String id) {
        this.mAESHardId = id;
    }

    public String getAESMcrpty() {
        return mAESMcrpty;
    }

    public void setAESMcrpty(String mcrpty) {
        this.mAESMcrpty = mcrpty;
    }

    public String getReturnMsg() {
        return mReturnMsg;
    }

    public void setReturnMsg(String mReturnMsg) {
        this.mReturnMsg = mReturnMsg;
    }

    public static HttpReturnData toObject(String data) throws JSONException {
        int index = 0;
        byte[] dataBytes = data.getBytes();
        for(int i=0; i<dataBytes.length; i++){
            if(dataBytes[i] == '{' && dataBytes[i+1] == '\"' && dataBytes[i+2] == 'r' &&
                    dataBytes[i+3] == 'e' && dataBytes[i+4] == 's' && dataBytes[i+5] == 'u'){
                index = i;
                break;
            }
        }
        data = data.substring(index);
        L.d(TAG, "toObject json="+data);
        HttpReturnData mReturnData = new HttpReturnData();
        JSONObject jsonRoot = new JSONObject(data);
        mReturnData.setReturnCode(jsonRoot.getInt(RESULT_CODE));
        mReturnData.setReturnMsg(jsonRoot.getString(RESULT_MSG));
        if(mReturnData.getReturnCode() == RESULT_CODE_SUCCESS){
            JSONArray jsonContentArray = jsonRoot.getJSONArray(RESULT_CONTENT);
            JSONObject jsonContent = jsonContentArray.getJSONObject(0);
            mReturnData.setAESHardId(jsonContent.getString(RESULT_CONTENT_HARD_ID));
            mReturnData.setAESMcrpty(jsonContent.getString(RESULT_CONTENT_MCRPTY));
        }

        return mReturnData;
    }
}
