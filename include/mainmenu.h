#ifndef MAINMENU_H
#define MAINMENU_H

#include "main.h"

// Main Menu
void DrawMainMenu(GameState *g);
void UpdateMainMenu(GameState *g, Vector2 mouse);

// Settings
void DrawSettings(GameState *g);
void UpdateSettings(GameState *g, Vector2 mouse);

// Accounts Manager
void DrawAccountsManager(const GameState *g);
void UpdateAccountsManager(GameState *g, Vector2 mouse);
void ShowAccountStatus(GameState *g, const char *msg);

// Account Creation
void DrawAccountCreate(const GameState *g);

#endif