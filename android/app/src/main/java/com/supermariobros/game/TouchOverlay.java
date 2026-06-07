package com.supermariobros.game;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.graphics.Typeface;
import android.util.DisplayMetrics;
import android.view.MotionEvent;
import android.view.View;

import java.util.HashMap;
import java.util.Map;

/**
 * Transparent overlay view drawn on top of the SDL surface.
 * Renders a D-pad on the left and action buttons on the right.
 *
 * Button IDs (must match AndroidJNI.cpp keyFromId):
 *   0 = LEFT, 1 = RIGHT, 2 = DUCK, 3 = JUMP
 *   4 = SPRINT, 5 = FIREBALL, 6 = PAUSE
 *   7 = MENU_ACCEPT, 8 = MENU_ESCAPE
 */
public class TouchOverlay extends View {

    // JNI methods implemented in AndroidJNI.cpp
    public static native void nativeSetKey(int keyId, boolean pressed);
    public static native void nativePressKey(int keyId);

    // Button IDs
    static final int KEY_LEFT        = 0;
    static final int KEY_RIGHT       = 1;
    static final int KEY_DUCK        = 2;
    static final int KEY_JUMP        = 3;
    static final int KEY_SPRINT      = 4;
    static final int KEY_FIREBALL    = 5;
    static final int KEY_PAUSE       = 6;
    static final int KEY_ACCEPT      = 7;
    static final int KEY_ESCAPE      = 8;

    private static class Button {
        int id;
        RectF rect;
        String label;
        boolean held = false;
        boolean isToggle = false; // true = one-shot tap

        Button(int id, RectF rect, String label, boolean isToggle) {
            this.id = id;
            this.rect = rect;
            this.label = label;
            this.isToggle = isToggle;
        }
    }

    private final Button[] buttons;
    private final Paint fillPaint   = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final Paint heldPaint   = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final Paint strokePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final Paint textPaint   = new Paint(Paint.ANTI_ALIAS_FLAG);

    // Map from pointer ID → button ID (for multi-touch)
    private final Map<Integer, Integer> pointerToButton = new HashMap<>();

    public TouchOverlay(Context context) {
        super(context);
        setClickable(true);

        DisplayMetrics dm = context.getResources().getDisplayMetrics();
        float sw = dm.widthPixels;
        float sh = dm.heightPixels;

        float pad = sh * 0.04f;   // outer padding
        float bs  = sh * 0.14f;   // base button size
        float gap = sh * 0.02f;   // gap between buttons

        // ── D-Pad (left side) ──────────────────────────────────────────
        float dpadCX = pad + bs * 1.5f;
        float dpadCY = sh - pad - bs * 1.5f;

        // LEFT / RIGHT / DUCK in a row
        float leftX  = dpadCX - bs - gap;
        float rightX = dpadCX + gap;
        float row1Y  = dpadCY - bs * 0.5f;

        // DUCK below center
        float duckX  = dpadCX - bs * 0.5f;
        float duckY  = dpadCY + gap;

        // ── Action buttons (right side) ────────────────────────────────
        float abRight = sw - pad;
        float abY     = sh - pad - bs;

        // JUMP  (A) — rightmost
        float jumpX   = abRight - bs;
        // SPRINT (B) — left of JUMP
        float sprintX = jumpX - bs - gap;
        // FIREBALL (Y) — above SPRINT
        float fbX     = sprintX;
        float fbY     = abY - bs - gap;

        // ── Pause button (top right) ───────────────────────────────────
        float pauseSize = sh * 0.07f;
        float pauseX    = sw - pad - pauseSize;
        float pauseY    = pad;

        buttons = new Button[]{
            new Button(KEY_LEFT,     rect(leftX,  row1Y, bs, bs), "◀", false),
            new Button(KEY_RIGHT,    rect(rightX, row1Y, bs, bs), "▶", false),
            new Button(KEY_DUCK,     rect(duckX,  duckY, bs, bs), "▼", false),
            new Button(KEY_JUMP,     rect(jumpX,  abY,   bs, bs), "A",  false),
            new Button(KEY_SPRINT,   rect(sprintX,abY,   bs, bs), "B",  false),
            new Button(KEY_FIREBALL, rect(fbX,    fbY,   bs, bs), "Y",  false),
            new Button(KEY_PAUSE,    rect(pauseX, pauseY, pauseSize, pauseSize), "❙❙", true),
        };

        // Paints
        fillPaint.setColor(Color.argb(100, 255, 255, 255));
        fillPaint.setStyle(Paint.Style.FILL);

        heldPaint.setColor(Color.argb(180, 255, 220, 50));
        heldPaint.setStyle(Paint.Style.FILL);

        strokePaint.setColor(Color.argb(160, 255, 255, 255));
        strokePaint.setStyle(Paint.Style.STROKE);
        strokePaint.setStrokeWidth(3f);

        textPaint.setColor(Color.WHITE);
        textPaint.setTextAlign(Paint.Align.CENTER);
        textPaint.setTypeface(Typeface.DEFAULT_BOLD);
    }

