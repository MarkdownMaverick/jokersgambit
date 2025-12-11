// useraccount.c

#include "useraccount.h"
#include <string.h>
#include <stdio.h>
#include <cjson/cJSON.h>
#define ACCOUNTS_FILE "accounts.json"
#define LEADERBOARD_FILE "leaderboard.json"

#define NUM_AI_ACCOUNTS (sizeof(AI_NAMES) / sizeof(AI_NAMES[0]))

bool IsAlpha(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

void InitAccounts(GameState *g) {
    g->account_count = 0;
    g->p1_account_index = -1;
    g->p2_account_index = -1;
    g->leaderboard_loaded = false;
    g->leaderboard_sort_by_moves = false;
    g->cover_p2_cards = false;
    for (int i = 0; i < MAX_ACCOUNTS; i++) {
        memset(&g->accounts[i], 0, sizeof(Account));
        g->accounts[i].balance = 10.00; 
        g->accounts[i].is_logged_in = false;
    }
}

void LoadAllAccounts(GameState *g)
{
    InitAccounts(g);

    FILE *f = fopen(ACCOUNTS_FILE, "r");
    if (!f)
    {
        return;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = (char *)malloc(size + 1);
    if (!buffer) { fclose(f); return; }

    size_t read_bytes = fread(buffer, 1, size, f);
    buffer[read_bytes] = '\0';
    fclose(f);

    cJSON *root = cJSON_Parse(buffer);
    free(buffer);
    if (!root)
    {
        return;
    }

    cJSON *arr = cJSON_GetObjectItem(root, "accounts");
    if (!cJSON_IsArray(arr))
    {
        cJSON_Delete(root);
        return;
    }

    g->account_count = 0;

    cJSON *elem = NULL;
    cJSON_ArrayForEach(elem, arr)
    {
        if (g->account_count >= MAX_ACCOUNTS) break;

        cJSON *jn = cJSON_GetObjectItem(elem, "first_name");
        cJSON *jl = cJSON_GetObjectItem(elem, "last_name");
        cJSON *jb = cJSON_GetObjectItem(elem, "balance");
        cJSON *jw = cJSON_GetObjectItem(elem, "wins");
        cJSON *jlost = cJSON_GetObjectItem(elem, "losses");
        cJSON *jai = cJSON_GetObjectItem(elem, "is_ai");

        if (!cJSON_IsString(jn) || !cJSON_IsString(jl) ||
            !cJSON_IsNumber(jb) || !cJSON_IsNumber(jw) ||
            !cJSON_IsNumber(jlost) || !cJSON_IsBool(jai))
        {
            continue;
        }

        Account *a = &g->accounts[g->account_count];

        strncpy(a->first_name, jn->valuestring, MAX_ACCOUNT_NAME_LEN);
        a->first_name[MAX_ACCOUNT_NAME_LEN] = '\0';
        strncpy(a->last_name, jl->valuestring, MAX_ACCOUNT_NAME_LEN);
        a->last_name[MAX_ACCOUNT_NAME_LEN] = '\0';

        a->balance = jb->valuedouble;
        a->wins    = jw->valueint;
        a->losses  = jlost->valueint;
        a->is_ai   = cJSON_IsTrue(jai);
        a->is_active = true;

        if (a->is_ai)
        {
            if (strcmp(a->first_name, "BOB") == 0)   a->ai_type = AI_BOB;
            else if (strcmp(a->first_name, "THEA") == 0)  a->ai_type = AI_THEA;
            else if (strcmp(a->first_name, "FLINT") == 0) a->ai_type = AI_FLINT;
        }

        g->account_count++;
    }

    cJSON_Delete(root);
}

void SaveAllAccounts(const GameState *g)
{
    cJSON *root = cJSON_CreateObject();
    cJSON *arr  = cJSON_CreateArray();
    cJSON_AddItemToObject(root, "accounts", arr);

    for (int i = 0; i < g->account_count; i++)
    {
        const Account *a = &g->accounts[i];
        if (!a->is_active) continue;

        cJSON *obj = cJSON_CreateObject();
        cJSON_AddStringToObject(obj, "first_name", a->first_name);
        cJSON_AddStringToObject(obj, "last_name",  a->last_name);
        cJSON_AddNumberToObject(obj, "balance",    a->balance);
        cJSON_AddNumberToObject(obj, "wins",       a->wins);
        cJSON_AddNumberToObject(obj, "losses",     a->losses);
        cJSON_AddBoolToObject(obj,   "is_ai",      a->is_ai ? 1 : 0);

        cJSON_AddItemToArray(arr, obj);
    }

    char *json_str = cJSON_Print(root);

    FILE *f = fopen(ACCOUNTS_FILE, "w");
    if (f)
    {
        fputs(json_str, f);
        fclose(f);
    }

    cJSON_free(json_str);
    cJSON_Delete(root);
}

void LoadLeaderboard(GameState *g) {
    FILE *f = fopen(LEADERBOARD_FILE, "r");
    if (!f) {
        TraceLog(LOG_WARNING, "LEADERBOARD: File not found. Skipping load.");
        g->leaderboard_count = 0;
        g->leaderboard_loaded = false;
        return;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = (char *)malloc(size + 1);
    if (!buffer) { fclose(f); return; }
    size_t read_bytes = fread(buffer, 1, size, f);
    buffer[read_bytes] = '\0';
    fclose(f);

    cJSON *root = cJSON_Parse(buffer);
    free(buffer);
    if (!root) {
        TraceLog(LOG_WARNING, "LEADERBOARD: JSON parse failed.");
        g->leaderboard_count = 0;
        g->leaderboard_loaded = false;
        return;
    }

    cJSON *arr = cJSON_GetObjectItem(root, "leaderboard");
    if (!cJSON_IsArray(arr)) {
        cJSON_Delete(root);
        g->leaderboard_count = 0;
        g->leaderboard_loaded = false;
        return;
    }

    g->leaderboard_count = 0;

    cJSON *elem = NULL;
    cJSON_ArrayForEach(elem, arr) {
        if (g->leaderboard_count >= MAX_LEADERBOARD_ENTRIES) break;

        cJSON *js = cJSON_GetObjectItem(elem, "score");
        cJSON *jn = cJSON_GetObjectItem(elem, "name");
        cJSON *jw = cJSON_GetObjectItem(elem, "winner");
        cJSON *jt = cJSON_GetObjectItem(elem, "timestamp");
        cJSON *jm = cJSON_GetObjectItem(elem, "moves");
        
        if (!cJSON_IsNumber(js) || !cJSON_IsString(jn)) continue;

        LeaderboardEntry *e = &g->leaderboard[g->leaderboard_count];
        e->total_winnings = (float)js->valuedouble;

        strncpy(e->entry_name, jn->valuestring, MAX_LEADERBOARD_ENTRY_NAME_LEN - 1);
        e->entry_name[MAX_LEADERBOARD_ENTRY_NAME_LEN - 1] = '\0';
        
        if (cJSON_IsString(jw)) {
            strncpy(e->winner_name, jw->valuestring, MAX_LEADERBOARD_WINNER_NAME_LEN - 1);
            e->winner_name[MAX_LEADERBOARD_WINNER_NAME_LEN - 1] = '\0';
        }
        
        if (cJSON_IsString(jt)) {
            strncpy(e->timestamp, jt->valuestring, MAX_LEADERBOARD_TIMESTAMP_LEN - 1);
            e->timestamp[MAX_LEADERBOARD_TIMESTAMP_LEN - 1] = '\0';
        }
        
        if (cJSON_IsNumber(jm)) {
            e->total_moves = jm->valueint;
        }

        g->leaderboard_count++;
    }

    cJSON_Delete(root);
    g->leaderboard_loaded = true;
}

void SaveLeaderboard(const GameState *g) {
    cJSON *root = cJSON_CreateObject();
    cJSON *arr  = cJSON_CreateArray();
    cJSON_AddItemToObject(root, "leaderboard", arr);

    for (int i = 0; i < g->leaderboard_count; i++) {
        const LeaderboardEntry *e = &g->leaderboard[i];

        cJSON *obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(obj, "score", e->total_winnings);
        cJSON_AddStringToObject(obj, "name",  e->entry_name);
        cJSON_AddStringToObject(obj, "winner", e->winner_name);
        cJSON_AddStringToObject(obj, "timestamp", e->timestamp);
        cJSON_AddNumberToObject(obj, "moves", e->total_moves);

        cJSON_AddItemToArray(arr, obj);
    }

    char *json_str = cJSON_Print(root);

    FILE *f = fopen(LEADERBOARD_FILE, "w");
    if (f) {
        fputs(json_str, f);
        fclose(f);
    }

    cJSON_free(json_str);
    cJSON_Delete(root);
}

bool IsNameValid(const char *name) {
    size_t len = strlen(name);
    if (len == 0 || len > MAX_ACCOUNT_NAME_LEN) return false;

    for (size_t i = 0; i < len; i++) {
        if (!IsAlpha(name[i])) return false;
    }
    return true;
}

bool DoesAccountExist(const GameState *g, const char *first, const char *last) {
    for (int i = 0; i < g->account_count; i++) {
        if (g->accounts[i].last_name[0] == '\0') continue; 
        
        if (strcmp(g->accounts[i].first_name, first) == 0 &&
            strcmp(g->accounts[i].last_name, last) == 0) {
            return true;
        }
    }
    return false;
}

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

    SaveAllAccounts(g);
    return true;
}

void DeleteAccount(GameState *g, int index)
{
    if (index < 0 || index >= g->account_count) return;

    for (int i = index; i < g->account_count - 1; i++)
        g->accounts[i] = g->accounts[i + 1];

    g->account_count--;

    if (g->p1_account_index == index) g->p1_account_index = -1;
    else if (g->p1_account_index > index) g->p1_account_index--;
    
    if (g->p2_account_index == index) g->p2_account_index = -1;
    else if (g->p2_account_index > index) g->p2_account_index--;

    SaveAllAccounts(g);
}

void LogoutAccount(GameState *g, int player) {
    int *index_ptr = (player == 1) ? &g->p1_account_index : &g->p2_account_index;
    if (*index_ptr != -1) {
        g->accounts[*index_ptr].is_logged_in = false;
        *index_ptr = -1;
    }
}

void LoginAccount(GameState *g, int index, int player) {
    LogoutAccount(g, player);

    int *index_ptr = (player == 1) ? &g->p1_account_index : &g->p2_account_index;
    if (index >= 0 && index < g->account_count) {
        int other_player_index = (player == 1) ? g->p2_account_index : g->p1_account_index;
        if (index != other_player_index) {
            g->accounts[index].is_logged_in = true;
            *index_ptr = index;
        }
    }
}

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

void UpdateAccountBalances(GameState *g)
{
    if (g->game_over && g->winner > 0)
    {
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

    SaveAllAccounts(g);
}