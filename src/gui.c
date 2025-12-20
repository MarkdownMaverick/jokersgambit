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
#define ATLAS_WIDTH 2600
#define ATLAS_HEIGHT 1500
#define ATLAS_COLS 13
#define ATLAS_ROWS 5
#define CARD_SOURCE_W (ATLAS_WIDTH / ATLAS_COLS)
#define CARD_SOURCE_H (ATLAS_HEIGHT / ATLAS_ROWS)

#include "gui.h"
#include "useraccount.h"

Texture2D g_atlas_texture = {0};

void LoadCardAtlas(void)
{
    g_atlas_texture = LoadTexture("assets/DECK0.png");
    if (g_atlas_texture.id == 0) {
        printf("ERROR: Failed to load card atlas DECK0.png\n");
    } else {
        printf("Card atlas loaded successfully: %dx%d\n", 
               g_atlas_texture.width, g_atlas_texture.height);
    }
}

void UnloadCardAtlas(void)
{
    UnloadTexture(g_atlas_texture);
}

Rectangle AtlasRect(int row, int col)
{
    return (Rectangle){
        (float)(col * ATLAS_CARD_WIDTH),
        (float)(row * ATLAS_CARD_HEIGHT),
        (float)ATLAS_CARD_WIDTH,
        (float)ATLAS_CARD_HEIGHT
    };
}

Rectangle GetAtlasSourceRect(Rank rank, Suit suit)
{
    int row = -1;
    int col = -1;
    if (rank == RANK_JOKER) return AtlasRect(4, 0); 
    if (rank == RANK_BACK || rank == RANK_NONE) return AtlasRect(4, 2); 

    switch (suit) {
        case SUIT_HEARTS:   row = 0; break;
        case SUIT_CLUBS:    row = 1; break;
        case SUIT_DIAMONDS: row = 2; break;
        case SUIT_SPADES:   row = 3; break;
        case SUIT_NONE:     row = 4; break;
        default:            row = 4; break;
    }

    switch (rank) {
        case RANK_2:     col = 0;  break;
        case RANK_3:     col = 1;  break;
        case RANK_4:     col = 2;  break;
        case RANK_5:     col = 3;  break;
        case RANK_6:     col = 4;  break;
        case RANK_7:     col = 5;  break;
        case RANK_8:     col = 6;  break;
        case RANK_9:     col = 7;  break;
        case RANK_10:    col = 8;  break;
        case RANK_JACK:  col = 9;  break;
        case RANK_QUEEN: col = 10; break;
        case RANK_KING:  col = 11; break;
        case RANK_ACE:   col = 12; break;
        case RANK_JOKER: col = 0;  break;
        case RANK_BACK:  col = 2;  break;
        case RANK_NONE:  col = 2;  break;
        default:         col = 2;  break;
    }
    return AtlasRect(row, col);
}

Rectangle GetAtlasBackCard(void)
{
    return AtlasRect(4, 2);
}

Rectangle GetAtlasTempCover(void)
{
    return AtlasRect(4, 3);
}

Rectangle GetAtlasJoker(int joker_index)
{
    if (joker_index == 0) return AtlasRect(4, 0); 
    else return AtlasRect(4, 1);
}

Rectangle GetDealingAnimationRect(void) {
    int frame_count = 11;
    double time = GetTime();
    int frame = ((int)(time * 12.0)) % frame_count; 
    return AtlasRect(4, 2 + frame); 
}

Rectangle GetCardSourceRect(Card c)
{
    int col;
    int row;
    
    if (c.rank == RANK_NONE)
    {
        row = 4; col = 2;
        return (Rectangle){(float)(col * CARD_SOURCE_W), (float)(row * CARD_SOURCE_H), (float)CARD_SOURCE_W, (float)CARD_SOURCE_H};
    }
    if (c.rank == RANK_JOKER)
    {
        row = 4;
        if (c.filename[1] == 'B') col = 1; else col = 0; 
        return (Rectangle){(float)(col * CARD_SOURCE_W), (float)(row * CARD_SOURCE_H), (float)CARD_SOURCE_W, (float)CARD_SOURCE_H};
    }
    switch (c.suit)
    {
    case SUIT_HEARTS: row = 0; break;
    case SUIT_CLUBS: row = 1; break;
    case SUIT_DIAMONDS: row = 2; break;
    case SUIT_SPADES: row = 3; break;
    case SUIT_NONE: row = 4; break;
    default: row = 0; break;
    }
    col = (int)c.rank;
    return (Rectangle){(float)(col * CARD_SOURCE_W), (float)(row * CARD_SOURCE_H), (float)CARD_SOURCE_W, (float)CARD_SOURCE_H};
}

