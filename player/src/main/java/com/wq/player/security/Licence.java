package com.wq.player.security;

import android.os.AsyncTask;
import android.os.Handler;
import android.os.Looper;

import com.wq.player.L;
import com.wq.player.ndk.NdkLicence;

import org.json.JSONException;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Created by qiangwang on 1/11/17.
 */

public class Licence {
    private static final String TAG = "VR_Licence";
    private static final String PARAMS_HARD_ID = "hardID";
    private static final String PARAMS_ANDROID_ID = "androidID";
    private static final String URL = "http://120.76.122.176/vr-api/check_auth.php";
    private NdkLicence mNdkLicence;
    private String mHardId = "1111111111111111";
    private HttpReturnData mHttpReturnData;
    private QueryTask mQueryTask = null;

    private static Licence mLicence = null;

    public static Licence getInstance(){
        if(mLicence == null){
            mLicence = new Licence();
        }
        return mLicence;
    }

    private Licence() {
        mNdkLicence = new NdkLicence();
    }

    private void resetQuery() {
        mQueryTask = null;
    }

    private String getHardId() {
        return limitByte(mHardId);
    }

    private void setHardId(String hardId) {
        mHardId = hardId;
    }

    public void setLicence(String hardId, Listener listener) {
        setHardId(hardId);
        addListener(listener);
        if (mQueryTask == null) {
            mQueryTask = new QueryTask();
            mQueryTask.execute();
        }
    }

    public void setLicence(Listener listener) {
        mListener = listener;
        if(mNdkLicence.hasLicence()){
            listenerOnSuccess();
        } else {
            listenerOnHardIDError();
        }
        /*if(mHttpReturnData != null) {
            String hardId = mHttpReturnData.getHardId();
            String result1 = mHttpReturnData.getAESHardId();
            String result2 = mHttpReturnData.getAESMcrpty();
            if (hardId != null && result1 != null && result2 != null) {
                if (mNdkLicence.isAllow(getHardId(), result1, result2)) {
                    listenerOnSuccess();
                } else {
                    listenerOnHardIDError();
                }
            } else {
                listenerOnHardIDError();
            }
        } else {
            listenerOnHardIDError();
        }*/
    }

    private String limitByte(String in, int size) {
        String out;
        if (in != null) {
            if (in.length() > size) {
                out = in.substring(0, size);
            } else if (in.length() < size) {
                String tmp = "00000000000000000000000000000000";
                in = in + tmp;
                out = in.substring(0, size);
            } else {
                out = new String(in);
            }
        } else {
            String tmp = "00000000000000000000000000000000";
            out = tmp.substring(0, size);
        }
        return out;
    }

    private String limitByte(String in) {
        return limitByte(in, 16);
    }

    private StringBuffer getRequestData(Map<String, String> params) throws UnsupportedEncodingException {
        StringBuffer stringBuffer = new StringBuffer();        //存储封装好的请求体信息
        for (Map.Entry<String, String> entry : params.entrySet()) {
            stringBuffer.append(entry.getKey())
                    .append("=")
                    .append(URLEncoder.encode(entry.getValue(), "UTF-8"))
                    .append("&");
        }
        stringBuffer.deleteCharAt(stringBuffer.length() - 1);    //删除最后的一个"&"
        return stringBuffer;
    }

    private String streamToString(InputStream is) {
        BufferedReader reader = new BufferedReader(new InputStreamReader(is));
        StringBuilder sb = new StringBuilder();
        String line;
        try {
            while ((line = reader.readLine()) != null) {
                sb.append(line + "\n");
            }
        } catch (IOException e) {
            L.e(TAG, "IOException", e);
            e.printStackTrace();
            listenerOnNetError();
        } finally {
            try {
                is.close();
            } catch (IOException e) {
                L.e(TAG, "IOException", e);
                e.printStackTrace();
            }
        }
        return sb.toString();
    }

    private class QueryTask extends AsyncTask<Void, Void, HttpReturnData> {

