package com.wq.playerdemo.ui.RecyclerView;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.support.v7.widget.RecyclerView;
import android.view.View;

/**
 * Created by qiangwang on 5/18/15.
 * RecyclerView的Item的装饰,这边仅仅添加了Item间的间隔图片
 */
public class DividerDecoration extends RecyclerView.ItemDecoration{
    private Drawable mDivider;

    public DividerDecoration(Context context) {
        // 获取间隔图片(系统的)
        TypedArray a = context.obtainStyledAttributes(new int[]{android.R.attr.listDivider});
        mDivider = a.getDrawable(0);
        a.recycle();
    }

    @Override
    public void onDrawOver(Canvas c, RecyclerView parent, RecyclerView.State state) {
        //super.onDrawOver(c, parent, state);
        // 设置间隔图片的位置
        // 这里是在每个Item的最下边
        int left = parent.getPaddingLeft();
        int right = parent.getWidth() - parent.getPaddingRight();
        int childCount = parent.getChildCount();
        for(int i=0;i < childCount;i++){
            View child = parent.getChildAt(i);
            RecyclerView.LayoutParams layoutParams = (RecyclerView.LayoutParams)child.getLayoutParams();
            int top = child.getTop() + layoutParams.bottomMargin;
            int bottom = top + mDivider.getIntrinsicHeight();
            mDivider.setBounds(left, top, right, bottom);
            mDivider.draw(c);
        }
    }

    @Override
    public void getItemOffsets(Rect outRect, View view, RecyclerView parent, RecyclerView.State state) {
        //super.getItemOffsets(outRect, view, parent, state);
        outRect.set(0, 0, 0, mDivider.getIntrinsicHeight());
    }
}
