#define UI_FRAME_W 480
#define UI_FRAME_H 150
#define P1_UI_X 10
#define P2_UI_X (SCREEN_W - UI_FRAME_W - 10)
#define UI_TEXT_OFFSET_X 45
#define UI_LINE_SPACING 20
#define P1_HAND_Y (SCREEN_H - CARD_H_SCALED - 130)
#define P2_HAND_Y (SCREEN_H - CARD_H_SCALED - 130)
#define HAND_CARD_GAP 5
#define P1_HAND_START_X 10
#define P2_HAND_START_X (SCREEN_W - 10 - (HAND_SIZE * CARD_W_SCALED + (HAND_SIZE - 1) * HAND_CARD_GAP))
#define DISCARD_PILE_Y (GRID_START_Y + KEYCARDS * ROW_SPACING + 0)
#define P1_DISCARD_X (KEYCARD_COL_X - CARD_W_SCALED - 100)
#define P2_DISCARD_X (KEYCARD_COL_X + CARD_W_SCALED + 100)
#include "gui.h"
#include "useraccount.h"
float GetRankY(int key_idx)
{
    return GRID_START_Y + key_idx * ROW_SPACING;
}
Rectangle KeyCardRect(int key_idx)
{
    float y = GetRankY(key_idx);
    return (Rectangle){KEYCARD_COL_X, y, CARD_W_SCALED, CARD_H_SCALED};
}
Rectangle SlotRect(int player, int key_idx, int slot_idx)
{
    float y = GetRankY(key_idx);
    float base_x = KEYCARD_COL_X + (player == 1 ? -KEY_TO_SLOT_GAP : CARD_W_SCALED + KEY_TO_SLOT_GAP);
    float x;
    if (player == 1)
    {
        x = base_x - (slot_idx + 1) * CARD_W_SCALED - slot_idx * SLOT_SPACING_X;
    }
    else
    {
        x = base_x + slot_idx * (CARD_W_SCALED + SLOT_SPACING_X);
    }
    return (Rectangle){x, y, CARD_W_SCALED, CARD_H_SCALED};
}
Rectangle HandRect(int player, int idx)
{
    float y = (player == 1) ? P1_HAND_Y : P2_HAND_Y;
    float start_x = (player == 1) ? P1_HAND_START_X : P2_HAND_START_X;
    float x = start_x + idx * (CARD_W_SCALED + HAND_CARD_GAP);
    return (Rectangle){x, y, CARD_W_SCALED, CARD_H_SCALED};
}
Rectangle ButtonRect(int player, int idx)
{
    Rectangle card_rect = HandRect(player, idx);
    float x = card_rect.x + (CARD_W_SCALED / 2.0f) - (BUTTON_W / 2.0f);
    float y = card_rect.y + CARD_H_SCALED + BUTTON_OFFSET_Y;
    return (Rectangle){x, y, BUTTON_W, BUTTON_H};
}
Rectangle DiscardPileRect(int player)
{
    float x = (player == 1) ? P1_DISCARD_X : P2_DISCARD_X;
    return (Rectangle){x, DISCARD_PILE_Y, CARD_W_SCALED, CARD_H_SCALED};
}
void DrawCard(Card c, Rectangle rect, Color tint)
{
    if (c.is_valid && c.texture.id)
    {
        DrawTexturePro(c.texture, (Rectangle){0, 0, (float)c.texture.width, (float)c.texture.height},
                       rect, (Vector2){0, 0}, 0.0f, tint);
    }
}
void DrawButton(Rectangle rect, bool is_hovered, bool is_enabled, const char *text)
{
    Color base_color = is_enabled ? RAYWHITE : DARKGRAY;
    Color tint = is_hovered ? Fade(base_color, 0.7f) : base_color;
    if (g_button_texture.id != 0)
    {
        DrawTexturePro(g_button_texture, (Rectangle){0, 0, (float)g_button_texture.width, (float)g_button_texture.height},
                       rect, (Vector2){0, 0}, 0.0f, tint);
    }
    int text_len = MeasureText(text, 14);
    DrawText(text, (int)(rect.x + rect.width / 2 - text_len / 2), (int)(rect.y + rect.height / 2 - 7), 14, BLACK);
}
static void SortLeaderboard(GameState *g)
{
    if (g->leaderboard_sort_by_rounds)
    {
        // Sort by LOWEST rounds first (fastest wins)
        for (int i = 0; i < g->leaderboard_count - 1; i++)
        {
            for (int j = i + 1; j < g->leaderboard_count; j++)
            {
                if (g->leaderboard[j].total_rounds < g->leaderboard[i].total_rounds)
                {
                    LeaderboardEntry temp = g->leaderboard[i];
                    g->leaderboard[i] = g->leaderboard[j];
                    g->leaderboard[j] = temp;
                }
            }
        }
    }
    else
    {
        // Sort by HIGHEST winnings first
        for (int i = 0; i < g->leaderboard_count - 1; i++)
        {
            for (int j = i + 1; j < g->leaderboard_count; j++)
            {
                if (g->leaderboard[j].total_winnings > g->leaderboard[i].total_winnings)
                {
                    LeaderboardEntry temp = g->leaderboard[i];
                    g->leaderboard[i] = g->leaderboard[j];
                    g->leaderboard[j] = temp;
                }
            }
        }
    }
}
void DrawLeaderboard(GameState *g) 
{
    SortLeaderboard(g);
    static int scroll_offset = 0;
    const int entries_per_page = 15;
    const int max_scroll = (g->leaderboard_count > entries_per_page) ? (g->leaderboard_count - entries_per_page) : 0;
    float wheel = GetMouseWheelMove();
    if (wheel > 0)
        scroll_offset = (scroll_offset > 0) ? scroll_offset - 1 : 0;
    if (wheel < 0)
        scroll_offset = (scroll_offset < max_scroll) ? scroll_offset + 1 : max_scroll;
    Rectangle bg = {CENTER_X - 650, 120, 1300, 900};
    // Dark, slightly transparent background
    DrawRectangleRec(bg, Fade(BLACK, 0.9f));
    float t_total = (float)GetTime();
    // Animated wave background
    Color dark_base = (Color){0, 0, 20, 255};
    Color bright_wave = (Color){0, 100, 255, 255};
    for (int y = (int)bg.y; y < (int)(bg.y + bg.height); y++)
    {
        float t_norm = (float)(y - bg.y) / bg.height;
        float wave_factor = sinf(t_total * 3.0f + y * 0.015f);
        float intensity = (wave_factor + 1.0f) * 0.5f;
        intensity *= (1.0f - t_norm * 0.4f);
        Color final_color = ColorLerp(dark_base, bright_wave, intensity * 0.4f);
        DrawRectangle(bg.x, y, (int)bg.width, 1, final_color);
    }
    // Border
    DrawRectangleLinesEx(bg, 9, Fade(DARKBLUE, 0.4f));
    DrawRectangleLinesEx(bg, 3, Fade(DARKBLUE, 0.8f));
    const char *title = "HIGH ROLLERS";
    int title_w = MeasureText(title, 50);
    DrawText(title, CENTER_X - title_w / 2, 130, 50, GOLD);
    // Sort button
    Vector2 mouse = GetMousePosition();
    Rectangle sort_btn = {CENTER_X + 400, 130, 200, 50};
    bool sort_hover = CheckCollisionPointRec(mouse, sort_btn);
    DrawRectangleRec(sort_btn, sort_hover ? GOLD : ORANGE);
    const char *sort_text = g->leaderboard_sort_by_rounds ? "BY ROUNDS" : "BY CASH";
    DrawText(sort_text, sort_btn.x + 30, sort_btn.y + 15, 25, BLACK);
    // Column headers
    int x_rank = (int)CENTER_X - 500;
    int x_name = x_rank + 100;
    int x_cash = x_rank + 500;
    int x_rounds = x_rank + 700;
    int x_date = x_rank + 900;
    DrawText("Rank", x_rank, 200, 28, Fade(WHITE, 0.9f));
    DrawText("Winner", x_name, 200, 28, Fade(WHITE, 0.9f));
    DrawText("Cash", x_cash, 200, 28, Fade(WHITE, 0.9f));
    DrawText("Rounds", x_rounds, 200, 28, Fade(WHITE, 0.9f));
    DrawText("Date", x_date, 200, 28, Fade(WHITE, 0.9f));
    // Display entries
    for (int i = 0; i < entries_per_page && (i + scroll_offset) < g->leaderboard_count; i++)
    {
        const LeaderboardEntry *e = &g->leaderboard[i + scroll_offset];
        int y = 240 + i * 48;
        int actual_rank = i + scroll_offset + 1;
        Color rank_color = (actual_rank == 1) ? GOLD : (actual_rank <= 3) ? ORANGE
                                                                          : Fade(WHITE, 0.7f);
        DrawText(TextFormat("#%d", actual_rank), x_rank, y, 32, rank_color);
        DrawText(e->winner_name, x_name, y + 4, 28, (actual_rank == 1) ? YELLOW : LIGHTGRAY);
        DrawText(TextFormat("$%.2f", e->total_winnings), x_cash, y + 4, 28, (actual_rank == 1) ? LIME : WHITE);
        DrawText(TextFormat("%d", e->total_rounds), x_rounds, y + 4, 28, Fade(SKYBLUE, 0.8f));
        DrawText(e->timestamp, x_date, y + 6, 24, Fade(PURPLE, 0.8f));
    }
    if (g->leaderboard_count > entries_per_page)
    {
        DrawText(TextFormat("Showing %d-%d of %d | Scroll to see more",
                            scroll_offset + 1,
                            (scroll_offset + entries_per_page > g->leaderboard_count) ? g->leaderboard_count : scroll_offset + entries_per_page,
                            g->leaderboard_count),
                 CENTER_X - 250, 980, 22, Fade(YELLOW, 0.8f));
    }
    DrawText("Press ESC or click anywhere to return", CENTER_X - 300, 1010, 24, Fade(WHITE, 0.6f));
}
Rectangle ContinueButtonRect(int player)
{
    // Get the reference button position (using index 4 for P1 end, index 0 for P2 start)
    // We adjust the X offset so they sit "outside" the hand range
    Rectangle ref_rect;
    float x;
    if (player == 1)
    {
        ref_rect = ButtonRect(1, 4);
        x = ref_rect.x + ref_rect.width + 10; // 10px after the last P1 button
    }
    else
    {
        ref_rect = ButtonRect(2, 0);
        x = ref_rect.x - 80 - 10; // 80px width + 10px gap before the first P2 button
    }
    return (Rectangle){x, ref_rect.y, 80, ref_rect.height};
}
void DrawContinueButtons(const GameState *g, Vector2 mouse)
{
    if (g->state != STATE_WAIT_FOR_TURN)
        return;
    for (int p = 1; p <= 2; p++)
    {
        // Check if player is human and not finished
        bool is_human_active = (p == 1) ? (!g->p1_done_placing && !IsPlayerAI(g, 1))
                                        : (!g->p2_done_placing && !IsPlayerAI(g, 2));
        if (is_human_active)
        {
            Rectangle btn = ContinueButtonRect(p);
            bool hover = CheckCollisionPointRec(mouse, btn);
            Color tint = hover ? Fade(RAYWHITE, 0.7f) : RAYWHITE;
            // Draw the texture (btn.png)
            if (g_button_texture.id != 0)
            {
                DrawTexturePro(g_button_texture,
                               (Rectangle){0, 0, (float)g_button_texture.width, (float)g_button_texture.height},
                               btn, (Vector2){0, 0}, 0.0f, tint);
            }
            else
            {
                DrawRectangleRec(btn, hover ? GOLD : ORANGE); // Fallback
            }
            int text_w = MeasureText("PASS", 20);
            DrawText("PASS", btn.x + (btn.width - text_w) / 2, btn.y + (btn.height / 2 - 10), 20, BLACK);
        }
    }
}
void DrawPlayerUI(const GameState *g, int player)
{
    // This function is perfectly fine as is.
    float adjusted_y = UI_Y - 120;
    Rectangle frame_rect = (player == 1) ? (Rectangle){P1_UI_X, adjusted_y, UI_FRAME_W, UI_FRAME_H}
                                         : (Rectangle){P2_UI_X, adjusted_y, UI_FRAME_W, UI_FRAME_H};
    float balance = (player == 1) ? g->p1_balance : g->p2_balance;
    int ranks = (player == 1) ? g->p1_completed_ranks : g->p2_completed_ranks;
    Color neon_color = (player == 1) ? SKYBLUE : (Color){255, 100, 100, 255};
    const char *player_name = GetPlayerName(g, player);
    float margin_x = 40.0f;
    float margin_y = 30.0f;
    Rectangle screen_rect = {
        frame_rect.x + margin_x,
        frame_rect.y + margin_y,
        frame_rect.width - (margin_x * 2),
        frame_rect.height - (margin_y * 1.5f)};
    DrawRectangleRec(screen_rect, Fade(BLACK, 0.85f));
    // Draw Screen Border (Glowing line)
    DrawRectangleLinesEx(screen_rect, 2, Fade(neon_color, 0.5f));
    // 3. Draw Digital Text
    int text_x = (int)screen_rect.x + 15;
    int text_y = (int)screen_rect.y + 15;
    int line_h = 24;
    DrawText(TextFormat("USER: %s", player_name), text_x, text_y, 20, neon_color);
    text_y += line_h;
    DrawText(TextFormat("CREDITS: $%.2f", balance), text_x, text_y, 20, neon_color);
    text_y += line_h;
    DrawText(TextFormat("STATUS: %d/%d Ranks | %d rounds", ranks, KEYCARDS, g->total_rounds), text_x, text_y, 20, neon_color);
}
void DrawGameLayout(const GameState *g)
{
    Vector2 mouse = GetMousePosition();
    DrawPlayerUI(g, 1);
    DrawPlayerUI(g, 2);
    // 1. Draw Keycards and Slots
    for (int k = 0; k < KEYCARDS; k++)
    {
        Rectangle key_rect = KeyCardRect(k);
        DrawCard(g->keycards[k], key_rect, RAYWHITE);
        // --- Player 1 Slots ---
        for (int s = 0; s < 3; s++)
        {
            Rectangle slot_rect = SlotRect(1, k, s);
            if (g->p1_slots[k][s].is_valid)
            {
                DrawCard(g->p1_slots[k][s], slot_rect, RAYWHITE);
            }
            else if (g_card_back_texture.id)
            {
                DrawTexturePro(g_card_back_texture,
                               (Rectangle){0, 0, (float)g_card_back_texture.width, (float)g_card_back_texture.height},
                               slot_rect, (Vector2){0, 0}, 0.0f, Fade(WHITE, 0.25f));
            }
        }
        // --- Player 2 Slots ---
        for (int s = 0; s < 3; s++)
        {
            Rectangle slot_rect = SlotRect(2, k, s);
            if (g->p2_slots[k][s].is_valid)
            {
                DrawCard(g->p2_slots[k][s], slot_rect, RAYWHITE);
            }
            else if (g_card_back_texture.id)
            {
                DrawTexturePro(g_card_back_texture,
                               (Rectangle){0, 0, (float)g_card_back_texture.width, (float)g_card_back_texture.height},
                               slot_rect, (Vector2){0, 0}, 0.0f, Fade(WHITE, 0.25f));
            }
        }
    }
    // 2. Draw Hands & Discard Buttons
    for (int i = 0; i < HAND_SIZE; i++)
    {
        // ============================================================
        // PLAYER 1 HAND & BUTTON
        // ============================================================
        Rectangle p1_rect = HandRect(1, i);
        if (i < g->p1_hand_size)
        {
            Color tint = (g->p1_discard_ready && i == g->p1_discard_idx) ? YELLOW : RAYWHITE;
            DrawCard(g->player1_hand[i], p1_rect, tint);
        }
        // P1 Button Logic
        bool p1_btn_enabled = false;
        const char *p1_btn_text = "---";
        if (g->state == STATE_P1_SELECT_DISCARD && i < g->p1_hand_size)
        {
            if (!IsPlayerAI(g, 1))
            {
                p1_btn_enabled = !g->p1_discard_ready;
                p1_btn_text = g->p1_discard_ready ? "READY" : "DISCARD";
            }
            else
            {
                p1_btn_text = "AI";
            }
        }
        else if (g->state == STATE_WAIT_FOR_TURN)
        {
            p1_btn_text = "PLACE";
        }
        bool p1_btn_hovered = CheckCollisionPointRec(mouse, ButtonRect(1, i));
        DrawButton(ButtonRect(1, i), p1_btn_hovered, p1_btn_enabled, p1_btn_text);
        // ============================================================
        // PLAYER 2 HAND & BUTTON
        // ============================================================
        Rectangle p2_rect = HandRect(2, i);
        if (i < g->p2_hand_size)
        {
            // Check if we should cover P2 cards
            bool should_cover = g->cover_p2_cards &&
                                !IsPlayerAI(g, 1) &&
                                g->state == STATE_P1_SELECT_DISCARD;
            if (should_cover)
            {
                // Draw card back
                if (g_card_back_texture.id)
                {
                    DrawTexturePro(g_card_back_texture,
                                   (Rectangle){0, 0, (float)g_card_back_texture.width, (float)g_card_back_texture.height},
                                   p2_rect, (Vector2){0, 0}, 0.0f, RAYWHITE);
                }
            }
            else
            {
                Color tint = (g->p2_discard_ready && i == g->p2_discard_idx) ? PURPLE : RAYWHITE;
                DrawCard(g->player2_hand[i], p2_rect, tint);
            }
        }
        // P2 Button Logic
        bool p2_btn_enabled = false;
        const char *p2_btn_text = "---";
        if (g->state == STATE_P1_SELECT_DISCARD && i < g->p2_hand_size)
        {
            if (!IsPlayerAI(g, 2))
            {
                p2_btn_enabled = !g->p2_discard_ready;
                p2_btn_text = g->p2_discard_ready ? "READY" : "DISCARD";
            }
            else
            {
                p2_btn_text = "AI";
            }
        }
        else if (g->state == STATE_WAIT_FOR_TURN)
        {
            p2_btn_text = "PLACE";
        }
        bool p2_btn_hovered = CheckCollisionPointRec(mouse, ButtonRect(2, i));
        DrawButton(ButtonRect(2, i), p2_btn_hovered, p2_btn_enabled, p2_btn_text);
    }
    // 4. Draw Discard Piles
    Rectangle d1_rect = DiscardPileRect(1);
    if (g->revealed_p1.is_valid)
    {
        DrawCard(g->revealed_p1, d1_rect, RAYWHITE);
    }
    else if (g_card_back_texture.id)
    {
        DrawTexturePro(g_card_back_texture,
                       (Rectangle){0, 0, (float)g_card_back_texture.width, (float)g_card_back_texture.height},
                       d1_rect, (Vector2){0, 0}, 0.0f, Fade(WHITE, 1.00f));
    }
    Rectangle d2_rect = DiscardPileRect(2);
    if (g->revealed_p2.is_valid)
    {
        DrawCard(g->revealed_p2, d2_rect, RAYWHITE);
    }
    else if (g_card_back_texture.id)
    {
        DrawTexturePro(g_card_back_texture,
                       (Rectangle){0, 0, (float)g_card_back_texture.width, (float)g_card_back_texture.height},
                       d2_rect, (Vector2){0, 0}, 0.0f, Fade(WHITE, 1.00f));
    }
    // Draw Continue Buttons (for placement phase)
    DrawContinueButtons(g, mouse);
}
void DrawGameOver(GameState *g)
{
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.8f));
    const char *winner_name = (g->winner == 1) ? GetPlayerName(g, 1) : GetPlayerName(g, 2);
    const char *loser_name = (g->winner == 1) ? GetPlayerName(g, 2) : GetPlayerName(g, 1);
    char win_text[128];
    snprintf(win_text, sizeof(win_text), "%s WINS!", winner_name);
    int tw = MeasureText(win_text, 90);
    DrawText(win_text, (SCREEN_W - tw) / 2, SCREEN_H / 2 - 250, 90, GOLD);
    DrawText("CONGRATULATIONS!",
             (SCREEN_W - MeasureText("CONGRATULATIONS!", 60)) / 2,
             SCREEN_H / 2 - 150, 60, YELLOW);
    // Get final scores (these already include the game balance)
    float winner_score = (g->winner == 1) ? g->final_score_p1 : g->final_score_p2;
    float loser_score = (g->winner == 1) ? g->final_score_p2 : g->final_score_p1;
    // Winner gets bonus: rounds × $10.00
    float winner_bonus = 10.0f * g->total_rounds;
    DrawText(TextFormat("Final Balance: $%.2f", winner_score),
             (SCREEN_W - MeasureText(TextFormat("Final Balance: $9999.99"), 40)) / 2,
             SCREEN_H / 2 - 50, 40, LIME);
    DrawText(TextFormat("Victory Bonus: +$%.2f (%d rounds × $10)", winner_bonus, g->total_rounds),
             (SCREEN_W - MeasureText(TextFormat("Victory Bonus: +$999.99 (99 rounds × $10)"), 35)) / 2,
             SCREEN_H / 2, 35, GREEN);
    // Loser loses flat $100.00
    float loser_penalty = -100.0f;
    DrawText(TextFormat("%s - Final Balance: $%.2f", loser_name, loser_score),
             (SCREEN_W - MeasureText(TextFormat("PlayerNameLong - Final Balance: $9999.99"), 30)) / 2,
             SCREEN_H / 2 + 60, 30, ORANGE);
    DrawText(TextFormat("Defeat Penalty: $%.2f", loser_penalty),
             (SCREEN_W - MeasureText(TextFormat("Defeat Penalty: $-999.99"), 28)) / 2,
             SCREEN_H / 2 + 95, 28, RED);
    // Animated coins background
    float t = (float)GetTime();
    int coinCount = 150;
    for (int i = 0; i < coinCount; i++)
    {
        float xPos = fmodf(i * 737.5f, (float)SCREEN_W);
        float speed = 200.0f + (fmodf(i * 13.0f, 100.0f));
        float yOffset = i * 50.0f;
        float yPos = fmodf(t * speed + yOffset, (float)SCREEN_H + 100) - 50;
        xPos += sinf(t * 3.0f + i) * 15.0f;
        float radius = 12.0f + fmodf(i, 8.0f);
        Vector2 center = {xPos, yPos};
        DrawCircleV(center, radius, GOLD);
        DrawCircleLinesV(center, radius, ORANGE);
        int fontSize = (int)(radius * 1.2f);
        const char *symbol = "$";
        int textWidth = MeasureText(symbol, fontSize);
        float spin = cosf(t * 5.0f + i) * 0.1f;
        DrawText(symbol,
                 (int)(center.x - textWidth / 2 + spin * radius),
                 (int)(center.y - fontSize / 2 + spin * radius),
                 fontSize, DARKGREEN);
        DrawCircleV((Vector2){center.x - radius / 3, center.y - radius / 3},
                    radius / 4, Fade(WHITE, 0.5f));
    }
    Vector2 mouse = GetMousePosition();
    Rectangle restart_btn = {CENTER_X - 450, SCREEN_H - 150, 280, 80};
    Rectangle menu_btn = {CENTER_X - 140, SCREEN_H - 150, 280, 80};
    Rectangle quit_btn = {CENTER_X + 170, SCREEN_H - 150, 280, 80};
    bool restart_hover = CheckCollisionPointRec(mouse, restart_btn);
    bool menu_hover = CheckCollisionPointRec(mouse, menu_btn);
    bool quit_hover = CheckCollisionPointRec(mouse, quit_btn);
    DrawRectangleRec(restart_btn, restart_hover ? LIME : DARKGREEN);
    DrawRectangleRec(menu_btn, menu_hover ? SKYBLUE : BLUE);
    DrawRectangleRec(quit_btn, quit_hover ? RED : MAROON);
    DrawText("RESTART", restart_btn.x + 70, restart_btn.y + 25, 30, WHITE);
    DrawText("MAIN MENU", menu_btn.x + 40, menu_btn.y + 25, 30, WHITE);
    DrawText("QUIT", quit_btn.x + 100, quit_btn.y + 25, 30, WHITE);
}