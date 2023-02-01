package com.liverussia.cr.core;

import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.appcompat.widget.SwitchCompat;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.fragment.app.DialogFragment;
import androidx.fragment.app.Fragment;

import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.FadingEdgeLayout;
import com.nvidia.devtech.NvEventQueueActivity;

import java.util.HashMap;

public class DialogClientSettingsCommonFragment extends Fragment implements ISaveableFragment {
    native boolean getNativeDamageInformer();
    native void setNativeDamageInformer(boolean state);
    native int getNativeFpsLimit();
    native void setNativeFpsCount(int fps);
    native void setNativeFpsCounterSettings(boolean b);
    native void setNativeOutfitGunsSettings(boolean b);
    native void setNativeHpArmourText(boolean b);
    native boolean getNativeFpsCounterSettings();
    native boolean getNativeOutfitGunsSettings();
    native boolean getNativeHpArmourText();

    native boolean getNativeShow3dText();
    native void setNativeShow3dText(boolean state);

    private SwitchCompat mSwitchFPSCounter;
    private SwitchCompat mSwitchOutfit;
    private SwitchCompat mSwitchHpArmour;
    SwitchCompat switch_damageinformer;
    SwitchCompat switch_3dtext_show;
    private SeekBar chat_line_count;
    private SeekBar chat_font_size;

    EditText fps_text;
    TextView fps_plus_butt;
    TextView fps_dec_butt;

    public native void ChatFontSizeChanged(int size);

    private NvEventQueueActivity mContext = null;

    private View mRootView = null;

    private boolean bChangeAllowed = true;

    private SeekBar.OnSeekBarChangeListener mListenerSeekBars;
    private HashMap<ViewGroup, Drawable> mOldDrawables;

    private ViewGroup mParentView = null;
   // public native void ChatLineChanged(int newcount);

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

        fps_text = mRootView.findViewById(R.id.fps_text);
        fps_text.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                int curFps;
                try {
                    curFps = Integer.parseInt(String.valueOf(fps_text.getText()));
                } catch (NumberFormatException e) {
                    curFps = 10;
                }

                setNativeFpsCount(curFps);
            }

            @Override
            public void afterTextChanged(Editable editable) {

            }
        });
        fps_plus_butt = mRootView.findViewById(R.id.fps_plus_butt);
        fps_plus_butt.setOnClickListener(view -> {
            int curFps = Integer.parseInt(String.valueOf(fps_text.getText()));
            curFps ++;
            fps_text.setText(String.format("%d", curFps));
            setNativeFpsCount(curFps);
        });
        fps_dec_butt = mRootView.findViewById(R.id.fps_dec_butt);
        fps_dec_butt.setOnClickListener(view -> {
            int curFps = Integer.parseInt(String.valueOf(fps_text.getText()));
            curFps --;
            fps_text.setText(String.format("%d", curFps));
            setNativeFpsCount(curFps);
        });

        mSwitchFPSCounter = mRootView.findViewById(R.id.switch_fps_counter);
        mSwitchHpArmour = mRootView.findViewById(R.id.switch_info_bar);
        switch_damageinformer = mRootView.findViewById(R.id.switch_damageinformer);
        switch_3dtext_show = mRootView.findViewById(R.id.switch_3dtext_show);
        mSwitchOutfit = mRootView.findViewById(R.id.switch_outfit_weapons);
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
//                FadingEdgeLayout chat = mContext.findViewById(R.id.chat_fade_box);
//                ConstraintLayout.LayoutParams layoutParams = (ConstraintLayout.LayoutParams) chat.getLayoutParams();
//                layoutParams.height = progress;
//                chat.setLayoutParams(layoutParams);

            }
            @Override public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override public void onStopTrackingTouch(SeekBar seekBar) {

              //  ChatLineChanged(seekBar.getProgress());
            }
        });

        getValues();

        mSwitchOutfit.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                setNativeOutfitGunsSettings(b);
            }
        });

        mSwitchHpArmour.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                setNativeHpArmourText(b);
            }
        });

        switch_damageinformer.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                setNativeDamageInformer(b);
            }
        });

        switch_3dtext_show.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                setNativeShow3dText(b);
            }
        });

        mSwitchFPSCounter.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                setNativeFpsCounterSettings(b);
            }
        });

        return mRootView;
    }

    @Override
    public void save() {

    }

    @Override
    public void getValues() {
        mSwitchFPSCounter.setChecked( getNativeFpsCounterSettings() );
        mSwitchHpArmour.setChecked( getNativeHpArmourText() );
        switch_damageinformer.setChecked(getNativeDamageInformer());
        switch_3dtext_show.setChecked(getNativeShow3dText());
        mSwitchOutfit.setChecked( getNativeOutfitGunsSettings() );
        fps_text.setText(String.format("%d", getNativeFpsLimit()));

        bChangeAllowed = false;

        bChangeAllowed = true;
    }

}
