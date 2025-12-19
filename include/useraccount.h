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
void RenamePlayerAccount(GameState *g, int index);
void LogoutAccount(GameState *g, int player);
void LoginAccount(GameState *g, int index, int player);
const char* GetPlayerName(const GameState *g, int player);
void UpdateAccountBalances(GameState *g);
bool IsAlpha(int c);

#endif