#ifndef GUI_H
#define GUI_H

#include "main.h"

// Layout Helpers
Rectangle HandRect(int player, int idx);
Rectangle ButtonRect(int player, int idx);
Rectangle SlotRect(int player, int key_idx, int slot_idx);
Rectangle KeyCardRect(int key_idx);
Rectangle DiscardPileRect(int player);
Rectangle ContinueButtonRect(int player);

void DrawGameLayout(const GameState *g);
void DrawPlayerUI(const GameState *g, int player);
void DrawLeaderboard(const GameState *g);
void DrawGameOver(GameState *g);
void DrawCard(Card c, Rectangle rect, Color tint);
void DrawButton(Rectangle rect, bool is_hovered, bool is_enabled, const char *text);

#endif