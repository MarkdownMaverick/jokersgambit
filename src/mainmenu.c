#include "mainmenu.h"
#include "useraccount.h" 
#include "gui.h"

// ─────────────────────────────────────────────────────────────────────────────
//  LOCAL STATE FOR ACCOUNT CREATION & ERROR HANDLING
// ─────────────────────────────────────────────────────────────────────────────
char new_first_name[MAX_ACCOUNT_NAME_LEN + 1] = {0};
char new_last_name[MAX_ACCOUNT_NAME_LEN + 1] = {0};
int current_name_field = 1; // 1: first, 2: last
double error_timer = 0.0;
char error_message[128] = {0};

void ShowAccountsError(const char *msg) {
    strncpy(error_message, msg, sizeof(error_message) - 1);
    error_timer = GetTime();
}

// ─────────────────────────────────────────────────────────────────────────────
//  MAIN MENU
// ─────────────────────────────────────────────────────────────────────────────

void DrawMainMenu(GameState *g) {
    Rectangle play_rect = {CENTER_X - 150, 300, 300, 60};
    Rectangle accounts_rect = {CENTER_X - 150, 380, 300, 60};
    Rectangle aivsai_rect = {CENTER_X - 150, 460, 300, 60};
    Rectangle leaderboard_rect = {CENTER_X - 150, 540, 300, 60};
    Vector2 mouse = GetMousePosition();

    DrawText("JOKERS GAMBIT", CENTER_X - 350, 150, 90, GOLD);

    // Play Button
    bool play_hover = CheckCollisionPointRec(mouse, play_rect);
    DrawRectangleRec(play_rect, play_hover ? LIME : GREEN);
    DrawText("PLAY", (int)play_rect.x + 110, (int)play_rect.y + 15, 30, BLACK);

    // Accounts Button
    bool acc_hover = CheckCollisionPointRec(mouse, accounts_rect);
    DrawRectangleRec(accounts_rect, acc_hover ? SKYBLUE : BLUE);
    DrawText("ACCOUNTS", (int)accounts_rect.x + 70, (int)accounts_rect.y + 15, 30, WHITE);

    // AI vs AI Button
    bool aivsai_hover = CheckCollisionPointRec(mouse, aivsai_rect);
    DrawRectangleRec(aivsai_rect, aivsai_hover ? ORANGE : ORANGE);
    DrawText("AI vs AI", (int)aivsai_rect.x + 80, (int)aivsai_rect.y + 15, 30, WHITE);

    // Leaderboard Button
    bool lb_hover = CheckCollisionPointRec(mouse, leaderboard_rect);
    DrawRectangleRec(leaderboard_rect, lb_hover ? YELLOW : GOLD);
    DrawText("LEADERBOARD", (int)leaderboard_rect.x + 30, (int)leaderboard_rect.y + 15, 30, BLACK);
}

