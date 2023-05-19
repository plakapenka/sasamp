package com.sasamp.cr.gui.util;

import android.content.Context;
import android.graphics.Canvas;

import androidx.annotation.NonNull;

import com.sasamp.cr.R;

public class OutlineTextView extends androidx.appcompat.widget.AppCompatTextView {
    public OutlineTextView(@NonNull Context context) {
        super(context);
    }

    // Constructors

    @Override
    public void draw(Canvas canvas) {
        for (int i = 0; i < 5; i++) {
            super.draw(canvas);
        }
    }

}

