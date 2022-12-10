package com.liverussia.cr.gui;

import android.app.Activity;
import android.util.Log;
import android.view.View;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.liverussia.cr.R;

import java.util.ArrayList;
import java.util.List;


public class Inventory {

    List<ArrayList<ConstraintLayout>> items = new ArrayList<>();

    private Activity activity;
    private ConstraintLayout inv_main_layout;
    private View inv_close_butt;
    private View isSelectedItem = null;

    public native void SwitchToggle(boolean toggle);
    public native void SendSelectItem(int matrindex, int pos);
    public native void SendClickButton(int buttonid);

    static final int INVENTAR_BUTTON_TRATE = 1;
    static final int INVENTAR_BUTTON_REPRT = 2;
    static final int INVENTAR_BUTTON_QUEST = 3;
    static final int INVENTAR_BUTTON_MMENU = 4;
    static final int INVENTAR_BUTTON_DONATE = 5;
    static final int INVENTAR_BUTTON_DELETE = 6;
    static final int INVENTAR_BUTTON_USE = 7;
    static final int INVENTAR_BUTTON_INFO = 8;

    private View inv_use_butt;
    private View inv_inf_butt;
    private View inv_sell_butt;
    private View inv_del_butt;
    private View inv_menu_butt;
    private View inv_help_butt;
    private View inv_shop_butt;
    private View inv_task_butt;

    private TextView inv_health_text;
    private TextView inv_satiety_text;
    private ProgressBar inv_progress;
    private TextView inv_progress_text;

    public Inventory(Activity activity)
    {
        this.activity = activity;

        InventoryInit();

        inv_progress = activity.findViewById(R.id.inv_progress);
        inv_progress_text = activity.findViewById(R.id.inv_progress_text);

        inv_satiety_text = activity.findViewById(R.id.inv_satiety_text);
        inv_health_text = activity.findViewById(R.id.inv_health_text);

        //кнопка задания
        inv_task_butt = activity.findViewById(R.id.inv_task_butt);
        inv_task_butt.setOnClickListener(view -> {
            SendClickButton(INVENTAR_BUTTON_QUEST);
        });

        //Кнопка магазин
        inv_shop_butt = activity.findViewById(R.id.inv_shop_butt);
        inv_shop_butt.setOnClickListener(view -> {
            SendClickButton(INVENTAR_BUTTON_DONATE);
        });

        //кнопка помощь
        inv_help_butt = activity.findViewById(R.id.inv_help_butt);
        inv_help_butt.setOnClickListener(view -> {
            SendClickButton(INVENTAR_BUTTON_REPRT);
        });

        //кнопка меню
        inv_menu_butt = activity.findViewById(R.id.inv_menu_butt);
        inv_menu_butt.setOnClickListener(view -> {
            SendClickButton(INVENTAR_BUTTON_MMENU);
        });

        // кнопка удалить
        inv_del_butt = activity.findViewById(R.id.inv_del_butt);
        inv_del_butt.setOnClickListener(view -> {
            SendClickButton(INVENTAR_BUTTON_DELETE);
        });

        //кнопка продать
        inv_sell_butt = activity.findViewById(R.id.inv_sell_butt);
        inv_sell_butt.setOnClickListener(view -> {
            SendClickButton(INVENTAR_BUTTON_TRATE);
        });

        // кнопка Использовать
        inv_use_butt = activity.findViewById(R.id.inv_use_butt);
        inv_use_butt.setOnClickListener(view -> {
            SendClickButton(INVENTAR_BUTTON_USE);
        });
        // Кнопка инфо
        inv_inf_butt = activity.findViewById(R.id.inv_inf_butt);
        inv_inf_butt.setOnClickListener(view -> {
            SendClickButton(INVENTAR_BUTTON_INFO);
        });

        inv_main_layout = activity.findViewById(R.id.inv_main_layout);
        inv_main_layout.setVisibility(View.GONE);

        inv_close_butt = activity.findViewById(R.id.inv_close_butt);
        inv_close_butt.setOnClickListener(view -> {
            inv_main_layout.setVisibility(View.GONE);
            SwitchToggle(false);
        });

        ArrayList <ConstraintLayout> colum1 = new ArrayList<>();
        colum1.add(activity.findViewById(R.id.inv_player_1));
        colum1.add(activity.findViewById(R.id.inv_player_2));
        colum1.add(activity.findViewById(R.id.inv_player_3));
        colum1.add(activity.findViewById(R.id.inv_player_4));
        items.add(colum1);

        ArrayList <ConstraintLayout> colum2 = new ArrayList<>();
        colum2.add(activity.findViewById(R.id.inv_main_1));
        colum2.add(activity.findViewById(R.id.inv_main_2));
        colum2.add(activity.findViewById(R.id.inv_main_3));
        colum2.add(activity.findViewById(R.id.inv_main_4));
        colum2.add(activity.findViewById(R.id.inv_main_5));
        colum2.add(activity.findViewById(R.id.inv_main_6));
        colum2.add(activity.findViewById(R.id.inv_main_7));
        colum2.add(activity.findViewById(R.id.inv_main_8));
        colum2.add(activity.findViewById(R.id.inv_main_9));
        colum2.add(activity.findViewById(R.id.inv_main_10));
        colum2.add(activity.findViewById(R.id.inv_main_11));
        colum2.add(activity.findViewById(R.id.inv_main_12));
        colum2.add(activity.findViewById(R.id.inv_main_13));
        colum2.add(activity.findViewById(R.id.inv_main_14));
        colum2.add(activity.findViewById(R.id.inv_main_15));
        colum2.add(activity.findViewById(R.id.inv_main_16));
        items.add(colum2);

        ArrayList <ConstraintLayout> colum3 = new ArrayList<>();
        colum3.add(activity.findViewById(R.id.inv_active_1));
        colum3.add(activity.findViewById(R.id.inv_active_2));
        colum3.add(activity.findViewById(R.id.inv_active_3));
        colum3.add(activity.findViewById(R.id.inv_active_4));
        items.add(colum3);

        for(int i = 0; i < items.size(); i++) {
            for (int j = 0; j < items.get(i).size(); j++) {
                items.get(i).get(j).setOnClickListener(view -> {
                   // if(isSelectedItem != null)// снятие выделения
                   // {
                        SendSelectItem(GetItemMat(view), GetItemPos(view)); // отправка пакета

                       // isSelectedItem.setBackgroundResource(R.drawable.inv_bg_shape);
                       // isSelectedItem = null;
                   // }
                   // else {// выделение
                       // isSelectedItem = view;
                       // view.setBackgroundResource(R.drawable.inv_bg_shape_active);

                     //   SendSelectItem(GetItemMat(isSelectedItem), GetItemPos(isSelectedItem)); // отправка пакета
                   // }

                });
            }
        }

    }
    public void InventoryUpdateItem(int matrixindex, int pos, String sprite, String caption, boolean active){
        activity.runOnUiThread(() -> {
            if(matrixindex < 1)return;

            ConstraintLayout item = items.get(matrixindex-1).get(pos);
            ConstraintLayout bgitem = (ConstraintLayout) item.getChildAt(1);

            if(!sprite.isEmpty()) {
                int id = activity.getResources().getIdentifier(sprite, "drawable", activity.getPackageName());
                item.getChildAt(0).setBackgroundResource(id);
            }
            else {
                item.getChildAt(0).setBackgroundResource(0);
            }

            if(!caption.isEmpty()) {
                bgitem.setVisibility(View.VISIBLE); // показывает фон кол-а

                TextView tw = (TextView) bgitem.getChildAt(0);
                tw.setText(caption);
            }
            else {
                bgitem.setVisibility(View.GONE);
            }
            if(active) {
                item.setBackgroundResource(R.drawable.inv_bg_shape_active);
            }
            else {
                item.setBackgroundResource(R.drawable.inv_bg_shape);
            }


            inv_main_layout.invalidate();
        });
    }