float GetRankY(int key_idx)
{
    return GRID_START_Y + (float)key_idx * ROW_SPACING;
}

Rectangle KeyCardRect(int key_idx)
{
    float y = GetRankY(key_idx);
    return (Rectangle){KEYCARD_COL_X, y, CARD_W_SCALED, CARD_H_SCALED};
}

Rectangle SlotRect(int player, int key_idx, int slot_idx)
{
    float y = GetRankY(key_idx);
    float base_x = KEYCARD_COL_X + (player == 1 ? -UI_PADDING_SMALL : CARD_W_SCALED + UI_PADDING_SMALL);
    float x;
    if (player == 1) x = base_x - (float)(slot_idx + 1) * CARD_W_SCALED - (float)slot_idx * UI_PADDING_SMALL;
    else x = base_x + (float)slot_idx * (CARD_W_SCALED + UI_PADDING_SMALL);
    return (Rectangle){x, y, CARD_W_SCALED, CARD_H_SCALED};
}

Rectangle HandRect(int player, int idx)
{
    float y = (player == 1) ? P1_HAND_Y : P2_HAND_Y;
    float start_x = (player == 1) ? P1_HAND_START_X : P2_HAND_START_X;
    float x = start_x + (float)idx * (CARD_W_SCALED + HAND_CARD_GAP);
    return (Rectangle){x, y, CARD_W_SCALED, CARD_H_SCALED};
}

Rectangle ButtonRect(int player, int idx)
{
    Rectangle card_rect = HandRect(player, idx);
    float x = card_rect.x + (CARD_W_SCALED / 2.0f) - (BUTTON_W / 2.0f);
    float y = card_rect.y + CARD_H_SCALED + UI_PADDING_LARGE;
    return (Rectangle){x, y, BUTTON_W, BUTTON_H};
}

Rectangle DiscardPileRect(int player)
{
    float x = (player == 1) ? P1_DISCARD_X : P2_DISCARD_X;
    return (Rectangle){x, DISCARD_PILE_Y, CARD_W_SCALED, CARD_H_SCALED};
}

void DrawCard(Card c, Rectangle rect, Color tint)
{
    if (!c.is_valid) return;
    Rectangle source = GetCardSourceRect(c);
    if (g_atlas_texture.id != 0)
        DrawTexturePro(g_atlas_texture, source, rect, (Vector2){0, 0}, 0.0f, tint);
}

void DrawButton(Rectangle rect, bool is_hovered, bool is_enabled, const char *text)
{
    Color base_color = is_enabled ? RAYWHITE : DARKGRAY;
    Color tint = is_hovered ? Fade(base_color, 0.7f) : base_color;
    if (g_button_texture.id != 0)
        DrawTexturePro(g_button_texture, (Rectangle){0, 0, (float)g_button_texture.width, (float)g_button_texture.height}, rect, (Vector2){0, 0}, 0.0f, tint);
    int text_len = MeasureText(text, 14);
    int text_x = (int)(rect.x + rect.width / 2.0f - (float)text_len / 2.0f);
    int text_y = (int)(rect.y + rect.height / 2.0f - 7.0f);
    DrawText(text, text_x, text_y, 14, BLACK);
}

