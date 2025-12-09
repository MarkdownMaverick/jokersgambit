#include "mainmenu.h"
#include "useraccount.h"
#include "gui.h"

// ─────────────────────────────────────────────────────────────────────────────
//  LOCAL STATE FOR ACCOUNT CREATION & ERROR HANDLING
// ─────────────────────────────────────────────────────────────────────────────
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
//  MAIN MENU
// ─────────────────────────────────────────────────────────────────────────────

void DrawMainMenu(GameState *g) {
    (void)g;  // Currently unused but kept for future features
    
    Rectangle play_rect = {CENTER_X - 150, 300, 300, 60};
    Rectangle accounts_rect = {CENTER_X - 150, 380, 300, 60};
    Rectangle settings_rect = {CENTER_X - 150, 460, 300, 60};
    Rectangle aivsai_rect = {CENTER_X - 150, 540, 300, 60};
    Rectangle leaderboard_rect = {CENTER_X - 150, 620, 300, 60};
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

    bool aivsai_hover = CheckCollisionPointRec(mouse, aivsai_rect);
    DrawRectangleRec(aivsai_rect, aivsai_hover ? ORANGE : ORANGE);
    DrawText("AI vs AI", (int)aivsai_rect.x + 80, (int)aivsai_rect.y + 15, 30, WHITE);

    bool lb_hover = CheckCollisionPointRec(mouse, leaderboard_rect);
    DrawRectangleRec(leaderboard_rect, lb_hover ? YELLOW : GOLD);
    DrawText("LEADERBOARD", (int)leaderboard_rect.x + 30, (int)leaderboard_rect.y + 15, 30, BLACK);
}

