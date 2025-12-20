#include "mainmenu.h"
#include "useraccount.h"
#include "gui.h"
#include <main.h>
#include <strings.h>

void ShowAccountStatus(GameState *g, const char *msg)
{
    strncpy(g->account_status_message, msg, sizeof(g->account_status_message) - 1);
    g->account_status_timer = GetTime();
}

void DrawShop(const GameState *g)
{
    DrawText("TOKEN SHOP", (int)(CENTER_X - 250.0f), 100, 80, GOLD);

    const char *p1 = (g->p1_account_index == -1) ? "Not Logged In" : GetPlayerName(g, 1);
    DrawText("P1 Active Account:", 150, 220, 35, LIME);
    DrawText(p1, 500, 220, 35, WHITE);

    if (g->p1_account_index >= 0)
    {
        DrawText(TextFormat("Credits: $%.2f", g->accounts[g->p1_account_index].credits), 150, 270, 30, LIME);
        DrawText(TextFormat("Tokens: %.1f", g->accounts[g->p1_account_index].tokens), 150, 310, 30, GOLD);
    }

    DrawText("AVAILABLE PURCHASES:", (int)(CENTER_X - 200), 380, 40, SKYBLUE);

    Vector2 mouse = GetMousePosition();
    Rectangle p1_buy_rect = {CENTER_X - 200, 500, 400, 150};

    bool p1_can_buy = (g->p1_account_index >= 0 &&
                       g->accounts[g->p1_account_index].credits >= TOKEN_PRICE);
    bool p1_hover = CheckCollisionPointRec(mouse, p1_buy_rect);

    Color p1_color = p1_can_buy ? (p1_hover ? LIME : DARKGREEN) : DARKGRAY;
    DrawRectangleRec(p1_buy_rect, p1_color);
    DrawRectangleLinesEx(p1_buy_rect, 3, GOLD);

    DrawText("BUY 1 TOKEN", (int)(p1_buy_rect.x + 110), (int)(p1_buy_rect.y + 30), 30, BLACK);
    DrawText(TextFormat("Cost: $%.2f", TOKEN_PRICE), (int)(p1_buy_rect.x + 120), (int)(p1_buy_rect.y + 75), 25, p1_can_buy ? YELLOW : RED);

    if (!p1_can_buy && g->p1_account_index >= 0)
    {
        if (g->accounts[g->p1_account_index].credits < 0)
            DrawText("Negative Balance!", (int)(p1_buy_rect.x + 100), (int)(p1_buy_rect.y + 110), 20, RED);
        else
            DrawText("Insufficient funds!", (int)(p1_buy_rect.x + 100), (int)(p1_buy_rect.y + 110), 20, RED);
    }

    DrawText("Tokens are used for specific game actions.", (int)(CENTER_X - 250), 700, 22, LIGHTGRAY);

    Rectangle back_rect = {CENTER_X - 150, SCREEN_H - 150, 300, 80};
    DrawRectangleRec(back_rect, CheckCollisionPointRec(mouse, back_rect) ? RED : MAROON);
    DrawText("BACK TO MENU", (int)(back_rect.x + 40), (int)(back_rect.y + 25), 30, WHITE);
}

void UpdateShop(GameState *g, Vector2 mouse)
{
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        return;

    Rectangle p1_buy_rect = {CENTER_X - 200, 500, 400, 150};
    Rectangle back_rect = {CENTER_X - 150, SCREEN_H - 150, 300, 80};

    if (CheckCollisionPointRec(mouse, p1_buy_rect))
    {
        if (g->p1_account_index >= 0)
        {
            if (g->accounts[g->p1_account_index].credits >= TOKEN_PRICE)
            {
                g->accounts[g->p1_account_index].credits -= TOKEN_PRICE;
                g->accounts[g->p1_account_index].tokens += 1.0;
                SaveAllAccounts(g);
                PlaySound(g_coin_sound);
            }
        }
    }
    else if (CheckCollisionPointRec(mouse, back_rect))
    {
        g->state = STATE_MAIN_MENU;
    }
}