void DrawLeaderboard(GameState *g)
{
    Rectangle bg = {CENTER_X - 650.0f, 120.0f, 1300.0f, 900.0f};
    DrawRectangleRec(bg, Fade(BLACK, 0.9f));
    DrawRectangleLinesEx(bg, 3.0f, Fade(DARKBLUE, 0.8f));
    
    DrawText("HIGH ROLLERS", CENTER_X - 200, 130, 50, GOLD);
    
    Rectangle sort_btn = {CENTER_X + 400.0f, 130.0f, 200.0f, 50.0f};
    DrawRectangleRec(sort_btn, GOLD);
    DrawText(g->leaderboard_sort_by_rounds ? "BY ROUNDS" : "BY CASH", (int)(sort_btn.x + 30), (int)(sort_btn.y + 15), 25, BLACK);

    int x_rank = CENTER_X - 500;
    int x_name = x_rank + 100;
    int x_cash = x_rank + 500;
    int x_rounds = x_rank + 700;
    int x_time = x_rank + 800;
    int x_date = x_rank + 900;

    DrawText("Rank", x_rank, 200, 28, WHITE);
    DrawText("Winner", x_name, 200, 28, WHITE);
    DrawText("Credits", x_cash, 200, 28, WHITE);
    DrawText("Rounds", x_rounds, 200, 28, WHITE);
    DrawText("Time", x_time, 200, 28, WHITE);
    DrawText("Date", x_date, 200, 28, WHITE);

    for (int i = 0; i < 15 && i < g->leaderboard_count; i++)
    {
        const LeaderboardEntry *e = &g->leaderboard[i];
        int y = 240 + i * 48;
        DrawText(TextFormat("#%d", i+1), x_rank, y, 28, GOLD);
        DrawText(e->winner_name, x_name, y, 28, WHITE);
        DrawText(TextFormat("$%.2f", e->total_winnings), x_cash, y, 28, LIME);
        DrawText(TextFormat("%d", e->total_rounds), x_rounds, y, 28, SKYBLUE);
        DrawText(e->timestamp, x_time, y, 24, GRAY);

        DrawText(e->timestamp, x_date, y, 24, GRAY);
    }
    
    DrawText("Click Anywhere to Return", CENTER_X - 250, 980, 24, WHITE);
}

Rectangle ContinueButtonRect(int player)
{
    Rectangle ref_rect;
    float x;
    if (player == 1)
    {
        ref_rect = ButtonRect(1, 4);
        x = ref_rect.x + ref_rect.width + 10.0f; 
    }
    else
    {
        ref_rect = ButtonRect(2, 0);
        x = ref_rect.x - 80.0f - 10.0f; 
    }
    return (Rectangle){x, ref_rect.y, 80.0f, ref_rect.height};
}

void DrawContinueButtons(const GameState *g, Vector2 mouse)
{
    if (g->state != STATE_WAIT_FOR_TURN)
        return;
    
    for (int p = 1; p <= 2; p++)
    {
        bool is_human_active = (p == 1) ? (!g->p1_done_placing && !IsPlayerAI(g, 1))
                                        : (!g->p2_done_placing && !IsPlayerAI(g, 2));
        
        if (is_human_active)
        {
            Rectangle btn = ContinueButtonRect(p);
            bool hover = CheckCollisionPointRec(mouse, btn);
            Color tint = hover ? Fade(RAYWHITE, 0.7f) : RAYWHITE;
            
            if (g_button_texture.id != 0)
                DrawTexturePro(g_button_texture, (Rectangle){0, 0, (float)g_button_texture.width, (float)g_button_texture.height}, btn, (Vector2){0, 0}, 0.0f, tint);
            else
                DrawRectangleRec(btn, hover ? GOLD : ORANGE); 
            
            int text_w = MeasureText("PASS", 20);
            int text_x = (int)(btn.x + (btn.width - (float)text_w) / 2.0f);
            int text_y = (int)(btn.y + (btn.height / 2.0f - 10.0f));
            DrawText("PASS", text_x, text_y, 20, BLACK);
        }
    }
}

