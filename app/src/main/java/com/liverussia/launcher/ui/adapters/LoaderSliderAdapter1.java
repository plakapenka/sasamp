package com.liverussia.launcher.ui.adapters;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.liverussia.cr.R;
import com.liverussia.launcher.domain.LoaderSliderItemData;
import com.smarteist.autoimageslider.SliderViewAdapter;

import java.util.ArrayList;
import java.util.List;

public class LoaderSliderAdapter1 extends SliderViewAdapter<LoaderSliderAdapter1.SliderAdapterViewHolder> {

    private final List<LoaderSliderItemData> mSliderItems;

    public LoaderSliderAdapter1(Context context, ArrayList<LoaderSliderItemData> sliderDataArrayList) {
        this.mSliderItems = sliderDataArrayList;
    }

    @Override
    public SliderAdapterViewHolder onCreateViewHolder(ViewGroup parent) {
        View inflate = LayoutInflater.from(parent.getContext()).inflate(R.layout.loader_slider_item_data, null);
        return new SliderAdapterViewHolder(inflate);
    }

    @Override
    public void onBindViewHolder(SliderAdapterViewHolder viewHolder, final int position) {
        final LoaderSliderItemData sliderItem = mSliderItems.get(position);
        viewHolder.text.setText(sliderItem.getText());
    }

    @Override
    public int getCount() {
        return mSliderItems.size();
    }

    static class SliderAdapterViewHolder extends SliderViewAdapter.ViewHolder {
        View itemView;
        TextView text;

        public SliderAdapterViewHolder(View itemView) {
            super(itemView);
            text = itemView.findViewById(R.id.loader_slider_item_data_text);
            this.itemView = itemView;
        }
    }
}
