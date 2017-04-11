package com.wq.playerdemo.ui.RecyclerView;

import android.graphics.Bitmap;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.ImageView;

import com.wq.playerdemo.R;
import com.wq.playerdemo.ui.imagemanager.ImageManager;

import java.util.ArrayList;

public class VideoListAdapter extends RecyclerView.Adapter{
    private ArrayList<Item> items;
    private VideoListViewHolder.OnItemClickListener itemClickListener;
    private VideoListViewHolder.OnItemLongClickListener itemLongClickListener;
    private int mWidth = 180;

    public VideoListAdapter(ArrayList<Item> items, int width){
        this.items = items;
        mWidth = width;
    }

    public void setData(ArrayList<Item> items){
        this.items = items;
        notifyDataSetChanged();
    }

    @Override
    public VideoListViewHolder onCreateViewHolder(ViewGroup viewGroup, int i) {
        View itemView = LayoutInflater.from(viewGroup.getContext()).
                inflate(R.layout.video_item, viewGroup, false);
        int[] res = {R.id.video_item_image, R.id.video_item_name};
        VideoListViewHolder viewHold = new VideoListViewHolder(itemView, res);
        viewHold.setItemClickListener(itemClickListener);
        viewHold.setItemLongClickListener(itemLongClickListener);
        FrameLayout root = (FrameLayout) itemView.findViewById(R.id.video_item_root);
        FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(mWidth, mWidth);
        root.setLayoutParams(lp);
        int padding = (int)(1.0f * mWidth / 20.0f);
        root.setPadding(padding, padding, padding, padding);
        return viewHold;
    }

    @Override
    public void onBindViewHolder(RecyclerView.ViewHolder viewHolder, int i) {
        if(items != null){
            ((VideoListViewHolder)viewHolder).getText().setText(items.get(i).getName());
            ((VideoListViewHolder)viewHolder).getImg().setImageResource(R.mipmap.ic_launcher);
            ImageManager.getInstance().getBitmap(
                    items.get(i).getURL(),
                    ((VideoListViewHolder) viewHolder).getImg(),
                    0, 0,
                    new ImageManager.OnImageListener() {
                        @Override
                        public void onImageDownload(Bitmap bmp, ImageView view, String url, boolean isTran) {
                            ImageManager.getInstance().setImageBitmap(view, bmp, isTran);
                        }
                    }
            );
        }
    }

    @Override
    public int getItemCount() {
        if(items != null){
            return items.size();
        }
        return 0;
    }



    public void setOnItemClickListener(VideoListViewHolder.OnItemClickListener itemClickListener) {
        this.itemClickListener = itemClickListener;
    }

    public void setOnItemLongClickListener(VideoListViewHolder.OnItemLongClickListener itemLongClickListener) {
        this.itemLongClickListener = itemLongClickListener;
    }
}
