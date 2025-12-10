#include "main.h"
#include "gui.h"
#include "aibots.h"
#include "mainmenu.h"
#include "useraccount.h"

Texture2D g_card_back_texture = {0};
Texture2D g_background_texture = {0};
Texture2D g_ui_frame_texture = {0};
Texture2D g_button_texture = {0};

GameState g_initial_state = {0};

float GetRewardMultiplier(int completed_ranks) {
    if (completed_ranks >= 2) return 4.0f;
    if (completed_ranks == 1) return 2.0f;
    return 1.0f;
}

Card BlankCard(void) {
    Card c = {0};
    c.is_valid = false;
    c.texture = (Texture2D){0};
    return c;
}

static void BuildFilename(Card *c) {
    if (c->rank == RANK_JOKER) {
        if (strlen(c->filename) == 0)
            strcpy(c->filename, "JA.png");
        return;
    }
    char rank_str[4] = {0};
    char suit_char = ' ';

    switch (c->rank) {
        case RANK_2: strcpy(rank_str, "2"); break;
        case RANK_3: strcpy(rank_str, "3"); break;
        case RANK_4: strcpy(rank_str, "4"); break;
        case RANK_5: strcpy(rank_str, "5"); break;
        case RANK_6: strcpy(rank_str, "6"); break;
        case RANK_7: strcpy(rank_str, "7"); break;
        case RANK_8: strcpy(rank_str, "8"); break;
        case RANK_9: strcpy(rank_str, "9"); break;
        case RANK_10: strcpy(rank_str, "10"); break;
        case RANK_JACK: strcpy(rank_str, "J"); break;
        case RANK_QUEEN: strcpy(rank_str, "Q"); break;
        case RANK_KING: strcpy(rank_str, "K"); break;
        case RANK_ACE: strcpy(rank_str, "A"); break;
        default: strcpy(rank_str, "ERR"); break;
    }
    
    switch (c->suit) {
        case SUIT_CLUBS: suit_char = 'C'; break;
        case SUIT_DIAMONDS: suit_char = 'D'; break;
        case SUIT_HEARTS: suit_char = 'H'; break;
        case SUIT_SPADES: suit_char = 'S'; break;
        default: suit_char = 'X'; break;
    }
    
    sprintf(c->filename, "%s%c.png", rank_str, suit_char);
}

static void LoadCardTexture(Card *c, const char *path) {
    if (c->rank != RANK_JOKER || strlen(c->filename) == 0)
        BuildFilename(c);
    char full[128];
    snprintf(full, sizeof(full), "%s%s", path, c->filename);
    c->texture = LoadTexture(full);
    c->is_valid = (c->texture.id != 0);
}

static bool IsRankCompleted(const Card slots[3]) {
    int count = 0;
    for (int i = 0; i < 3; i++)
        if (slots[i].is_valid) count++;
    return count == 3;
}

void CheckRankCompletionBonus(GameState *g, int player, int key_idx, int cards_before) {
    Card(*slots)[3] = (player == 1) ? g->p1_slots : g->p2_slots;
    int cards_after = 0;
    for (int s = 0; s < 3; s++) {
        if (slots[key_idx][s].is_valid) cards_after++;
    }
    if (cards_after == 3 && cards_before < 3) {
        float *balance = (player == 1) ? &g->p1_balance : &g->p2_balance;
        int *completed = (player == 1) ? &g->p1_completed_ranks : &g->p2_completed_ranks;
        float mult = GetRewardMultiplier(*completed);
        *balance += REWARD_COMPLETION * mult;
        (*completed)++;
    }
}

Card DrawFromDeck(GameState *g) {
    if (g->top_card_index < 0) return BlankCard();
    Card c = g->deck[g->top_card_index--];
    g->current_deck_size--;
    return c;
}

void ReturnToDeck(GameState *g, Card c) {
    if (!c.is_valid || g->current_deck_size >= TOTAL_DECK_CARDS) return;
    g->deck[++g->top_card_index] = c;
    g->current_deck_size++;
    if (g->top_card_index > 0 && rand() % 3 == 0) {
        int r = rand() % (g->top_card_index + 1);
        Card t = g->deck[g->top_card_index];
        g->deck[g->top_card_index] = g->deck[r];
        g->deck[r] = t;
    }
}