    public void InventoryItemActive(int matrixindex, int pos, boolean active){
        activity.runOnUiThread(() -> {
            if(matrixindex < 1)return;

            ConstraintLayout item = items.get(matrixindex-1).get(pos);

            if(active) {
                item.setBackgroundResource(R.drawable.inv_bg_shape_active);
            }
            else {
                item.setBackgroundResource(R.drawable.inv_bg_shape);
            }

            inv_main_layout.invalidate();
        });
    }

    public void UpdateCarryng(int matrixindex, int brutto, int maxbrutto){
        activity.runOnUiThread(() -> {
            // if(matrixindex < 1)return;
            inv_progress.setProgress(brutto);
            inv_progress.setMax(maxbrutto);
            inv_progress_text.setText(String.format("%d/%d кг.", brutto, maxbrutto));

            inv_main_layout.invalidate();
        });
    }
    public void ToggleShow(boolean toggle, float satiety, float health)
    {
        if(toggle){
            activity.runOnUiThread(() -> {
                inv_satiety_text.setText(String.format("%.0f", satiety));
                inv_health_text.setText(String.format("%.0f", health));

                inv_main_layout.setVisibility(View.VISIBLE);
            });
        }else{
            activity.runOnUiThread(() -> {
                inv_main_layout.setVisibility(View.GONE);
            });
        }

    }
    public int GetItemPos(View view)
    {
        for(int i = 0; i < items.size(); i++)
        {
            for (int j = 0; j < items.get(i).size(); j++)
            {
                if(items.get(i).get(j) == view)
                {
                    return j;
                }
            }
        }
        return -1;
    }
    public int GetItemMat(View view)
    {
        for(int i = 0; i < items.size(); i++)
        {
            for (int j = 0; j < items.get(i).size(); j++)
            {
                if(items.get(i).get(j) == view)
                {
                    return i+1;
                }
            }
        }
        return -1;
    }
    public native void InventoryInit();
}
