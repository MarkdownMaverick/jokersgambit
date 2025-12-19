#include "useraccount.h"
#include <cjson/cJSON.h>

bool IsAlpha(int c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool IsNameValid(const char *name)
{
    if (name == NULL || strlen(name) == 0) return false;
    if (strlen(name) > MAX_ACCOUNT_NAME_LEN) return false;
    
    for (size_t i = 0; i < strlen(name); i++) {
        if (!IsAlpha(name[i])) return false;
    }
    return true;
}

void InitAccounts(GameState *g)
{
    g->account_count = 0;
    
    // Create default AI accounts
    strcpy(g->accounts[0].first_name, "BOB");
    strcpy(g->accounts[0].last_name, "");
    g->accounts[0].balance = 1000.0;
    g->accounts[0].wins = 0;
    g->accounts[0].losses = 0;
    g->accounts[0].is_ai = true;
    g->accounts[0].ai_type = AI_BOB;
    g->accounts[0].is_active = true;
    g->accounts[0].is_logged_in = false;
    
    strcpy(g->accounts[1].first_name, "THEA");
    strcpy(g->accounts[1].last_name, "");
    g->accounts[1].balance = 1000.0;
    g->accounts[1].wins = 0;
    g->accounts[1].losses = 0;
    g->accounts[1].is_ai = true;
    g->accounts[1].ai_type = AI_THEA;
    g->accounts[1].is_active = true;
    g->accounts[1].is_logged_in = false;
    
    strcpy(g->accounts[2].first_name, "FLINT");
    strcpy(g->accounts[2].last_name, "");
    g->accounts[2].balance = 1000.0;
    g->accounts[2].wins = 0;
    g->accounts[2].losses = 0;
    g->accounts[2].is_ai = true;
    g->accounts[2].ai_type = AI_FLINT;
    g->accounts[2].is_active = true;
    g->accounts[2].is_logged_in = false;
    
    g->account_count = 3;
    g->p1_account_index = -1;
    g->p2_account_index = -1;
}

void LoadAllAccounts(GameState *g)
{
    FILE *fp = fopen(ACCOUNTS_FILE, "r");
    if (!fp) {
        printf("No accounts file found, using defaults\n");
        return;
    }
    
    // Get file size
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    // Allocate buffer
    char *buffer = (char*)malloc((size_t)fsize + 1);
    if (!buffer) {
        printf("ERROR: Failed to allocate memory for accounts file\n");
        fclose(fp);
        return;
    }
    
    // Read file
    size_t read_size = fread(buffer, 1, (size_t)fsize, fp);
    buffer[read_size] = '\0';
    fclose(fp);
    fp = NULL; // Set to NULL after closing
    
    // Parse JSON
    cJSON *root = cJSON_Parse(buffer);
    if (!root) {
        printf("ERROR: Failed to parse accounts JSON\n");
        free(buffer);
        return;
    }
    
    cJSON *accounts_array = cJSON_GetObjectItem(root, "accounts");
    if (!cJSON_IsArray(accounts_array)) {
        printf("ERROR: accounts is not an array\n");
        cJSON_Delete(root);
        free(buffer);
        return;
    }
    
    int count = 0;
    cJSON *account = NULL;
    cJSON_ArrayForEach(account, accounts_array) {
        if (count >= MAX_ACCOUNTS) break;
        
        cJSON *first = cJSON_GetObjectItem(account, "first_name");
        cJSON *last = cJSON_GetObjectItem(account, "last_name");
        cJSON *bal = cJSON_GetObjectItem(account, "balance");
        cJSON *wins = cJSON_GetObjectItem(account, "wins");
        cJSON *losses = cJSON_GetObjectItem(account, "losses");
        cJSON *is_ai = cJSON_GetObjectItem(account, "is_ai");
        cJSON *ai_type = cJSON_GetObjectItem(account, "ai_type");
        cJSON *is_active = cJSON_GetObjectItem(account, "is_active");
        
        if (cJSON_IsString(first)) {
            strncpy(g->accounts[count].first_name, first->valuestring, MAX_ACCOUNT_NAME_LEN);
            g->accounts[count].first_name[MAX_ACCOUNT_NAME_LEN] = '\0';
        }
        if (cJSON_IsString(last)) {
            strncpy(g->accounts[count].last_name, last->valuestring, MAX_ACCOUNT_NAME_LEN);
            g->accounts[count].last_name[MAX_ACCOUNT_NAME_LEN] = '\0';
        }
        if (cJSON_IsNumber(bal)) {
            g->accounts[count].balance = bal->valuedouble;
        }
        if (cJSON_IsNumber(wins)) {
            g->accounts[count].wins = wins->valueint;
        }
        if (cJSON_IsNumber(losses)) {
            g->accounts[count].losses = losses->valueint;
        }
        if (cJSON_IsBool(is_ai)) {
            g->accounts[count].is_ai = cJSON_IsTrue(is_ai);
        }
        if (cJSON_IsNumber(ai_type)) {
            g->accounts[count].ai_type = (AIType)ai_type->valueint;
        }
        if (cJSON_IsBool(is_active)) {
            g->accounts[count].is_active = cJSON_IsTrue(is_active);
        }
        
        g->accounts[count].is_logged_in = false;
        count++;
    }
    
    g->account_count = count;
    
    // Clean up
    cJSON_Delete(root);
    free(buffer);
    
    printf("Loaded %d accounts\n", count);
}

void SaveAllAccounts(const GameState *g)
{
    cJSON *root = cJSON_CreateObject();
    if (!root) {
        printf("ERROR: Failed to create JSON root\n");
        return;
    }
    
    cJSON *accounts_array = cJSON_CreateArray();
    if (!accounts_array) {
        printf("ERROR: Failed to create accounts array\n");
        cJSON_Delete(root);
        return;
    }
    
    cJSON_AddItemToObject(root, "accounts", accounts_array);
    
    for (int i = 0; i < g->account_count; i++) {
        cJSON *acc = cJSON_CreateObject();
        if (!acc) continue;
        
        cJSON_AddStringToObject(acc, "first_name", g->accounts[i].first_name);
        cJSON_AddStringToObject(acc, "last_name", g->accounts[i].last_name);
        cJSON_AddNumberToObject(acc, "balance", g->accounts[i].balance);
        cJSON_AddNumberToObject(acc, "wins", g->accounts[i].wins);
        cJSON_AddNumberToObject(acc, "losses", g->accounts[i].losses);
        cJSON_AddBoolToObject(acc, "is_ai", g->accounts[i].is_ai);
        cJSON_AddNumberToObject(acc, "ai_type", (int)g->accounts[i].ai_type);
        cJSON_AddBoolToObject(acc, "is_active", g->accounts[i].is_active);
        
        cJSON_AddItemToArray(accounts_array, acc);
    }
    
    char *json_string = cJSON_Print(root);
    if (!json_string) {
        printf("ERROR: Failed to print JSON\n");
        cJSON_Delete(root);
        return;
    }
    
    FILE *fp = fopen(ACCOUNTS_FILE, "w");
    if (!fp) {
        printf("ERROR: Failed to open accounts file for writing\n");
        free(json_string);
        cJSON_Delete(root);
        return;
    }
    
    fprintf(fp, "%s", json_string);
    fclose(fp);
    
    free(json_string);
    cJSON_Delete(root);
    
    printf("Saved %d accounts\n", g->account_count);
}

void LoadLeaderboard(GameState *g)
{
    FILE *fp = fopen(LEADERBOARD_FILE, "r");
    if (!fp) {
        printf("No leaderboard file found\n");
        g->leaderboard_count = 0;
        g->leaderboard_loaded = true;
        return;
    }
    
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *buffer = (char*)malloc((size_t)fsize + 1);
    if (!buffer) {
        printf("ERROR: Failed to allocate memory for leaderboard\n");
        fclose(fp);
        return;
    }
    
    size_t read_size = fread(buffer, 1, (size_t)fsize, fp);
    buffer[read_size] = '\0';
    fclose(fp);
    fp = NULL;
    
    cJSON *root = cJSON_Parse(buffer);
    if (!root) {
        printf("ERROR: Failed to parse leaderboard JSON\n");
        free(buffer);
        return;
    }
    
    cJSON *entries = cJSON_GetObjectItem(root, "entries");
    if (!cJSON_IsArray(entries)) {
        printf("ERROR: entries is not an array\n");
        cJSON_Delete(root);
        free(buffer);
        return;
    }
    
    int count = 0;
    cJSON *entry = NULL;
    cJSON_ArrayForEach(entry, entries) {
        if (count >= MAX_LEADERBOARD_ENTRIES) break;
        
        cJSON *winnings = cJSON_GetObjectItem(entry, "total_winnings");
        cJSON *balance = cJSON_GetObjectItem(entry, "final_balance");
        cJSON *bonus = cJSON_GetObjectItem(entry, "bonus");
        cJSON *rounds = cJSON_GetObjectItem(entry, "total_rounds");
        cJSON *entry_name = cJSON_GetObjectItem(entry, "entry_name");
        cJSON *winner = cJSON_GetObjectItem(entry, "winner_name");
        cJSON *timestamp = cJSON_GetObjectItem(entry, "timestamp");
        
        if (cJSON_IsNumber(winnings)) {
            g->leaderboard[count].total_winnings = (float)winnings->valuedouble;
        }
        if (cJSON_IsNumber(balance)) {
            g->leaderboard[count].final_balance = (float)balance->valuedouble;
        }
        if (cJSON_IsNumber(bonus)) {
            g->leaderboard[count].bonus = (float)bonus->valuedouble;
        }
        if (cJSON_IsNumber(rounds)) {
            g->leaderboard[count].total_rounds = rounds->valueint;
        }
        if (cJSON_IsString(entry_name)) {
            strncpy(g->leaderboard[count].entry_name, entry_name->valuestring, 
                    MAX_LEADERBOARD_ENTRY_NAME_LEN - 1);
            g->leaderboard[count].entry_name[MAX_LEADERBOARD_ENTRY_NAME_LEN - 1] = '\0';
        }
        if (cJSON_IsString(winner)) {
            strncpy(g->leaderboard[count].winner_name, winner->valuestring, 
                    MAX_LEADERBOARD_WINNER_NAME_LEN - 1);
            g->leaderboard[count].winner_name[MAX_LEADERBOARD_WINNER_NAME_LEN - 1] = '\0';
        }
        if (cJSON_IsString(timestamp)) {
            strncpy(g->leaderboard[count].timestamp, timestamp->valuestring, 
                    MAX_LEADERBOARD_TIMESTAMP_LEN - 1);
            g->leaderboard[count].timestamp[MAX_LEADERBOARD_TIMESTAMP_LEN - 1] = '\0';
        }
        
        count++;
    }
    
    g->leaderboard_count = count;
    g->leaderboard_loaded = true;
    
    cJSON_Delete(root);
    free(buffer);
    
    printf("Loaded %d leaderboard entries\n", count);
}

void SaveLeaderboard(const GameState *g)
{
    cJSON *root = cJSON_CreateObject();
    if (!root) {
        printf("ERROR: Failed to create leaderboard JSON root\n");
        return;
    }
    
    cJSON *entries = cJSON_CreateArray();
    if (!entries) {
        printf("ERROR: Failed to create entries array\n");
        cJSON_Delete(root);
        return;
    }
    
    cJSON_AddItemToObject(root, "entries", entries);
    
    for (int i = 0; i < g->leaderboard_count; i++) {
        cJSON *entry = cJSON_CreateObject();
        if (!entry) continue;
        
        cJSON_AddNumberToObject(entry, "total_winnings", g->leaderboard[i].total_winnings);
        cJSON_AddNumberToObject(entry, "final_balance", g->leaderboard[i].final_balance);
        cJSON_AddNumberToObject(entry, "bonus", g->leaderboard[i].bonus);
        cJSON_AddNumberToObject(entry, "total_rounds", g->leaderboard[i].total_rounds);
        cJSON_AddStringToObject(entry, "entry_name", g->leaderboard[i].entry_name);
        cJSON_AddStringToObject(entry, "winner_name", g->leaderboard[i].winner_name);
        cJSON_AddStringToObject(entry, "timestamp", g->leaderboard[i].timestamp);
        
        cJSON_AddItemToArray(entries, entry);
    }
    
    char *json_string = cJSON_Print(root);
    if (!json_string) {
        printf("ERROR: Failed to print JSON\n");
        cJSON_Delete(root);
        return;
    }
    
    FILE *fp = fopen(LEADERBOARD_FILE, "w");
    if (!fp) {
        printf("ERROR: Failed to open leaderboard file for writing\n");
        free(json_string);
        cJSON_Delete(root);
        return;
    }
    
    fprintf(fp, "%s", json_string);
    fclose(fp);
    
    free(json_string);
    cJSON_Delete(root);
    
    printf("Saved %d leaderboard entries\n", g->leaderboard_count);
}

void RenamePlayerAccount(GameState *g, int index)
{
    if (index < 0 || index >= g->account_count) return;
    if (g->accounts[index].is_ai) return; // Cannot rename AI accounts
    
    // Shift accounts down // This function is renamed and will be used later , players can no longer delete the default accounts.
    for (int i = index; i < g->account_count - 1; i++) {
        g->accounts[i] = g->accounts[i + 1];
    }
    g->account_count--;
    
    // Update player indices if needed
    if (g->p1_account_index == index) g->p1_account_index = -1;
    else if (g->p1_account_index > index) g->p1_account_index--;
    
    if (g->p2_account_index == index) g->p2_account_index = -1;
    else if (g->p2_account_index > index) g->p2_account_index--;
    
    SaveAllAccounts(g);
}

void LogoutAccount(GameState *g, int player)
{
    if (player == 1) {
        if (g->p1_account_index >= 0) {
            g->accounts[g->p1_account_index].is_logged_in = false;
        }
        g->p1_account_index = -1;
    } else if (player == 2) {
        if (g->p2_account_index >= 0) {
            g->accounts[g->p2_account_index].is_logged_in = false;
        }
        g->p2_account_index = -1;
    }
}

void LoginAccount(GameState *g, int index, int player)
{
    if (index < 0 || index >= g->account_count) return;
    
    // Cannot play against yourself
    if (player == 1 && g->p2_account_index == index) return;
    if (player == 2 && g->p1_account_index == index) return;
    
    if (player == 1) {
        LogoutAccount(g, 1);
        g->p1_account_index = index;
        g->accounts[index].is_logged_in = true;
    } else if (player == 2) {
        LogoutAccount(g, 2);
        g->p2_account_index = index;
        g->accounts[index].is_logged_in = true;
    }
}

const char* GetPlayerName(const GameState *g, int player)
{
    static char name_buffer[MAX_ACCOUNT_NAME_LEN * 2 + 2];
    
    int idx = (player == 1) ? g->p1_account_index : g->p2_account_index;
    if (idx < 0 || idx >= g->account_count) {
        return "Unknown";
    }
    
    if (strlen(g->accounts[idx].last_name) > 0) {
        snprintf(name_buffer, sizeof(name_buffer), "%s %s", 
                 g->accounts[idx].first_name, g->accounts[idx].last_name);
    } else {
        snprintf(name_buffer, sizeof(name_buffer), "%s", g->accounts[idx].first_name);
    }
    
    return name_buffer;
}

void UpdateAccountBalances(GameState *g)
{
    if (g->p1_account_index >= 0 && g->p1_account_index < g->account_count) {
        g->accounts[g->p1_account_index].balance = g->p1_balance;
    }
    if (g->p2_account_index >= 0 && g->p2_account_index < g->account_count) {
        g->accounts[g->p2_account_index].balance = g->p2_balance;
    }
    SaveAllAccounts(g);
}