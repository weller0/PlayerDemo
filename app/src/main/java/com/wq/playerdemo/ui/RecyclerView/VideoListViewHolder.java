package com.wq.playerdemo.ui.RecyclerView;

import android.support.v7.widget.RecyclerView;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

public class VideoListViewHolder extends RecyclerView.ViewHolder {
    private ImageView img;
    private TextView text;
    public VideoListViewHolder(View itemView, int[] res) {
        super(itemView);
        img = (ImageView)itemView.findViewById(res[0]);
        text = (TextView)itemView.findViewById(res[1]);

        itemView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(itemClickListener != null){
                    itemClickListener.onClick(v, getPosition());
                }
            }
        });

        itemView.setOnLongClickListener(new View.OnLongClickListener() {
            @Override
            public boolean onLongClick(View v) {
                if(itemLongClickListener != null){
                    itemLongClickListener.onLongClick(v, getPosition());
                }
                return true;
            }
        });
    }

    private OnItemClickListener itemClickListener;
    private OnItemLongClickListener itemLongClickListener;
    public interface OnItemClickListener{
        void onClick(View view, int position);
    }

    public interface OnItemLongClickListener{
        void onLongClick(View view, int position);
    }

    public void setItemClickListener(OnItemClickListener itemClickListener) {
        this.itemClickListener = itemClickListener;
    }

    public void setItemLongClickListener(OnItemLongClickListener itemLongClickListener) {
        this.itemLongClickListener = itemLongClickListener;
    }

    public ImageView getImg() {
        return img;
    }

    public void setImg(ImageView img) {
        this.img = img;
    }

    public TextView getText() {
        return text;
    }

    public void setText(TextView text) {
        this.text = text;
    }
}
