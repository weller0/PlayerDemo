package com.wq.playerdemo;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

/**
 * Created by arena on 2017/2/14.
 */

public class ImageUtil {
    public static Bitmap[] bitmap = new Bitmap[5];
    public static void init(Context context){
        bitmap[0] = BitmapFactory.decodeResource(context.getResources(), R.drawable.img_1);
        bitmap[1] = BitmapFactory.decodeResource(context.getResources(), R.drawable.img_2);
        bitmap[2] = BitmapFactory.decodeResource(context.getResources(), R.drawable.img_3);
        bitmap[3] = BitmapFactory.decodeResource(context.getResources(), R.drawable.img_4);
        bitmap[4] = BitmapFactory.decodeResource(context.getResources(), R.drawable.img_5);
    }
}
