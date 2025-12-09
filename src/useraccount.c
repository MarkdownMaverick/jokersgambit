// useraccount.c

#include "useraccount.h"
#include <string.h>
#include <stdio.h>

#define ACCOUNTS_FILE "accounts.txt"
#define LEADERBOARD_FILE "leader.txt"

// Array of permanent AI names
static const char *AI_NAMES[] = {"BOB", "THEA", "FLINT"};
#define NUM_AI_ACCOUNTS (sizeof(AI_NAMES) / sizeof(AI_NAMES[0]))

// Utility function to check if a character is an alphabet letter
bool IsAlpha(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}


// ─────────────────────────────────────────────────────────────────────────────
//  INITIALIZATION AND ACCOUNT FILE I/O IMPLEMENTATION
// ─────────────────────────────────────────────────────────────────────────────

void InitAccounts(GameState *g) {
    g->account_count = 0;
    g->p1_account_index = -1;
    g->p2_account_index = -1;
    g->leaderboard_loaded = false;
    for (int i = 0; i < MAX_ACCOUNTS; i++) {
        memset(&g->accounts[i], 0, sizeof(Account));
        g->accounts[i].balance = 10.00; 
        g->accounts[i].is_logged_in = false;
    }
}

// src/useraccount.c — FIXED LoadAllAccounts (now parses W: L:)
void LoadAllAccounts(GameState *g)
{
    InitAccounts(g);  // Reset

    FILE *f = fopen(ACCOUNTS_FILE, "r");
    if (!f)
    {
        // Create default AI
        for (int i = 0; i < NUM_AI_ACCOUNTS; i++)
        {
            Account *a = &g->accounts[g->account_count++];
            strncpy(a->first_name, AI_NAMES[i], MAX_ACCOUNT_NAME_LEN);
            a->first_name[MAX_ACCOUNT_NAME_LEN] = '\0';
            a->last_name[0] = '\0';
            a->balance = 10.00;
            a->wins = 0;
            a->losses = 0;
            a->is_ai = true;
            a->is_active = true;
        }
        SaveAllAccounts(g);  // Save defaults
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), f) && g->account_count < MAX_ACCOUNTS)
    {
        char first[MAX_ACCOUNT_NAME_LEN + 1] = {0};
        char last[MAX_ACCOUNT_NAME_LEN + 1]  = {0};
        double balance = 0.0;
        int wins = 0, losses = 0;
        int is_ai = 0;

        // Human: "jimmy choose: $ -80.00 W:0 L:1"
        // AI: "BOB AI: $100.00 W:1 L:0"
        int fields = sscanf(line, "%12s %12[^:]: $%lf W:%d L:%d",
                            first, last, &balance, &wins, &losses);

        if (fields >= 4)  // At least name + balance + W + L
        {
            Account *a = &g->accounts[g->account_count];

            // First name always
            strncpy(a->first_name, first, MAX_ACCOUNT_NAME_LEN);
            a->first_name[MAX_ACCOUNT_NAME_LEN] = '\0';

            // Detect AI vs Human
            if (strcmp(last, "AI") == 0)
            {
                a->last_name[0] = '\0';  // AI has no last
                is_ai = 1;
            }
            else
            {
                // Human: last is the last name
                const char *src = last;
                if (src[0] == ' ') src++;  // Skip space if any
                strncpy(a->last_name, src, MAX_ACCOUNT_NAME_LEN);
                 a->last_name[MAX_ACCOUNT_NAME_LEN] = '\0';
            }

            a->balance = balance;
            a->wins = wins;
            a->losses = losses;
            a->is_ai = is_ai;
            a->is_active = true;

            g->account_count++;
        }
    }
    fclose(f);

    // Fallback if empty
    if (g->account_count == 0)
    {
        // Recreate AI
        for (int i = 0; i < NUM_AI_ACCOUNTS; i++)
        {
            Account *a = &g->accounts[g->account_count++];
            strncpy(a->first_name, AI_NAMES[i], MAX_ACCOUNT_NAME_LEN);
            a->first_name[MAX_ACCOUNT_NAME_LEN] = '\0';
            a->last_name[0] = '\0';
            a->balance = 10.00;
            a->wins = 0;
            a->losses = 0;
            a->is_ai = true;
            
            // Set AI type based on name
            if (strcmp(AI_NAMES[i], "BOB") == 0) a->ai_type = AI_BOB;
            else if (strcmp(AI_NAMES[i], "THEA") == 0) a->ai_type = AI_THEA;
            else if (strcmp(AI_NAMES[i], "FLINT") == 0) a->ai_type = AI_FLINT;
            
            a->is_active = true;
        }
        SaveAllAccounts(g);
    }
    
    // DEFAULT BOB LOGIN: Only on application start (p2_account_index will be -1 on first load)
    if (g->p2_account_index == -1)
    {
        for (int i = 0; i < g->account_count; i++)
        {
            if (g->accounts[i].is_ai && strcmp(g->accounts[i].first_name, "BOB") == 0)
            {
                LoginAccount(g, i, 2);
                TraceLog(LOG_INFO, "Default: BOB auto-logged into P2 slot");
                break;
            }
        }
    }
}