static void TriggerSweep(GameState *g, int player) {
    for (int k = 0; k < KEYCARDS; k++) {
        if (player == 0 || player == 1) {
            if (!IsRankCompleted(g->p1_slots[k])) {
                for (int s = 0; s < 3; s++)
                    if (g->p1_slots[k][s].is_valid) {
                        ReturnToDeck(g, g->p1_slots[k][s]);
                        g->p1_slots[k][s] = BlankCard();
                    }
            }
        }
        if (player == 0 || player == 2) {
            if (!IsRankCompleted(g->p2_slots[k])) {
                for (int s = 0; s < 3; s++)
                    if (g->p2_slots[k][s].is_valid) {
                        ReturnToDeck(g, g->p2_slots[k][s]);
                        g->p2_slots[k][s] = BlankCard();
                    }
            }
        }
    }
}

static void JokersGambit(GameState *g) {
    TriggerSweep(g, 0);
    for (int i = 0; i < g->p1_hand_size; i++) ReturnToDeck(g, g->player1_hand[i]);
    for (int i = 0; i < g->p2_hand_size; i++) ReturnToDeck(g, g->player2_hand[i]);

    for (int i = 0; i < HAND_SIZE; i++) {
        g->player1_hand[i] = DrawFromDeck(g);
        g->player2_hand[i] = DrawFromDeck(g);
    }
    g->p1_hand_size = g->p2_hand_size = HAND_SIZE;
}

// REPLACE the existing STATE_WAIT_FOR_TURN initialization in ResolveDiscards with:
void ResolveDiscards(GameState *g) {
    Card d1 = g->revealed_p1;
    Card d2 = g->revealed_p2;
    bool j1 = (d1.rank == RANK_JOKER);
    bool j2 = (d2.rank == RANK_JOKER);

    g->p1_balance -= COST_DISCARD;
    g->p2_balance -= COST_DISCARD;

    float mult1 = GetRewardMultiplier(g->p1_completed_ranks);
    float mult2 = GetRewardMultiplier(g->p2_completed_ranks);

    if (j1 && j2) {
        JokersGambit(g);
        g->p1_balance -= PENALTY_JOKER;
        g->p2_balance -= PENALTY_JOKER;
        g->p1_balance += (REWARD_DOUBLE_JOKER * mult1);
        g->p2_balance += (REWARD_DOUBLE_JOKER * mult2);
        ReturnToDeck(g, d1);
        ReturnToDeck(g, d2);
    } else if (j1 || j2) {
        int opp = j1 ? 2 : 1;
        TriggerSweep(g, opp);
        if (j1) g->p1_balance -= PENALTY_JOKER;
        if (j2) g->p2_balance -= PENALTY_JOKER;
        g->player1_hand[g->p1_hand_size++] = DrawFromDeck(g);
        g->player2_hand[g->p2_hand_size++] = DrawFromDeck(g);
        ReturnToDeck(g, d1);
        ReturnToDeck(g, d2);
    } else if (d1.rank == d2.rank) {
        TriggerSweep(g, 0);
        g->p1_balance += (REWARD_MATCH * mult1);
        g->p2_balance += (REWARD_MATCH * mult2);
        g->player1_hand[g->p1_hand_size++] = DrawFromDeck(g);
        g->player2_hand[g->p2_hand_size++] = DrawFromDeck(g);
        ReturnToDeck(g, d1);
        ReturnToDeck(g, d2);
    } else {
        g->player1_hand[g->p1_hand_size++] = DrawFromDeck(g);
        g->player2_hand[g->p2_hand_size++] = DrawFromDeck(g);
        ReturnToDeck(g, d1);
        ReturnToDeck(g, d2);
    }

    g->p1_done_placing = false;
    g->p2_done_placing = false;
    g->total_moves++;
    
    // Keep revealed cards visible during placement phase
    // They will be hidden when transitioning to next discard phase
}

void RefreshHands(GameState *g) {
    for (int i = 0; i < g->p1_hand_size; i++) ReturnToDeck(g, g->player1_hand[i]);
    for (int i = 0; i < g->p2_hand_size; i++) ReturnToDeck(g, g->player2_hand[i]);

    for (int i = 0; i < HAND_SIZE; i++) {
        g->player1_hand[i] = DrawFromDeck(g);
        g->player2_hand[i] = DrawFromDeck(g);
    }
    g->p1_hand_size = g->p2_hand_size = HAND_SIZE;
}

