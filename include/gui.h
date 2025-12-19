#ifndef GUI_H
#define GUI_H
#include "main.h"
// #include "raylib.h"
// Atlas layout constants
#define ATLAS_COLS 13
#define ATLAS_ROWS 5
#define ATLAS_CARD_WIDTH 200
#define ATLAS_CARD_HEIGHT 300
#define ATLAS_TOTAL_WIDTH 2600
#define ATLAS_TOTAL_HEIGHT 1500

typedef struct {
    int row;
    int col;
} AtlasPosition;

Rectangle HandRect(int player, int idx);
Rectangle ButtonRect(int player, int idx);
Rectangle SlotRect(int player, int key_idx, int slot_idx);
Rectangle KeyCardRect(int key_idx);
Rectangle DiscardPileRect(int player);
Rectangle ContinueButtonRect(int player);
Rectangle GetCardSourceRect(Card c);
void DrawContinueButtons(const GameState *g, Vector2 mouse);

float GetRankY(int key_idx);

extern Texture2D g_atlas_texture;
void LoadCardAtlas(void);
void UnloadCardAtlas(void);
// Get the source rectangle for a specific card in the atlas
Rectangle GetAtlasSourceRect(Rank rank, Suit suit);
// Get atlas position for special cards
Rectangle GetAtlasBackCard(void);
Rectangle GetAtlasTempCover(void);
Rectangle GetAtlasJoker(int joker_index); // 0 for JA, 1 for JB
Rectangle GetDealingAnimationRect(void);
// Helper to get atlas position from row/col
Rectangle AtlasRect(int row, int col);
void DrawGameLayout(const GameState *g);
void DrawPlayerUI(const GameState *g, int player);
void DrawLeaderboard(GameState *g);  
void DrawGameOver(GameState *g);
void DrawCard(Card c, Rectangle rect, Color tint);
void DrawButton(Rectangle rect, bool is_hovered, bool is_enabled, const char *text);

#endif