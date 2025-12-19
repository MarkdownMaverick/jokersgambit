#include "mainmenu.h"
#include "useraccount.h"
#include "gui.h"
#include <main.h>
char new_first_name[MAX_ACCOUNT_NAME_LEN + 1] = {0};
char new_last_name[MAX_ACCOUNT_NAME_LEN + 1] = {0};
int current_name_field = 1;
double error_timer = 0.0;
char error_message[128] = {0};
void ShowAccountStatus(GameState *g, const char *msg)
{
    strncpy(g->account_status_message, msg, sizeof(g->account_status_message) - 1);
    g->account_status_timer = GetTime();
}
// ─────────────────────────────────────────────────────────────────────────────
//  MAIN MENU 
// ─────────────────────────────────────────────────────────────────────────────
void DrawMainMenu(GameState *g)
{
    (void)g;
    Rectangle play_rect = {CENTER_X - 150, 300, 300, 60};
    Rectangle accounts_rect = {CENTER_X - 150, 400, 300, 60};
    Rectangle settings_rect = {CENTER_X - 150, 500, 300, 60};
    Rectangle leaderboard_rect = {CENTER_X - 150, 600, 300, 60};
    Vector2 mouse = GetMousePosition();
    DrawText("JOKERS GAMBIT", (int)(CENTER_X - 350.0f), 150, 90, GOLD);
    bool play_hover = CheckCollisionPointRec(mouse, play_rect);
    DrawRectangleRec(play_rect, play_hover ? LIME : GREEN);
    DrawText("PLAY", (int)play_rect.x + 110, (int)play_rect.y + 15, 30, BLACK);
    bool acc_hover = CheckCollisionPointRec(mouse, accounts_rect);
    DrawRectangleRec(accounts_rect, acc_hover ? SKYBLUE : BLUE);
    DrawText("ACCOUNTS", (int)accounts_rect.x + 70, (int)accounts_rect.y + 15, 30, WHITE);
    bool set_hover = CheckCollisionPointRec(mouse, settings_rect);
    DrawRectangleRec(settings_rect, set_hover ? PURPLE : DARKPURPLE);
    DrawText("SETTINGS", (int)settings_rect.x + 75, (int)settings_rect.y + 15, 30, WHITE);
    bool lb_hover = CheckCollisionPointRec(mouse, leaderboard_rect);
    DrawRectangleRec(leaderboard_rect, lb_hover ? YELLOW : GOLD);
    DrawText("LEADERBOARD", (int)leaderboard_rect.x + 30, (int)leaderboard_rect.y + 15, 30, BLACK);
}
void UpdateMainMenu(GameState *g, Vector2 mouse)
{
    Rectangle play_rect = {CENTER_X - 150, 300, 300, 60};
    Rectangle accounts_rect = {CENTER_X - 150, 400, 300, 60};
    Rectangle settings_rect = {CENTER_X - 150, 500, 300, 60};
    Rectangle leaderboard_rect = {CENTER_X - 150, 600, 300, 60};
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (CheckCollisionPointRec(mouse, play_rect))
        {
            // Check if both players are logged in
            if (g->p1_account_index == -1 || g->p2_account_index == -1)
            {
                ShowAccountStatus(g, "Please log in 2 accounts to start");
                g->state = STATE_ACCOUNTS_MANAGER;
            }
            else
            {
                // Determine game mode based on who's logged in
                bool p1_is_ai = g->accounts[g->p1_account_index].is_ai;
                bool p2_is_ai = g->accounts[g->p2_account_index].is_ai;
                if (!p1_is_ai && !p2_is_ai)
                {
                    g->mode = MODE_PVP;
                }
                else if (p1_is_ai && p2_is_ai)
                {
                    g->mode = MODE_AIVSAI;
                }
                else
                {
                    g->mode = MODE_PVAI;
                }
                RestartGameKeepingAccounts(g);
                g->state = STATE_P1_SELECT_DISCARD;
            }
        }
        if (CheckCollisionPointRec(mouse, accounts_rect))
        {
            g->state = STATE_ACCOUNTS_MANAGER;
        }
        if (CheckCollisionPointRec(mouse, settings_rect))
        {
            g->state = STATE_SETTINGS;
        }
        if (CheckCollisionPointRec(mouse, leaderboard_rect))
        {
            g->state = STATE_LEADERBOARD;
        }
    }
}
// ─────────────────────────────────────────────────────────────────────────────
//  SETTINGS SCREEN - AI SELECTION REMOVED, CARD COVER TOGGLE ADDED
// ─────────────────────────────────────────────────────────────────────────────
void DrawSettings(GameState *g)
{
    DrawText("SETTINGS", (int)(CENTER_X - 200.0f), 100, 80, GOLD);
    // Card Cover Toggle
    DrawText("P2 Card Privacy:", (int)(CENTER_X - 300.0f), 300, 40, WHITE);
    Rectangle cover_btn_rect = {CENTER_X - 150, 380, 300, 80};
    Vector2 mouse = GetMousePosition();
    bool cover_hover = CheckCollisionPointRec(mouse, cover_btn_rect);
    Color cover_color = g->cover_p2_cards ? LIME : RED;
    DrawRectangleRec(cover_btn_rect, cover_hover ? Fade(cover_color, 0.8f) : cover_color);
    const char *cover_text = g->cover_p2_cards ? "COVERED" : "UNCOVERED";
    int text_w = MeasureText(cover_text, 35);
    DrawText(cover_text, (int)(CENTER_X - (float)text_w / 2.0f), 380 + 25, 35, BLACK);
    DrawText("(Hides P2 cards from view during local play)", (int)(CENTER_X - 300.0f), 480, 25, LIGHTGRAY);
    // AI Move Speed Setting
    DrawText("AI Move Speed:", (int)(CENTER_X - 300.0f), 550, 40, WHITE);
    Rectangle ai_speed_rect = {CENTER_X - 150, 620, 300, 80};
    bool ai_speed_hover = CheckCollisionPointRec(mouse, ai_speed_rect);
    // Determine current speed text and color
    const char *speed_text;
    Color speed_color;
if (fabsf(g->ai_move_delay - 1.0f) < 0.001f)
    {
        speed_text = "FAST";
        speed_color = LIME;
    }
    else if (fabsf(g->ai_move_delay - 2.0f) < 0.001f)
    {
        speed_text = "MEDIUM";
        speed_color = ORANGE;
    }
    else
    {
        speed_text = "SLOW";
        speed_color = RED;
    }
    DrawRectangleRec(ai_speed_rect, ai_speed_hover ? Fade(speed_color, 0.8f) : speed_color);
    int speed_text_w = MeasureText(speed_text, 35);
    DrawText(speed_text, (int)(CENTER_X - (float)speed_text_w / 2.0f), 620 + 25, 35, BLACK);
    DrawText("(Click to cycle: Fast → Medium → Slow)", (int)(CENTER_X - 270.0f), 720, 25, LIGHTGRAY);
    // Back Button
    Rectangle back_rect = {CENTER_X - 150, SCREEN_H - 180, 300, 80};
    bool back_hover = CheckCollisionPointRec(mouse, back_rect);
    DrawRectangleRec(back_rect, back_hover ? RED : MAROON);
    DrawText("BACK", (int)back_rect.x + 100, (int)(back_rect.y + 25.0f), 35, WHITE);
}
void UpdateSettings(GameState *g, Vector2 mouse)
{
    Rectangle cover_btn_rect = {CENTER_X - 150, 380, 300, 80};
    Rectangle ai_speed_rect = {CENTER_X - 150, 620, 300, 80};
    Rectangle back_rect = {CENTER_X - 150, SCREEN_H - 180, 300, 80};
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (CheckCollisionPointRec(mouse, cover_btn_rect))
        {
            g->cover_p2_cards = !g->cover_p2_cards;
        }
        else if (CheckCollisionPointRec(mouse, ai_speed_rect))
        {
            // Cycle through speeds: Fast (1.0) → Medium (2.0) → Slow (3.0) → Fast
            if (fabsf(g->ai_move_delay - 1.0f) < EPSILON)
            {
                g->ai_move_delay = 2.0f; // Fast → Medium
            }
            else if (fabsf(g->ai_move_delay - 2.0f) < EPSILON)

            {
                g->ai_move_delay = 3.0f; // Medium → Slow
            }
            else
            {
                g->ai_move_delay = 1.0f; // Slow → Fast
            }
        }
        else if (CheckCollisionPointRec(mouse, back_rect))
        {
            g->state = STATE_MAIN_MENU;
        }
    }
}
// ─────────────────────────────────────────────────────────────────────────────
//  ACCOUNT MANAGER - REDESIGNED WITH P1/P2 SLOT SYSTEM
// ─────────────────────────────────────────────────────────────────────────────
void DrawAccountsManager(const GameState *g)
{
    DrawText("ACCOUNT MANAGER", (int)(CENTER_X - 900.0f), 80, 20, GOLD);
    // 1. Draw Current Login Status Top Header
    const char *p1 = (g->p1_account_index == -1) ? "---" : GetPlayerName(g, 1);
    const char *p2 = (g->p2_account_index == -1) ? "---" : GetPlayerName(g, 2);
    DrawText("P1 Current:", 150, 160, 40, LIME);
    DrawText(p1, 400, 160, 40, WHITE);
    DrawText("P2 Current:", 950, 160, 40, ORANGE);
    DrawText(p2, 1200, 160, 40, WHITE);
    // 2. Feedback Message (Green fade text)
    if (GetTime() - g->account_status_timer < 2.0)
    {
        float alpha = 1.0f - (float)(GetTime() - g->account_status_timer) / 2.0f;
        DrawText(g->account_status_message, (int)(CENTER_X - (float)MeasureText(g->account_status_message, 30) / 2.0f), 220, 30, Fade(GREEN, alpha));
    }
    int y = 280;
    // 3. Draw Accounts List
    for (int i = 0; i < g->account_count; i++)
    {
        const Account *a = &g->accounts[i];
        bool is_p1 = (i == g->p1_account_index);
        bool is_p2 = (i == g->p2_account_index);
        // Background
        Rectangle row_rect = {150, (float)y, SCREEN_W - 300, 100};
        Color bg_col = a->is_ai ? Fade(PURPLE, 0.2f) : Fade(DARKBLUE, 0.3f);
        if (is_p1)
            bg_col = Fade(LIME, 0.2f);
        if (is_p2)
            bg_col = Fade(ORANGE, 0.2f);
        DrawRectangleRec(row_rect, bg_col);
        DrawRectangleLinesEx(row_rect, 2, a->is_ai ? PURPLE : DARKGRAY);
        // Name & Stats
        DrawText(a->first_name, (int)(row_rect.x + 20.0f), (int)(row_rect.y + 20.0f), 40, WHITE);
        if (!a->is_ai)
            DrawText(a->last_name, (int)(row_rect.x + 20.0f + (float)MeasureText(a->first_name, 40) + 10.0f), (int)(row_rect.y + 20.0f), 40, WHITE);
        DrawText(TextFormat("$%.2f  W:%d L:%d", a->balance, a->wins, a->losses), (int)(row_rect.x + 20.0f), (int)(row_rect.y + 65.0f), 20, LIGHTGRAY);
        // AI Label
        if (a->is_ai)
        {
            DrawText("[AI]", (int)(row_rect.x + 350.0f), (int)(row_rect.y + 30.0f), 30, PURPLE);
        }
        // --- BUTTONS ---
        float btn_w = 140;
        float btn_h = 40;
        float start_x = row_rect.x + 600;
        // P1 Button
        Rectangle btn_p1 = {start_x, row_rect.y + 30, btn_w, btn_h};
        bool hover_p1 = CheckCollisionPointRec(GetMousePosition(), btn_p1);
        Color p1_col = is_p1 ? RED : LIME;
        const char *p1_txt = is_p1 ? "LOGOUT P1" : "LOGIN P1";
        DrawRectangleRec(btn_p1, hover_p1 ? Fade(p1_col, 0.8f) : p1_col);
        DrawText(p1_txt, (int)(btn_p1.x + 10.0f), (int)(btn_p1.y + 10.0f), 20, BLACK);
        // P2 Button
        Rectangle btn_p2 = {start_x + 160, row_rect.y + 30, btn_w, btn_h};
        bool hover_p2 = CheckCollisionPointRec(GetMousePosition(), btn_p2);
        Color p2_col = is_p2 ? RED : ORANGE;
        const char *p2_txt = is_p2 ? "LOGOUT P2" : "LOGIN P2";
        DrawRectangleRec(btn_p2, hover_p2 ? Fade(p2_col, 0.8f) : p2_col);
        DrawText(p2_txt, (int)(btn_p2.x + 10.0f), (int)(btn_p2.y + 10.0f), 20, BLACK);
    
        y += 110;
    }
    // Bottom Navigation
    Rectangle create = {CENTER_X - 300, SCREEN_H - 120, 280, 80};
    Rectangle back = {CENTER_X + 20, SCREEN_H - 120, 280, 80};
    DrawRectangleRec(create, CheckCollisionPointRec(GetMousePosition(), create) ? LIME : DARKGREEN);
    DrawRectangleRec(back, CheckCollisionPointRec(GetMousePosition(), back) ? RED : MAROON);
    DrawText("CREATE NEW", (int)(create.x + 50.0f), (int)(create.y + 25.0f), 30, BLACK);
    DrawText("BACK", (int)(back.x + 100.0f), (int)(back.y + 25.0f), 30, WHITE);
}
void UpdateAccountsManager(GameState *g, Vector2 mouse)
{
    int y = 280;
    bool action_taken = false;
    for (int i = 0; i < g->account_count; i++)
    {
        Rectangle row_rect = {150, (float)y, SCREEN_W - 300, 100};
        float btn_w = 140;
        float btn_h = 40;
        float start_x = row_rect.x + 600;
        Rectangle btn_p1 = {start_x, row_rect.y + 30, btn_w, btn_h};
        Rectangle btn_p2 = {start_x + 160, row_rect.y + 30, btn_w, btn_h};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            // P1 Login/Logout
            if (CheckCollisionPointRec(mouse, btn_p1))
            {
                if (g->p1_account_index == i)
                {
                    LogoutAccount(g, 1);
                    ShowAccountStatus(g, "Logged P1 Out");
                }
                else
                {
                    // Check if same account
                    if (g->p2_account_index == i)
                    {
                        ShowAccountStatus(g, "Cannot play against yourself!");
                    }
                    else
                    {
                        LoginAccount(g, i, 1);
                        ShowAccountStatus(g, TextFormat("%s logged into P1", g->accounts[i].first_name));
                    }
                }
            }
            // P2 Login/Logout
            else if (CheckCollisionPointRec(mouse, btn_p2))
            {
                if (g->p2_account_index == i)
                {
                    LogoutAccount(g, 2);
                    ShowAccountStatus(g, "Logged P2 Out");
                }
                else
                {
                    // Check if same account
                    if (g->p1_account_index == i)
                    {
                        ShowAccountStatus(g, "Cannot play against yourself!");
                    }
                    else
                    {
                        LoginAccount(g, i, 2);
                        ShowAccountStatus(g, TextFormat("%s logged into P2", g->accounts[i].first_name));
                    }
                }
            }
                    }
        if (action_taken)
            break;
        y += 110;
    }
    // Navigation Buttons
    Rectangle create = {CENTER_X - 300, SCREEN_H - 120, 280, 80};
    Rectangle back = {CENTER_X + 20, SCREEN_H - 120, 280, 80};
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (CheckCollisionPointRec(mouse, create))
        {
            current_name_field = 1;
            memset(new_first_name, 0, sizeof(new_first_name));
            memset(new_last_name, 0, sizeof(new_last_name));
            g->state = STATE_ACCOUNT_CREATE;
        }
        if (CheckCollisionPointRec(mouse, back))
        {
            g->state = STATE_MAIN_MENU;
        }
    }
}