float CalculateFinalScore(float balance, int total_moves, bool is_winner) {
    if (total_moves <= 0) return balance;
    float move_bonus = is_winner ? (10.0f * total_moves) : (-10.0f * total_moves);
    return balance + move_bonus;
}

void UpdateWinStats(GameState *g) {
    g->p1_completed_ranks = 0;
    g->p2_completed_ranks = 0;
    for (int k = 0; k < KEYCARDS; k++) {
        if (IsRankCompleted(g->p1_slots[k])) g->p1_completed_ranks++;
        if (IsRankCompleted(g->p2_slots[k])) g->p2_completed_ranks++;
    }
}

void AddLeaderboardEntry(GameState *g, int winner) {
    if (g->leaderboard_count >= MAX_LEADERBOARD_ENTRIES) {
        for (int i = 0; i < MAX_LEADERBOARD_ENTRIES - 1; i++)
            g->leaderboard[i] = g->leaderboard[i + 1];
        g->leaderboard_count = MAX_LEADERBOARD_ENTRIES - 1;
    }
    
    LeaderboardEntry *e = &g->leaderboard[g->leaderboard_count];
    const char *p1_name_str = GetPlayerName(g, 1);
    const char *p2_name_str = GetPlayerName(g, 2);
    
    if (g->mode == MODE_PVAI)
        snprintf(e->entry_name, 64, "%s_vs_%s", p1_name_str, p2_name_str);
    else if (g->mode == MODE_PVP)
        snprintf(e->entry_name, 64, "%s_vs_%s", p1_name_str, p2_name_str);
    else
        snprintf(e->entry_name, 64, "Flint_vs_Thea");

    e->final_balance = (winner == 1) ? g->p1_balance : g->p2_balance;
    float final_score_winner = CalculateFinalScore(e->final_balance, g->total_moves, true);
    e->total_winnings = final_score_winner;
    e->bonus = e->total_winnings - e->final_balance;
    
    snprintf(e->winner_name, 32, "%s", (winner == 1) ? p1_name_str : p2_name_str);

    time_t timer;
    time(&timer);
    struct tm *tm_info = localtime(&timer);
    strftime(e->timestamp, 32, "(%m/%d)", tm_info);
    g->leaderboard_count++;
    
    SaveLeaderboard(g);
}


