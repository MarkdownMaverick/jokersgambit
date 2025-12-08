#define UI_FRAME_W 480
#define UI_FRAME_H 150
#define P1_UI_X 10
#define P2_UI_X (SCREEN_W - UI_FRAME_W - 10)
#define UI_Y 650
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

//  LAYOUT CALCULATIONS
// ─────────────────────────────────────────────────────────────────────────────
float GetRankY(int key_idx)
{
    return GRID_START_Y + key_idx * ROW_SPACING;
}
Rectangle GambitSlotRect(int player) {
    // Position the Gambit Slots at the bottom center of the screen
    float x_offset = SCREEN_W / 2.0f;
    float x = (player == 1) ? x_offset - CARD_W_SCALED - 100 : x_offset + 100;
    float y = SCREEN_H - CARD_H_SCALED - 10;
    return (Rectangle){x, y, CARD_W_SCALED, CARD_H_SCALED};
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

// ─────────────────────────────────────────────────────────────────────────────
//  DRAWING IMPLEMENTATIONS
// ─────────────────────────────────────────────────────────────────────────────
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

void DrawLeaderboard(const GameState *g)
{
    static int scroll_offset = 0;
    const int entries_per_page = 15;
    const int max_scroll = (g->leaderboard_count > entries_per_page) ? (g->leaderboard_count - entries_per_page) : 0;

    // Handle scroll input
    float wheel = GetMouseWheelMove();
    if (wheel > 0)
        scroll_offset = (scroll_offset > 0) ? scroll_offset - 1 : 0;
    if (wheel < 0)
        scroll_offset = (scroll_offset < max_scroll) ? scroll_offset + 1 : max_scroll;

    Rectangle bg = {CENTER_X - 650, 120, 1300, 900};
    for (int y = (int)bg.y; y < (int)(bg.y + bg.height); y++)
    {
        float t = (float)(y - bg.y) / bg.height;
        Color col = ColorLerp((Color){100, 52, 60, 105}, (Color){0, 52, 60, 105}, t);
        DrawRectangle(bg.x, y, (int)bg.width, 1, col);
    }
    DrawRectangleLinesEx(bg, 6, Fade(DARKPURPLE, 0.8f));

    const char *title = "HIGH ROLLERS";
    int title_w = MeasureText(title, 50);
    DrawText(title, CENTER_X - title_w / 2, 130, 50, GOLD);

    int x_rank = (int)CENTER_X - 500;
    int x_name = x_rank + 100;
    int x_total = x_rank + 500;
    int x_game = x_rank + 780;

    DrawText("Rank", x_rank, 200, 28, Fade(WHITE, 0.9f));
    DrawText("Player", x_name, 200, 28, Fade(WHITE, 0.9f));
    DrawText("Total", x_total, 200, 28, Fade(WHITE, 0.9f));
    DrawText("Date", x_game, 200, 28, Fade(WHITE, 0.9f));

    // Draw entries with scroll
    for (int i = 0; i < entries_per_page && (i + scroll_offset) < g->leaderboard_count; i++)
    {
        const LeaderboardEntry *e = &g->leaderboard[i + scroll_offset];
        int y = 240 + i * 48;
        int actual_rank = i + scroll_offset + 1;

        Color rank_color = (actual_rank == 1) ? GOLD : (actual_rank <= 3) ? ORANGE
                                                                          : Fade(WHITE, 0.7f);
        DrawText(TextFormat("#%d", actual_rank), x_rank, y, 32, rank_color);
        DrawText(e->winner_name, x_name, y + 4, 28, (actual_rank == 1) ? YELLOW : LIGHTGRAY);
        DrawText(TextFormat("$%.2f", e->total_winnings), x_total, y + 4, 28,
                 (actual_rank == 1) ? LIME : WHITE);
        DrawText(e->timestamp, x_game, y + 6, 24, Fade(GRAY, 0.8f));
    }

    // Scroll indicator
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

// In gui.c - Update the DrawGameOver function to show move bonuses:

void DrawGameOver(GameState *g)
{
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.8f));
    
    const char *winner_name = (g->winner == 1) ? GetPlayerName(g, 1) : GetPlayerName(g, 2);
    char win_text[128];
    snprintf(win_text, sizeof(win_text), "%s WINS!", winner_name);
    
    int tw = MeasureText(win_text, 90);
    DrawText(win_text, (SCREEN_W - tw) / 2, SCREEN_H / 2 - 150, 90, GOLD);

    DrawText("CONGRATULATIONS!", 
             (SCREEN_W - MeasureText("CONGRATULATIONS!", 60)) / 2, 
             SCREEN_H / 2 - 60, 60, YELLOW);
    
    // Display winner's final score
    float winner_score = (g->winner == 1) ? g->final_score_p1 : g->final_score_p2;
    float winner_bonus = 10.0f * g->total_moves;
    
    DrawText(TextFormat("Final Balance: $%.2f", winner_score),
             (SCREEN_W - MeasureText(TextFormat("Final Balance: $9999.99"), 40)) / 2,
             SCREEN_H / 2 + 20, 40, LIME);
    
    DrawText(TextFormat("Move Bonus: +$%.2f (%d moves)", winner_bonus, g->total_moves),
             (SCREEN_W - MeasureText(TextFormat("Move Bonus: +$999.99 (99 moves)"), 35)) / 2,
             SCREEN_H / 2 + 70, 35, GREEN);
    
    // Display loser's penalty
    const char *loser_name = (g->winner == 1) ? GetPlayerName(g, 2) : GetPlayerName(g, 1);
    float loser_score = (g->winner == 1) ? g->final_score_p2 : g->final_score_p1;
    float loser_penalty = -10.0f * g->total_moves;
    
    DrawText(TextFormat("%s: $%.2f (Penalty: $%.2f)", loser_name, loser_score, loser_penalty),
             (SCREEN_W - MeasureText(TextFormat("PlayerName: $9999.99 (Penalty: $-999.99)"), 30)) / 2,
             SCREEN_H / 2 + 120, 30, ORANGE);

    // Decorative particles
    float t = (float)GetTime();
    for (int i = 0; i < 60; i++)
    {
        float a = i * 0.1047f + t;
        float r = 100 + sinf(t * 2 + i * 0.3f) * 60;
        Vector2 p = {SCREEN_W / 2 + cosf(a) * r, SCREEN_H / 2 + sinf(a) * r};
        Color c = (i % 3 == 0) ? ORANGE : (i % 3 == 1) ? WHITE : GREEN;
        DrawCircleV(p, 6, c);
    }
    
    DrawText("Press R to Restart • ESC to Quit", 
             (SCREEN_W - 480) / 2, SCREEN_H - 100, 36, WHITE);
}