void DrawMainMenu(GameState *g)
{
    (void)g;
    Rectangle play_rect = {CENTER_X - 150, 280, 300, 60};
    Rectangle accounts_rect = {CENTER_X - 150, 370, 300, 60};
    Rectangle shop_rect = {CENTER_X - 150, 460, 300, 60};
    Rectangle settings_rect = {CENTER_X - 150, 550, 300, 60};
    Rectangle leaderboard_rect = {CENTER_X - 150, 640, 300, 60};
    Vector2 mouse = GetMousePosition();

    DrawText("JOKERS GAMBIT", (int)(CENTER_X - 350.0f), 130, 90, GOLD);

    DrawRectangleRec(play_rect, CheckCollisionPointRec(mouse, play_rect) ? LIME : GREEN);
    DrawText("PLAY", (int)play_rect.x + 110, (int)play_rect.y + 15, 30, BLACK);

    DrawRectangleRec(accounts_rect, CheckCollisionPointRec(mouse, accounts_rect) ? SKYBLUE : BLUE);
    DrawText("ACCOUNTS", (int)accounts_rect.x + 70, (int)accounts_rect.y + 15, 30, WHITE);

    DrawRectangleRec(shop_rect, CheckCollisionPointRec(mouse, shop_rect) ? GOLD : ORANGE);
    DrawText("SHOP", (int)shop_rect.x + 105, (int)shop_rect.y + 15, 30, BLACK);

    DrawRectangleRec(settings_rect, CheckCollisionPointRec(mouse, settings_rect) ? PURPLE : DARKPURPLE);
    DrawText("SETTINGS", (int)settings_rect.x + 75, (int)settings_rect.y + 15, 30, WHITE);

    DrawRectangleRec(leaderboard_rect, CheckCollisionPointRec(mouse, leaderboard_rect) ? YELLOW : GOLD);
    DrawText("LEADERBOARD", (int)leaderboard_rect.x + 30, (int)leaderboard_rect.y + 15, 30, BLACK);
}

void UpdateMainMenu(GameState *g, Vector2 mouse)
{
    Rectangle play_rect = {CENTER_X - 150, 280, 300, 60};
    Rectangle accounts_rect = {CENTER_X - 150, 370, 300, 60};
    Rectangle shop_rect = {CENTER_X - 150, 460, 300, 60};
    Rectangle settings_rect = {CENTER_X - 150, 550, 300, 60};
    Rectangle leaderboard_rect = {CENTER_X - 150, 640, 300, 60};

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (CheckCollisionPointRec(mouse, play_rect))
        {
            if (g->p1_account_index == -1 || g->p2_account_index == -1)
            {
                ShowAccountStatus(g, "Please log in 2 accounts to start");
                g->state = STATE_ACCOUNTS_MANAGER;
            }
            else
            {
                bool p1_is_ai = g->accounts[g->p1_account_index].is_ai;
                bool p2_is_ai = g->accounts[g->p2_account_index].is_ai;
                if (!p1_is_ai && !p2_is_ai)
                    g->mode = MODE_PVP;
                else if (p1_is_ai && p2_is_ai)
                    g->mode = MODE_AIVSAI;
                else
                    g->mode = MODE_PVAI;

                RestartGameKeepingAccounts(g);
                g->state = STATE_P1_SELECT_DISCARD;
            }
        }
        if (CheckCollisionPointRec(mouse, accounts_rect))
            g->state = STATE_ACCOUNTS_MANAGER;
        if (CheckCollisionPointRec(mouse, shop_rect))
            g->state = STATE_SHOP;
        if (CheckCollisionPointRec(mouse, settings_rect))
            g->state = STATE_SETTINGS;
        if (CheckCollisionPointRec(mouse, leaderboard_rect))
            g->state = STATE_LEADERBOARD;
    }
}