void DrawPlayerUI(const GameState *g, int player)
{
    float adjusted_y = UI_Y - 20.0f;
    Rectangle frame_rect = (player == 1) ? (Rectangle){P1_UI_X, adjusted_y, UI_FRAME_W, UI_FRAME_H}
                                         : (Rectangle){P2_UI_X, adjusted_y, UI_FRAME_W, UI_FRAME_H};
    
    float credits = (player == 1) ? g->p1_credits : g->p2_credits;
    float temp_credits = (player == 1) ? g->p1_temp_credits : g->p2_temp_credits;
    int ranks = (player == 1) ? g->p1_completed_ranks : g->p2_completed_ranks;
    
    Color neon_color = (player == 1) ? SKYBLUE : (Color){255, 100, 100, 255};
    const char *player_name = GetPlayerName(g, player);
    
    float margin_x = 25.0f;
    float margin_y = 25.0f;
    
    Rectangle screen_rect = {
        frame_rect.x + margin_x,
        frame_rect.y + margin_y,
        frame_rect.width - (margin_x * 2.0f),
        frame_rect.height - (margin_y * 1.5f)};
    
    DrawRectangleRec(screen_rect, Fade(BLACK, 0.85f));
    DrawRectangleLinesEx(screen_rect, 2.0f, Fade(neon_color, 0.5f));
    
    int text_x = (int)(screen_rect.x + 15.0f);
    int text_y = (int)(screen_rect.y + 15.0f);
    int line_h = 24;
    
    DrawText(TextFormat("USER: %s", player_name), text_x, text_y, 20, neon_color);
    text_y += line_h;
    DrawText(TextFormat("CREDITS: $%.2f", credits), text_x, text_y, 20, neon_color);
    text_y += line_h;
    // New Temp Credits Display
    DrawText(TextFormat("GAME BALANCE: $%.2f", temp_credits), text_x, text_y, 20, YELLOW);
    text_y += line_h;
    DrawText(TextFormat("COMPLETED: %d/%d ROUND #%d...", ranks, KEYCARDS, g->total_rounds), text_x, text_y, 20, neon_color);
}

void DrawGameLayout(const GameState *g)
{
    Vector2 mouse = GetMousePosition();
    DrawPlayerUI(g, 1);
    DrawPlayerUI(g, 2);
    
    for (int k = 0; k < KEYCARDS; k++)
    {
        DrawCard(g->keycards[k], KeyCardRect(k), RAYWHITE);
        for (int s = 0; s < 3; s++)
        {
            Rectangle slot_rect = SlotRect(1, k, s);
            if (g->p1_slots[k][s].is_valid) DrawCard(g->p1_slots[k][s], slot_rect, RAYWHITE);
            else DrawTexturePro(g_atlas_texture, GetAtlasBackCard(), slot_rect, (Vector2){0, 0}, 0.0f, Fade(WHITE, 0.25f));
        }
        for (int s = 0; s < 3; s++)
        {
            Rectangle slot_rect = SlotRect(2, k, s);
            if (g->p2_slots[k][s].is_valid) DrawCard(g->p2_slots[k][s], slot_rect, RAYWHITE);
            else DrawTexturePro(g_atlas_texture, GetAtlasBackCard(), slot_rect, (Vector2){0, 0}, 0.0f, Fade(WHITE, 0.25f));
        }
    }
    
    for (int i = 0; i < HAND_SIZE; i++)
    {
        Rectangle p1_rect = HandRect(1, i);
        if (i < g->p1_hand_size)
        {
            Color tint = (g->p1_discard_ready && i == g->p1_discard_idx) ? YELLOW : RAYWHITE;
            DrawCard(g->player1_hand[i], p1_rect, tint);
        }
        
        bool p1_btn_enabled = false;
        const char *p1_btn_text = "---";
        if (g->state == STATE_P1_SELECT_DISCARD && i < g->p1_hand_size)
        {
            if (!IsPlayerAI(g, 1)) { p1_btn_enabled = !g->p1_discard_ready; p1_btn_text = g->p1_discard_ready ? "READY" : "DISCARD"; }
            else p1_btn_text = "AI";
        }
        else if (g->state == STATE_WAIT_FOR_TURN) p1_btn_text = "PLACE";
        DrawButton(ButtonRect(1, i), CheckCollisionPointRec(mouse, ButtonRect(1, i)), p1_btn_enabled, p1_btn_text);
        
        Rectangle p2_rect = HandRect(2, i);
        if (i < g->p2_hand_size)
        {
            bool should_cover = g->cover_p2_cards && !IsPlayerAI(g, 1) && g->state == STATE_P1_SELECT_DISCARD;
            if (should_cover) DrawTexturePro(g_atlas_texture, GetAtlasTempCover(), p2_rect, (Vector2){0, 0}, 0.0f, WHITE);
            else
            {
                Color tint = (g->p2_discard_ready && i == g->p2_discard_idx) ? PURPLE : RAYWHITE;
                DrawCard(g->player2_hand[i], p2_rect, tint);
            }
        }
        
        bool p2_btn_enabled = false;
        const char *p2_btn_text = "---";
        if (g->state == STATE_P1_SELECT_DISCARD && i < g->p2_hand_size)
        {
            if (!IsPlayerAI(g, 2)) { p2_btn_enabled = !g->p2_discard_ready; p2_btn_text = g->p2_discard_ready ? "READY" : "DISCARD"; }
            else p2_btn_text = "AI";
        }
        else if (g->state == STATE_WAIT_FOR_TURN) p2_btn_text = "PLACE";
        DrawButton(ButtonRect(2, i), CheckCollisionPointRec(mouse, ButtonRect(2, i)), p2_btn_enabled, p2_btn_text);
    }
    
    Rectangle d1_rect = DiscardPileRect(1);
    if (g->revealed_p1.is_valid) DrawCard(g->revealed_p1, d1_rect, RAYWHITE);
    else DrawTexturePro(g_atlas_texture, GetAtlasBackCard(), d1_rect, (Vector2){0, 0}, 0.0f, WHITE);
    
    Rectangle d2_rect = DiscardPileRect(2);
    if (g->revealed_p2.is_valid) DrawCard(g->revealed_p2, d2_rect, RAYWHITE);
    else DrawTexturePro(g_atlas_texture, GetAtlasBackCard(), d2_rect, (Vector2){0, 0}, 0.0f, WHITE);
    
    DrawContinueButtons(g, mouse);
}