// src/useraccount.c — FIXED SaveAllAccounts (format as requested)
void SaveAllAccounts(const GameState *g)
{
    FILE *f = fopen(ACCOUNTS_FILE, "w");
    if (!f) return;

    for (int i = 0; i < g->account_count; i++)
    {
        const Account *a = &g->accounts[i];
        if (a->is_active)
        {
            if (a->is_ai)
            {
                fprintf(f, "%s AI: $%.2lf W:%d L:%d\n",
                        a->first_name, a->balance, a->wins, a->losses);
            }
            else
            {
                fprintf(f, "%s %s: $%.2lf W:%d L:%d\n",
                        a->first_name, a->last_name, a->balance, a->wins, a->losses);
            }
        }
    }
    fclose(f);
}

// ─────────────────────────────────────────────────────────────────────────────
//  LEADERBOARD FILE I/O IMPLEMENTATION (MISSING FUNCTIONS ADDED HERE)
// ─────────────────────────────────────────────────────────────────────────────

void LoadLeaderboard(GameState *g) {
    // FIX: Declare FILE pointer 'f' and open file
    FILE *f = fopen(LEADERBOARD_FILE, "r"); 
    if (!f) {
        TraceLog(LOG_WARNING, "LEADERBOARD: File not found. Skipping load.");
        g->leaderboard_count = 0;
        return;
    }

    // FIX: Declare temporary line buffer 'line'
    char line[256]; 
    g->leaderboard_count = 0;

    while (fgets(line, sizeof(line), f) && g->leaderboard_count < MAX_LEADERBOARD_ENTRIES) {
        LeaderboardEntry *e = &g->leaderboard[g->leaderboard_count];
        
        // FIX: Use the new member names: e->total_winnings and e->entry_name
        int scanned = sscanf(line, "Score: %f - Name: %127[^\n]", &e->total_winnings, e->entry_name); 

        if (scanned == 2) {
            // Null-terminate entry_name to be safe
            e->entry_name[MAX_LEADERBOARD_ENTRY_NAME_LEN - 1] = '\0';
            g->leaderboard_count++;
        }
    }

    fclose(f);
    g->leaderboard_loaded = true;
}

void SaveLeaderboard(const GameState *g) {
    // FIX: Declare FILE pointer 'f' and open file for writing
    FILE *f = fopen(LEADERBOARD_FILE, "w"); 
    if (!f) {
        TraceLog(LOG_ERROR, "LEADERBOARD: Failed to open for writing.");
        return;
    }

    for (int i = 0; i < g->leaderboard_count; i++) {
        const LeaderboardEntry *e = &g->leaderboard[i];
        // Save format: "Score: 12.34 - Name: P1 vs P2"
        // FIX: Use the new member names: e->total_winnings and e->entry_name
        fprintf(f, "Score: %.2f - Name: %s\n", e->total_winnings, e->entry_name);
    }

    fclose(f);
}

// ─────────────────────────────────────────────────────────────────────────────
//  ACCOUNT MANAGEMENT LOGIC
// ─────────────────────────────────────────────────────────────────────────────

bool IsNameValid(const char *name) {
    // Check length
    size_t len = strlen(name);
    if (len == 0 || len > MAX_ACCOUNT_NAME_LEN) return false;

    // Check characters
    for (size_t i = 0; i < len; i++) {
        if (!IsAlpha(name[i])) return false;
    }
    return true;
}

// Checks if an account with the same first and last name exists
bool DoesAccountExist(const GameState *g, const char *first, const char *last) {
    for (int i = 0; i < g->account_count; i++) {
        // AI accounts have an empty last name, so skip them
        if (g->accounts[i].last_name[0] == '\0') continue; 
        
        if (strcmp(g->accounts[i].first_name, first) == 0 &&
            strcmp(g->accounts[i].last_name, last) == 0) {
            return true;
        }
    }
    return false;
}

