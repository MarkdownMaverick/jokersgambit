#include "mainmenu.h"
#include "useraccount.h"
#include "gui.h"

char new_first_name[MAX_ACCOUNT_NAME_LEN + 1] = {0};
char new_last_name[MAX_ACCOUNT_NAME_LEN + 1] = {0};
int current_name_field = 1;
double error_timer = 0.0;
char error_message[128] = {0};

void ShowAccountStatus(GameState *g, const char *msg) {
    strncpy(g->account_status_message, msg, sizeof(g->account_status_message) - 1);
    g->account_status_timer = GetTime();
}

// ─────────────────────────────────────────────────────────────────────────────
//  MAIN MENU - AI vs AI REMOVED
// ─────────────────────────────────────────────────────────────────────────────

void DrawMainMenu(GameState *g) {
    (void)g;
    
    Rectangle play_rect = {CENTER_X - 150, 300, 300, 60};
    Rectangle accounts_rect = {CENTER_X - 150, 400, 300, 60};
    Rectangle settings_rect = {CENTER_X - 150, 500, 300, 60};
    Rectangle leaderboard_rect = {CENTER_X - 150, 600, 300, 60};
    Vector2 mouse = GetMousePosition();

    DrawText("JOKERS GAMBIT", CENTER_X - 350, 150, 90, GOLD);

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

void UpdateMainMenu(GameState *g, Vector2 mouse) {
    Rectangle play_rect = {CENTER_X - 150, 300, 300, 60};
    Rectangle accounts_rect = {CENTER_X - 150, 400, 300, 60};
    Rectangle settings_rect = {CENTER_X - 150, 500, 300, 60};
    Rectangle leaderboard_rect = {CENTER_X - 150, 600, 300, 60};

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, play_rect)) {
            // Check if both players are logged in
            if (g->p1_account_index == -1 || g->p2_account_index == -1) {
                ShowAccountStatus(g, "Please log in 2 accounts to start");
                g->state = STATE_ACCOUNTS_MANAGER;
            } else {
                // Determine game mode based on who's logged in
                bool p1_is_ai = g->accounts[g->p1_account_index].is_ai;
                bool p2_is_ai = g->accounts[g->p2_account_index].is_ai;
                
                if (!p1_is_ai && !p2_is_ai) {
                    g->mode = MODE_PVP;
                } else if (p1_is_ai && p2_is_ai) {
                    g->mode = MODE_AIVSAI;
                } else {
                    g->mode = MODE_PVAI;
                }
                
                RestartGameKeepingAccounts(g);
                g->state = STATE_P1_SELECT_DISCARD;
            }
        }
        if (CheckCollisionPointRec(mouse, accounts_rect)) {
            g->state = STATE_ACCOUNTS_MANAGER;
        }
        if (CheckCollisionPointRec(mouse, settings_rect)) {
            g->state = STATE_SETTINGS;
        }
        if (CheckCollisionPointRec(mouse, leaderboard_rect)) {
            g->state = STATE_LEADERBOARD;
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  SETTINGS SCREEN - AI SELECTION REMOVED, CARD COVER TOGGLE ADDED
// ─────────────────────────────────────────────────────────────────────────────

void DrawSettings(GameState *g) {
    DrawText("SETTINGS", CENTER_X - 200, 100, 80, GOLD);

    // Card Cover Toggle
    DrawText("P2 Card Privacy:", CENTER_X - 300, 300, 40, WHITE);
    
    Rectangle cover_btn_rect = {CENTER_X - 150, 380, 300, 80};
    Vector2 mouse = GetMousePosition();
    bool cover_hover = CheckCollisionPointRec(mouse, cover_btn_rect);
    
    Color cover_color = g->cover_p2_cards ? LIME : RED;
    DrawRectangleRec(cover_btn_rect, cover_hover ? Fade(cover_color, 0.8f) : cover_color);
    
    const char *cover_text = g->cover_p2_cards ? "COVERED" : "UNCOVERED";
    int text_w = MeasureText(cover_text, 35);
    DrawText(cover_text, CENTER_X - text_w/2, 380 + 25, 35, BLACK);
    
    DrawText("(Hides P2 cards from view during local play)", CENTER_X - 300, 480, 25, LIGHTGRAY);
    
    // Back Button
    Rectangle back_rect = {CENTER_X - 150, SCREEN_H - 180, 300, 80};
    bool back_hover = CheckCollisionPointRec(mouse, back_rect);
    DrawRectangleRec(back_rect, back_hover ? RED : MAROON);
    DrawText("BACK", back_rect.x + 100, back_rect.y + 25, 35, WHITE);
}

void UpdateSettings(GameState *g, Vector2 mouse) {
    Rectangle cover_btn_rect = {CENTER_X - 150, 380, 300, 80};
    Rectangle back_rect = {CENTER_X - 150, SCREEN_H - 180, 300, 80};
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, cover_btn_rect)) {
            g->cover_p2_cards = !g->cover_p2_cards;
        }
        else if (CheckCollisionPointRec(mouse, back_rect)) {
            g->state = STATE_MAIN_MENU;
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  ACCOUNT MANAGER - REDESIGNED WITH P1/P2 SLOT SYSTEM
// ─────────────────────────────────────────────────────────────────────────────

void DrawAccountsManager(const GameState *g) {
    DrawText("ACCOUNT MANAGER", CENTER_X - 900, 80, 20, GOLD);

    // 1. Draw Current Login Status Top Header
    const char *p1 = (g->p1_account_index == -1) ? "---" : GetPlayerName(g, 1);
    const char *p2 = (g->p2_account_index == -1) ? "---" : GetPlayerName(g, 2);

    DrawText("P1 Current:", 150, 160, 40, LIME);
    DrawText(p1, 400, 160, 40, WHITE);
    DrawText("P2 Current:", 950, 160, 40, ORANGE);
    DrawText(p2, 1200, 160, 40, WHITE);

    // 2. Feedback Message (Green fade text)
    if (GetTime() - g->account_status_timer < 2.0) {
        float alpha = 1.0f - (float)(GetTime() - g->account_status_timer) / 2.0f;
        DrawText(g->account_status_message, CENTER_X - MeasureText(g->account_status_message, 30)/2, 220, 30, Fade(GREEN, alpha));
    }

    int y = 280;

    // 3. Draw Accounts List
    for (int i = 0; i < g->account_count; i++) {
        const Account *a = &g->accounts[i];
        bool is_p1 = (i == g->p1_account_index);
        bool is_p2 = (i == g->p2_account_index);
        
        // Background
        Rectangle row_rect = {150, (float)y, SCREEN_W - 300, 100};
        Color bg_col = a->is_ai ? Fade(PURPLE, 0.2f) : Fade(DARKBLUE, 0.3f);
        if (is_p1) bg_col = Fade(LIME, 0.2f);
        if (is_p2) bg_col = Fade(ORANGE, 0.2f);
        
        DrawRectangleRec(row_rect, bg_col);
        DrawRectangleLinesEx(row_rect, 2, a->is_ai ? PURPLE : DARKGRAY);

        // Name & Stats
        DrawText(a->first_name, row_rect.x + 20, row_rect.y + 20, 40, WHITE);
        if (!a->is_ai) DrawText(a->last_name, row_rect.x + 20 + MeasureText(a->first_name, 40) + 10, row_rect.y + 20, 40, WHITE);
        DrawText(TextFormat("$%.2f  W:%d L:%d", a->balance, a->wins, a->losses), row_rect.x + 20, row_rect.y + 65, 20, LIGHTGRAY);

        // AI Label
        if (a->is_ai) {
            DrawText("[AI]", row_rect.x + 350, row_rect.y + 30, 30, PURPLE);
        }

        // --- BUTTONS ---
        float btn_w = 140;
        float btn_h = 40;
        float start_x = row_rect.x + 600;
        
        // P1 Button
        Rectangle btn_p1 = {start_x, row_rect.y + 30, btn_w, btn_h};
        bool hover_p1 = CheckCollisionPointRec(GetMousePosition(), btn_p1);
        Color p1_col = is_p1 ? RED : LIME; 
        const char* p1_txt = is_p1 ? "LOGOUT P1" : "LOGIN P1";
        DrawRectangleRec(btn_p1, hover_p1 ? Fade(p1_col, 0.8f) : p1_col);
        DrawText(p1_txt, btn_p1.x + 10, btn_p1.y + 10, 20, BLACK);

        // P2 Button
        Rectangle btn_p2 = {start_x + 160, row_rect.y + 30, btn_w, btn_h};
        bool hover_p2 = CheckCollisionPointRec(GetMousePosition(), btn_p2);
        Color p2_col = is_p2 ? RED : ORANGE;
        const char* p2_txt = is_p2 ? "LOGOUT P2" : "LOGIN P2";
        DrawRectangleRec(btn_p2, hover_p2 ? Fade(p2_col, 0.8f) : p2_col);
        DrawText(p2_txt, btn_p2.x + 10, btn_p2.y + 10, 20, BLACK);

        // Delete Button (only for human accounts)
        if (!a->is_ai) {
            Rectangle btn_del = {row_rect.x + row_rect.width - 100, row_rect.y + 30, 80, btn_h};
            bool hover_del = CheckCollisionPointRec(GetMousePosition(), btn_del);
            DrawRectangleRec(btn_del, hover_del ? RED : MAROON);
            DrawText("DEL", btn_del.x + 20, btn_del.y + 10, 20, WHITE);
        }
      
        y += 110;
    }

    // Bottom Navigation
    Rectangle create = {CENTER_X - 300, SCREEN_H - 120, 280, 80};
    Rectangle back = {CENTER_X + 20, SCREEN_H - 120, 280, 80};
    DrawRectangleRec(create, CheckCollisionPointRec(GetMousePosition(), create) ? LIME : DARKGREEN);
    DrawRectangleRec(back, CheckCollisionPointRec(GetMousePosition(), back) ? RED : MAROON);
    DrawText("CREATE NEW", create.x + 50, create.y + 25, 30, BLACK);
    DrawText("BACK", back.x + 100, back.y + 25, 30, WHITE);
}

void UpdateAccountsManager(GameState *g, Vector2 mouse) {
    int y = 280;
    bool action_taken = false;

    for (int i = 0; i < g->account_count; i++) {
        Rectangle row_rect = {150, (float)y, SCREEN_W - 300, 100};
        float btn_w = 140;
        float btn_h = 40;
        float start_x = row_rect.x + 600;

        Rectangle btn_p1 = {start_x, row_rect.y + 30, btn_w, btn_h};
        Rectangle btn_p2 = {start_x + 160, row_rect.y + 30, btn_w, btn_h};
        Rectangle btn_del = {row_rect.x + row_rect.width - 100, row_rect.y + 30, 80, btn_h};

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // P1 Login/Logout
            if (CheckCollisionPointRec(mouse, btn_p1)) {
                if (g->p1_account_index == i) {
                    LogoutAccount(g, 1);
                    ShowAccountStatus(g, "Logged P1 Out");
                } else {
                    // Check if same account
                    if (g->p2_account_index == i) {
                        ShowAccountStatus(g, "Cannot play against yourself!");
                    } else {
                        LoginAccount(g, i, 1);
                        ShowAccountStatus(g, TextFormat("%s logged into P1", g->accounts[i].first_name));
                    }
                }
            }
            // P2 Login/Logout
            else if (CheckCollisionPointRec(mouse, btn_p2)) {
                if (g->p2_account_index == i) {
                    LogoutAccount(g, 2);
                    ShowAccountStatus(g, "Logged P2 Out");
                } else {
                    // Check if same account
                    if (g->p1_account_index == i) {
                        ShowAccountStatus(g, "Cannot play against yourself!");
                    } else {
                        LoginAccount(g, i, 2);
                        ShowAccountStatus(g, TextFormat("%s logged into P2", g->accounts[i].first_name));
                    }
                }
            }
            // Delete (only for human accounts not logged in)
            else if (!g->accounts[i].is_ai && CheckCollisionPointRec(mouse, btn_del)) {
                if (g->p1_account_index == i || g->p2_account_index == i) {
                    ShowAccountStatus(g, "Cannot delete logged-in account!");
                } else {
                    DeleteAccount(g, i);
                    ShowAccountStatus(g, "Account deleted");
                    action_taken = true;
                }
            }
        }
        
        if (action_taken) break;
        y += 110;
    }

    // Navigation Buttons
    Rectangle create = {CENTER_X - 300, SCREEN_H - 120, 280, 80};
    Rectangle back = {CENTER_X + 20, SCREEN_H - 120, 280, 80};

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, create)) {
            current_name_field = 1;
            memset(new_first_name, 0, sizeof(new_first_name));
            memset(new_last_name, 0, sizeof(new_last_name));
            g->state = STATE_ACCOUNT_CREATE;
        }
        if (CheckCollisionPointRec(mouse, back)) {
            g->state = STATE_MAIN_MENU;
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  ACCOUNT CREATION
// ─────────────────────────────────────────────────────────────────────────────

void DrawAccountCreate(const GameState *g) {
    (void)g;
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.8f));

    DrawText("NEW ACCOUNT", CENTER_X - 280, 200, 80, LIME);

    const char *label1 = "First Name";
    const char *label2 = "Last Name";
    Color col1 = (current_name_field == 1) ? YELLOW : WHITE;
    Color col2 = (current_name_field == 2) ? YELLOW : WHITE;

     DrawText(label1, CENTER_X - 150, 360, 40, col1);
     DrawText(label2, CENTER_X - 150, 480, 40, col2);
     
     Rectangle r1 = {CENTER_X - 140, 400, 280, 70};
     Rectangle r2 = {CENTER_X - 140, 520, 280, 70};

    DrawRectangleRec(r1, DARKGRAY);
    DrawRectangleLinesEx(r1, 4, col1);
    DrawRectangleRec(r2, DARKGRAY);
    DrawRectangleLinesEx(r2, 4, col2);

    DrawText(new_first_name, r1.x + 20, r1.y + 20, 40, WHITE);
    DrawText(new_last_name, r2.x + 20, r2.y + 20, 40, WHITE);

    if (current_name_field == 1 && ((int)(GetTime() * 2) % 2))
        DrawText("_", r1.x + 25 + MeasureText(new_first_name, 40), r1.y + 20, 40, WHITE);
    if (current_name_field == 2 && ((int)(GetTime() * 2) % 2))
        DrawText("_", r2.x + 25 + MeasureText(new_last_name, 40), r2.y + 20, 40, WHITE);

    Rectangle confirm = {CENTER_X - 220, 820, 200, 80};
    Rectangle cancel = {CENTER_X + 20, 820, 200, 80};

    bool ok = strlen(new_first_name) >= 2 && strlen(new_last_name) >= 2;
    DrawRectangleRec(confirm, ok ? LIME : DARKGREEN);
    DrawRectangleRec(cancel, RED);
    DrawText("CREATE", confirm.x + 40, confirm.y + 25, 35, BLACK);
    DrawText("CANCEL", cancel.x + 40, cancel.y + 25, 35, WHITE);
    DrawRectangleLines(confirm.x, confirm.y, confirm.width, confirm.height, RED);
    DrawRectangleLines(cancel.x, cancel.y, cancel.width, cancel.height, BLUE);
}

