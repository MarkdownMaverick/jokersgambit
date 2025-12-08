#include "aibots.h"

void AI_SelectDiscard(GameState *g, int player) {
    Card *hand = (player == 1) ? g->player1_hand : g->player2_hand;
    int hand_size = (player == 1) ? g->p1_hand_size : g->p2_hand_size;
    int best_idx = 0;
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

    if (joker_idx != -1) best_idx = joker_idx;
    else if (low_card_idx != -1) best_idx = low_card_idx;
    else best_idx = 0;

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