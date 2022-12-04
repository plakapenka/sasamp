package com.liverussia.launcher.ui.holder;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Base64;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.liverussia.cr.R;
import com.liverussia.launcher.domain.LoaderSliderItemData;
import com.liverussia.launcher.domain.RouletteItem;

public class LoaderSliderViewHolder extends RecyclerView.ViewHolder {
    private TextView imageView;

    public LoaderSliderViewHolder(@NonNull View itemView) {
        super(itemView);
        imageView = itemView.findViewById(R.id.loader_slider_item_data_text);
    }

    public void setImage(LoaderSliderItemData sliderItem){
        //TODO use glide or picasso in case you get image from internet

        imageView.setText(sliderItem.getText());

    }
}