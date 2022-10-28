package com.liverussia.launcher.ui.adapters;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.liverussia.cr.R;
import com.liverussia.launcher.ui.RouletteUiPanelViewHolder;
import com.liverussia.launcher.domain.RouletteItem;

import org.jetbrains.annotations.NotNull;

import java.util.List;

public class RouletteUIPanelAdapter extends RecyclerView.Adapter<RouletteUiPanelViewHolder> {

    private List<RouletteItem> sliderItems;

    public RouletteUIPanelAdapter(List<RouletteItem> sliderItems) {
        this.sliderItems = sliderItems;
    }

    @NotNull
    @Override
    public RouletteUiPanelViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(parent.getContext())
                .inflate(
                        R.layout.roulette_item,
                        parent,
                        false
                );

        return new RouletteUiPanelViewHolder(itemView);
    }

    @Override
    public void onBindViewHolder(@NonNull RouletteUiPanelViewHolder holder, int position) {
        holder.setImage(sliderItems.get(position));
    }

    @Override
    public int getItemCount() {
        return sliderItems.size();
    }
}