void InitGame(GameState *g) {
    // Save persistent data
    Account saved_accounts[MAX_ACCOUNTS];
    LeaderboardEntry saved_leaderboard[MAX_LEADERBOARD_ENTRIES];
    int saved_account_count = g->account_count;
    int saved_p1_idx = g->p1_account_index;
    int saved_p2_idx = g->p2_account_index;
    GameMode saved_mode = g->mode;
    int saved_leaderboard_count = g->leaderboard_count;
    AIType saved_opponent_ai = g->selected_opponent_ai;

    memcpy(saved_accounts, g->accounts, sizeof(saved_accounts));
    memcpy(saved_leaderboard, g->leaderboard, sizeof(saved_leaderboard));

    // FULL CLEAN RESET
    *g = (GameState){0};

    // Restore persistent data
    memcpy(g->accounts, saved_accounts, sizeof(saved_accounts));
    memcpy(g->leaderboard, saved_leaderboard, sizeof(saved_leaderboard));
    g->account_count = saved_account_count;
    g->p1_account_index = saved_p1_idx;
    g->p2_account_index = saved_p2_idx;
    g->mode = saved_mode;
    g->leaderboard_count = saved_leaderboard_count;
    g->selected_opponent_ai = saved_opponent_ai;

    // Build deck (existing code continues...)
    Rank keys[5] = {RANK_ACE, RANK_KING, RANK_QUEEN, RANK_JACK, RANK_10};
    int idx = 0;
    
    for (int s = 0; s < 4; s++) {
        for (int r = 0; r < 13; r++) {
            bool is_key = (s == SUIT_HEARTS && (r == 0 || r == 12 || r == 11 || r == 10 || r == 9));
            if (!is_key) {
                g->deck[idx] = (Card){.rank = (Rank)r, .suit = (Suit)s};
                LoadCardTexture(&g->deck[idx], "cards/");
                if (g->deck[idx].is_valid) idx++;
            }
        }
    }
    
    for (int i = 0; i < 2; i++) {
        g->deck[idx] = (Card){.rank = RANK_JOKER, .suit = SUIT_NONE};
        snprintf(g->deck[idx].filename, 16, i == 0 ? "JA.png" : "JB.png");
        LoadCardTexture(&g->deck[idx], "cards/");
        if (g->deck[idx].is_valid) idx++;
    }
    
    g->current_deck_size = idx;
    g->top_card_index = idx - 1;

    // Keycards
    for (int i = 0; i < KEYCARDS; i++) {
        g->keycards[i] = (Card){.rank = keys[i], .suit = SUIT_HEARTS};
        LoadCardTexture(&g->keycards[i], "keycards/");
    }

    // Shuffle
    for (int i = g->top_card_index; i > 0; i--) {
        int j = rand() % (i + 1);
        Card t = g->deck[i];
        g->deck[i] = g->deck[j];
        g->deck[j] = t;
    }

    // Deal hands
    for (int i = 0; i < HAND_SIZE; i++) {
        g->player1_hand[i] = DrawFromDeck(g);
        g->player2_hand[i] = DrawFromDeck(g);
    }
    g->p1_hand_size = g->p2_hand_size = HAND_SIZE;

    // FIX: Load balances correctly for both human and AI accounts
    // Player 1 balance
    if (g->p1_account_index != -1) {
        g->p1_balance = (float)g->accounts[g->p1_account_index].balance;
    } else {
        g->p1_balance = 10.0f;
    }
    
    // Player 2 balance - check if it's PvAI mode and load selected AI's balance
    if (g->p2_account_index != -1) {
        g->p2_balance = (float)g->accounts[g->p2_account_index].balance;
    } else if (g->mode == MODE_PVAI) {
        // Find the AI account matching the selected opponent
        for (int i = 0; i < g->account_count; i++) {
            if (g->accounts[i].is_ai && g->accounts[i].ai_type == g->selected_opponent_ai) {
                g->p2_balance = (float)g->accounts[i].balance;
                g->p2_account_index = i; // Auto-login the AI
                g->accounts[i].is_logged_in = true;
                break;
            }
        }
        // Fallback if AI not found
        if (g->p2_account_index == -1) {
            g->p2_balance = 10.0f;
        }
    } else {
        g->p2_balance = 10.0f;
    }

    // Reset round state
    g->state = STATE_P1_SELECT_DISCARD;
    g->p1_completed_ranks = g->p2_completed_ranks = 0;
    g->total_moves = 0;
    g->placement_phases_count = 0;
    g->p1_done_placing = g->p2_done_placing = false;
    g->game_over = false;
    g->p1_selected = g->p2_selected = false;
}
void RestartGameKeepingAccounts(GameState *g) {
    InitGame(g);
    g->p1_balance = (g->p1_account_index != -1) ? (float)g->accounts[g->p1_account_index].balance : 10.0f;
    g->p2_balance = (g->p2_account_index != -1) ? (float)g->accounts[g->p2_account_index].balance : 10.0f;
}

