package com.liverussia.launcher.ui.dialogs;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Base64;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.fragment.app.DialogFragment;
import androidx.fragment.app.FragmentManager;

import com.liverussia.cr.R;
import com.liverussia.launcher.async.dto.response.PrizeInfoResponseDto;

public class PrizeDialog extends DialogFragment implements View.OnClickListener {

    public static final String TAG = "prize_dialog";

    private TextView btnGoBack;
    private TextView prizeAdditionalInfoTextView;
    private ImageView prizeInfoImageView;
    private PrizeInfoResponseDto prizeInfo;

    public static PrizeDialog display(FragmentManager fragmentManager, PrizeInfoResponseDto prizeInfo) {
        PrizeDialog dialog = new PrizeDialog(prizeInfo);
        dialog.show(fragmentManager, TAG);
        return dialog;
    }

    public PrizeDialog(PrizeInfoResponseDto prizeInfo) {
        super();
        this.prizeInfo = prizeInfo;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setStyle(DialogFragment.STYLE_NORMAL, R.style.AppTheme_FullScreenDialog);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        super.onCreateView(inflater, container, savedInstanceState);
        View view = inflater.inflate(R.layout.prize_dialog, container, false);

        btnGoBack = view.findViewById(R.id.btnGoBack);
        btnGoBack.setOnClickListener(this);

        prizeAdditionalInfoTextView = view.findViewById(R.id.prize_info_additional_info);
        prizeAdditionalInfoTextView.setText(prizeInfo.getAdditionalInfo());

        prizeInfoImageView = view.findViewById(R.id.prize_info_image);
        prizeInfoImageView.setImageBitmap(getDecodedImage());


        return view;
    }

    private Bitmap getDecodedImage() {
        byte[] imageBytes = Base64.decode(prizeInfo.getBase64Image(), Base64.DEFAULT);
        return BitmapFactory.decodeByteArray(imageBytes, 0, imageBytes.length);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btnGoBack:
                dismiss();
                break;
            default:
                break;
        }
    }
}
