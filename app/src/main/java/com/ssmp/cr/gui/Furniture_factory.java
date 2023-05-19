package com.ssmp.cr.gui;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.Point;
import android.graphics.PorterDuff;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;

import com.google.android.material.button.MaterialButton;
import com.ssmp.cr.R;
import com.ssmp.cr.gui.util.Utils;

import java.util.ArrayList;
import java.util.Random;

public class Furniture_factory {
    Activity activity;
    FrameLayout furniture_factory_main_layout;
    private ArrayList<ImageView> furniture_part;
    private ArrayList<ImageView> furniture_readypart;
    private MaterialButton furniture_factory_button_continue;
    private int totalscores;

    native void InitFurnitureFactory();
    native void SendSucces(int buttonID);

    public Furniture_factory(Activity activity)
    {
        InitFurnitureFactory();

        this.activity = activity;
        furniture_part = new ArrayList<>();
        furniture_readypart = new ArrayList<>();

        furniture_factory_main_layout = activity.findViewById(R.id.furniture_factory_main_layout);
        furniture_factory_main_layout.setVisibility(View.GONE);

        furniture_factory_button_continue = activity.findViewById(R.id.furniture_factory_button_continue);
        furniture_factory_button_continue.setOnClickListener(view -> {
            activity.runOnUiThread(() -> Utils.HideLayout(furniture_factory_main_layout, true));
            SendSucces(0);
        });

        furniture_part.add(activity.findViewById(R.id.furniture_part1));
        furniture_part.add(activity.findViewById(R.id.furniture_part2));
        furniture_part.add(activity.findViewById(R.id.furniture_part3));
        furniture_part.add(activity.findViewById(R.id.furniture_part4));

        furniture_readypart.add(activity.findViewById(R.id.furniture_readypart1));
        furniture_readypart.add(activity.findViewById(R.id.furniture_readypart2));
        furniture_readypart.add(activity.findViewById(R.id.furniture_readypart3));
        furniture_readypart.add(activity.findViewById(R.id.furniture_readypart4));

        for(int i = 0; i < furniture_part.size(); i++){
            furniture_part.get(i).setOnTouchListener(touchListener);
        }


        //Bitmap bitmap = BitmapFactory.decodeResource(activity.getResources(), R.drawable.furniture_door);

    }
    void ShowFactory(int furnitureType)
    {
        Log.d("sdf", "furnitureType ="+furnitureType);
        activity.runOnUiThread(() -> {
            totalscores = 0;
            Bitmap bitmap;
            switch (furnitureType) {
                case 0: {
                    bitmap = BitmapFactory.decodeResource(activity.getResources(), R.drawable.furniture_chair);
                    break;
                }
                case 1: {
                    bitmap = BitmapFactory.decodeResource(activity.getResources(), R.drawable.furniture_closet);
                    break;
                }
                default: {
                    bitmap = BitmapFactory.decodeResource(activity.getResources(), R.drawable.furniture_door);
                    break;
                }
            }

            Bitmap[] ggg = breakBitmap(bitmap);

            for (int i = 0; i < furniture_part.size(); i++) {
                FrameLayout.LayoutParams layoutParams = (FrameLayout.LayoutParams) furniture_part.get(i).getLayoutParams();

                furniture_part.get(i).setImageBitmap(ggg[i]);

                layoutParams.leftMargin = new Random().nextInt(1000);
                layoutParams.topMargin = new Random().nextInt(500);
                furniture_part.get(i).setLayoutParams(layoutParams);
                furniture_part.get(i).setVisibility(View.VISIBLE);

                furniture_readypart.get(i).setImageBitmap(ggg[i]);
                furniture_readypart.get(i).setColorFilter(Color.parseColor("#cfd8dc"), PorterDuff.Mode.SRC_ATOP);

            }
            Utils.ShowLayout(furniture_factory_main_layout, true);
        });
    }
    //    int pxToDp(int dp) {
//        Resources r = activity.getResources();
//        float px = TypedValue.applyDimension(
//                TypedValue.COMPLEX_UNIT_DIP,
//                dp,
//                r.getDisplayMetrics()
//        );
//        return (int)(px);
//    }
    private int xDelta, yDelta;
    private View.OnTouchListener touchListener = new View.OnTouchListener() {
        @Override public boolean onTouch(View view, MotionEvent event) {
            final int x = (int) event.getRawX();
            final int y = (int) event.getRawY();

            FrameLayout.LayoutParams layoutParams = (FrameLayout.LayoutParams) view.getLayoutParams();

            int needIdx = getFurnitureIndex(view);
            LinearLayout.LayoutParams needParam = (LinearLayout.LayoutParams) furniture_readypart.get(needIdx).getLayoutParams();

            switch (event.getAction() & MotionEvent.ACTION_MASK) {
                case MotionEvent.ACTION_DOWN: {
                    xDelta = x - layoutParams.leftMargin;
                    yDelta = y - layoutParams.topMargin;
                    break;
                }
                case MotionEvent.ACTION_UP: {
                    if(Math.abs( getLocationView(furniture_readypart.get(needIdx)).x - layoutParams.leftMargin ) < 10)
                    {
                        furniture_readypart.get(needIdx).clearColorFilter();
                        activity.runOnUiThread(() -> view.setVisibility(View.GONE));
                        totalscores += 25;
                    }
                    if(totalscores >= 100)
                    {
                        activity.runOnUiThread(() -> Utils.HideLayout(furniture_factory_main_layout, true));
                        SendSucces(1);
                    }

                    break;
                }
                case MotionEvent.ACTION_MOVE: {

                    layoutParams.leftMargin = x - xDelta;
                    layoutParams.topMargin = y - yDelta;
                    layoutParams.rightMargin = 0;
                    layoutParams.bottomMargin = 0;
                    view.setLayoutParams(layoutParams);
                    //Log.d("asdf", "X = "+CalculateConformity());
                    break;
                }
            }
            // furniture_factory_main_layout.invalidate();
            return true;
        }
    };
    Bitmap[] breakBitmap(Bitmap bitmap) {
        // Разрезает картинку на 4 куска
        int halfWidth = bitmap.getWidth() / 2;
        int halfHeight = bitmap.getHeight() / 2;

        Bitmap topLeft = Bitmap.createBitmap(bitmap, 0, 0, halfWidth, halfHeight);
        Bitmap topRight = Bitmap.createBitmap(bitmap, halfWidth, 0, halfWidth, halfHeight);
        Bitmap bottomLeft = Bitmap.createBitmap(bitmap, 0, halfHeight, halfWidth, halfHeight);
        Bitmap bottomRight = Bitmap.createBitmap(bitmap, halfWidth, halfHeight, halfWidth, halfHeight);

        return new Bitmap[] {topLeft, topRight, bottomLeft, bottomRight};
    }

    public static Point getLocationView(View view){
        int[] location = new int[2];
        view.getLocationOnScreen(location);
        return new Point(location[0], location[1]);
    }
    int getFurnitureIndex(View view)
    {
        for(int i = 0; i < furniture_part.size(); i++) {
            if (furniture_part.get(i) == view) {
                return i;
            }
        }
        return -1;
    }
}