void DrawSettings(GameState *g)
{
    DrawText("SETTINGS", (int)(CENTER_X - 200.0f), 100, 80, GOLD);

    DrawText("P2 Card Privacy:", (int)(CENTER_X - 300.0f), 300, 40, WHITE);
    Rectangle cover_btn_rect = {CENTER_X - 150, 380, 300, 80};
    Vector2 mouse = GetMousePosition();

    Color cover_color = g->cover_p2_cards ? LIME : RED;
    DrawRectangleRec(cover_btn_rect, CheckCollisionPointRec(mouse, cover_btn_rect) ? Fade(cover_color, 0.8f) : cover_color);
    DrawText(g->cover_p2_cards ? "COVERED" : "UNCOVERED", (int)(CENTER_X - (float)MeasureText(g->cover_p2_cards ? "COVERED" : "UNCOVERED", 35) / 2.0f), 405, 35, BLACK);
    DrawText("(Hides P2 cards from view during local play)", (int)(CENTER_X - 300.0f), 480, 25, LIGHTGRAY);

    DrawText("AI Move Speed:", (int)(CENTER_X - 300.0f), 550, 40, WHITE);
    Rectangle ai_speed_rect = {CENTER_X - 150, 620, 300, 80};

    const char *speed_text;
    Color speed_color;
    if (fabsf(g->ai_move_delay - 0.5f) < 0.001f)
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

    DrawRectangleRec(ai_speed_rect, CheckCollisionPointRec(mouse, ai_speed_rect) ? Fade(speed_color, 0.8f) : speed_color);
    DrawText(speed_text, (int)(CENTER_X - (float)MeasureText(speed_text, 35) / 2), 645, 35, BLACK);
    DrawText("(Click to cycle: Fast -> Medium -> Slow)", (int)(CENTER_X - 270.0f), 720, 25, LIGHTGRAY);

    Rectangle back_rect = {CENTER_X - 150, SCREEN_H - 180, 300, 80};
    DrawRectangleRec(back_rect, CheckCollisionPointRec(mouse, back_rect) ? RED : MAROON);
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
            g->cover_p2_cards = !g->cover_p2_cards;
        else if (CheckCollisionPointRec(mouse, ai_speed_rect))
        {
            if (fabsf(g->ai_move_delay - 0.5f) < EPSILON)
                g->ai_move_delay = 2.0f;
            else if (fabsf(g->ai_move_delay - 2.0f) < EPSILON)
                g->ai_move_delay = 3.0f;
            else
                g->ai_move_delay = 0.5f;
        }
        else if (CheckCollisionPointRec(mouse, back_rect))
            g->state = STATE_MAIN_MENU;
    }
}

