package com.liverussia.cr.gui.donate;

import android.annotation.SuppressLint;
import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.button.MaterialButton;
import com.liverussia.cr.R;
import com.liverussia.cr.core.Samp;

import org.checkerframework.checker.units.qual.C;

import java.util.List;

public class DonateAdapter extends RecyclerView.Adapter<DonateAdapter.ViewHolder>
{
    int selected_item = -1;
    public List<DonateItem> items;
    Donate donate;
    LayoutInflater mInflater;
    //ItemClickListener mClickListener;

    // data is passed into the constructor
    DonateAdapter(Context context, List<DonateItem> items, Donate donate) {
        this.donate = donate;
        this.mInflater = LayoutInflater.from(context);
        this.items = items;
    }

    // inflates the cell layout from xml when needed
    @Override
    @NonNull
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = mInflater.inflate(R.layout.donate_cell, parent, false);
        return new ViewHolder(view);
    }

    // binds the data to the TextView in each cell
    @SuppressLint("DefaultLocale")
    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
        if(selected_item == position)
        {
            holder.donate_cell_buttons_layout.setVisibility(View.VISIBLE);
        }
        else {
            holder.donate_cell_buttons_layout.setVisibility(View.GONE);
        }
        holder.donate_cell_price_text.setText( String.format("%s", Samp.formatter.format( items.get(position).price) ) );
        holder.donate_cell_img.setImageResource( items.get(position).imgRecourse );
        holder.donate_cell_name.setText( items.get(position).name );
    }

    // total number of cells
    @Override
    public int getItemCount() {
        return items.size();
    }




    // stores and recycles views as they are scrolled off screen
    public class ViewHolder extends RecyclerView.ViewHolder
    {

        TextView donate_cell_price_text;
        TextView donate_cell_name;
        ImageView donate_cell_img;
        ConstraintLayout donate_cell_buttons_layout;

        MaterialButton donate_cell_buy_butt;
        MaterialButton donate_cell_cancel_butt;
        MaterialButton donate_cell_info_butt;

        ViewHolder(View itemView) {
            super(itemView);
            donate_cell_buy_butt = itemView.findViewById(R.id.donate_cell_buy_butt);
            donate_cell_info_butt = itemView.findViewById(R.id.donate_cell_info_butt);
            donate_cell_price_text = itemView.findViewById(R.id.donate_cell_price_text);
            donate_cell_img = itemView.findViewById(R.id.donate_cell_img);
            donate_cell_name = itemView.findViewById(R.id.donate_cell_name);
            donate_cell_buttons_layout = itemView.findViewById(R.id.donate_cell_buttons_layout);
            donate_cell_cancel_butt = itemView.findViewById(R.id.donate_cell_cancel_butt);

            donate_cell_buy_butt.setOnClickListener(view -> {
                int pos = this.getBindingAdapterPosition();
                donate.clickBuy( items.get(pos) );
            });

            donate_cell_info_butt.setOnClickListener(view -> {
                int pos = this.getBindingAdapterPosition();
                donate.clickInfo( items.get(pos) );
            });

            donate_cell_cancel_butt.setOnClickListener(view -> {
                int pos = this.getBindingAdapterPosition();
                selected_item = -1;
                notifyItemChanged(pos);
            });

            itemView.setOnClickListener(view -> {
                int pos = this.getBindingAdapterPosition();
                int olspos = selected_item;

                if(selected_item == pos) {
                    selected_item = -1;
                }
                else {
                    selected_item = pos;
                }
                notifyItemChanged(olspos);
                notifyItemChanged(pos);
            });
        }

    }


}