void UpdateMainMenu(GameState *g, Vector2 mouse) {
    Rectangle play_rect = {CENTER_X - 150, 300, 300, 60};
    Rectangle accounts_rect = {CENTER_X - 150, 380, 300, 60};
    Rectangle settings_rect = {CENTER_X - 150, 460, 300, 60};
    Rectangle aivsai_rect = {CENTER_X - 150, 540, 300, 60};
    Rectangle leaderboard_rect = {CENTER_X - 150, 620, 300, 60};

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, play_rect)) {
            if (g->p1_account_index == -1) {
                g->state = STATE_ACCOUNTS_MANAGER;
            } else {
                if (g->p2_account_index == -1) {
                    g->mode = MODE_PVAI;
                } else {
                    g->mode = MODE_PVP;
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
        if (CheckCollisionPointRec(mouse, aivsai_rect)) {
            // AI vs AI mode - logout current accounts and login FLINT & THEA
            LogoutAccount(g, 1);
            LogoutAccount(g, 2);
            
            // Find and login FLINT to P1
            for (int i = 0; i < g->account_count; i++) {
                if (g->accounts[i].is_ai && g->accounts[i].ai_type == AI_FLINT) {
                    LoginAccount(g, i, 1);
                    break;
                }
            }
            
            // Find and login THEA to P2
            for (int i = 0; i < g->account_count; i++) {
                if (g->accounts[i].is_ai && g->accounts[i].ai_type == AI_THEA) {
                    LoginAccount(g, i, 2);
                    break;
                }
            }
            
            g->mode = MODE_AIVSAI;
            RestartGameKeepingAccounts(g);
            g->state = STATE_P1_SELECT_DISCARD;
        }
        if (CheckCollisionPointRec(mouse, leaderboard_rect)) {
            g->state = STATE_LEADERBOARD;
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  SETTINGS SCREEN
// ─────────────────────────────────────────────────────────────────────────────

void DrawSettings(GameState *g) {
    DrawText("SETTINGS", CENTER_X - 200, 100, 80, GOLD);
    
    DrawText("Select AI Opponent for PvAI Mode:", CENTER_X - 350, 250, 40, WHITE);
    
    // AI Selection Buttons
    Rectangle bob_rect = {CENTER_X - 400, 350, 220, 80};
    Rectangle thea_rect = {CENTER_X - 110, 350, 220, 80};
    Rectangle flint_rect = {CENTER_X + 180, 350, 220, 80};
    
    Vector2 mouse = GetMousePosition();
    bool bob_hover = CheckCollisionPointRec(mouse, bob_rect);
    bool thea_hover = CheckCollisionPointRec(mouse, thea_rect);
    bool flint_hover = CheckCollisionPointRec(mouse, flint_rect);
    
    // Highlight selected AI
    Color bob_color = (g->selected_opponent_ai == AI_BOB) ? LIME : (bob_hover ? SKYBLUE : BLUE);
    Color thea_color = (g->selected_opponent_ai == AI_THEA) ? LIME : (thea_hover ? SKYBLUE : BLUE);
    Color flint_color = (g->selected_opponent_ai == AI_FLINT) ? LIME : (flint_hover ? SKYBLUE : BLUE);
    
    DrawRectangleRec(bob_rect, bob_color);
    DrawRectangleRec(thea_rect, thea_color);
    DrawRectangleRec(flint_rect, flint_color);
    
    DrawText("BOB", bob_rect.x + 60, bob_rect.y + 25, 35, BLACK);
    DrawText("THEA", thea_rect.x + 50, thea_rect.y + 25, 35, BLACK);
    DrawText("FLINT", flint_rect.x + 45, flint_rect.y + 25, 35, BLACK);
    
    // AI Descriptions
    int desc_y = 480;
    DrawText("BOB: Prefers Jokers, then low cards", CENTER_X - 400, desc_y, 25, LIGHTGRAY);
    DrawText("THEA: Random discard strategy", CENTER_X - 400, desc_y + 40, 25, LIGHTGRAY);
    DrawText("FLINT: Never discards Jokers", CENTER_X - 400, desc_y + 80, 25, LIGHTGRAY);
    
    // Back Button
    Rectangle back_rect = {CENTER_X - 150, SCREEN_H - 180, 300, 80};
    bool back_hover = CheckCollisionPointRec(mouse, back_rect);
    DrawRectangleRec(back_rect, back_hover ? RED : MAROON);
    DrawText("BACK", back_rect.x + 100, back_rect.y + 25, 35, WHITE);
}

void UpdateSettings(GameState *g, Vector2 mouse) {
    Rectangle bob_rect = {CENTER_X - 400, 350, 220, 80};
    Rectangle thea_rect = {CENTER_X - 110, 350, 220, 80};
    Rectangle flint_rect = {CENTER_X + 180, 350, 220, 80};
    Rectangle back_rect = {CENTER_X - 150, SCREEN_H - 180, 300, 80};
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, bob_rect)) {
            g->selected_opponent_ai = AI_BOB;
        }
        else if (CheckCollisionPointRec(mouse, thea_rect)) {
            g->selected_opponent_ai = AI_THEA;
        }
        else if (CheckCollisionPointRec(mouse, flint_rect)) {
            g->selected_opponent_ai = AI_FLINT;
        }
        else if (CheckCollisionPointRec(mouse, back_rect)) {
            g->state = STATE_MAIN_MENU;
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  ACCOUNT MANAGER
// ─────────────────────────────────────────────────────────────────────────────
// In mainmenu.c

void DrawAccountsManager(const GameState *g) {
    DrawText("ACCOUNT MANAGER", CENTER_X - 320, 60, 70, GOLD);

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

        // --- BUTTONS ---
        float btn_w = 140;
        float btn_h = 40;
        float start_x = row_rect.x + 600;
        
        if (!a->is_ai) {
            // HUMAN CONTROLS: [P1 LOGIN/OUT] [P2 LOGIN/OUT] [DELETE]
            
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

            // Delete Button
            Rectangle btn_del = {row_rect.x + row_rect.width - 100, row_rect.y + 30, 80, btn_h};
            bool hover_del = CheckCollisionPointRec(GetMousePosition(), btn_del);
            DrawRectangleRec(btn_del, hover_del ? RED : MAROON);
            DrawText("DEL", btn_del.x + 20, btn_del.y + 10, 20, WHITE);

        } else {
            // AI CONTROLS: [ASSIGN P2] [LOGOUT P2]
            // We focus on P2 for AI since P1 is usually the human
            
            Rectangle btn_ai = {start_x + 160, row_rect.y + 30, btn_w + 40, btn_h};
            bool hover_ai = CheckCollisionPointRec(GetMousePosition(), btn_ai);
            
            if (is_p2) {
                DrawRectangleRec(btn_ai, hover_ai ? RED : MAROON);
                DrawText(TextFormat("LOG %s OUT", a->first_name), btn_ai.x + 10, btn_ai.y + 10, 20, WHITE);
            } else {
                DrawRectangleRec(btn_ai, hover_ai ? SKYBLUE : BLUE);
                DrawText(TextFormat("%s P2 SLOT", a->first_name), btn_ai.x + 10, btn_ai.y + 10, 20, WHITE);
            }
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

// In mainmenu.c

void UpdateAccountsManager(GameState *g, Vector2 mouse) {
    int y = 280;
    bool action_taken = false;

    for (int i = 0; i < g->account_count; i++) {
        Rectangle row_rect = {150, (float)y, SCREEN_W - 300, 100};
        float btn_w = 140;
        float btn_h = 40;
        float start_x = row_rect.x + 600;

        if (!g->accounts[i].is_ai) {
            // HUMAN BUTTONS
            Rectangle btn_p1 = {start_x, row_rect.y + 30, btn_w, btn_h};
            Rectangle btn_p2 = {start_x + 160, row_rect.y + 30, btn_w, btn_h};
            Rectangle btn_del = {row_rect.x + row_rect.width - 100, row_rect.y + 30, 80, btn_h};

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, btn_p1)) {
                    if (g->p1_account_index == i) {
                        LogoutAccount(g, 1);
                        ShowAccountStatus(g, "Logged P1 Out");
                    } else {
                        LoginAccount(g, i, 1);
                        ShowAccountStatus(g, "Logged P1 In");
                    }
                }
                else if (CheckCollisionPointRec(mouse, btn_p2)) {
                    if (g->p2_account_index == i) {
                        LogoutAccount(g, 2);
                        ShowAccountStatus(g, "Logged P2 Out");
                    } else {
                        LoginAccount(g, i, 2);
                        ShowAccountStatus(g, "Logged P2 In");
                    }
                }
                else if (CheckCollisionPointRec(mouse, btn_del)) {
                    // Prevent deleting logged in accounts
                    if (g->p1_account_index == i || g->p2_account_index == i) {
                        DeleteAccount(g, i);
                        action_taken = true; // Break loop since list changed
                    }
                }
            }
        } else {
            // AI BUTTONS (P2 Focus)
            Rectangle btn_ai = {start_x + 160, row_rect.y + 30, btn_w + 40, btn_h};
            
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, btn_ai)) {
                if (g->p2_account_index == i) {
                    LogoutAccount(g, 2);
                    ShowAccountStatus(g, TextFormat("Logged %s Out", g->accounts[i].first_name));
                } else {
                    LoginAccount(g, i, 2);
                    ShowAccountStatus(g, TextFormat("Assigned %s to P2", g->accounts[i].first_name));
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
            // Reset fields
            current_name_field = 1;
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

    DrawText(label1, CENTER_X - 300, 340, 40, col1);
    DrawText(label2, CENTER_X + 20, 340, 40, col2);

    Rectangle r1 = {CENTER_X - 300, 400, 280, 70};
    Rectangle r2 = {CENTER_X + 20, 400, 280, 70};

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

    Rectangle confirm = {CENTER_X - 220, 520, 200, 80};
    Rectangle cancel = {CENTER_X + 20, 520, 200, 80};

    bool ok = strlen(new_first_name) >= 2 && strlen(new_last_name) >= 2;
    DrawRectangleRec(confirm, ok ? LIME : DARKGREEN);
    DrawRectangleRec(cancel, RED);
    DrawText("CREATE", confirm.x + 40, confirm.y + 25, 35, BLACK);
    DrawText("CANCEL", cancel.x + 40, cancel.y + 25, 35, WHITE);
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
        Rectangle confirm = {CENTER_X - 220, 520, 200, 80};
        Rectangle cancel = {CENTER_X + 20, 520, 200, 80};

        if (CheckCollisionPointRec(m, confirm)) {
            if (strlen(new_first_name) >= 2 && strlen(new_last_name) >= 2) {
                if (CreateNewAccount(g, new_first_name, new_last_name))
                    g->state = STATE_ACCOUNTS_MANAGER;
                else
                    ShowAccountStatus(g, "Name already taken!");
            }
        }
        else if (CheckCollisionPointRec(m, cancel))
            g->state = STATE_ACCOUNTS_MANAGER;
    }
}