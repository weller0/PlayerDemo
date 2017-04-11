package com.wq.player.opengl;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;

import com.wq.player.L;
import com.wq.player.ndk.NdkSensor;

public class SensorCtrl implements SensorEventListener {
    private static String TAG = "VR_SensorCtrl";
    private SensorManager mSensorManager;
    private Sensor mSensor;
    private NdkSensor mNdkSensor;

    public SensorCtrl(Context context){
        mSensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
        mSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);
        mNdkSensor = new NdkSensor();
    }

    public void registerListener() {
        mSensorManager.registerListener(this, mSensor, SensorManager.SENSOR_DELAY_GAME);
    }

    public void unRegisterListener() {
        mSensorManager.unregisterListener(this, mSensor);
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        switch (event.sensor.getType()) {
            case Sensor.TYPE_GYROSCOPE:
                L.d(TAG, "gyroscope("+event.values[0]+", "+event.values[1]+", "+event.values[2]+").");
                mNdkSensor.onSensor(event.values[0], event.values[1], event.values[2], event.timestamp);
                break;
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
    }
}
