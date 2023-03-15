package com.liverussia.cr.gui.tab;

import android.app.Activity;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.view.animation.AnimationUtils;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.RecyclerView;

import com.liverussia.cr.R;
import com.liverussia.cr.core.Samp;
import com.liverussia.cr.gui.util.LinearLayoutManagerWrapper;
import com.liverussia.cr.gui.util.Utils;
import com.nvidia.devtech.NvEventQueueActivity;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

public class Tab {
    public native void initTab();

    public native void onTabClose();
    public ImageView mClearImg;
    public ConstraintLayout mInputLayout;
    public TextView mOnline;
    public List<PlayerData> mPlayerData;
    public RecyclerView mPlayersList;
    public EditText mSearch;
    public ImageView mSearchImg;
    private TabAdapter mTabAdapter;
    public Activity activity;

    public Tab(Activity activity) {
        initTab();
        this.activity = activity;
        mInputLayout= activity.findViewById(R.id.hassle_tab_main);

        mSearch = activity.findViewById(R.id.search_view);

        mSearch.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {
            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
            }

            @Override
            public void afterTextChanged(Editable editable) {
                mTabAdapter.getFilter().filter(editable);
                setVisibleIconInSearchView(editable.toString());
            }
        });

        mSearchImg = activity.findViewById(R.id.icon_search_view);
        mClearImg = activity.findViewById(R.id.icon_clear_search_text);

        mClearImg.setOnClickListener(view ->  clearEditText(view) );
        mSearchImg.setOnClickListener(view -> {
            clearEditText(view);

        });
        mOnline = (TextView) activity.findViewById(R.id.players_list_value_players);
        mPlayersList = (RecyclerView) activity.findViewById(R.id.players_list);
        mPlayerData = new ArrayList();

        mTabAdapter = new TabAdapter(mPlayerData);

        ((Button) activity.findViewById(R.id.players_list_close)).setOnClickListener(view ->  {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            close(activity, view);
        });
        Utils.HideLayout(this.mInputLayout, false);
    }

    public void clearEditText(View view) {
        mSearch.setText("");
        setVisibleIconInSearchView("");
    }

    public void close(Activity activity, View view) {
        ((InputMethodManager) activity.getSystemService("input_method")).hideSoftInputFromWindow(view.getWindowToken(), 0);
        Utils.HideLayout(mInputLayout, true);
        onTabClose();
        this.mPlayersList.getRecycledViewPool().clear();
        TabAdapter adapter = (TabAdapter) this.mPlayersList.getAdapter();
        if (adapter != null) {
            adapter.notifyDataSetChanged();
        }
        //mPlayerData.sort();
        this.mPlayerData.clear();
    }

    public void clearStat() {
        this.mPlayerData.clear();
    }

    public void setStat(int id, String name, int score, int ping) {
       // Log.d("afd", "Update");
        activity.runOnUiThread( () ->
                this.mPlayerData.add(new PlayerData(id, name, score, ping)) );
    }

    public void show() {
        Log.d("afd", "show");
        activity.runOnUiThread( () ->
        {
            this.mOnline.setText(this.mPlayerData.size() + "/1000");

            mTabAdapter = new TabAdapter(this.mPlayerData);
            mPlayersList.setLayoutManager(new LinearLayoutManagerWrapper(Samp.getInstance()));
            mPlayersList.setAdapter(this.mTabAdapter);
            mSearch.setText("");
            setVisibleIconInSearchView("");
            Utils.ShowLayout(this.mInputLayout, true);
          //  this.mInputLayout.setVisibility(View.VISIBLE);
        });
    }

    public void setVisibleIconInSearchView(String str) {
        activity.runOnUiThread( () ->
        {
            if (str.isEmpty()) {
                this.mSearchImg.setVisibility(View.VISIBLE);
                this.mClearImg.setVisibility(View.INVISIBLE);
                return;
            }
            this.mSearchImg.setVisibility(View.INVISIBLE);
            this.mClearImg.setVisibility(View.VISIBLE);
        });
    }

}