        @Override
        protected HttpReturnData doInBackground(Void... params) {
            HttpURLConnection connection = null;
            HttpReturnData mReturnData = null;
            try {
                Map<String, String> postParams = new HashMap<>();
                postParams.put(PARAMS_HARD_ID, mNdkLicence.getEncodeH(getHardId()));
                postParams.put(PARAMS_ANDROID_ID, mNdkLicence.getEncodeA());
                byte[] data = getRequestData(postParams).toString().getBytes();

                URL url = new URL(URL);
                connection = (HttpURLConnection) url.openConnection();
                // 设置输入和输出流
                connection.setDoOutput(true);
                connection.setDoInput(true);
                connection.setRequestMethod("POST");
                connection.setUseCaches(false);
                connection.setRequestProperty("Charset", "UTF-8");
                connection.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
                connection.setRequestProperty("Content-Length", String.valueOf(data.length));
                connection.setConnectTimeout(5000);
                connection.setReadTimeout(10000);
                connection.connect();

                OutputStream outputStream = connection.getOutputStream();
                outputStream.write(data);
                outputStream.flush();
                outputStream.close();

                // 获取返回数据
                if (connection.getResponseCode() == 200) {
                    InputStream is = connection.getInputStream();
                    mReturnData = HttpReturnData.toObject(streamToString(is));
                    is.close();
                } else {
                    L.d(TAG, "getResponseCode=" + connection.getResponseCode());
                    listenerOnNetError();
                }
            } catch (MalformedURLException e) {
                L.e(TAG, "MalformedURLException", e);
                listenerOnNetError();
            } catch (IOException e) {
                L.e(TAG, "IOException", e);
                listenerOnNetError();
            } catch (JSONException e) {
                L.e(TAG, "JSONException", e);
                listenerOnNetError();
            } finally {
                if (connection != null) {
                    connection.disconnect();
                }
            }
            return mReturnData;
        }

        @Override
        protected void onPostExecute(HttpReturnData result) {
            mHttpReturnData = null;
            if (result != null) {
                if (result.getReturnCode() == HttpReturnData.RESULT_CODE_SUCCESS) {
                    mHttpReturnData = result;
                    if (mNdkLicence.isAllow(
                            getHardId(),
                            mHttpReturnData.getAESHardId(),
                            mHttpReturnData.getAESMcrpty())) {
                        mHttpReturnData.setHardId(getHardId());
                        listenerOnSuccess();
                    } else {
                        listenerOnHardIDError();
                    }
                } else {
                    mNdkLicence.isAllow(null, null, null);
                    listenerOnHardIDError();
                }
            }
            resetQuery();
        }
    }

    public interface Listener {
        void onSuccess();

        void onNetError();

        void onHardIDError();
    }

    private List<Listener> mListeners = new ArrayList<>();
    private Listener mListener;

    private void addListener(Listener listener) {
        if(listener != null) mListeners.add(listener);
    }

    public void removeListener(Listener listener) {
        if(listener != null) mListeners.remove(listener);
    }

    public void removeAllListener() {
        mListeners.clear();
    }

    Handler mainHandler = new Handler(Looper.getMainLooper());

    private void listenerOnSuccess() {
        mainHandler.post(new Runnable() {
            @Override
            public void run() {
                if(mListener != null){
                    mListener.onSuccess();
                }
                for (Listener listener : mListeners) {
                    listener.onSuccess();
                }
            }
        });
    }

    private void listenerOnNetError() {
        mainHandler.post(new Runnable() {
            @Override
            public void run() {
                if(mListener != null){
                    mListener.onNetError();
                }
                for (Listener listener : mListeners) {
                    listener.onNetError();
                }
            }
        });
    }

    private void listenerOnHardIDError() {
        mainHandler.post(new Runnable() {
            @Override
            public void run() {
                if(mListener != null){
                    mListener.onHardIDError();
                }
                for (Listener listener : mListeners) {
                    listener.onHardIDError();
                }
            }
        });
    }
}
