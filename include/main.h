#ifndef MAIN_H
#define MAIN_H
#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
// UI Layout constants
#define CENTER_X (SCREEN_W / 2.0f)
#define GRID_START_Y 50
#define ROW_SPACING 150
#define KEYCARD_COL_X (CENTER_X - CARD_W_SCALED / 2.0f)
#define SLOT_SPACING_X (CARD_W_SCALED - 105)
#define KEY_TO_SLOT_GAP 1
#define BUTTON_W 80
#define BUTTON_H 80
#define BUTTON_OFFSET_Y 20
#define SCREEN_W 1900
#define SCREEN_H 1080
#define UI_Y (SCREEN_H * 0.6f)
// Game constants / card ui layout
#define HAND_SIZE 5
#define KEYCARDS 5
#define TOTAL_DECK_CARDS 49
#define CARD_SCALE 0.65f
#define CARD_W_SCALED (160 * CARD_SCALE)
#define CARD_H_SCALED (220 * CARD_SCALE)
// Account and leaderboard limits
#define MAX_ACCOUNTS 5
#define MAX_ACCOUNT_NAME_LEN 12
#define MAX_LEADERBOARD_ENTRIES 100
#define MAX_LEADERBOARD_ENTRY_NAME_LEN 64
#define MAX_LEADERBOARD_WINNER_NAME_LEN 32
#define MAX_LEADERBOARD_TIMESTAMP_LEN 32
#define MAX_LEADERBOARD_NAME_LEN 128
// Game constants for economy and rewards/costs.
#define COST_DISCARD 1.00f
#define COST_PER_ROUND 5.00f 
#define REWARD_MATCH 10.00f
#define REWARD_DOUBLE_JOKER 50.00f
#define REWARD_PLACEMENT 2.00f
#define REWARD_COMPLETION 20.00f
#define JOKER_DISCARD 5.00f
#define SPEED_BONUS_BASE 250.00f
#define BONUS_CAP 500.00f
typedef enum
{
    AI_BOB = 0,
    AI_THEA = 1,
    AI_FLINT = 2
} AIType;
typedef enum
{
    RANK_2 = 0,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
    RANK_9,
    RANK_10,
    RANK_JACK,
    RANK_QUEEN,
    RANK_KING,
    RANK_ACE,
    RANK_JOKER
} Rank;
typedef enum
{
    SUIT_CLUBS,
    SUIT_DIAMONDS,
    SUIT_HEARTS,
    SUIT_SPADES,
    SUIT_NONE
} Suit;
typedef struct
{
    Rank rank;
    Suit suit;
    bool is_valid;
    Texture2D texture;
    char filename[16];
} Card;
typedef enum
{
    STATE_MAIN_MENU,
    STATE_ACCOUNTS_MANAGER,
    STATE_ACCOUNT_CREATE,
    STATE_SETTINGS,
    STATE_ROUND_START,
    STATE_P1_SELECT_DISCARD,
    STATE_P2_SELECT_DISCARD,
    STATE_REVEAL_AND_RESOLVE,
    STATE_WAIT_FOR_TURN,
    STATE_HAND_RESHUFFLE,
    STATE_COVER_ANIMATION,
    STATE_ROUNDS_COMPLETED,
    STATE_CHECK_WIN,
    STATE_GAME_OVER,
    STATE_LEADERBOARD
} GameStateEnum;
typedef enum
{
    MODE_PVP,
    MODE_PVAI,
    MODE_AIVSAI
} GameMode;
typedef struct LeaderboardEntry
{
    float total_winnings;
    float final_balance;
    float bonus;
    int total_rounds;
    char entry_name[MAX_LEADERBOARD_ENTRY_NAME_LEN];
    char winner_name[MAX_LEADERBOARD_WINNER_NAME_LEN];
    char timestamp[MAX_LEADERBOARD_TIMESTAMP_LEN];
} LeaderboardEntry;
typedef struct Account
{
    char first_name[MAX_ACCOUNT_NAME_LEN + 1];
    char last_name[MAX_ACCOUNT_NAME_LEN + 1];
    double balance;
    int wins;
    int losses;
    bool is_ai;
    AIType ai_type;
    bool is_active;
    bool is_logged_in;
} Account;
typedef struct
{
    float ai_move_delay;
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
    Card delay_discard_p1; // Added for discard delay
    Card delay_discard_p2;
    bool discards_pending; // Flag to indicate pending discards
    bool p1_discard_ready;
    bool p2_discard_ready;
    float Reshuffle_cover_timer;
    GameStateEnum state;
    GameMode mode;
    float ai_timer;
    bool p1_ai_done_placing_rounds;
    float p1_balance;
    float p2_balance;
    int total_rounds;
    int placement_phases_count;
    Account accounts[MAX_ACCOUNTS];
    int account_count;
    int p1_account_index;
    int p2_account_index;
    int name_input_target;
    AIType selected_opponent_ai;
    bool game_over;
    int winner;
    float final_score_p1;
    float final_score_p2;
    double win_timer_start;
    LeaderboardEntry leaderboard[MAX_LEADERBOARD_ENTRIES];
    int leaderboard_count;
    bool leaderboard_loaded;
    bool leaderboard_sort_by_rounds;
    char account_status_message[128];
    double account_status_timer;
    bool cover_p2_cards;
} GameState;
extern Texture2D g_card_back_texture;
extern Texture2D g_background_texture;
extern Texture2D g_ui_frame_texture;
extern Texture2D g_button_texture;
extern Texture2D g_temp_cover_texture;
extern Sound g_discard_sound;
extern Sound g_place_sound;
extern Sound g_filled_rank_sound;
extern Sound g_reveal_sound;
extern Sound g_win_sound;
extern Sound g_joker_sound;
extern Sound g_matching_jokers_sound;
extern Sound g_matching_cards_sound;
extern Sound g_continue_sound;
extern Sound g_coin_sound;
extern Sound g_beep_sound;
extern Music g_background_music;
Card DrawFromDeck(GameState *g);
void CheckRankCompletionBonus(GameState *g, int player, int key_idx, int cards_before);
float GetRewardMultiplier(int completed_ranks);
void RestartGameKeepingAccounts(GameState *g);
Card BlankCard(void);
float CalculateFinalScore(float balance, int total_rounds, bool is_winner);
void ProcessPendingDiscards(GameState *g);
void ResolveDiscards(GameState *g);
void RefreshHands(GameState *g);
void UpdateWinStats(GameState *g);
void CompleteRound(GameState *g);
bool IsPlayerAI(const GameState *g, int player);
void UpdateAccountBalances(GameState *g);
void LoadAllAccounts(GameState *g);
void SaveAllAccounts(const GameState *g);
void LoadLeaderboard(GameState *g);
void SaveLeaderboard(const GameState *g);
void InitAccounts(GameState *g);
#endif