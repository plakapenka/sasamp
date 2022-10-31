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

import com.bumptech.glide.Glide;
import com.liverussia.cr.R;

import java.util.List;

public class PossiblePrizesItemListAdapter extends ArrayAdapter<String> {

    private final Context context;

    public PossiblePrizesItemListAdapter(@NonNull Context context, List<String> dataModalArrayList) {
        super(context, 0, dataModalArrayList);
        this.context = context;
    }

    @NonNull
    @Override
    public View getView(int position, @Nullable View convertView, @NonNull ViewGroup parent) {

        View item = convertView;
        if (item == null) {
            item = LayoutInflater.from(getContext()).inflate(R.layout.possible_prize_item, parent, false);
        }

        String url = getItem(position);
        ImageView imageView = item.findViewById(R.id.possible_prize_item_image);
        Glide.with(this.context).load(url).into(imageView);

        return item;
    }
}