void DrawPlayerUI(const GameState *g, int player)
{
    Rectangle frame_rect = (player == 1) ? (Rectangle){P1_UI_X, UI_Y, UI_FRAME_W, UI_FRAME_H}
                                         : (Rectangle){P2_UI_X, UI_Y, UI_FRAME_W, UI_FRAME_H};
    float balance = (player == 1) ? g->p1_balance : g->p2_balance;
    int ranks = (player == 1) ? g->p1_completed_ranks : g->p2_completed_ranks;
    Color p_color = (player == 1) ? BLUE : RED;

    if (g_ui_frame_texture.id)
    {
        DrawTexturePro(g_ui_frame_texture,
                       (Rectangle){0, 0, (float)g_ui_frame_texture.width, (float)g_ui_frame_texture.height},
                       frame_rect, (Vector2){0, 0}, 0.0f, RAYWHITE);
    }
    else
    {
        DrawRectangleRec(frame_rect, RAYWHITE);
        DrawRectangleLinesEx(frame_rect, 3, p_color);
    }

    int y1 = (int)frame_rect.y + UI_TEXT_OFFSET_X;
    DrawText(TextFormat("   ||| $%.2f", balance), (int)frame_rect.x + UI_TEXT_OFFSET_X, y1, 21, p_color);

    int y2 = y1 + UI_LINE_SPACING;
    DrawText(TextFormat("   ||| Ranks: %d/%d", ranks, KEYCARDS), (int)frame_rect.x + UI_TEXT_OFFSET_X, y2, 21, p_color);

    int y3 = y2 + UI_LINE_SPACING;
    DrawText(TextFormat("   ||| Moves: %d", g->total_moves), (int)frame_rect.x + UI_TEXT_OFFSET_X, y3, 21, p_color);
}

