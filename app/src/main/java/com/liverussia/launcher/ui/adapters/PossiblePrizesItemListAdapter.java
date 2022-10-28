package com.liverussia.launcher.ui.adapters;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Base64;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.liverussia.cr.R;

import java.util.List;

public class PossiblePrizesItemListAdapter extends ArrayAdapter<String> {

    public PossiblePrizesItemListAdapter(@NonNull Context context, List<String> dataModalArrayList) {
        super(context, 0, dataModalArrayList);
    }

    @NonNull
    @Override
    public View getView(int position, @Nullable View convertView, @NonNull ViewGroup parent) {

        View item = convertView;
        if (item == null) {
            item = LayoutInflater.from(getContext()).inflate(R.layout.possible_prize_item, parent, false);
        }

        String base64Image = getItem(position);

        byte[] imageBytes = Base64.decode(base64Image, Base64.DEFAULT);
        Bitmap decodedImage = BitmapFactory.decodeByteArray(imageBytes, 0, imageBytes.length);


        ImageView imageView = item.findViewById(R.id.possible_prize_item_image);

//        Picasso.get().load(dataModal.getImgUrl()).into(courseIV);

        imageView.setImageBitmap(decodedImage);

        return item;
    }
}
