package com.liverussia.cr.core;

import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.TypedValue;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.appcompat.widget.SwitchCompat;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.fragment.app.DialogFragment;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.RecyclerView;

import com.factor.bouncy.BouncyRecyclerView;
import com.nvidia.devtech.NvEventQueueActivity;
import com.liverussia.cr.R;

import java.util.HashMap;

public class DialogClientSettingsCommonFragment extends Fragment implements ISaveableFragment {
    private SwitchCompat mSwitchKeyboard;
    private SwitchCompat mSwitchCutout;
    private SwitchCompat mSwitchFPSCounter;
    private SwitchCompat mSwitchOutfit;
    private SwitchCompat mSwitchHpArmour;
    private SwitchCompat mSwitchRadarrect;
    private SwitchCompat mSwitchSkyBox;
    private SeekBar chat_line_count;
    private SeekBar chat_font_size;

    public native void ChatFontSizeChanged(int size);

    private NvEventQueueActivity mContext = null;

    private View mRootView = null;

    private boolean bChangeAllowed = true;

    private SeekBar.OnSeekBarChangeListener mListenerSeekBars;
    private HashMap<ViewGroup, Drawable> mOldDrawables;

    private ViewGroup mParentView = null;
    public native void ChatLineChanged(int newcount);

    public static DialogClientSettingsCommonFragment createInstance(String txt)
    {
        DialogClientSettingsCommonFragment fragment = new DialogClientSettingsCommonFragment();
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(null);
    }

    @Override
    public void onViewStateRestored(@Nullable Bundle savedInstanceState) {
        super.onViewStateRestored(null);
    }

    @Override
    public void onSaveInstanceState(final Bundle outState) {
        outState.putSerializable("android:support:fragments", null);
        super.onSaveInstanceState(outState);
        outState.putSerializable("android:support:fragments", null);
    }

