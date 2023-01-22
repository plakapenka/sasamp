package com.liverussia.cr.gui;

import android.app.Activity;
import android.content.Context;
import android.text.Html;
import android.util.TypedValue;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.liverussia.cr.R;

import java.util.List;

public class Killist {
    Activity activity;
    KillListAdapter adapter;
    RecyclerView kill_list;

    native void init();

    public Killist(Activity activity)
    {
        init();
        this.activity = activity;

        kill_list = activity.findViewById(R.id.kill_list);

        adapter = new KillListAdapter(activity);
        kill_list.setAdapter(adapter);
    }

    void addItem(String killertext, String deathtext, int gun, int ream) {
        adapter.addItem(killertext, deathtext, gun);
    }

    public class KillListAdapter  extends RecyclerView.Adapter<KillListAdapter.ViewHolder> {

        final LayoutInflater inflater;
        List<String> killertext;
        List<String> deathtext;
        List<Integer> gun;

        KillListAdapter(Context context) {
            this.inflater = LayoutInflater.from(context);
        }
        @Override
        public KillListAdapter.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {

            View view = inflater.inflate(R.layout.itemkilllist, parent, false);

            return new ViewHolder(view);
        }

        @Override
        public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
            holder.kill_list_killer_textview.setText(killertext.get(position));
            holder.kill_list_death_textview.setText(deathtext.get(position));
        }

        @Override
        public int getItemCount() {
            return 0;
        }

        public class ViewHolder extends RecyclerView.ViewHolder {
            final TextView kill_list_killer_textview;
            final TextView kill_list_death_textview;

            ViewHolder(View view){
                super(view);
                kill_list_killer_textview = view.findViewById(R.id.kill_list_killer_textview);
                kill_list_death_textview = view.findViewById(R.id.kill_list_death_textview);
            }
        }
        public void addItem(String killer, String death, int gun) {
            activity.runOnUiThread(() -> {
                if(this.killertext != null && this.killertext.size() > 4){
                    this.killertext.remove(0);
                    this.deathtext.remove(0);
                    this.gun.remove(0);
                    notifyItemRemoved(0);
                }
                this.killertext.add(killer);
                this.deathtext.add(death);
                this.gun.add(gun);
                notifyItemInserted(this.killertext.size()-1);

            });

        }
    }
}