    private static RectF rect(float x, float y, float w, float h) {
        return new RectF(x, y, x + w, y + h);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        for (Button b : buttons) {
            float radius = b.rect.height() * 0.3f;
            canvas.drawRoundRect(b.rect, radius, radius, b.held ? heldPaint : fillPaint);
            canvas.drawRoundRect(b.rect, radius, radius, strokePaint);
            textPaint.setTextSize(b.rect.height() * 0.40f);
            canvas.drawText(b.label,
                b.rect.centerX(),
                b.rect.centerY() + textPaint.getTextSize() * 0.35f,
                textPaint);
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int action = event.getActionMasked();
        int pIdx   = event.getActionIndex();
        int pId    = event.getPointerId(pIdx);

        switch (action) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN: {
                float x = event.getX(pIdx);
                float y = event.getY(pIdx);
                Button hit = hitTest(x, y);
                if (hit != null) {
                    pointerToButton.put(pId, hit.id);
                    pressButton(hit, true);
                }
                break;
            }

            case MotionEvent.ACTION_MOVE: {
                // Re-check each active pointer (finger may slide)
                for (int i = 0; i < event.getPointerCount(); i++) {
                    int pid = event.getPointerId(i);
                    float x = event.getX(i);
                    float y = event.getY(i);

                    Integer prevId = pointerToButton.get(pid);
                    Button hit = hitTest(x, y);

                    if (hit == null) {
                        if (prevId != null) {
                            releaseButton(prevId);
                            pointerToButton.remove(pid);
                        }
                    } else if (prevId == null || prevId != hit.id) {
                        if (prevId != null) releaseButton(prevId);
                        pointerToButton.put(pid, hit.id);
                        pressButton(hit, false); // no one-shot on slide
                    }
                }
                break;
            }

            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP:
            case MotionEvent.ACTION_CANCEL: {
                Integer btnId = pointerToButton.remove(pId);
                if (btnId != null) releaseButton(btnId);
                break;
            }
        }
        invalidate();
        return true;
    }

    private Button hitTest(float x, float y) {
        for (Button b : buttons) {
            if (b.rect.contains(x, y)) return b;
        }
        return null;
    }

    private void pressButton(Button b, boolean allowOneShot) {
        if (b.isToggle && allowOneShot) {
            nativePressKey(b.id);
            // Flash briefly
            b.held = true;
            postDelayed(() -> { b.held = false; invalidate(); }, 120);
        } else {
            b.held = true;
            nativeSetKey(b.id, true);
        }
    }

    private void releaseButton(int btnId) {
        for (Button b : buttons) {
            if (b.id == btnId) {
                b.held = false;
                if (!b.isToggle) {
                    nativeSetKey(b.id, false);
                }
                break;
            }
        }
    }
}