int main(void) {
    srand((unsigned)time(NULL));
    InitWindow(SCREEN_W, SCREEN_H, "JOKERS GAMBIT");
    SetTargetFPS(60);

    g_card_back_texture = LoadTexture("keycards/BACK.png");
    g_background_texture = LoadTexture("keycards/background.png");
    g_ui_frame_texture = LoadTexture("keycards/frame.png");
    g_button_texture = LoadTexture("keycards/btn.png");

    GameState g;
    InitAccounts(&g);
    LoadAllAccounts(&g);
    LoadLeaderboard(&g);
    InitGame(&g);
    g.state = STATE_MAIN_MENU;
    g.account_status_message[0] = '\0';
    g.account_status_timer = 0.0;

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();
        Rectangle menu_btn_rect = {40, 20, 300, 70};
        Rectangle restart_btn_rect = {SCREEN_W - 340, 20, 300, 70};
        Rectangle skip_btn_rect = {CENTER_X - 150, SCREEN_H - 120, 300, 60};
        bool game_active = (g.state >= STATE_P1_SELECT_DISCARD && g.state <= STATE_CHECK_WIN);

        // Menu/Restart button handling
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mouse, menu_btn_rect) &&
                (game_active || g.state == STATE_GAME_OVER || g.state == STATE_LEADERBOARD ||
                 g.state == STATE_ACCOUNTS_MANAGER || g.state == STATE_ACCOUNT_CREATE || g.state == STATE_SETTINGS)) {
                // FIX: Save balances before returning to menu
                if (game_active) {
                    if (g.p1_account_index != -1)
                        g.accounts[g.p1_account_index].balance = (double)g.p1_balance;
                    if (g.p2_account_index != -1)
                        g.accounts[g.p2_account_index].balance = (double)g.p2_balance;
                    SaveAllAccounts(&g);
                }
                g.state = STATE_MAIN_MENU;
            }
            if (CheckCollisionPointRec(mouse, restart_btn_rect) && (game_active || g.state == STATE_GAME_OVER)) {
                // FIX: Save balances before restarting
                if (g.p1_account_index != -1)
                    g.accounts[g.p1_account_index].balance = (double)g.p1_balance;
                if (g.p2_account_index != -1)
                    g.accounts[g.p2_account_index].balance = (double)g.p2_balance;
                SaveAllAccounts(&g);
                RestartGameKeepingAccounts(&g);
                g.state = STATE_P1_SELECT_DISCARD;
            }
            if (g.state == STATE_LEADERBOARD && !CheckCollisionPointRec(mouse, menu_btn_rect)) {
                g.state = STATE_MAIN_MENU;
            }
        }

        // State updates
        if (g.state == STATE_MAIN_MENU) {
            UpdateMainMenu(&g, mouse);
        }
        else if (g.state == STATE_SETTINGS) {
            UpdateSettings(&g, mouse);
        }
        else if (g.state == STATE_ACCOUNTS_MANAGER) {
            UpdateAccountsManager(&g, mouse);
        }
        else if (g.state == STATE_ACCOUNT_CREATE) {
            UpdateAccountCreate(&g);
        }
        else if (g.state == STATE_REVEAL_AND_RESOLVE && g.p1_selected && g.p2_selected) {
            g.revealed_p1 = g.player1_hand[g.p1_discard_idx];
            g.revealed_p2 = g.player2_hand[g.p2_discard_idx];
            
            for (int i = g.p1_discard_idx; i < g.p1_hand_size - 1; i++)
                g.player1_hand[i] = g.player1_hand[i + 1];
            g.p1_hand_size--;

            for (int i = g.p2_discard_idx; i < g.p2_hand_size - 1; i++)
                g.player2_hand[i] = g.player2_hand[i + 1];
            g.p2_hand_size--;

            ResolveDiscards(&g);
            g.p1_selected = false;
            g.p2_selected = false;
            g.state = STATE_WAIT_FOR_TURN;
            g.ai_timer = 0;
        }
        else if (g.state == STATE_BLOCK_DECAY) {
            g.placement_phases_count++;
            if (g.placement_phases_count % 3 == 0) RefreshHands(&g);
            g.revealed_p1 = BlankCard();
            g.revealed_p2 = BlankCard();
            g.state = STATE_CHECK_WIN;
        }
        else if (g.state == STATE_CHECK_WIN) {
            UpdateWinStats(&g);
            if (g.p1_completed_ranks >= 3 || g.p2_completed_ranks >= 3) {
                g.winner = (g.p1_completed_ranks >= 3) ? 1 : 2;
                g.game_over = true;
                g.state = STATE_GAME_OVER;
                g.win_timer_start = GetTime();
                
                g.final_score_p1 = CalculateFinalScore(g.p1_balance, g.total_moves, g.winner == 1);
                g.final_score_p2 = CalculateFinalScore(g.p2_balance, g.total_moves, g.winner == 2);
                
                g.p1_balance = g.final_score_p1;
                g.p2_balance = g.final_score_p2;
                
                UpdateAccountBalances(&g);
                AddLeaderboardEntry(&g, g.winner);
            } else {
                g.revealed_p1 = BlankCard();
                g.revealed_p2 = BlankCard();
                g.state = STATE_P1_SELECT_DISCARD;
            }
        }
        else if (g.state == STATE_GAME_OVER) {
            if (IsKeyPressed(KEY_R)) {
                RestartGameKeepingAccounts(&g);
                g.state = STATE_P1_SELECT_DISCARD;
            }
            if (GetTime() - g.win_timer_start > 10.0) {
                RestartGameKeepingAccounts(&g);
                g.state = STATE_P1_SELECT_DISCARD;
            }
        }

        // Game logic
        if (game_active) {
            if (g.mode == MODE_AIVSAI && g.state == STATE_P1_SELECT_DISCARD) {
                AI_SelectDiscard(&g, 1);
                g.state = STATE_P2_SELECT_DISCARD;
            }
            if (g.state == STATE_P2_SELECT_DISCARD && (g.mode == MODE_PVAI || g.mode == MODE_AIVSAI)) {
                AI_SelectDiscard(&g, 2);
                g.state = STATE_REVEAL_AND_RESOLVE;
            }

            if (g.state == STATE_WAIT_FOR_TURN) {
                if (g.mode == MODE_AIVSAI && !g.p1_done_placing) {
                    AI_UpdatePlacementPhase(&g, 1);
                }
                if ((g.mode == MODE_PVAI || g.mode == MODE_AIVSAI) && !g.p2_done_placing) {
                    if (g.mode == MODE_PVAI || g.p1_ai_done_placing_moves) {
                        AI_UpdatePlacementPhase(&g, 2);
                    }
                }
                if (g.p1_done_placing && g.p2_done_placing) {
                    g.state = STATE_BLOCK_DECAY;
                    g.p1_ai_done_placing_moves = false;
                }
            }

            bool skip_placement_pressed = IsKeyPressed(KEY_SPACE);
            if (CheckCollisionPointRec(mouse, skip_btn_rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                skip_placement_pressed = true;
            }

            if (g.state == STATE_WAIT_FOR_TURN) {
                float mult1 = GetRewardMultiplier(g.p1_completed_ranks);
                float mult2 = GetRewardMultiplier(g.p2_completed_ranks);
                float reward1 = REWARD_PLACEMENT * mult1;
                float reward2 = REWARD_PLACEMENT * mult2;

                if (g.mode != MODE_AIVSAI && !g.p1_done_placing) {
                    for (int i = 0; i < g.p1_hand_size; i++) {
                        Rectangle card_rect = HandRect(1, i);
                        if (CheckCollisionPointRec(mouse, card_rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            Card c = g.player1_hand[i];
                            if (c.suit != SUIT_HEARTS && c.rank != RANK_JOKER) {
                                bool placed = false;
                                for (int k = 0; k < KEYCARDS; k++) {
                                    if (c.rank == g.keycards[k].rank) {
                                        int cards_before = 0;
                                        for (int s_check = 0; s_check < 3; s_check++)
                                            if (g.p1_slots[k][s_check].is_valid) cards_before++;
                                        
                                        if (cards_before < 3) {
                                            for (int s = 0; s < 3; s++) {
                                                if (!g.p1_slots[k][s].is_valid) {
                                                    g.p1_slots[k][s] = c;
                                                    g.p1_balance += reward1;
                                                    CheckRankCompletionBonus(&g, 1, k, cards_before);
                                                    for (int j = i; j < g.p1_hand_size - 1; j++)
                                                        g.player1_hand[j] = g.player1_hand[j + 1];
                                                    g.p1_hand_size--;
                                                    g.player1_hand[g.p1_hand_size++] = DrawFromDeck(&g);
                                                    placed = true;
                                                    break;
                                                }
                                            }
                                        }
                                        if (placed) break;
                                    }
                                }
                            }
                        }
                    }
                    if (skip_placement_pressed) g.p1_done_placing = true;
                }

                if (g.mode == MODE_PVP && !g.p2_done_placing) {
                    for (int i = 0; i < g.p2_hand_size; i++) {
                        Rectangle card_rect = HandRect(2, i);
                        if (CheckCollisionPointRec(mouse, card_rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            Card c = g.player2_hand[i];
                            if (c.suit != SUIT_HEARTS && c.rank != RANK_JOKER) {
                                bool placed = false;
                                for (int k = 0; k < KEYCARDS; k++) {
                                    if (c.rank == g.keycards[k].rank) {
                                        int cards_before = 0;
                                        for (int s_check = 0; s_check < 3; s_check++)
                                            if (g.p2_slots[k][s_check].is_valid) cards_before++;
                                        
                                        if (cards_before < 3) {
                                            for (int s = 0; s < 3; s++) {
                                                if (!g.p2_slots[k][s].is_valid) {
                                                    g.p2_slots[k][s] = c;
                                                    g.p2_balance += reward2;
                                                    CheckRankCompletionBonus(&g, 2, k, cards_before);
                                                    for (int j = i; j < g.p2_hand_size - 1; j++)
                                                        g.player2_hand[j] = g.player2_hand[j + 1];
                                                    g.p2_hand_size--;
                                                    g.player2_hand[g.p2_hand_size++] = DrawFromDeck(&g);
                                                    placed = true;
                                                    break;
                                                }
                                            }
                                        }
                                        if (placed) break;
                                    }
                                }
                            }
                        }
                    }
                    if (skip_placement_pressed) g.p2_done_placing = true;
                }
            }

            // Human discard selection
            if ((g.state == STATE_P1_SELECT_DISCARD && g.mode != MODE_AIVSAI) ||
                (g.state == STATE_P2_SELECT_DISCARD && g.mode == MODE_PVP)) {
                int player = (g.state == STATE_P1_SELECT_DISCARD) ? 1 : 2;
                int hand_size = (player == 1) ? g.p1_hand_size : g.p2_hand_size;
                bool *selected_ptr = (player == 1) ? &g.p1_selected : &g.p2_selected;
                int *idx_ptr = (player == 1) ? &g.p1_discard_idx : &g.p2_discard_idx;
                
                if (!*selected_ptr) {
                    for (int i = 0; i < hand_size; i++) {
                        Rectangle btn_rect = ButtonRect(player, i);
                        if (CheckCollisionPointRec(mouse, btn_rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            *idx_ptr = i;
                            *selected_ptr = true;
                            if (g.state == STATE_P1_SELECT_DISCARD)
                                g.state = STATE_P2_SELECT_DISCARD;
                            else if (g.state == STATE_P2_SELECT_DISCARD)
                                g.state = STATE_REVEAL_AND_RESOLVE;
                            break;
                        }
                    }
                }
            }
        }

        // Drawing
        BeginDrawing();
        if (g_background_texture.id)
            DrawTexture(g_background_texture, 0, 0, RAYWHITE);
        else
            ClearBackground(RAYWHITE);

        if (g.state == STATE_MAIN_MENU) {
            DrawMainMenu(&g);
        }
        else if (g.state == STATE_SETTINGS) {
            DrawSettings(&g);
        }
        else if (g.state == STATE_ACCOUNTS_MANAGER) {
            DrawAccountsManager(&g);
        }
        else if (g.state == STATE_ACCOUNT_CREATE) {
            DrawAccountCreate(&g);
        }
        else if (g.state == STATE_LEADERBOARD) {
            DrawLeaderboard(&g);
        }
        else if (g.state == STATE_GAME_OVER) {
            DrawGameOver(&g);
        }
        else {
            DrawGameLayout(&g);
            
            if (g.state == STATE_WAIT_FOR_TURN && g.mode != MODE_AIVSAI) {
                bool skip_hover = CheckCollisionPointRec(mouse, skip_btn_rect);
                DrawRectangleRec(skip_btn_rect, skip_hover ? GOLD : ORANGE);
                DrawText("CONTINUE", (int)skip_btn_rect.x + 15, (int)skip_btn_rect.y + 20, 25, BLACK);
            }
            
            bool menu_hover = CheckCollisionPointRec(mouse, menu_btn_rect);
            bool restart_hover = CheckCollisionPointRec(mouse, restart_btn_rect);
            DrawRectangleRec(menu_btn_rect, menu_hover ? LIME : DARKGREEN);
            DrawRectangleRec(restart_btn_rect, restart_hover ? SKYBLUE : BLUE);
            DrawText("MAIN MENU", (int)menu_btn_rect.x + 40, (int)menu_btn_rect.y + 20, 30, WHITE);
            DrawText("RESTART", (int)restart_btn_rect.x + 60, (int)restart_btn_rect.y + 20, 30, WHITE);
        }
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}