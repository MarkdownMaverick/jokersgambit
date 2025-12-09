// aibots.c

#include "aibots.h"

// BOB AI: Original behavior - prefers Jokers, then low cards
static void AI_Bob_SelectDiscard(Card *hand, int hand_size, int *best_idx) {
    int joker_idx = -1;
    int low_card_idx = -1;

    for (int i = 0; i < hand_size; i++) {
        if (hand[i].rank == RANK_JOKER) {
            joker_idx = i;
            break;
        }
        if (hand[i].rank >= RANK_2 && hand[i].rank <= RANK_9) {
            if (low_card_idx == -1) low_card_idx = i;
        }
    }

    if (joker_idx != -1) *best_idx = joker_idx;
    else if (low_card_idx != -1) *best_idx = low_card_idx;
    else *best_idx = 0;
}

// THEA AI: Discards randomly with no Joker preference
static void AI_Thea_SelectDiscard(Card *hand, int hand_size, int *best_idx) {
    (void)hand;  // Unused in random selection
    *best_idx = rand() % hand_size;
}

// FLINT AI: Never discards Jokers, prefers low cards
static void AI_Flint_SelectDiscard(Card *hand, int hand_size, int *best_idx) {
    int low_card_idx = -1;
    int non_joker_idx = -1;

    for (int i = 0; i < hand_size; i++) {
        if (hand[i].rank != RANK_JOKER) {
            if (non_joker_idx == -1) non_joker_idx = i;
            if (hand[i].rank >= RANK_2 && hand[i].rank <= RANK_9) {
                if (low_card_idx == -1) low_card_idx = i;
            }
        }
    }

    if (low_card_idx != -1) *best_idx = low_card_idx;
    else if (non_joker_idx != -1) *best_idx = non_joker_idx;
    else *best_idx = 0;  // Fallback (shouldn't happen if hand has non-Jokers)
}

void AI_SelectDiscard(GameState *g, int player) {
    Card *hand = (player == 1) ? g->player1_hand : g->player2_hand;
    int hand_size = (player == 1) ? g->p1_hand_size : g->p2_hand_size;
    int best_idx = 0;

    // Determine which AI type to use
    AIType ai_type = AI_BOB;  // Default
    
    if (g->mode == MODE_AIVSAI) {
        // In AI vs AI mode:
        // Player 1 = FLINT (index 2), Player 2 = THEA (index 1)
        if (player == 1) {
            // Find FLINT account
            for (int i = 0; i < g->account_count; i++) {
                if (g->accounts[i].is_ai && g->accounts[i].ai_type == AI_FLINT) {
                    ai_type = AI_FLINT;
                    break;
                }
            }
        } else {
            // Find THEA account
            for (int i = 0; i < g->account_count; i++) {
                if (g->accounts[i].is_ai && g->accounts[i].ai_type == AI_THEA) {
                    ai_type = AI_THEA;
                    break;
                }
            }
        }
    } else if (g->mode == MODE_PVAI && player == 2) {
        // In PvAI mode, use the selected opponent AI
        ai_type = g->selected_opponent_ai;
    } else if (player == 1 && g->p1_account_index != -1 && g->accounts[g->p1_account_index].is_ai) {
        ai_type = g->accounts[g->p1_account_index].ai_type;
    } else if (player == 2 && g->p2_account_index != -1 && g->accounts[g->p2_account_index].is_ai) {
        ai_type = g->accounts[g->p2_account_index].ai_type;
    }

    // Call appropriate AI function
    switch (ai_type) {
        case AI_BOB:
            AI_Bob_SelectDiscard(hand, hand_size, &best_idx);
            break;
        case AI_THEA:
            AI_Thea_SelectDiscard(hand, hand_size, &best_idx);
            break;
        case AI_FLINT:
            AI_Flint_SelectDiscard(hand, hand_size, &best_idx);
            break;
    }

    if (player == 1) {
        g->p1_discard_idx = best_idx;
        g->p1_selected = true;
    } else {
        g->p2_discard_idx = best_idx;
        g->p2_selected = true;
    }
}

void AI_UpdatePlacementPhase(GameState *g, int player) {
    Card *hand = (player == 1) ? g->player1_hand : g->player2_hand;
    int *hand_size = (player == 1) ? &g->p1_hand_size : &g->p2_hand_size;
    Card(*slots)[3] = (player == 1) ? g->p1_slots : g->p2_slots;
    float *balance = (player == 1) ? &g->p1_balance : &g->p2_balance;
    int *ranks_complete = (player == 1) ? &g->p1_completed_ranks : &g->p2_completed_ranks;
    bool *done_placing = (player == 1) ? &g->p1_done_placing : &g->p2_done_placing;

    if (*done_placing) return;
    if (g->mode == MODE_AIVSAI && player == 2 && !g->p1_ai_done_placing_moves) return;

    g->ai_timer += GetFrameTime();

    if (g->ai_timer > AI_MOVE_DELAY) {
        g->ai_timer = 0;
        float mult = GetRewardMultiplier(*ranks_complete);
        float placement_reward = REWARD_PLACEMENT * mult;

        for (int i = 0; i < *hand_size; i++) {
            Card c = hand[i];
            if (c.suit != SUIT_HEARTS && c.rank != RANK_JOKER) {
                for (int k = 0; k < KEYCARDS; k++) {
                    if (c.rank == g->keycards[k].rank) {
                        int cards_before = 0;
                        for (int s_check = 0; s_check < 3; s_check++)
                            if (slots[k][s_check].is_valid) cards_before++;
                        if (cards_before == 3) continue;

                        for (int s = 0; s < 3; s++) {
                            if (!slots[k][s].is_valid) {
                                slots[k][s] = c;
                                *balance += placement_reward;
                                CheckRankCompletionBonus(g, player, k, cards_before);

                                for (int j = i; j < *hand_size - 1; j++)
                                    hand[j] = hand[j + 1];
                                (*hand_size)--;
                                hand[*hand_size] = DrawFromDeck(g);
                                (*hand_size)++;
                                return;
                            }
                        }
                    }
                }
            }
        }
        *done_placing = true;
        if (player == 1) g->p1_ai_done_placing_moves = true;
    }
}