package com.liverussia.cr.gui;

import static java.lang.Thread.sleep;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.media.Image;
import android.text.Html;
import android.util.Log;
import android.util.TypedValue;
import android.view.ActionMode;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AnimationUtils;
import android.view.inputmethod.InputMethodManager;
import android.widget.ArrayAdapter;
import android.widget.AutoCompleteTextView;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.factor.bouncy.BouncyRecyclerView;
import com.liverussia.cr.gui.util.Utils;
import com.liverussia.launcher.async.dto.response.Servers;
import com.liverussia.launcher.domain.enums.NativeStorageElements;
import com.liverussia.launcher.storage.NativeStorage;
import com.liverussia.launcher.utils.MainUtils;
import com.nvidia.devtech.NvEventQueueActivity;
import com.liverussia.cr.R;
import com.skydoves.progressview.ProgressView;

import org.jetbrains.annotations.NotNull;

import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.util.ArrayList;
import java.util.Formatter;
import java.util.List;
import java.util.Locale;
import java.util.Timer;
import java.util.TimerTask;

public class HudManager {
    private Timer timer;
    private Activity activity;
    private ConstraintLayout hud_main;
    private ConstraintLayout target_notify;
    private ConstraintLayout bus_layout;
    private TextView bus_text;
    private ImageView target_notify_exit;
    private ImageView target_notify_status;
    private TextView target_notify_text;
    private TextView hud_money;
    private TextView hud_money_dif;
    private ImageView hud_weapon;
    private ImageView hud_menu;
    private TextView hud_ammo;
    private TextView levelinfo;
    private ImageView hud_greenzone;
    private ImageView hud_gpsactive;
    private ConstraintLayout hud_serverlogo;
    private ArrayList<ImageView> hud_wanted;
    private ProgressBar progressHP;
    private ProgressBar progressArmor;

    private ImageView enter_passenger;
    private ImageView enterexit_driver;
    private ImageView lock_vehicle;
    private ImageView button_horn;
    private ConstraintLayout opg_war_layout;
    private TextView opg_attacker_score;
    private TextView opg_defender_score;
    private TextView opg_time_text;
    private ImageView hud_bg;
    private EditText chat_input;
    private ConstraintLayout chat_input_layout;
    private TextView armour_text;
    private TextView hp_text;
    private TextView me_button;
    private TextView try_button;
    private TextView do_button;
    private View hide_chat;
    private ConstraintLayout chat_box;

    // заработано
    private ConstraintLayout salary_job_layout;
    private TextView salary_job_salary_text;
    private TextView salary_job_lvl_text;
    private ProgressView salary_job_progress;
    private TextView salary_job_exp_text;

    private int cursorPos = 0;
    private final int INVALID = -1;
    private final int ME_BUTTON = 0;
    private final int DO_BUTTON = 1;
    private final int TRY_BUTTON = 2;
    private int chat_button = -1;
    long buttonLockCD;
    private boolean isHudSetPos = false;
    private int chatFontSize;
    private int old_salary_exp;

    private int current_real_salary;
    private int current_visual_salary;
    Thread thread_update_salary;

    private int current_real_money;
    private int current_visual_money;
    Thread thread_update_money;

    private RecyclerView chat;
    //int defaultChatHeight;
    int defaultChatFontSize;

    DecimalFormat formatter;

    public native void HudInit();
    public native void ClickEnterPassengerButton();
    public native void ClickEnterExitVehicleButton();
    public native void ClickLockVehicleButton();
    public native void PressedHorn(boolean pressed);
    public native void SetRadarBgPos(float x1, float y1, float x2, float y2);
    public native void SetRadarPos(float x1, float y1);
    public native void ToggleKeyBoard(boolean toggle);
    public native void SendChatMessage(byte str[]);
    public native void SendChatButton(int buttonID);
    public native void ChatSetCursor(int start, int end);
    ChatAdapter adapter;

    ArrayList<String> chat_lines = new ArrayList<>();