    @Override
    public View onCreateView(LayoutInflater inflater,
                             ViewGroup container,
                             Bundle savedInstanceState) {



        mContext = (NvEventQueueActivity) getActivity();

        mRootView = inflater.inflate(R.layout.dialog_settings_common,container,false);

        mSwitchKeyboard = mRootView.findViewById(R.id.switch_android_keyboard);
        mSwitchCutout = mRootView.findViewById(R.id.switch_cutout);
        mSwitchFPSCounter = mRootView.findViewById(R.id.switch_fps_counter);
        mSwitchHpArmour = mRootView.findViewById(R.id.switch_info_bar);
        mSwitchOutfit = mRootView.findViewById(R.id.switch_outfit_weapons);
        mSwitchRadarrect = mRootView.findViewById(R.id.switch_radar_rect);
        mSwitchSkyBox = mRootView.findViewById(R.id.switch_skybox);
        chat_line_count = mRootView.findViewById(R.id.chat_line_count);
        chat_line_count.setProgress(mContext.findViewById(R.id.chat).getLayoutParams().height);
        chat_font_size = mRootView.findViewById(R.id.chat_font_size);
        TextView chatlinelayout = mContext.findViewById(R.id.chat_line_text);
        chat_font_size.setProgress((int) chatlinelayout.getTextSize());

        chat_font_size.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
//                TextView chatlinelayout = mContext.findViewById(R.id.chat_line_text);
//                TextView chat_shadow = mContext.findViewById(R.id.chat_line_shadow);
//                chatlinelayout.setTextSize(TypedValue.COMPLEX_UNIT_PX, progress);
//                chat_shadow.setTextSize(TypedValue.COMPLEX_UNIT_PX, progress);
                ChatFontSizeChanged(progress);
            }
            @Override public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        chat_line_count.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
               // ChatLineChanged(progress);
                BouncyRecyclerView chat = mContext.findViewById(R.id.chat);
                ConstraintLayout.LayoutParams layoutParams = (ConstraintLayout.LayoutParams) chat.getLayoutParams();
                layoutParams.height = progress;
                chat.setLayoutParams(layoutParams);

            }
            @Override public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override public void onStopTrackingTouch(SeekBar seekBar) {

                ChatLineChanged(seekBar.getProgress());
            }
        });

        getValues();
        setSeekBarListeners();

        mSwitchCutout.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                mContext.setNativeCutoutSettings(b);

                DialogFragment newFragment = new NotificationDialogFragment();
                newFragment.show(mContext.getSupportFragmentManager(), "missiles");

            }
        });

        mSwitchSkyBox.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                mContext.setNativeSkyBox(b);
            }
        });

        mSwitchKeyboard.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                mContext.setNativeKeyboardSettings(b);
            }
        });

        mSwitchRadarrect.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                mContext.setNativeRadarrect(b);
            }
        });

        mSwitchOutfit.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                mContext.setNativeOutfitGunsSettings(b);
            }
        });

        mSwitchHpArmour.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                mContext.setNativeHpArmourText(b);
            }
        });

        mSwitchFPSCounter.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                mContext.setNativeFpsCounterSettings(b);
            }
        });

        return mRootView;
    }

    private void setSeekBarListeners()
    {
        mListenerSeekBars = new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                if(bChangeAllowed)
                {
                    passValuesToNative();
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar)
            {
                mContext.onSettingsWindowSave();
            }
        };

        for(int i = DialogClientSettings.mSettingsComonStart; i < DialogClientSettings.mSettingsComonEnd; i++)
        {
            String widgetX = "hud_element_pos_x_" + i;
            String widgetY = "hud_element_pos_y_" + i;

            int resIDX = mContext.getResources().getIdentifier(widgetX, "id", mContext.getPackageName());
            int resIDY = mContext.getResources().getIdentifier(widgetY, "id", mContext.getPackageName());

            SeekBar barX = mRootView.findViewById(resIDX);
            SeekBar barY = mRootView.findViewById(resIDY);

            if(barX != null)
            {
                barX.setOnSeekBarChangeListener(mListenerSeekBars);
            }
            if(barY != null)
            {
                barY.setOnSeekBarChangeListener(mListenerSeekBars);
            }
        }

        for(int i = DialogClientSettings.mSettingsComonStart; i < DialogClientSettings.mSettingsComonEnd; i++)
        {
            String widgetX = "hud_element_scale_x_" + i;
            String widgetY = "hud_element_scale_y_" + i;

            int resIDX = mContext.getResources().getIdentifier(widgetX, "id", mContext.getPackageName());
            int resIDY = mContext.getResources().getIdentifier(widgetY, "id", mContext.getPackageName());

            SeekBar barX = mRootView.findViewById(resIDX);
            SeekBar barY = mRootView.findViewById(resIDY);

            if(barX != null)
            {
                barX.setOnSeekBarChangeListener(mListenerSeekBars);
            }
            if(barY != null)
            {
                barY.setOnSeekBarChangeListener(mListenerSeekBars);
            }
        }
    }

    @Override
    public void save() {

    }

    @Override
    public void getValues() {
        mSwitchKeyboard.setChecked(mContext.getNativeKeyboardSettings());
        mSwitchCutout.setChecked(mContext.getNativeCutoutSettings());
        mSwitchFPSCounter.setChecked(mContext.getNativeFpsCounterSettings());
        mSwitchHpArmour.setChecked(mContext.getNativeHpArmourText());
        mSwitchOutfit.setChecked(mContext.getNativeOutfitGunsSettings());
        mSwitchRadarrect.setChecked(mContext.getNativeRadarrect());
        mSwitchSkyBox.setChecked(mContext.getNativeSkyBox());

        bChangeAllowed = false;
        for(int i = DialogClientSettings.mSettingsComonStart; i < DialogClientSettings.mSettingsComonEnd; i++)
        {
            String widgetX = "hud_element_pos_x_" + i;
            String widgetY = "hud_element_pos_y_" + i;

            int resIDX = mContext.getResources().getIdentifier(widgetX, "id", mContext.getPackageName());
            int resIDY = mContext.getResources().getIdentifier(widgetY, "id", mContext.getPackageName());

            SeekBar barX = mRootView.findViewById(resIDX);
            SeekBar barY = mRootView.findViewById(resIDY);

            int[] pos = mContext.getNativeHudElementPosition(i);

            if(pos[0] == -1)
            {
                pos[0] = 1;
            }
            if(pos[1] == -1)
            {
                pos[1] = 1;
            }

            if(barX != null)
            {
                barX.setProgress(pos[0]);
            }
            if(barY != null)
            {
                barY.setProgress(pos[1]);
            }
        }

        for(int i = DialogClientSettings.mSettingsComonStart; i < DialogClientSettings.mSettingsComonEnd; i++)
        {
            String widgetX = "hud_element_scale_x_" + i;
            String widgetY = "hud_element_scale_y_" + i;

            int resIDX = mContext.getResources().getIdentifier(widgetX, "id", mContext.getPackageName());
            int resIDY = mContext.getResources().getIdentifier(widgetY, "id", mContext.getPackageName());

            SeekBar barX = mRootView.findViewById(resIDX);
            SeekBar barY = mRootView.findViewById(resIDY);

            int[] pos = mContext.getNativeHudElementScale(i);

            if(pos[0] == -1)
            {
                pos[0] = 1;
            }
            if(pos[1] == -1)
            {
                pos[1] = 1;
            }

            if(barX != null && pos[0] != -1)
            {
                barX.setProgress(pos[0]);
            }
            if(barY != null && pos[1] != -1)
            {
                barY.setProgress(pos[1]);
            }
        }

        bChangeAllowed = true;
    }
    public void passValuesToNative() {
        for (int i = DialogClientSettings.mSettingsComonStart; i < DialogClientSettings.mSettingsComonEnd; i++) {
            String widgetX = "hud_element_pos_x_" + i;
            String widgetY = "hud_element_pos_y_" + i;

            int resIDX = mContext.getResources().getIdentifier(widgetX, "id", mContext.getPackageName());
            int resIDY = mContext.getResources().getIdentifier(widgetY, "id", mContext.getPackageName());

            SeekBar barX = mRootView.findViewById(resIDX);
            SeekBar barY = mRootView.findViewById(resIDY);
            int x = -1;
            int y = -1;
            if (barX != null) {
                x = barX.getProgress();
            }
            if (barY != null) {
                y = barY.getProgress();
            }

            mContext.setNativeHudElementPosition(i, x, y);
        }

        for (int i = DialogClientSettings.mSettingsComonStart; i < DialogClientSettings.mSettingsComonEnd; i++) {
            String widgetX = "hud_element_scale_x_" + i;
            String widgetY = "hud_element_scale_y_" + i;

            int resIDX = mContext.getResources().getIdentifier(widgetX, "id", mContext.getPackageName());
            int resIDY = mContext.getResources().getIdentifier(widgetY, "id", mContext.getPackageName());

            SeekBar barX = mRootView.findViewById(resIDX);
            SeekBar barY = mRootView.findViewById(resIDY);
            int x = -1;
            int y = -1;
            if (barX != null) {
                x = barX.getProgress();
            }
            if (barY != null) {
                y = barY.getProgress();
            }

            mContext.setNativeHudElementScale(i, x, y);
        }
    }
}
