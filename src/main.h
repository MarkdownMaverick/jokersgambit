#ifndef MAIN_H
#define MAIN_H

#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// ─────────────────────────────────────────────────────────────────────────────
//  CONSTANTS
// ─────────────────────────────────────────────────────────────────────────────
#define SCREEN_W 1900
#define SCREEN_H 1100
#define SPEED_BONUS_BASE 50.00f 
#define IDEAL_MOVES 50          
#define BONUS_CAP 100.00f

#define HAND_SIZE 5
#define KEYCARDS 5
#define TOTAL_DECK_CARDS 49
#define CARD_SCALE 0.65f
#define CARD_W_SCALED (160 * CARD_SCALE)
#define CARD_H_SCALED (220 * CARD_SCALE)
#define AI_MOVE_DELAY 1.0f 

// --- ACCOUNT CONSTANTS ---
#define MAX_ACCOUNTS 5
#define MAX_ACCOUNT_NAME_LEN 12 
#define MAX_LEADERBOARD_ENTRIES 100  // HIGH ROLLERS TOP 100

// Monetary Constants
#define COST_DISCARD 0.10f
#define REWARD_MATCH 1.00f
#define REWARD_DOUBLE_JOKER 5.00f
#define REWARD_PLACEMENT 0.10f
#define REWARD_COMPLETION 1.00f
#define PENALTY_JOKER 1.00f
// UI Layout
#define CENTER_X (SCREEN_W / 2.0f)
#define GRID_START_Y 160
#define ROW_SPACING 150
#define KEYCARD_COL_X (CENTER_X - CARD_W_SCALED / 2.0f)
#define SLOT_SPACING_X (CARD_W_SCALED - 105) 
#define KEY_TO_SLOT_GAP 1                    
#define BUTTON_W 80        
#define BUTTON_H 80        
#define BUTTON_OFFSET_Y 20 
// In main.h
#define MAX_LEADERBOARD_ENTRY_NAME_LEN 64
#define MAX_LEADERBOARD_WINNER_NAME_LEN 32
#define MAX_LEADERBOARD_TIMESTAMP_LEN 32
// Define a constant for the leaderboard name length, aligning with useraccount.c's buffer size
#define MAX_LEADERBOARD_NAME_LEN 128


// ─────────────────────────────────────────────────────────────────────────────
//  DATA TYPES
// ─────────────────────────────────────────────────────────────────────────────
typedef enum {
    RANK_2 = 0, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_9, RANK_10,
    RANK_JACK, RANK_QUEEN, RANK_KING, RANK_ACE, RANK_JOKER
} Rank;

typedef enum {
    SUIT_CLUBS, SUIT_DIAMONDS, SUIT_HEARTS, SUIT_SPADES, SUIT_NONE
} Suit;

typedef struct {
    Rank rank;
    Suit suit;
    bool is_valid;
    Texture2D texture;
    char filename[16];
} Card;

typedef enum {
    STATE_MAIN_MENU,
    STATE_ACCOUNTS_MANAGER,
    STATE_ACCOUNT_CREATE,
    STATE_P1_SELECT_DISCARD,
    STATE_P2_SELECT_DISCARD,
    STATE_REVEAL_AND_RESOLVE,
    STATE_WAIT_FOR_TURN,
    STATE_BLOCK_DECAY,
    STATE_CHECK_WIN,
    STATE_GAME_OVER,
    STATE_LEADERBOARD
} GameStateEnum;

typedef enum { MODE_PVP, MODE_PVAI, MODE_AIVSAI } GameMode;

// In main.h, define the LeaderboardEntry struct:
typedef struct LeaderboardEntry {
    float total_winnings;
    float final_balance;
    float bonus;
    char entry_name[MAX_LEADERBOARD_ENTRY_NAME_LEN];    // e.g. "Player1_vs_Bob" (used for file I/O)
    char winner_name[MAX_LEADERBOARD_WINNER_NAME_LEN];  // e.g. "Player1"
    char timestamp[MAX_LEADERBOARD_TIMESTAMP_LEN];      // e.g. "(12/07)"
} LeaderboardEntry;



typedef struct Account {
    char first_name[MAX_ACCOUNT_NAME_LEN + 1];
    char last_name[MAX_ACCOUNT_NAME_LEN + 1];
    double balance;
    // --- NEW FIELDS ---
    int wins;
    int losses;
    bool is_ai;        // <── ADD THIS LINE
    bool is_active;
    // ------------------
    bool is_logged_in;
} Account;

typedef struct {
    Card deck[TOTAL_DECK_CARDS];
    int top_card_index;
    int current_deck_size;

    Card player1_hand[HAND_SIZE];
    Card player2_hand[HAND_SIZE];
    int p1_hand_size;
    int p2_hand_size;

    Card keycards[KEYCARDS];
    Card p1_slots[KEYCARDS][3];
    Card p2_slots[KEYCARDS][3];

    int p1_completed_ranks;
    int p2_completed_ranks;

    bool p1_done_placing;
    bool p2_done_placing;

    int p1_discard_idx;
    bool p1_selected;
    int p2_discard_idx;
    bool p2_selected;
    Card revealed_p1;
    Card revealed_p2;
    // --- NEW JOKER GAMBIT SLOTS ---
    Card p1_gambit_slot; // Slot for player 1's deposited Joker
    Card p2_gambit_slot; // Slot for player 2's deposited Joker
    // ------------------------------
    GameStateEnum state;
    GameMode mode;

    float ai_timer;
    bool p1_ai_done_placing_moves;

    float p1_balance;
    float p2_balance;
    int total_moves;            
    int placement_phases_count; 

    Account accounts[MAX_ACCOUNTS];
    int account_count;
    int p1_account_index;
    int p2_account_index;
    int name_input_target; 

    bool game_over;
    int winner;
    float final_score_p1;
    float final_score_p2;
    double win_timer_start;
    
    LeaderboardEntry leaderboard[MAX_LEADERBOARD_ENTRIES];
    int leaderboard_count;
    bool leaderboard_loaded;
} GameState;

// ─────────────────────────────────────────────────────────────────────────────
//  GLOBALS EXTERN
// ─────────────────────────────────────────────────────────────────────────────
extern Texture2D g_card_back_texture;
extern Texture2D g_background_texture;
extern Texture2D g_ui_frame_texture;
extern Texture2D g_button_texture;

// Helper prototypes
Card DrawFromDeck(GameState *g);
void CheckRankCompletionBonus(GameState *g, int player, int key_idx, int cards_before);
float GetRewardMultiplier(int completed_ranks);
void RestartGameKeepingAccounts(GameState *g);
Card BlankCard(void);
float CalculateFinalScore(float balance, int total_moves, bool is_winner);
void ResolveDiscards(GameState *g);
void RefreshHands(GameState *g);
void UpdateWinStats(GameState *g);

// Account/IO prototypes
const char* GetPlayerName(const GameState *g, int player);
void UpdateAccountBalances(GameState *g);
void LoadAllAccounts(GameState *g);
void SaveAllAccounts(const GameState *g);
void LoadLeaderboard(GameState *g);
void SaveLeaderboard(const GameState *g);
void InitAccounts(GameState *g);

#endif