void UpdateAccountCreate(GameState *g) {
    int key = GetCharPressed();
    char *field = (current_name_field == 1) ? new_first_name : new_last_name;

    while (key) {
        if ((key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z')) {
            int len = strlen(field);
            if (len < MAX_ACCOUNT_NAME_LEN) {
                field[len] = (char)key;
                field[len + 1] = '\0';
            }
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && strlen(field) > 0)
        field[strlen(field) - 1] = '\0';

    if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_ENTER))
        current_name_field = (current_name_field == 1) ? 2 : 1;

    Vector2 m = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Rectangle confirm = {CENTER_X - 220, 820, 200, 80};
        Rectangle cancel = {CENTER_X + 20, 820, 200, 80};

        if (CheckCollisionPointRec(m, confirm)) {
            if (strlen(new_first_name) >= 2 && strlen(new_last_name) >= 2) {
                if (CreateNewAccount(g, new_first_name, new_last_name)) {
                    ShowAccountStatus(g, "Account created successfully!");
                    g->state = STATE_ACCOUNTS_MANAGER;
                } else {
                    ShowAccountStatus(g, "Name already taken!");
                }
            }
        }
        else if (CheckCollisionPointRec(m, cancel))
            g->state = STATE_ACCOUNTS_MANAGER;
    }
}