// src/useraccount.c — FIXED CreateNewAccount (now sets wins/losses = 0)
bool CreateNewAccount(GameState *g, const char *first, const char *last)
{
    if (g->account_count >= MAX_ACCOUNTS) return false;
    if (!IsNameValid(first) || !IsNameValid(last)) return false;
    if (DoesAccountExist(g, first, last)) return false;

    Account *a = &g->accounts[g->account_count++];
    strncpy(a->first_name, first, MAX_ACCOUNT_NAME_LEN);
    a->first_name[MAX_ACCOUNT_NAME_LEN] = '\0';
    strncpy(a->last_name, last, MAX_ACCOUNT_NAME_LEN);
    a->last_name[MAX_ACCOUNT_NAME_LEN] = '\0';
    a->balance = 10.00;
    a->wins = 0;
    a->losses = 0;
    a->is_ai = false;
    a->is_active = true;

    SaveAllAccounts(g);  // Save immediately
    return true;
}

// src/useraccount.c — FIXED DeleteAccount (saves after delete)
void DeleteAccount(GameState *g, int index)
{
    if (index < 0 || index >= g->account_count) return;

    // Shift array
    for (int i = index; i < g->account_count - 1; i++)
        g->accounts[i] = g->accounts[i + 1];

    g->account_count--;

    // Update indices
    if (g->p1_account_index == index) g->p1_account_index = -1;
    else if (g->p1_account_index > index) g->p1_account_index--;
    
    if (g->p2_account_index == index) g->p2_account_index = -1;
    else if (g->p2_account_index > index) g->p2_account_index--;

    SaveAllAccounts(g);  // Save immediately
}

void LogoutAccount(GameState *g, int player) {
    int *index_ptr = (player == 1) ? &g->p1_account_index : &g->p2_account_index;
    if (*index_ptr != -1) {
        g->accounts[*index_ptr].is_logged_in = false;
        *index_ptr = -1;
    }
}

void LoginAccount(GameState *g, int index, int player) {
    // Logout the previous player
    LogoutAccount(g, player);

    // Login new player
    int *index_ptr = (player == 1) ? &g->p1_account_index : &g->p2_account_index;
    if (index >= 0 && index < g->account_count) {
        // Prevent logging into the same account on both players
        int other_player_index = (player == 1) ? g->p2_account_index : g->p1_account_index;
        if (index != other_player_index) {
            g->accounts[index].is_logged_in = true;
            *index_ptr = index;
        }
    }
}

// src/useraccount.c — FIXED GetPlayerName (remove unused statics outside)
const char* GetPlayerName(const GameState *g, int player) {
    static char full_name_p1[MAX_ACCOUNT_NAME_LEN * 2 + 3];
    static char full_name_p2[MAX_ACCOUNT_NAME_LEN * 2 + 3];
    
    int index = (player == 1) ? g->p1_account_index : g->p2_account_index;
    
    if (index >= 0 && index < g->account_count && g->accounts[index].is_active) {
        char *full_name = (player == 1) ? full_name_p1 : full_name_p2;
        snprintf(full_name, sizeof(full_name_p1), "%s %s", 
                 g->accounts[index].first_name, 
                 g->accounts[index].last_name[0] ? g->accounts[index].last_name : "");
        return full_name;
    }
    
    return (player == 1) ? "P1: Log In" : "P2: Log In";
}

// ─────────────────────────────────────────────────────────────────────────────
//  BALANCE UPDATE LOGIC (Called from main.c on Game Over)
// ─────────────────────────────────────────────────────────────────────────────

//extern float CalculateFinalScore(float balance, int total_moves, bool is_winner);

// src/useraccount.c — FIXED UpdateAccountBalances (remove unused loser)
void UpdateAccountBalances(GameState *g)
{
    if (g->game_over && g->winner > 0)
    {
        // Update balances (your game logic already has final balances in g->p1_balance)
        if (g->p1_account_index != -1)
        {
            g->accounts[g->p1_account_index].balance = (double)g->p1_balance;
            if (g->winner == 1) g->accounts[g->p1_account_index].wins++;
            else g->accounts[g->p1_account_index].losses++;
        }
        
        if (g->p2_account_index != -1)
        {
            g->accounts[g->p2_account_index].balance = (double)g->p2_balance;
            if (g->winner == 2) g->accounts[g->p2_account_index].wins++;
            else g->accounts[g->p2_account_index].losses++;
        }
    }

    SaveAllAccounts(g);  // Save only here, on win/lose
}

