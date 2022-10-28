package com.liverussia.launcher.ui.adapters;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.liverussia.cr.R;
import com.liverussia.launcher.domain.LoaderSliderItemData;
import com.liverussia.launcher.ui.LoaderSliderViewHolder;

import org.jetbrains.annotations.NotNull;

import java.util.ArrayList;
import java.util.List;

public class LoaderSliderAdapter extends RecyclerView.Adapter<LoaderSliderViewHolder> {

    private List<LoaderSliderItemData> sliderItems;

    public LoaderSliderAdapter(List<LoaderSliderItemData> sliderItems) {
        List<LoaderSliderItemData> sliderItemsWithFakeElements = new ArrayList<>();
        sliderItemsWithFakeElements.add(sliderItems.get(sliderItems.size() - 1));
        sliderItemsWithFakeElements.addAll(sliderItems);
        sliderItemsWithFakeElements.add(sliderItems.get(0));

        this.sliderItems = sliderItemsWithFakeElements;
    }

    @NotNull
    @Override
    public LoaderSliderViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(parent.getContext())
                .inflate(
                        R.layout.loader_slider_item_data,
                        parent,
                        false
                );

        return new LoaderSliderViewHolder(itemView);
    }

    @Override
    public void onBindViewHolder(@NonNull LoaderSliderViewHolder holder, int position) {
        holder.setImage(sliderItems.get(position));
    }

    @Override
    public int getItemCount() {
        return sliderItems.size();
    }
}