    @SuppressLint("ClickableViewAccessibility")
    public HudManager(Activity aactivity) {
        activity = aactivity;

        salary_job_layout = activity.findViewById(R.id.salary_job_layout);
        salary_job_layout.setVisibility(View.GONE);
        salary_job_salary_text = activity.findViewById(R.id.salary_job_salary_text);
        salary_job_lvl_text = activity.findViewById(R.id.salary_job_lvl_text);
        salary_job_progress = activity.findViewById(R.id.salary_job_progress);
        salary_job_exp_text = activity.findViewById(R.id.salary_job_exp_text);

        // TODO hueta
        String selectedServer = NativeStorage.getClientProperty(NativeStorageElements.SERVER, activity);
        InitServerLogo(Integer.parseInt(selectedServer));

        armour_text = activity.findViewById(R.id.armour_text);
        hp_text = activity.findViewById(R.id.hp_text);

        // ================ CHAT
        chat_box = activity.findViewById(R.id.chat_box);

        hide_chat = activity.findViewById(R.id.hide_chat);
        hide_chat.setOnClickListener(view -> {
            if(chat_box.getVisibility() == View.GONE){
                activity.runOnUiThread(() -> {
                    Utils.ShowLayout(chat_box, true);
                    hide_chat.setScaleY(1);
                });
            } else{
                activity.runOnUiThread(() -> {
                    Utils.HideLayout(chat_box, true);
                    hide_chat.setScaleY(-1);
                });
            }
        });

        me_button = activity.findViewById(R.id.me_button);
        me_button.setOnClickListener(view -> {
            if(chat_button == ME_BUTTON){
                me_button.setBackgroundTintList(null);
                chat_button = INVALID;
            }else {
                chat_button = ME_BUTTON;
                me_button.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#9c27b0")));
                try_button.setBackgroundTintList(null);
                do_button.setBackgroundTintList(null);
            }
            SendChatButton(chat_button);
        });

        try_button = activity.findViewById(R.id.try_button);
        try_button.setOnClickListener(view -> {
            if(chat_button == TRY_BUTTON){
                try_button.setBackgroundTintList(null);
                chat_button = INVALID;
            }else {
                chat_button = TRY_BUTTON;
                try_button.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#087f23")));
                me_button.setBackgroundTintList(null);
                do_button.setBackgroundTintList(null);
            }
            SendChatButton(chat_button);
        });

        do_button = activity.findViewById(R.id.do_button);
        do_button.setOnClickListener(view -> {
            if(chat_button == DO_BUTTON){
                do_button.setBackgroundTintList(null);
                chat_button = INVALID;
            }else {
                chat_button = DO_BUTTON;
                do_button.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#c67100")));
                try_button.setBackgroundTintList(null);
                me_button.setBackgroundTintList(null);
            }
            SendChatButton(chat_button);
        });

        chat_input_layout = activity.findViewById(R.id.chat_input_layout);
        chat_input_layout.setVisibility(View.GONE);
        chat_input = activity.findViewById(R.id.chat_input);
        chat_input.setShowSoftInputOnFocus(false);

       // String[] cities = {"Москва", "Самара", "Вологда", "Волгоград", "Саратов", "Воронеж"};
       // AutoCompleteTextView autoCompleteTextView = findViewById(R.id.autocomplete);
        // Создаем адаптер для автозаполнения элемента AutoCompleteTextView
      //  ArrayAdapter<String> autocompleete = new ArrayAdapter (activity, R.layout.chat_autocompleete_line, cities);
      //  chat_input.setAdapter(autocompleete);

//        chat_input.setOnEditorActionListener(new EditText.OnEditorActionListener() {
//            @Override
//            public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
//                if (actionId == EditorInfo.IME_ACTION_SEND) {
//                    try {
//                        SendChatMessage(chat_input.getText().toString().getBytes("windows-1251"));
//                    } catch (UnsupportedEncodingException e) {
//                        e.printStackTrace();
//                    }
//                    chat_input.getText().clear();
//                    ClickChatj();
//                    return true;
//                }
//                return false;
//            }
//        });

        defaultChatFontSize = 27;

        chat = activity.findViewById(R.id.chat);
        //defaultChatHeight = chat.getLayoutParams().height;

        LinearLayoutManager mLayoutManager = new LinearLayoutManager(activity);

        mLayoutManager.setStackFromEnd(true);
        chat.setLayoutManager(mLayoutManager);


        // создаем адаптер
        adapter = new ChatAdapter(activity, chat_lines);
        // устанавливаем для списка адаптер
        chat.setAdapter(adapter);

        HudInit();
        DecimalFormatSymbols otherSymbols = new DecimalFormatSymbols(Locale.getDefault());
        otherSymbols.setGroupingSeparator('.');
        formatter = new DecimalFormat("###,###.###", otherSymbols);

        hud_bg = activity.findViewById(R.id.hud_bg);
        // OPG WAR
        opg_war_layout = activity.findViewById(R.id.opg_war_layout);
        opg_attacker_score = activity.findViewById(R.id.opg_attacker_score);
        opg_defender_score = activity.findViewById(R.id.opg_defender_score);
        opg_time_text = activity.findViewById(R.id.opg_time_text);

        opg_war_layout.setVisibility(View.GONE);

        // кнопка сигнала
        button_horn = activity.findViewById(R.id.button_horn);
        button_horn.setVisibility(View.GONE);

        button_horn.setOnTouchListener(new View.OnTouchListener(){
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN: // нажатие
                    case MotionEvent.ACTION_MOVE:
                        PressedHorn(true);
                        break;
                    case MotionEvent.ACTION_UP: // отпускание
                    case MotionEvent.ACTION_CANCEL:
                        PressedHorn(false);
                        break;
                }
                return true;
            }
        });
        // кнопка закрыть/открыть тачку
        lock_vehicle = activity.findViewById(R.id.vehicle_lock_butt);
        lock_vehicle.setVisibility(View.GONE);
        lock_vehicle.setOnClickListener(view -> {
            long currTime = System.currentTimeMillis()/1000;
            if(buttonLockCD > currTime)
            {
                return;
            }
            buttonLockCD = currTime+2;
            ClickLockVehicleButton();
        });

        // кнопка сесть/вылезть водителем
        enterexit_driver = activity.findViewById(R.id.enterexit_driver);
        enterexit_driver.setVisibility(View.GONE);
        enterexit_driver.setOnClickListener(view -> {
            ClickEnterExitVehicleButton();
        });

        // кнопка сесть пассажиром
        enter_passenger = activity.findViewById(R.id.enter_passenger);
        enter_passenger.setVisibility(View.GONE);
        enter_passenger.setOnClickListener(view ->
        {
            ClickEnterPassengerButton();
        });

        ///
        hud_main = aactivity.findViewById(R.id.hud_main);
       // hud_main.setVisibility(View.GONE);

        bus_layout = aactivity.findViewById(R.id.bus_layout);
        bus_layout.setVisibility(View.GONE);

        bus_text = aactivity.findViewById(R.id.bus_text);

        target_notify = aactivity.findViewById(R.id.target_notify);
        target_notify.setVisibility(View.GONE);

        target_notify_exit = aactivity.findViewById(R.id.target_notify_exit);
        target_notify_status = aactivity.findViewById(R.id.target_notify_status);
        target_notify_text = aactivity.findViewById(R.id.target_notify_text);

        hud_greenzone = aactivity.findViewById(R.id.hud_greenzone);
        hud_greenzone.setVisibility(View.GONE);
        hud_gpsactive = aactivity.findViewById(R.id.hud_gpsactive);
        hud_gpsactive.setVisibility(View.GONE);
        hud_serverlogo = aactivity.findViewById(R.id.hud_logo);
        hud_serverlogo.setVisibility(View.GONE);

        levelinfo = aactivity.findViewById(R.id.levelinfo);

        hud_ammo = aactivity.findViewById(R.id.hud_ammo);

        progressArmor = aactivity.findViewById(R.id.hud_armor_pb);
        progressHP = aactivity.findViewById(R.id.hud_health_pb);

        hud_money = aactivity.findViewById(R.id.hud_money);
        hud_money_dif = activity.findViewById(R.id.hud_money_dif);
        hud_weapon = aactivity.findViewById(R.id.hud_weapon);
        hud_menu = aactivity.findViewById(R.id.hud_menu);

        hud_wanted = new ArrayList<>();
        hud_wanted.add(activity.findViewById(R.id.hud_star_1));
        hud_wanted.add(activity.findViewById(R.id.hud_star_2));
        hud_wanted.add(activity.findViewById(R.id.hud_star_3));
        hud_wanted.add(activity.findViewById(R.id.hud_star_4));
        hud_wanted.add(activity.findViewById(R.id.hud_star_5));
        hud_wanted.add(activity.findViewById(R.id.hud_star_6));
        for (int i = 0; i < hud_wanted.size(); i++) {
            hud_wanted.get(i).setVisibility(View.GONE);
        }

        hud_menu.setOnClickListener( view -> {
            NvEventQueueActivity.getInstance().showMenu();
         //   NvEventQueueActivity.getInstance().togglePlayer(1);
        });
        hud_weapon.setOnClickListener(v -> NvEventQueueActivity.getInstance().onWeaponChanged());

        hud_bg.post(() -> {
            if(isHudSetPos)return;

            SetRadarBgPos(hud_bg.getX(), hud_bg.getY(), hud_bg.getX()+hud_bg.getWidth(), hud_bg.getY()+hud_bg.getHeight());


            //
           // int screenwidth = activity.getResources().getDisplayMetrics().widthPixels;
            int screenwidth = hud_main.getWidth();
            int screenheight = hud_main.getHeight();


            float real_prcX= ( ( hud_bg.getX() + (hud_bg.getWidth()/2)) /screenwidth)*100;
            float real_prcY = ( ( hud_bg.getY() + (hud_bg.getHeight()/2.2f) ) /screenheight)*100;

            float gtaX =  (640*(real_prcX/100f));
            float gtaY =  (480*(real_prcY/100f));

//            float real_prcscaleX = ( ( hud_bg.getX()+hud_bg.getWidth() ) /screenwidth)*100;
//            float real_prcscaleY = ( ( hud_bg.getY()+hud_bg.getHeight() ) /screenheight)*100;
//
//            float gtaX =  (640*(real_prcX/100f));
//            float gtaY =  (480*(real_prcY/100f));

            SetRadarPos(gtaX, gtaY);

            activity.runOnUiThread(() -> {
                hud_main.setVisibility(View.GONE);
                hud_bg.setVisibility(View.GONE);
            });

            isHudSetPos = true;
        });

        Utils.HideLayout(hud_gpsactive, false);
    }

    public void ToggleHpText(boolean toggle)
    {
        activity.runOnUiThread(() -> {
            if(!toggle){
                armour_text.setVisibility(View.GONE);
                hp_text.setVisibility(View.GONE);
            } else {
                armour_text.setVisibility(View.VISIBLE);
                hp_text.setVisibility(View.VISIBLE);
            }
        });

    }
    public void ToggleChat(boolean toggle){
        activity.runOnUiThread(()-> {
            if(toggle){
                chat.setVisibility(View.VISIBLE);
            } else {
                chat.setVisibility(View.GONE);
            }
        });
    }
    public void AddChatMessage(String msg){
        adapter.addItem(msg);
    }
    public void ToggleEnterPassengerButton(boolean toggle)
    {
        if(toggle)
        {
            activity.runOnUiThread(() -> Utils.ShowLayout(enter_passenger, true) );
        }
        else
        {
            activity.runOnUiThread(() -> Utils.HideLayout(enter_passenger, true) );
        }
    }
    public void UpdateOpgWarLayout(int time, int attackscore, int defscore)
    {
        activity.runOnUiThread(() -> {
            if(time == 0)
            {
                opg_war_layout.setVisibility(View.GONE);
                return;
            }
            opg_war_layout.setVisibility(View.VISIBLE);
            opg_attacker_score.setText(String.format("Нападающая ОПГ: %d", attackscore));
            opg_defender_score.setText(String.format("Защищающаяся ОПГ: %d", defscore));
            opg_time_text.setText(String.format("%d секунд", time));
        } );

    }
    public void ToggleHornButton(boolean toggle)
    {
        if(toggle)
        {
            activity.runOnUiThread(() -> Utils.ShowLayout(button_horn, true) );
        }
        else
        {
            activity.runOnUiThread(() -> Utils.HideLayout(button_horn, true) );
        }
    }
    public void ToggleEnterExitVehicleButton(boolean toggle)
    {
        if(toggle)
        {
            activity.runOnUiThread(() -> Utils.ShowLayout(enterexit_driver, true) );
        }
        else
        {
            activity.runOnUiThread(() -> Utils.HideLayout(enterexit_driver, true) );
        }
    }
    public void ToggleLockVehicleButton(boolean toggle)
    {
        if(toggle)
        {
            activity.runOnUiThread(() -> Utils.ShowLayout(lock_vehicle, true) );
        }
        else
        {
            activity.runOnUiThread(() -> Utils.HideLayout(lock_vehicle, true) );
        }
    }
    public void ChangeChatHeight(int height)
    {
        activity.runOnUiThread(() -> {
//            FrameLayout.LayoutParams layoutParams = (FrameLayout.LayoutParams) chat.getLayoutParams();
//            if(height == -1){
//                layoutParams.height = defaultChatHeight;
//            }else{
//                layoutParams.height = height;
//            }
//            chat.setLayoutParams(layoutParams);
        });
    }
    public void ChangeChatFontSize(int size)
    {
        activity.runOnUiThread(() -> {
           // TextView chat_line = activity.findViewById(R.id.chat_line_text);
           // TextView chat_line_shadow = activity.findViewById(R.id.chat_line_shadow);
            if(size == -1){
                chatFontSize = defaultChatFontSize;
             //   chat_line.setTextSize(TypedValue.COMPLEX_UNIT_PX, defaultChatFontSize);
               // chat_line_shadow.setTextSize(TypedValue.COMPLEX_UNIT_PX, defaultChatFontSize);
            }else{
                chatFontSize = size;
//                chat_line.setTextSize(TypedValue.COMPLEX_UNIT_PX, size);
//                chat_line_shadow.setTextSize(TypedValue.COMPLEX_UNIT_PX, size);
            }
            adapter = new ChatAdapter(activity, adapter.getItems());
            // устанавливаем для списка адаптер
            chat.setAdapter(adapter);
        });
    }

    public void UpdateHudInfo(int health, int armour, int hunger, int weaponid, int ammo, int ammoclip)
    {
        activity.runOnUiThread(() ->
        {
            progressHP.setProgress(health);
            progressArmor.setProgress(armour);
            armour_text.setText(String.format("%d", armour));
            hp_text.setText(String.format("%d", health));

            int id = activity.getResources().getIdentifier(String.format("weapon_%d", weaponid), "drawable", activity.getPackageName());
            if(id > 0)hud_weapon.setImageResource(id);

            if (weaponid > 15 & weaponid < 44 & weaponid != 21) {
                Utils.ShowLayout(hud_ammo, false);
                String ss = String.format("%d<font color='#B0B0B0'>/%d</font>", ammoclip, ammo - ammoclip);
                hud_ammo.setText(Html.fromHtml(ss));
            } else {
                hud_ammo.setVisibility(View.GONE);
            }
        });
    }
    public void UpdateMoney(int money)
    {
        int old_money = current_real_money;
        current_real_money = money;

        int reference = ( current_real_money - old_money );

        TimerTask task = new TimerTask() {
            public void run() {
                activity.runOnUiThread(() -> {
                    hud_money_dif.setVisibility(View.GONE);
                });
                if(thread_update_money!= null && thread_update_money.isAlive()){
                    thread_update_money.interrupt();
                }

                try {
                    thread_update_money = new Thread(runnable_update_money);
                    thread_update_money.start();
                } catch (Exception e) {
                    activity.runOnUiThread(() -> {
                        hud_money.setText(formatter.format(current_visual_money));
                    });
                }

                timer = null;
            }
        };

        if(old_money > 0){
            if(reference < 0 ) {//потратил
                activity.runOnUiThread(() -> {
                    hud_money_dif.setTextColor(Color.parseColor("#f44336"));
                    hud_money_dif.setText(formatter.format(reference));
                    hud_money_dif.setVisibility(View.VISIBLE);
                });
                if(timer != null) timer.cancel();
                Timer timer = new Timer("Timer");
                timer.schedule(task, 4000L);
            }else {
                activity.runOnUiThread(() -> {
                    hud_money_dif.setTextColor(Color.parseColor("#76ff03"));
                    hud_money_dif.setText("+" + formatter.format(reference));
                    hud_money_dif.setVisibility(View.VISIBLE);
                });
                if(timer != null) timer.cancel();
                Timer timer = new Timer("Timer");
                timer.schedule(task, 4000L);
            }
        } else {
            if(thread_update_money!= null && thread_update_money.isAlive()){
                thread_update_money.interrupt();
            }
            try {
                thread_update_money = new Thread(runnable_update_money);
                thread_update_money.start();
            } catch (Exception e) {
                activity.runOnUiThread(() -> {
                    hud_money.setText(formatter.format(current_visual_money));
                });
            }
        }



    }
    Runnable runnable_update_money = new Runnable() { // так вот такая вот хуйня ради маленького эффекта денег )
        @Override
        public void run() {
            while (current_visual_money < current_real_money){
                current_visual_money += Math.ceil( Math.abs(current_real_money - current_visual_money)*0.005 );
                if(current_visual_money > current_real_money) {
                    current_visual_money = current_real_money;
                }
                activity.runOnUiThread(() -> {
                    hud_money.setText(formatter.format(current_visual_money));
                });
                try {
                    sleep(10);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            while (current_visual_money > current_real_money) {
                current_visual_money -= Math.ceil(  Math.abs(current_real_money - current_visual_money)*0.005  );
                if(current_visual_money < current_real_money) {
                    current_visual_money = current_real_money;
                }
                activity.runOnUiThread(() -> {
                    hud_money.setText(formatter.format(current_visual_money));
                });

                try {
                    sleep(10);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    };
    public void UpdateWanted(int wantedLVL)
    {
        activity.runOnUiThread(() -> {
            for (int i = 0; i < hud_wanted.size(); i++) {
                if (i < wantedLVL) {
                    hud_wanted.get(i).setVisibility(View.VISIBLE);
                } else {
                    hud_wanted.get(i).setVisibility(View.GONE);
                }
            }
        });
    }
    public void ToggleAll(boolean toggle)
    {
        if(!toggle)
        {
            activity.runOnUiThread(() -> hud_main.setVisibility(View.GONE) );
        }
        else
        {
            activity.runOnUiThread(() -> hud_main.setVisibility(View.VISIBLE) );
        }
    }

    public void ShowGreenZone() { Utils.ShowLayout(hud_greenzone, true); }

    public void HideGreenZone()
    {
        Utils.HideLayout(hud_greenzone, true);
    }

    public void ShowGPS() { Utils.ShowLayout(hud_gpsactive, true); }

    public void HideGPS()
    {
        Utils.HideLayout(hud_gpsactive, true);
    }

    public void InitServerLogo(int serverid) {

//        activity.runOnUiThread(() -> {
//            MainUtils global = (MainUtils)activity.getApplication();
//            List<Servers> list = global.SERVERS;
//            Servers servers = list.get(serverid);
//
//            ImageView img = activity.findViewById(R.id.hud_logo_img);
//            TextView text = activity.findViewById(R.id.hud_logo_text);
//
//            int MainColor = Color.parseColor(servers.getColor());
//
//            img.setColorFilter(MainColor, PorterDuff.Mode.SRC_ATOP);
//
//            text.setTextColor(MainColor);
////            text.setShadowLayer(1.0f, 1, 1, MainColor);
//
////            hud_serverlogo.setVisibility(View.VISIBLE);
//
//           // hud_serverlogo.post(() ->{
//            text.setText(servers.getname()+" ");
//            });

    }
    public void HideServerLogo()
    {
        Utils.HideLayout(hud_serverlogo, false);
    }
    public void ShowServerLogo()
    {
        Utils.ShowLayout(hud_serverlogo, false);
    }
//    public void HideServer() { Utils.HideLayout(hud_serverinfo, true); }

    public void UpdateLevelInfo(int level, int currentexp, int maxexp) {
        String strlevelinfo = String.format("LVL %d [EXP %d/%d]", level, currentexp, maxexp);
        levelinfo.setText(strlevelinfo);
    }

    @SuppressLint("DefaultLocale")
    public void UpdateSalary(int salary, int lvl, float exp) {
        if(current_visual_salary > salary)
        {
            current_visual_salary = 0;
        }
        //current_visual_salary = 0;
       // current_visual_salary = current_real_salary;
        current_real_salary = salary;

        activity.runOnUiThread(() -> {
            salary_job_exp_text.setText(String.format("%.2f / 100", exp));
            salary_job_lvl_text.setText(String.format("Ваш уровень работника: %d", lvl));

            if (old_salary_exp > salary) {
                salary_job_progress.setProgress(0);
            }
            salary_job_progress.setProgress(exp);

            salary_job_layout.setVisibility(View.VISIBLE);
        });
        if(thread_update_salary != null && thread_update_salary.isAlive()){
            thread_update_salary.interrupt();
        }
        if(salary == 0) {
            current_visual_salary = 0;
            activity.runOnUiThread(() -> salary_job_salary_text.setText("Заработано: 0 рублей"));

        } else {
            thread_update_salary = new Thread(runnable_update_salary_money);
            thread_update_salary.start();
        }
        TimerTask task = new TimerTask() {
            public void run() {
                activity.runOnUiThread(() -> {
                    salary_job_layout.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.popup_hide_to_top));
                    salary_job_layout.setVisibility(View.GONE);
                });
            }
        };
        Timer timer = new Timer("Timer");

        timer.schedule(task, 5000L);
    }

    Runnable runnable_update_salary_money = new Runnable() { // так вот такая вот хуйня ради маленького эффекта денег )
        @Override
        public void run() {
            while (current_visual_salary < current_real_salary){
                current_visual_salary ++;
                activity.runOnUiThread(() -> {
                    salary_job_salary_text.setText(String.format("Заработано: %s рублей", formatter.format(current_visual_salary)));
                });

                try {
                    sleep(10);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    };


    public void ShowUpdateTargetNotify(int type, String text) {
        Utils.ShowLayout(target_notify, true);
        if (type == 0)
        {
            target_notify_text.setText(text);
            target_notify_text.setTextColor(0xFFFFFFFF);
            target_notify_status.setImageResource(R.drawable.target_notify_none);
        }
        else if (type == 1)
        {
            target_notify_text.setText(text);
            target_notify_text.setTextColor(0xFF33C44F);
            target_notify_status.setImageResource(R.drawable.target_notify_success);
        }
        target_notify_exit.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                NvEventQueueActivity.getInstance().onTargetNotifyClose();
            }
        });
    }

    public void HideTargetNotify() {
        Utils.HideLayout(target_notify, true);
    }

    public void ShowBusInfo(int time) {
        bus_text.setText(String.format("%d", time));
        bus_layout.setVisibility(View.VISIBLE);
    }

    public void HideBusInfo() { bus_layout.setVisibility(View.GONE); }

    public void AddToChatInput(String msg){
        activity.runOnUiThread(() -> {
            chat_input.setText(msg);
            // Log.d("af", "start = "+chat_input.getSelectionStart()+" size = "+chat_input.getText().length());
           // if(!msg.isEmpty()) {
             //   if (cursorPos == chat_input.getText().length()-1) {
                    int len = chat_input.getText().length();
                    if(len >= 0) chat_input.setSelection(len);
              //      cursorPos = chat_input.getSelectionStart();
              //  }
          //  }
          //  else{
           //     cursorPos = 0;
           // }
           //
        });

    }
    public void ClickChatj(){
        activity.runOnUiThread(() -> {
            InputMethodManager imm = (InputMethodManager) activity.getSystemService(Context.INPUT_METHOD_SERVICE);
            if (chat_input_layout.getVisibility() == View.VISIBLE) {
                chat_input_layout.setVisibility(View.GONE);
                ToggleKeyBoard(false);
                cursorPos = 0;
                //   imm.hideSoftInputFromWindow(chat_input.getWindowToken(), 0);

            } else {
                chat_input_layout.setVisibility(View.VISIBLE);
                chat_input.requestFocus();
                ToggleKeyBoard(true);
                cursorPos = chat_input.getText().length();
               //  imm.showSoftInput(chat_input, InputMethodManager.SHOW_IMPLICIT);
            }
        });

       // ClickChat();
    }
    public void ToggleChatInput(boolean toggle){
        activity.runOnUiThread(() ->
        {
            if(toggle){
                chat_input_layout.setVisibility(View.VISIBLE);
            }else {
                chat_input_layout.setVisibility(View.GONE);
            }
        });
    }
    public class ChatAdapter  extends RecyclerView.Adapter<ChatAdapter.ViewHolder>{

        private final LayoutInflater inflater;
        private List<String> chat_lines;

        ChatAdapter(Context context, List<String> chat_lines) {
            this.chat_lines = chat_lines;
            this.inflater = LayoutInflater.from(context);
        }
        @NotNull
        @Override
        public ChatAdapter.ViewHolder onCreateViewHolder(@NotNull ViewGroup parent, int viewType) {

            View view = inflater.inflate(R.layout.chatline, parent, false);
            view.setOnClickListener(view1 -> {
                //ClickChat();
                ClickChatj();
            });
            return new ViewHolder(view);
        }

        @Override
        public void onBindViewHolder(ChatAdapter.ViewHolder holder, int position) {
            holder.chat_line_text.setText(Html.fromHtml(chat_lines.get(position)));
            holder.chat_line_shadow.setText(Html.fromHtml(chat_lines.get(position)).toString());

            holder.chat_line_shadow.setTextSize(TypedValue.COMPLEX_UNIT_PX, chatFontSize);
            holder.chat_line_text.setTextSize(TypedValue.COMPLEX_UNIT_PX, chatFontSize);
        }

        @Override
        public int getItemCount() {
            return chat_lines.size();
        }

        public List getItems() {
            return chat_lines;
        }

        public class ViewHolder extends RecyclerView.ViewHolder {
            final TextView chat_line_text;
            final TextView chat_line_shadow;
            ViewHolder(View view){
                super(view);
                chat_line_text = view.findViewById(R.id.chat_line_text);
                chat_line_shadow = view.findViewById(R.id.chat_line_shadow);
            }
        }
        public void addItem(String item) {
            activity.runOnUiThread(() -> {
                if(this.chat_lines.size() > 40){
                    this.chat_lines.remove(0);
                    notifyItemRemoved(0);
                }
                this.chat_lines.add(" "+item+" ");
                notifyItemInserted(this.chat_lines.size()-1);

                if(chat.getScrollState() == RecyclerView.SCROLL_STATE_IDLE) {
                    chat.scrollToPosition(this.chat_lines.size()-1);
                }
            });

        }
    }
}