void UpdateMainMenu(GameState *g, Vector2 mouse) {
    Rectangle play_rect = {CENTER_X - 150, 300, 300, 60};
    Rectangle accounts_rect = {CENTER_X - 150, 380, 300, 60};
    Rectangle aivsai_rect = {CENTER_X - 150, 460, 300, 60};
    Rectangle leaderboard_rect = {CENTER_X - 150, 540, 300, 60};

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, play_rect)) {
            // Play requires at least one account logged in
            if (g->p1_account_index == -1) {
                g->state = STATE_ACCOUNTS_MANAGER;
            } else {
                // If P1 is logged in, determine mode and start
                if (g->p2_account_index == -1) {
                    g->mode = MODE_PVAI;
                } else {
                    g->mode = MODE_PVP;
                }
                RestartGameKeepingAccounts(g); // Initialize game with logged-in balances/mode
                g->state = STATE_P1_SELECT_DISCARD;
            }
        }
        if (CheckCollisionPointRec(mouse, accounts_rect)) {
            g->state = STATE_ACCOUNTS_MANAGER;
        }
        if (CheckCollisionPointRec(mouse, aivsai_rect)) {
            // AI vs AI mode - no accounts needed
            g->mode = MODE_AIVSAI;
            g->p1_account_index = -1;
            g->p2_account_index = -1;
            RestartGameKeepingAccounts(g);
            g->state = STATE_P1_SELECT_DISCARD;
        }
        if (CheckCollisionPointRec(mouse, leaderboard_rect)) {
            g->state = STATE_LEADERBOARD;
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  ACCOUNT MANAGER
// ─────────────────────────────────────────────────────────────────────────────


// ─────────────────────────────────────────────────────────────────────────────
//  CLEAN & BEAUTIFUL ACCOUNT MANAGER
// ─────────────────────────────────────────────────────────────────────────────

void DrawAccountsManager(const GameState *g)
{
    DrawText("ACCOUNT MANAGER", CENTER_X - 320, 60, 70, GOLD);
    
    // Current players
    const char *p1 = (g->p1_account_index == -1) ? "Not logged in" : GetPlayerName(g, 1);
    const char *p2 = (g->p2_account_index == -1) ? "Not logged in" : GetPlayerName(g, 2);

    DrawText("PLAYER 1:", 150, 160, 40, LIME);
    DrawText(p1, 420, 160, 40, (g->p1_account_index == -1) ? YELLOW : GRAY);

    DrawText("PLAYER 2:", 150, 220, 40, ORANGE);
    DrawText(p2, 420, 220, 40, (g->p2_account_index == -1) ? GRAY : YELLOW);

    DrawText("Click an account to assign it → Player 1 → Player 2 → logout", 150, 290, 28, DARKGRAY);

    // Account list — SHOW human accounts ON TOP AND THE THREE AI BELOW THE 
    int y = 360;
    for (int i = 0; i < g->account_count; i++)
    {
        const Account *a = &g->accounts[i];

        bool is_p1 = (i == g->p1_account_index);
        bool is_p2 = (i == g->p2_account_index);
        Color bg     = is_p1 ? Fade(LIME, 0.3f) : is_p2 ? Fade(ORANGE, 0.3f) : Fade(DARKBLUE, 0.35f);
        Color border = is_p1 ? LIME : is_p2 ? ORANGE : DARKBLUE;

        Rectangle box = {150, (float)y, SCREEN_W - 300, 100};
        DrawRectangleRec(box, bg);
        DrawRectangleLinesEx(box, 5, border);

        char name[64];
        snprintf(name, sizeof(name), "%s %s", a->first_name, a->last_name[0] ? a->last_name : "");
        DrawText(name, box.x + 40, box.y + 15, 40, WHITE);
        DrawText(TextFormat("Balance: $%.2f", a->balance), box.x + 40, box.y + 60, 30, GREEN);

        if (is_p1) DrawText("PLAYER 1", box.x + 550, box.y + 20, 30, LIME);
        if (is_p2) DrawText("PLAYER 2", box.x + 550, box.y + 20, 30, ORANGE);
        // DELETE button
        Rectangle del = {box.x + box.width - 160, box.y + 25, 120, 50};
        bool hover_del = CheckCollisionPointRec(GetMousePosition(), del);
        DrawRectangleRec(del, hover_del ? RED : MAROON);
        DrawText("DELETE", del.x + 15, del.y + 12, 28, WHITE);

        y += 120;
    }

    // Bottom buttons
    Rectangle create = {CENTER_X - 300, SCREEN_H - 160, 280, 90};
    Rectangle back   = {CENTER_X + 20,   SCREEN_H - 160, 280, 90};

    bool hover_create = CheckCollisionPointRec(GetMousePosition(), create);
    bool hover_back   = CheckCollisionPointRec(GetMousePosition(), back);

    DrawRectangleRec(create, hover_create ? LIME : DARKGREEN);
    DrawRectangleRec(back,   hover_back   ? RED  : MAROON);
    DrawText("CREATE NEW", create.x + 30, create.y + 28, 38, BLACK);
    DrawText("BACK", back.x + 90, back.y + 28, 40, WHITE);
}
void UpdateAccountsManager(GameState *g, Vector2 mouse)
{
    (void)mouse;  // Ignore mouse — keyboard only now!

    // Keyboard selection (UP/DOWN to pick account)
    static int selected_idx = -1;  // Current highlighted account (-1 = none)
    int human_count = 0;
    for (int i = 0; i < g->account_count; i++) if (!g->accounts[i].is_ai) human_count++;

    if (IsKeyPressed(KEY_DOWN)) selected_idx = (selected_idx + 1) % human_count;
    if (IsKeyPressed(KEY_UP))   selected_idx = (selected_idx - 1 + human_count) % human_count;

    // Find the actual array index for selected (skip AI)
    int actual_idx = -1;
    int count = 0;
    for (int i = 0; i < g->account_count; i++)
    {
        if (!g->accounts[i].is_ai)
        {
            if (count == selected_idx)
            {
                actual_idx = i;
                break;
            }
            count++;
        }
    }

    // Keybinds: 1/2 for login/logout, 3/4 for delete
    if (actual_idx != -1)
    {
        if (IsKeyPressed(KEY_ONE))
        {
            if (g->p1_account_index == actual_idx) g->p1_account_index = -1;  // Logout
            else g->p1_account_index = actual_idx;  // Login
        }
        if (IsKeyPressed(KEY_TWO))
        {
            if (g->p2_account_index == actual_idx) g->p2_account_index = -1;  // Logout
            else g->p2_account_index = actual_idx;  // Login
        }
        if (IsKeyPressed(KEY_THREE) && g->p1_account_index != -1)
        {
            DeleteAccount(g, g->p1_account_index);
            g->p1_account_index = -1;
        }
        if (IsKeyPressed(KEY_FOUR) && g->p2_account_index != -1)
        {
            DeleteAccount(g, g->p2_account_index);
            g->p2_account_index = -1;
        }
    }

    // Bottom buttons (keyboard too: C for create, B/ESC for back)
    if (IsKeyPressed(KEY_C))
    {
        memset(new_first_name, 0, sizeof(new_first_name));
        memset(new_last_name, 0, sizeof(new_last_name));
        current_name_field = 1;
        g->state = STATE_ACCOUNT_CREATE;
    }
    if (IsKeyPressed(KEY_B) || IsKeyPressed(KEY_ESCAPE))
    {
        g->state = STATE_MAIN_MENU;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  ACCOUNT CREATION
// ─────────────────────────────────────────────────────────────────────────────

void DrawAccountCreate(const GameState *g)
{
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

    DrawRectangleRec(r1, DARKGRAY); DrawRectangleLinesEx(r1, 4, col1);
    DrawRectangleRec(r2, DARKGRAY); DrawRectangleLinesEx(r2, 4, col2);

    DrawText(new_first_name, r1.x + 20, r1.y + 20, 40, WHITE);
    DrawText(new_last_name,  r2.x + 20, r2.y + 20, 40, WHITE);

    // Cursor blink
    if (current_name_field == 1 && ((int)(GetTime()*2)%2))
        DrawText("_", r1.x + 25 + MeasureText(new_first_name,40), r1.y + 20, 40, WHITE);
    if (current_name_field == 2 && ((int)(GetTime()*2)%2))
        DrawText("_", r2.x + 25 + MeasureText(new_last_name,40), r2.y + 20, 40, WHITE);

    // Buttons
    Rectangle confirm = {CENTER_X - 220, 520, 200, 80};
    Rectangle cancel  = {CENTER_X + 20, 520, 200, 80};

    bool ok = strlen(new_first_name)>=2 && strlen(new_last_name)>=2;
    DrawRectangleRec(confirm, ok ? LIME : DARKGREEN);
    DrawRectangleRec(cancel, RED);
    DrawText("CREATE", confirm.x + 40, confirm.y + 25, 35, BLACK);
    DrawText("CANCEL", cancel.x + 40, cancel.y + 25, 35, WHITE);
}

void UpdateAccountCreate(GameState *g)
{
    int key = GetCharPressed();
    char *field = (current_name_field == 1) ? new_first_name : new_last_name;

    while (key)
    {
        if ((key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z'))  // <-- fixed parentheses
        {
            int len = strlen(field);
            if (len < MAX_ACCOUNT_NAME_LEN)
            {
                field[len] = (char)key;
                field[len+1] = '\0';
            }
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && strlen(field) > 0)
        field[strlen(field)-1] = '\0';

    if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_ENTER))
        current_name_field = (current_name_field == 1) ? 2 : 1;

    Vector2 m = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Rectangle confirm = {CENTER_X - 220, 520, 200, 80};
        Rectangle cancel  = {CENTER_X + 20, 520, 200, 80};

        if (CheckCollisionPointRec(m, confirm))
        {
            if (strlen(new_first_name) >= 2 && strlen(new_last_name) >= 2)
            {
                if (CreateNewAccount(g, new_first_name, new_last_name))
                    g->state = STATE_ACCOUNTS_MANAGER;
                else
                    ShowAccountsError("Name already taken!");
            }
        }
        else if (CheckCollisionPointRec(m, cancel))
            g->state = STATE_ACCOUNTS_MANAGER;
    }
}