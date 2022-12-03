package com.liverussia.cr.gui.util;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.text.Html;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.Button;
import android.widget.TextView;
import com.liverussia.cr.gui.util.Utils;

import com.liverussia.cr.R;

public class StrokedTextView extends androidx.appcompat.widget.AppCompatTextView {

	private static final int DEFAULT_STROKE_WIDTH = 0;

	// fields
	private int _strokeColor;
	private float _strokeWidth;
	private String strWithColor;
	private String strWithoutColor;

	// constructors
	public StrokedTextView(Context context) {
		this(context, null, 0);
	}

	public StrokedTextView(Context context, AttributeSet attrs) {
		this(context, attrs, 0);
	}

	public StrokedTextView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);

		if(attrs != null) {
			TypedArray a = context.obtainStyledAttributes(attrs,R.styleable.StrokedTextAttrs);
			_strokeColor = a.getColor(R.styleable.StrokedTextAttrs_textStrokeColor,
					getCurrentTextColor());
			_strokeWidth = a.getFloat(R.styleable.StrokedTextAttrs_textStrokeWidth,
					DEFAULT_STROKE_WIDTH);

			a.recycle();
		}
		else {
			_strokeColor = getCurrentTextColor();
			_strokeWidth = DEFAULT_STROKE_WIDTH;
		}
		//convert values specified in dp in XML layout to
		//px, otherwise stroke width would appear different
		//on different screens
		_strokeWidth = dpToPx(context, _strokeWidth);
	}

	// getters + setters
	public void setStrokeColor(int color) {
		_strokeColor = color;
	}

	public void setStrokeWidth(int width) {
		_strokeWidth = width;
	}

	// overridden methods
	@Override
	protected void onDraw(Canvas canvas) {
		if(_strokeWidth > 0) {
			//set paint to fill mode
			Paint p = getPaint();
			p.setStyle(Paint.Style.FILL);
			//draw the fill part of text
			super.onDraw(canvas);
			//save the text color
			int currentTextColor = getCurrentTextColor();
			//set paint to stroke mode and specify
			//stroke color and width
			p.setStyle(Paint.Style.STROKE);
			p.setStrokeWidth(_strokeWidth);
			setTextColor(_strokeColor);
			//draw text stroke
			super.onDraw(canvas);
			//revert the color back to the one
			//initially specified
			//setTextColor(currentTextColor);
		} else {
			super.onDraw(canvas);
		}
	}

	/**
	 * Convenience method to convert density independent pixel(dp) value
	 * into device display specific pixel value.
	 * @param context Context to access device specific display metrics
	 * @param dp density independent pixel value
	 * @return device specific pixel value.
	 */
	public static int dpToPx(Context context, float dp)
	{
		final float scale= context.getResources().getDisplayMetrics().density;
		return (int) (dp * scale + 0.5f);
	}
}