void DrawGameLayout(const GameState *g)
{
    Vector2 mouse = GetMousePosition();
    DrawPlayerUI(g, 1);
    DrawPlayerUI(g, 2);

    // 1. Draw Keycards and Slots
    for (int k = 0; k < KEYCARDS; k++)
    {
        // Keycard (Center)
        Rectangle key_rect = KeyCardRect(k);
        DrawCard(g->keycards[k], key_rect, RAYWHITE);

        // Player 1 Slots (Left)
        for (int s = 0; s < 3; s++)
        {
            Rectangle slot_rect = SlotRect(1, k, s);
            if (g->p1_slots[k][s].is_valid)
            {
                DrawCard(g->p1_slots[k][s], slot_rect, RAYWHITE);
            }
        }
        // Player 2 Slots (Right)
        for (int s = 0; s < 3; s++)
        {
            Rectangle slot_rect = SlotRect(2, k, s);
            if (g->p2_slots[k][s].is_valid)
            {
                DrawCard(g->p2_slots[k][s], slot_rect, RAYWHITE);
            }
            Rectangle p1_gambit_rect = GambitSlotRect(1);
            Rectangle p2_gambit_rect = GambitSlotRect(2);

            // P1 Gambit Slot
            DrawRectangleRec(p1_gambit_rect, g->p1_gambit_slot.is_valid ? Fade(GOLD, 0.5f) : Fade(DARKGRAY, 0.5f));
            DrawText("P1 GAMBIT SLOT", (int)p1_gambit_rect.x, (int)p1_gambit_rect.y - 30, 20, GOLD);
            if (g->p1_gambit_slot.is_valid)
            {
                DrawCard(g->p1_gambit_slot, p1_gambit_rect, RAYWHITE);
            }

            // P2 Gambit Slot
            DrawRectangleRec(p2_gambit_rect, g->p2_gambit_slot.is_valid ? Fade(GOLD, 0.5f) : Fade(DARKGRAY, 0.5f));
            DrawText("P2 GAMBIT SLOT", (int)p2_gambit_rect.x, (int)p2_gambit_rect.y - 30, 20, GOLD);
            if (g->p2_gambit_slot.is_valid)
            {
                DrawCard(g->p2_gambit_slot, p2_gambit_rect, RAYWHITE);
            }
        }
    }

    // 2. Draw Hands & Discard Buttons
    for (int i = 0; i < HAND_SIZE; i++)
    {
        // --- Player 1 ---
        Rectangle p1_rect = HandRect(1, i);
        if (i < g->p1_hand_size)
        {
            Color tint = (g->p1_selected && i == g->p1_discard_idx) ? YELLOW : RAYWHITE;
            DrawCard(g->player1_hand[i], p1_rect, tint);
        }

        bool p1_btn_enabled = (g->state == STATE_P1_SELECT_DISCARD && i < g->p1_hand_size);
        bool p1_btn_hovered = CheckCollisionPointRec(mouse, ButtonRect(1, i));
        const char *p1_text = (g->state == STATE_P1_SELECT_DISCARD) ? "Discard" : "Place";
        DrawButton(ButtonRect(1, i), p1_btn_hovered, p1_btn_enabled, p1_text);

        // --- Player 2 ---
        Rectangle p2_rect = HandRect(2, i);
        if (i < g->p2_hand_size)
        {
            Color tint = (g->p2_selected && i == g->p2_discard_idx) ? YELLOW : RAYWHITE;
            DrawCard(g->player2_hand[i], p2_rect, tint);
        }

        bool p2_btn_enabled = (g->state == STATE_P2_SELECT_DISCARD && g->mode == MODE_PVP && i < g->p2_hand_size);
        bool p2_btn_hovered = CheckCollisionPointRec(mouse, ButtonRect(2, i));
        const char *p2_text = (g->state == STATE_P2_SELECT_DISCARD) ? "Discard" : "Place";
        DrawButton(ButtonRect(2, i), p2_btn_hovered, p2_btn_enabled, p2_text);
    }

    // 3. Draw Deck
    Rectangle deck_rect = {CENTER_X - CARD_W_SCALED / 2, DISCARD_PILE_Y, CARD_W_SCALED, CARD_H_SCALED};
    if (g_card_back_texture.id)
    {
        DrawTexturePro(g_card_back_texture,
                       (Rectangle){0, 0, (float)g_card_back_texture.width, (float)g_card_back_texture.height},
                       deck_rect, (Vector2){0, 0}, 0.0f, RAYWHITE);
    }
    DrawText(TextFormat("Deck: %d", g->current_deck_size), (int)deck_rect.x, (int)deck_rect.y + CARD_H_SCALED + 10, 20, BLACK);

    // 4. Draw Discard Piles (Revealed Cards)
    if (g->revealed_p1.is_valid)
    {
        DrawCard(g->revealed_p1, DiscardPileRect(1), RAYWHITE);
    }
    if (g->revealed_p2.is_valid)
    {
        DrawCard(g->revealed_p2, DiscardPileRect(2), RAYWHITE);
    }
}