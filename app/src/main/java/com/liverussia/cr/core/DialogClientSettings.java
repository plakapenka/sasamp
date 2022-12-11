package com.liverussia.cr.core;

import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.SeekBar;

import androidx.appcompat.widget.AppCompatButton;
import androidx.fragment.app.DialogFragment;
import androidx.viewpager.widget.ViewPager;

import com.google.android.material.tabs.TabLayout;
import com.nvidia.devtech.NvEventQueueActivity;
import com.liverussia.cr.R;


public class DialogClientSettings extends DialogFragment {
    TabLayout tabLayout;
    ViewPager viewPager;

    static final int mSettingsHudCount = 10;
    static final int mSettingsHudFPSStart = 10;
    static final int mSettingsHudFPSEnd = 12;

    static final int mSettingsWeaponsStart = 12;
    static final int mSettingsWeaponsEnd = 14;

    static final int mSettingsComonStart = 14;
    static final int mSettingsComonEnd = 15;
    NvEventQueueActivity mContext = null;

    @Override
    public View onCreateView(LayoutInflater inflater,
                             ViewGroup container,
                             Bundle savedInstanceState)
    {
        View rootview = inflater.inflate(R.layout.dialog_settings,null,false);
        tabLayout = (TabLayout) rootview.findViewById(R.id.tabLayout);
        viewPager = (ViewPager) rootview.findViewById(R.id.masterViewPager);
        final DialogClientSettingsAdapter adapter = new DialogClientSettingsAdapter(getChildFragmentManager(), 0);
        adapter.addFragment("Основное",DialogClientSettingsCommonFragment.createInstance("common"));

        viewPager.setAdapter(adapter);
        tabLayout.setupWithViewPager(viewPager);

        getDialog().getWindow().setBackgroundDrawable(new ColorDrawable(android.graphics.Color.TRANSPARENT));
        getDialog().getWindow().setDimAmount(.0f);

        mContext = (NvEventQueueActivity) getActivity();

        ((AppCompatButton)rootview.findViewById(R.id.dialog_settings_button_close)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mContext.onSettingsWindowSave();
                getDialog().dismiss();
            }
        });

        ((AppCompatButton)rootview.findViewById(R.id.dialog_settings_button_reset)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mContext.onSettingsWindowDefaults(tabLayout.getSelectedTabPosition() + 1);

                ISaveableFragment fragment = (ISaveableFragment)adapter.getItem(tabLayout.getSelectedTabPosition());
                fragment.getValues();

                mContext.onSettingsWindowSave();

                getDialog().dismiss();

            }
        });

        setCancelable(false);

        return rootview;
    }
}
