#ifndef USERACCOUNT_H
#define USERACCOUNT_H

#include "main.h"

// I/O Functions
void InitAccounts(GameState *g);
void LoadAllAccounts(GameState *g);
void SaveAllAccounts(const GameState *g);
void LoadLeaderboard(GameState *g);
void SaveLeaderboard(const GameState *g);

// Account Management Logic
bool IsNameValid(const char *name);
bool CreateNewAccount(GameState *g, const char *first, const char *last);
void DeleteAccount(GameState *g, int index);
void LogoutAccount(GameState *g, int player);
void LoginAccount(GameState *g, int index, int player);
const char* GetPlayerName(const GameState *g, int player);
void UpdateAccountBalances(GameState *g);
bool IsAlpha(int c);
// src/useraccount.h — ADD wins/losses if not already there (from your code, it is)
typedef struct Account {
    char first_name[MAX_ACCOUNT_NAME_LEN + 1];
    char last_name[MAX_ACCOUNT_NAME_LEN + 1];
    double balance;
    int wins;     // <--- ADD if missing
    int losses;   // <--- ADD if missing
    bool is_ai;
    bool is_active;
    bool is_logged_in;
} Account;
#endif