void DrawGameOver(GameState *g)
{
    DrawRectangle(0, 0, (int)SCREEN_W, (int)SCREEN_H, Fade(BLACK, 0.8f));
    const char *winner_name = (g->winner == 1) ? GetPlayerName(g, 1) : GetPlayerName(g, 2);
    char win_text[128];
    snprintf(win_text, sizeof(win_text), "The Winner: %s!", winner_name);
    
DrawText(win_text, (int)(CENTER_X - (float)MeasureText(win_text, 90)/2.0f), (int)(SCREEN_H/2 - 250), 90, GOLD);
    DrawText("CONGRATULATIONS!", (int)(CENTER_X - (float)MeasureText("CONGRATULATIONS!", 60)/2.0f), (int)(SCREEN_H/2 - 150), 60, YELLOW);
    
    // Final Score Display
    float winner_score = (g->winner == 1) ? g->final_score_p1 : g->final_score_p2;
    // Note: This score is calculated in State Machine, just display it
    DrawText(TextFormat("Final Result: $%.2f", winner_score), (int)(CENTER_X - (float)MeasureText(TextFormat("Final Result: $%.2f", winner_score), 40)/2.0f), (int)(SCREEN_H/2 - 50), 40, LIME);
    Vector2 mouse = GetMousePosition();
    Rectangle restart_btn = {CENTER_X - 450, SCREEN_H - 150, 280, 80};
    Rectangle menu_btn = {CENTER_X - 140, SCREEN_H - 150, 280, 80};
    Rectangle quit_btn = {CENTER_X + 170, SCREEN_H - 150, 280, 80};
    
    DrawRectangleRec(restart_btn, CheckCollisionPointRec(mouse, restart_btn) ? LIME : DARKGREEN);
    DrawRectangleRec(menu_btn, CheckCollisionPointRec(mouse, menu_btn) ? SKYBLUE : BLUE);
    DrawRectangleRec(quit_btn, CheckCollisionPointRec(mouse, quit_btn) ? RED : MAROON);
    
    DrawText("RESTART", (int)(restart_btn.x + 70), (int)(restart_btn.y + 25), 30, WHITE);
    DrawText("MAIN MENU", (int)(menu_btn.x + 40), (int)(menu_btn.y + 25), 30, WHITE);
    DrawText("QUIT", (int)(quit_btn.x + 100), (int)(quit_btn.y + 25), 30, WHITE);

    if (g->mode == MODE_PVAI) {
         DrawText("Restart Costs 100 Credits", (int)(restart_btn.x + 15), (int)(restart_btn.y + 85), 20, LIGHTGRAY);
    }
}