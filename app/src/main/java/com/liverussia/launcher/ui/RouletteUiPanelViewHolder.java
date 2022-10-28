package com.liverussia.launcher.ui;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Base64;
import android.view.View;
import android.widget.ImageView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.liverussia.cr.R;
import com.liverussia.launcher.domain.RouletteItem;

public class RouletteUiPanelViewHolder extends RecyclerView.ViewHolder {
    private ImageView imageView;

    public RouletteUiPanelViewHolder(@NonNull View itemView) {
        super(itemView);
        imageView = itemView.findViewById(R.id.roulette_item);
    }

    public void setImage(RouletteItem sliderItem){
        //TODO use glide or picasso in case you get image from internet

        if (sliderItem.getResourceImage() != null) {
            imageView.setImageResource(sliderItem.getResourceImage());
        } else {
            buildImageFromBase64(sliderItem.getBase64Image());
        }

    }

    private void buildImageFromBase64(String base64Image) {
        byte[] imageBytes = Base64.decode(base64Image, Base64.DEFAULT);
        Bitmap decodedImage = BitmapFactory.decodeByteArray(imageBytes, 0, imageBytes.length);
        imageView.setImageBitmap(decodedImage);
    }
}
