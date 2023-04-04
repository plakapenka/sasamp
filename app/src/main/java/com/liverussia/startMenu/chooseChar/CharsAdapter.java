package com.liverussia.startMenu.chooseChar;

import static com.liverussia.cr.core.Samp.activity;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Color;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.liverussia.cr.R;
import com.liverussia.cr.core.Samp;
import com.liverussia.launcher.storage.Storage;

import java.util.List;

public class CharsAdapter extends RecyclerView.Adapter<CharsAdapter.ViewHolder>
{
    int selected_item = -1;
    public List<CharData> items;
    public CharsList charsList;
    LayoutInflater mInflater;
    //ItemClickListener mClickListener;

    // data is passed into the constructor
    CharsAdapter(Context context, List<CharData> list, CharsList charsList) {
        this.charsList = charsList;
        this.mInflater = LayoutInflater.from(context);
        this.items = list;
    }

    // inflates the cell layout from xml when needed
    @Override
    @NonNull
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = mInflater.inflate(R.layout.select_char_item, parent, false);
        return new ViewHolder(view);
    }

    // binds the data to the TextView in each cell
    @SuppressLint("DefaultLocale")
    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
        if(items.get(position).level == -1 && items.get(position).money == -1) {
            holder.char_skin.setImageResource(R.drawable.add_char_icon);
            holder.char_nick.setText("");
            holder.char_lvl.setText("");
            holder.char_money.setText( "Создать нового персонажа" );
            holder.char_money.setTextColor(Color.parseColor("#ffffff"));
            return;
        }
        // FIXME: удалить
        if(items.get(position).level == -2 && items.get(position).money == -2) {
            holder.char_skin.setImageResource(R.drawable.add_char_icon);
            holder.char_nick.setText("");
            holder.char_lvl.setText("");
            holder.char_money.setText( "Перенести из старой системы" );
            holder.char_money.setTextColor(Color.parseColor("#ffffff"));
            return;
        }
        holder.char_skin.setImageResource( activity.getResources().getIdentifier("skin_" + items.get(position).skin, "drawable", activity.getPackageName()) );

        holder.char_nick.setText( items.get(position).nickname );
        holder.char_money.setText( String.format("Наличка: %s рублей", Samp.formatter.format( items.get(position).money )) );
        holder.char_lvl.setText( String.format("Уровень: %d", items.get(position).level) );
    }

    // total number of cells
    @Override
    public int getItemCount() {
        return items.size();
    }

    // stores and recycles views as they are scrolled off screen
    public class ViewHolder extends RecyclerView.ViewHolder
    {
        TextView char_nick;
        TextView char_money;
        ImageView char_skin;
        TextView char_lvl;

        ViewHolder(View itemView) {
            super(itemView);
            char_nick = itemView.findViewById(R.id.char_nick);
            char_skin = itemView.findViewById(R.id.char_skin);
            char_money = itemView.findViewById(R.id.char_money);
            char_lvl = itemView.findViewById(R.id.char_lvl);

            itemView.setOnClickListener(view -> {
                int pos = this.getBindingAdapterPosition();

                if(items.get(pos).level == -1 && items.get(pos).money == -1)
                {
                    charsList.onCreateRegisterChar();
                    return;
                }
                // FIXME: удалить перенос
                if(items.get(pos).level == -2 && items.get(pos).money == -2)
                {
                    charsList.createTransfer();
                    return;
                }

                Storage.setProperty(activity, "nickname", items.get(pos).nickname);
                charsList.toggle(false);
              //  charsList
//                Storage.setProperty(activity, "selectedServerName", items.get(pos).name);
//                Storage.setProperty(activity, "selectedServerIp", items.get(pos).ip);
//                Storage.setProperty(activity, "selectedServerPort", items.get(pos).port);
//
//                fromActivity.toggle(false);
            });
        }

    }


}
