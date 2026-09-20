package com.shahdag.game;

import android.app.Activity;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;
import android.graphics.Color;
import android.graphics.Typeface;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.FrameLayout;
import android.view.Gravity;

public class MainActivity extends Activity {
    FrameLayout root;
    GameGLView gameView;

    @Override public void onCreate(Bundle b) {
        super.onCreate(b);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().setNavigationBarColor(Color.BLACK);
        showMainMenu();
    }

    TextView title(String t, int size) {
        TextView v = new TextView(this);
        v.setText(t); v.setTextColor(Color.WHITE); v.setTextSize(size); v.setTypeface(Typeface.DEFAULT_BOLD);
        v.setGravity(Gravity.CENTER);
        return v;
    }

    Button button(String text) {
        Button b = new Button(this);
        b.setText(text); b.setTextColor(Color.WHITE); b.setTextSize(16); b.setAllCaps(false);
        b.setBackgroundColor(Color.rgb(35,35,35)); b.setPadding(18, 8, 18, 8);
        return b;
    }

    void showMainMenu() {
        LinearLayout box = new LinearLayout(this); box.setOrientation(LinearLayout.VERTICAL); box.setGravity(Gravity.CENTER); box.setPadding(40,40,40,40); box.setBackgroundColor(Color.rgb(4,4,5));
        TextView t = title("SHAHDAG", 38); box.addView(t, new LinearLayout.LayoutParams(-1, 80));
        TextView s = title("MOBILE ALPHA • RESIDENTIAL OPERATION", 14); s.setTextColor(Color.LTGRAY); box.addView(s, new LinearLayout.LayoutParams(-1, 50));
        Button play = button("PLAY"); Button settings = button("SETTINGS"); Button exit = button("EXIT");
        box.addView(play, new LinearLayout.LayoutParams(420,65)); box.addView(settings, new LinearLayout.LayoutParams(420,65)); box.addView(exit, new LinearLayout.LayoutParams(420,65));
        play.setOnClickListener(v -> showLoadout()); settings.setOnClickListener(v -> showSettings()); exit.setOnClickListener(v -> finish());
        root = new FrameLayout(this); root.addView(box); setContentView(root);
    }

    void showLoadout() {
        LinearLayout box = new LinearLayout(this); box.setOrientation(LinearLayout.VERTICAL); box.setGravity(Gravity.CENTER); box.setPadding(36,30,36,30); box.setBackgroundColor(Color.rgb(6,7,8));
        box.addView(title("RESIDENTIAL OPERATION", 28), new LinearLayout.LayoutParams(-1,70));
        TextView info = title("MAP 01 • SMALL SUBURBAN HOUSE\nPRIMARY: CARBINE • OPTIC\nSECONDARY: PISTOL\nEQUIPMENT: FLASH / BREACH", 15); info.setTextColor(Color.LTGRAY); box.addView(info, new LinearLayout.LayoutParams(-1,170));
        Button start = button("DEPLOY"); Button back = button("BACK");
        box.addView(start,new LinearLayout.LayoutParams(360,65)); box.addView(back,new LinearLayout.LayoutParams(360,65));
        start.setOnClickListener(v -> startGame()); back.setOnClickListener(v -> showMainMenu());
        root = new FrameLayout(this); root.addView(box); setContentView(root);
    }

    void showSettings() {
        LinearLayout box = new LinearLayout(this); box.setOrientation(LinearLayout.VERTICAL); box.setGravity(Gravity.CENTER); box.setBackgroundColor(Color.rgb(5,5,6));
        TextView t = title("SETTINGS", 28); box.addView(t,new LinearLayout.LayoutParams(-1,70));
        TextView info = title("GRAPHICS: MOBILE PERFORMANCE\nFPS CAP: 60 / 90\nLOOK SENSITIVITY: 1.0\nAUDIO: 100%",15); info.setTextColor(Color.LTGRAY); box.addView(info,new LinearLayout.LayoutParams(-1,160));
        Button back = button("BACK"); box.addView(back,new LinearLayout.LayoutParams(360,65)); back.setOnClickListener(v->showMainMenu());
        root = new FrameLayout(this); root.addView(box); setContentView(root);
    }

    void startGame() {
        gameView = new GameGLView(this); setContentView(gameView); gameView.requestFocus();
    }

    @Override protected void onPause() { super.onPause(); if(gameView != null) gameView.onPause(); }
    @Override protected void onResume() { super.onResume(); if(gameView != null) gameView.onResume(); }
}