void DrawAccountsManager(const GameState *g)
{
    DrawText("ACCOUNT MANAGER", (int)(CENTER_X - 900.0f), 80, 20, GOLD);

    const char *p1 = (g->p1_account_index == -1) ? "---" : GetPlayerName(g, 1);
    const char *p2 = (g->p2_account_index == -1) ? "---" : GetPlayerName(g, 2);
    DrawText("P1 Current:", 150, 160, 40, LIME);
    DrawText(p1, 400, 160, 40, WHITE);
    DrawText("P2 Current:", 950, 160, 40, ORANGE);
    DrawText(p2, 1200, 160, 40, WHITE);

    if (GetTime() - g->account_status_timer < 2.0)
    {
        float alpha = 1.0f - (float)(GetTime() - g->account_status_timer) / 2.0f;
        DrawText(g->account_status_message, (int)(CENTER_X - (float)MeasureText(g->account_status_message, 30) / 2.0f), 220, 30, Fade(GREEN, alpha));
    }

    int y = 280;
    for (int i = 0; i < g->account_count; i++)
    {
        const Account *a = &g->accounts[i];
        bool is_p1 = (i == g->p1_account_index);
        bool is_p2 = (i == g->p2_account_index);
        Rectangle row_rect = {150, (float)y, SCREEN_W - 300, 100};
        Color bg_col = a->is_ai ? Fade(PURPLE, 0.2f) : Fade(DARKBLUE, 0.3f);
        if (is_p1)
            bg_col = Fade(LIME, 0.2f);
        if (is_p2)
            bg_col = Fade(ORANGE, 0.2f);

        DrawRectangleRec(row_rect, bg_col);
        DrawRectangleLinesEx(row_rect, 2, a->is_ai ? PURPLE : DARKGRAY);

        DrawText(a->first_name, (int)(row_rect.x + 20), (int)(row_rect.y + 20), 40, WHITE);
        if (!a->is_ai)
            DrawText(a->last_name, (int)(row_rect.x + 20 + (float)MeasureText(a->first_name, 40) + 10), (int)(row_rect.y + 20), 40, WHITE);
        DrawText(TextFormat("$%.2f  Tokens: %.1f  W:%d L:%d", a->credits, a->tokens, a->wins, a->losses), (int)(row_rect.x + 20), (int)(row_rect.y + 65), 20, LIGHTGRAY);
        if (a->is_ai)
            DrawText("[AI]", (int)(row_rect.x + 350), (int)(row_rect.y + 30), 30, PURPLE);

        float btn_w = 140;
        float btn_h = 40;
        float start_x = row_rect.x + 600;

        Rectangle btn_p1 = {start_x, row_rect.y + 30, btn_w, btn_h};
        bool hover_p1 = CheckCollisionPointRec(GetMousePosition(), btn_p1);
        Color p1_col = is_p1 ? RED : LIME;
        DrawRectangleRec(btn_p1, hover_p1 ? Fade(p1_col, 0.8f) : p1_col);
        DrawText(is_p1 ? "LOGOUT P1" : "LOGIN P1", (int)(btn_p1.x + 10), (int)(btn_p1.y + 10), 20, BLACK);

        Rectangle btn_p2 = {start_x + 160, row_rect.y + 30, btn_w, btn_h};
        bool hover_p2 = CheckCollisionPointRec(GetMousePosition(), btn_p2);
        Color p2_col = is_p2 ? RED : ORANGE;
        DrawRectangleRec(btn_p2, hover_p2 ? Fade(p2_col, 0.8f) : p2_col);
        DrawText(is_p2 ? "LOGOUT P2" : "LOGIN P2", (int)(btn_p2.x + 10), (int)(btn_p2.y + 10), 20, BLACK);

        y += 110;
    }

    Rectangle quickplay = {CENTER_X - 300, SCREEN_H - 120, 280, 80};
    Rectangle back = {CENTER_X + 20, SCREEN_H - 120, 280, 80};

    DrawRectangleRec(quickplay, CheckCollisionPointRec(GetMousePosition(), quickplay) ? LIME : DARKGREEN);
    DrawRectangleRec(back, CheckCollisionPointRec(GetMousePosition(), back) ? RED : MAROON);

    DrawText("QUICK PLAY", (int)(quickplay.x + 50), (int)(quickplay.y + 25), 30, BLACK);
    DrawText("BACK", (int)(back.x + 100), (int)(back.y + 25), 30, WHITE);
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
            if (CheckCollisionPointRec(mouse, btn_p1))
            {
                if (g->p1_account_index == i)
                    LogoutAccount(g, 1);
                else
                {
                    if (g->p2_account_index == i)
                        ShowAccountStatus(g, "Account already in use by P2!");
                    else
                    {
                        LoginAccount(g, i, 1);
                        ShowAccountStatus(g, TextFormat("%s logged into P1", g->accounts[i].first_name));
                    }
                }
            }
            else if (CheckCollisionPointRec(mouse, btn_p2))
            {
                if (g->p2_account_index == i)
                    LogoutAccount(g, 2);
                else
                {
                    if (g->p1_account_index == i)
                        ShowAccountStatus(g, "Account already in use by P1!");
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

    Rectangle quickplay = {CENTER_X - 300, SCREEN_H - 120, 280, 80};
    Rectangle back = {CENTER_X + 20, SCREEN_H - 120, 280, 80};

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (CheckCollisionPointRec(mouse, quickplay))
        {
            // Auto-login default accounts if none selected
            // P1 defaults to "player one" (Index 3 usually), P2 defaults to BOB (Index 0)
            if (g->p1_account_index == -1)
            {
                // Find 'player one'
                for (int i = 0; i < g->account_count; i++)
                {
                    if (strcasecmp(g->accounts[i].first_name, "player") == 0 && strcasecmp(g->accounts[i].last_name, "one") == 0)
                    {
                        LoginAccount(g, i, 1);
                        break;
                    }
                }
            }
            if (g->p2_account_index == -1)
            {
                // Find 'BOB'
                for (int i = 0; i < g->account_count; i++)
                {
                    if (strcasecmp(g->accounts[i].first_name, "BOB") == 0)
                    {
                        if (g->p1_account_index != i)
                            LoginAccount(g, i, 2);
                        break;
                    }
                }
            }

            bool p1_is_ai = (g->p1_account_index != -1) && g->accounts[g->p1_account_index].is_ai;
            bool p2_is_ai = (g->p2_account_index != -1) && g->accounts[g->p2_account_index].is_ai;
            if (!p1_is_ai && !p2_is_ai)
                g->mode = MODE_PVP;
            else if (p1_is_ai && p2_is_ai)
                g->mode = MODE_AIVSAI;
            else
                g->mode = MODE_PVAI;

            RestartGameKeepingAccounts(g);
            g->state = STATE_P1_SELECT_DISCARD;
        }
        if (CheckCollisionPointRec(mouse, back))
            g->state = STATE_MAIN_MENU;
    }
}