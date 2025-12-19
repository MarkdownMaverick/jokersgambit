#include "main.h"
#include "gui.h"
#include "aibots.h"
#include "mainmenu.h"
#include "useraccount.h"
#define GAME_WIDTH 1900
#define GAME_HEIGHT 1080
#define MAX_SCALE 1.0f
float scale = 1.0f;
Vector2 offset = {0, 0};
Texture2D g_background_texture = {0};
Texture2D g_ui_frame_texture = {0};
Texture2D g_button_texture = {0};
Sound g_discard_sound = {0};
Sound g_place_sound = {0};
Sound g_filled_rank_sound = {0};
Sound g_reveal_sound = {0};
Sound g_win_sound = {0};
Sound g_joker_sound = {0};
Sound g_matching_jokers_sound = {0};
Sound g_matching_cards_sound = {0};
Sound g_continue_sound = {0};
Sound g_coin_sound = {0};
Sound g_shuffle_sound = {0};
Music g_background_music = {0};
GameState g_initial_state = {0};
float GetRewardMultiplier(int completed_ranks)
{
    if (completed_ranks >= 2)
        return 4.0f;
    if (completed_ranks == 1)
        return 2.0f;
    return 1.0f;
}
Card BlankCard(void)
{
    Card c = {0};
    c.is_valid = false;
    c.texture = (Texture2D){0};
    return c;
}
static void BuildFilename(Card *c)
{

    char rank_str[4] = {0};
    char suit_char = ' ';
    switch (c->rank)
    {
    case RANK_2:
        strcpy(rank_str, "2");
        break;
    case RANK_3:
        strcpy(rank_str, "3");
        break;
    case RANK_4:
        strcpy(rank_str, "4");
        break;
    case RANK_5:
        strcpy(rank_str, "5");
        break;
    case RANK_6:
        strcpy(rank_str, "6");
        break;
    case RANK_7:
        strcpy(rank_str, "7");
        break;
    case RANK_8:
        strcpy(rank_str, "8");
        break;
    case RANK_9:
        strcpy(rank_str, "9");
        break;
    case RANK_10:
        strcpy(rank_str, "10");
        break;
    case RANK_JACK:
        strcpy(rank_str, "J");
        break;
    case RANK_QUEEN:
        strcpy(rank_str, "Q");
        break;
    case RANK_KING:
        strcpy(rank_str, "K");
        break;
    case RANK_ACE:
        strcpy(rank_str, "A");
        break;
    case RANK_JOKER:
    case RANK_BACK:
    case RANK_NONE:
    default:
        strcpy(rank_str, "ERR");
        break;
    }
    switch (c->suit)
    {
    case SUIT_CLUBS:
        suit_char = 'C';
        break;
    case SUIT_DIAMONDS:
        suit_char = 'D';
        break;
    case SUIT_HEARTS:
        suit_char = 'H';
        break;
    case SUIT_SPADES:
        suit_char = 'S';
        break;
    case SUIT_NONE:
        suit_char = 'X';
        break;
    default:
        suit_char = 'X';
        break;
    }
    sprintf(c->filename, "%s%c.png", rank_str, suit_char);
}
bool IsPlayerAI(const GameState *g, int player)
{
    int account_idx = (player == 1) ? g->p1_account_index : g->p2_account_index;
    if (account_idx >= 0 && account_idx < g->account_count)
    {
        return g->accounts[account_idx].is_ai;
    }
    return false;
}

static bool IsRankCompleted(const Card slots[3])
{
    int count = 0;
    for (int i = 0; i < 3; i++)
        if (slots[i].is_valid)
            count++;
    return count == 3;
}
void CheckRankCompletionBonus(GameState *g, int player, int key_idx, int cards_before)
{
    Card(*slots)[3] = (player == 1) ? g->p1_slots : g->p2_slots;
    int cards_after = 0;
    for (int s = 0; s < 3; s++)
    {
        if (slots[key_idx][s].is_valid)
            cards_after++;
    }
    if (cards_after == 3 && cards_before < 3)
    {
        float *balance = (player == 1) ? &g->p1_balance : &g->p2_balance;
        int *completed = (player == 1) ? &g->p1_completed_ranks : &g->p2_completed_ranks;
        float mult = GetRewardMultiplier(*completed);
        *balance += REWARD_COMPLETION * mult;
        (*completed)++;
        PlaySound(g_filled_rank_sound);
    }
}
Card DrawFromDeck(GameState *g)
{
    if (g->top_card_index < 0)
        return BlankCard();
    Card c = g->deck[g->top_card_index--];
    g->current_deck_size--;
    return c;
}
void ReturnToDeck(GameState *g, Card c)
{
    if (!c.is_valid || g->current_deck_size >= TOTAL_DECK_CARDS)
        return;
    g->top_card_index++; // 1. Place the card at the current top of the deck
    g->deck[g->top_card_index] = c;
    g->current_deck_size++;
    // ALWAYS shuffle the returned card into a random position instead of only a 20% chance.
    if (g->top_card_index > 0)
    {
        int r = rand() % (g->top_card_index + 1);
        Card t = g->deck[g->top_card_index];
        g->deck[g->top_card_index] = g->deck[r];
        g->deck[r] = t;
    }
}
static void TriggerSweep(GameState *g, int player)
{
    for (int k = 0; k < KEYCARDS; k++)
    {
        if (player == 0 || player == 1)
        {
            if (!IsRankCompleted(g->p1_slots[k]))
            {
                for (int s = 0; s < 3; s++)
                    if (g->p1_slots[k][s].is_valid)
                    {
                        ReturnToDeck(g, g->p1_slots[k][s]);
                        g->p1_slots[k][s] = BlankCard();
                    }
            }
        }
        if (player == 0 || player == 2)
        {
            if (!IsRankCompleted(g->p2_slots[k]))
            {
                for (int s = 0; s < 3; s++)
                    if (g->p2_slots[k][s].is_valid)
                    {
                        ReturnToDeck(g, g->p2_slots[k][s]);
                        g->p2_slots[k][s] = BlankCard();
                    }
            }
        }
    }
}
static void JokersGambit(GameState *g)
{
    TriggerSweep(g, 0);
    for (int i = 0; i < g->p1_hand_size; i++)
        ReturnToDeck(g, g->player1_hand[i]);
    for (int i = 0; i < g->p2_hand_size; i++)
        ReturnToDeck(g, g->player2_hand[i]);
    for (int i = 0; i < HAND_SIZE; i++)
    {
        g->player1_hand[i] = DrawFromDeck(g);
        g->player2_hand[i] = DrawFromDeck(g);
    }
    g->p1_hand_size = g->p2_hand_size = HAND_SIZE;
}
void ResolveDiscards(GameState *g)
{
    // First, process any pending discards from the PREVIOUS round
    ProcessPendingDiscards(g);
    Card d1 = g->revealed_p1;
    Card d2 = g->revealed_p2;
    bool j1 = (d1.rank == RANK_JOKER);
    bool j2 = (d2.rank == RANK_JOKER);
    g->p1_balance -= COST_DISCARD;
    g->p2_balance -= COST_DISCARD;
    float mult1 = GetRewardMultiplier(g->p1_completed_ranks);
    float mult2 = GetRewardMultiplier(g->p2_completed_ranks);
    if (j1 && j2)
    {
        JokersGambit(g);
        g->p1_balance -= JOKER_DISCARD;
        g->p2_balance -= JOKER_DISCARD;
        g->p1_balance += (REWARD_DOUBLE_JOKER * mult1);
        g->p2_balance += (REWARD_DOUBLE_JOKER * mult2);
        // Store these discards to be returned NEXT round
        g->delay_discard_p1 = d1;
        g->delay_discard_p2 = d2;
        g->discards_pending = true;
        PlaySound(g_matching_jokers_sound);
    }
    else if (j1 || j2)
    {
        int opp = j1 ? 2 : 1;
        TriggerSweep(g, opp);
        if (j1)
            g->p1_balance -= JOKER_DISCARD;
        if (j2)
            g->p2_balance -= JOKER_DISCARD;
        g->player1_hand[g->p1_hand_size++] = DrawFromDeck(g);
        g->player2_hand[g->p2_hand_size++] = DrawFromDeck(g);
        // Store these discards to be returned NEXT round
        g->delay_discard_p1 = d1;
        g->delay_discard_p2 = d2;
        g->discards_pending = true;
        PlaySound(g_joker_sound);
    }
    else if (d1.rank == d2.rank)
    {
        TriggerSweep(g, 0);
        g->p1_balance += (REWARD_MATCH * mult1);
        g->p2_balance += (REWARD_MATCH * mult2);
        g->player1_hand[g->p1_hand_size++] = DrawFromDeck(g);
        g->player2_hand[g->p2_hand_size++] = DrawFromDeck(g);
        // Store these discards to be returned NEXT round
        g->delay_discard_p1 = d1;
        g->delay_discard_p2 = d2;
        g->discards_pending = true;
        PlaySound(g_matching_cards_sound);
    }
    else
    {
        // Normal discard - no special effect
        g->player1_hand[g->p1_hand_size++] = DrawFromDeck(g);
        g->player2_hand[g->p2_hand_size++] = DrawFromDeck(g);
        // Store these discards to be returned NEXT round
        g->delay_discard_p1 = d1;
        g->delay_discard_p2 = d2;
        g->discards_pending = true;
    }
    g->p1_done_placing = false;
    g->p2_done_placing = false;
    g->total_rounds++;
}
void RefreshHands(GameState *g)
{
    for (int i = 0; i < g->p1_hand_size; i++)
        ReturnToDeck(g, g->player1_hand[i]);
    for (int i = 0; i < g->p2_hand_size; i++)
        ReturnToDeck(g, g->player2_hand[i]);
    for (int i = 0; i < HAND_SIZE; i++)
    {
        g->player1_hand[i] = DrawFromDeck(g);
        g->player2_hand[i] = DrawFromDeck(g);
    }
    g->p1_hand_size = g->p2_hand_size = HAND_SIZE;
}
float CalculateFinalScore(float balance, int total_rounds, bool is_winner)
{
    if (total_rounds <= 0)
        return balance;
    if (is_winner)
    {
        // Winner gets: current balance + (rounds × $10.00)
        float winner_bonus = (float)REWARD_MATCH * (float)total_rounds;
        return balance + winner_bonus;
    }
    else
    {
        // Loser gets: current balance - $100.00
        float loser_penalty = 100.0f;
        return balance - loser_penalty;
    }
}
void UpdateWinStats(GameState *g)
{
    g->p1_completed_ranks = 0;
    g->p2_completed_ranks = 0;
    for (int k = 0; k < KEYCARDS; k++)
    {
        if (IsRankCompleted(g->p1_slots[k]))
            g->p1_completed_ranks++;
        if (IsRankCompleted(g->p2_slots[k]))
            g->p2_completed_ranks++;
    }
}
void AddLeaderboardEntry(GameState *g, int winner)
{
    if (g->leaderboard_count >= MAX_LEADERBOARD_ENTRIES)
    {
        // Remove the lowest scoring entry to make room
        // First, sort by total_winnings to find the worst entry
        int worst_idx = 0;
        float worst_score = g->leaderboard[0].total_winnings;
        for (int i = 1; i < g->leaderboard_count; i++)
        {
            if (g->leaderboard[i].total_winnings < worst_score)
            {
                worst_score = g->leaderboard[i].total_winnings;
                worst_idx = i;
            }
        }
        // Shift entries to remove the worst one
        for (int i = worst_idx; i < MAX_LEADERBOARD_ENTRIES - 1; i++)
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
        snprintf(e->entry_name, 64, "%s_vs_%s", p1_name_str, p2_name_str);
    e->final_balance = (winner == 1) ? g->p1_balance : g->p2_balance;
    float final_score_winner = CalculateFinalScore(e->final_balance, g->total_rounds, true);
    e->total_winnings = final_score_winner;
    e->bonus = e->total_winnings - e->final_balance;
    e->total_rounds = g->total_rounds;
    snprintf(e->winner_name, 32, "%s", (winner == 1) ? p1_name_str : p2_name_str);
    time_t timer;
    time(&timer);
    struct tm *tm_info = localtime(&timer);
    strftime(e->timestamp, 32, "%m/%d/%Y", tm_info); // Capital Y for 4-digit year
    g->leaderboard_count++;
    // Sort the leaderboard after adding the new entry
    // This ensures it's always in the correct order
    // Sort by highest winnings (default view)
    for (int i = 0; i < g->leaderboard_count - 1; i++)
    {
        for (int j = i + 1; j < g->leaderboard_count; j++)
        {
            if (g->leaderboard[j].total_winnings > g->leaderboard[i].total_winnings)
            {
                LeaderboardEntry temp = g->leaderboard[i];
                g->leaderboard[i] = g->leaderboard[j];
                g->leaderboard[j] = temp;
            }
        }
    }
    SaveLeaderboard(g);
}
void ProcessPendingDiscards(GameState *g)
{
    if (g->discards_pending)
    {
        // This is to prevent predictable deck ordering
        if (rand() % 2 == 0)
        {
            if (g->delay_discard_p1.is_valid)
            {
                ReturnToDeck(g, g->delay_discard_p1);
                g->delay_discard_p1 = BlankCard();
            }
            if (g->delay_discard_p2.is_valid)
            {
                ReturnToDeck(g, g->delay_discard_p2);
                g->delay_discard_p2 = BlankCard();
            }
        }
        else
        {
            if (g->delay_discard_p2.is_valid)
            {
                ReturnToDeck(g, g->delay_discard_p2);
                g->delay_discard_p2 = BlankCard();
            }
            if (g->delay_discard_p1.is_valid)
            {
                ReturnToDeck(g, g->delay_discard_p1);
                g->delay_discard_p1 = BlankCard();
            }
        }
        g->discards_pending = false;
    }
}
// main.c - Fixed InitGame function to preserve ai_move_delay
void InitGame(GameState *g)
{
    Account saved_accounts[MAX_ACCOUNTS];
    LeaderboardEntry saved_leaderboard[MAX_LEADERBOARD_ENTRIES];
    int saved_account_count = g->account_count;
    int saved_p1_idx = g->p1_account_index;
    int saved_p2_idx = g->p2_account_index;
    GameMode saved_mode = g->mode;
    int saved_leaderboard_count = g->leaderboard_count;
    AIType saved_opponent_ai = g->selected_opponent_ai;
    bool saved_cover = g->cover_p2_cards;
    bool saved_sort = g->leaderboard_sort_by_rounds;
    float saved_ai_delay = g->ai_move_delay;
    memcpy(saved_accounts, g->accounts, sizeof(saved_accounts));
    memcpy(saved_leaderboard, g->leaderboard, sizeof(saved_leaderboard));
    *g = (GameState){0};
    memcpy(g->accounts, saved_accounts, sizeof(saved_accounts));
    memcpy(g->leaderboard, saved_leaderboard, sizeof(saved_leaderboard));
    g->account_count = saved_account_count;
    g->p1_account_index = saved_p1_idx;
    g->p2_account_index = saved_p2_idx;
    g->mode = saved_mode;
    g->leaderboard_count = saved_leaderboard_count;
    g->selected_opponent_ai = saved_opponent_ai;
    g->cover_p2_cards = saved_cover;
    g->leaderboard_sort_by_rounds = saved_sort;
    g->ai_move_delay = saved_ai_delay;
    Rank keys[5] = {RANK_ACE, RANK_KING, RANK_QUEEN, RANK_JACK, RANK_10};
    int idx = 0;
    for (int s = 0; s < 4; s++)
    {
        for (int r = 0; r < 13; r++)
        {
            // Skip the 5 Hearts keycards (10, J, Q, K, A)
            bool is_key = (s == SUIT_HEARTS &&
                           (r == RANK_10 || r == RANK_JACK || r == RANK_QUEEN ||
                            r == RANK_KING || r == RANK_ACE));
            if (!is_key)
            {
                g->deck[idx].rank = (Rank)r;
                g->deck[idx].suit = (Suit)s;
                g->deck[idx].is_valid = true;          // Card is valid
                g->deck[idx].texture = (Texture2D){0}; // Not used with atlas
                BuildFilename(&g->deck[idx]);          // Set filename for joker detection
                idx++;
            }
        }
    }
    // Add the 2 Jokers
    for (int i = 0; i < 2; i++)
    {
        g->deck[idx].rank = RANK_JOKER;
        g->deck[idx].suit = SUIT_NONE;
        g->deck[idx].is_valid = true;
        g->deck[idx].texture = (Texture2D){0};
        snprintf(g->deck[idx].filename, 16, i == 0 ? "JA.png" : "JB.png");
        idx++;
    }
    g->current_deck_size = idx;
    g->top_card_index = idx - 1;
    // Initialize the 5 Hearts keycards
    for (int i = 0; i < KEYCARDS; i++)
    {
        g->keycards[i].rank = keys[i];
        g->keycards[i].suit = SUIT_HEARTS;
        g->keycards[i].is_valid = true;
        g->keycards[i].texture = (Texture2D){0};
        BuildFilename(&g->keycards[i]);
    }
    // Shuffle the deck
    for (int i = g->top_card_index; i > 0; i--)
    {
        int j = rand() % (i + 1);
        Card t = g->deck[i];
        g->deck[i] = g->deck[j];
        g->deck[j] = t;
    }
    // Deal initial hands
    for (int i = 0; i < HAND_SIZE; i++)
    {
        g->player1_hand[i] = DrawFromDeck(g);
        g->player2_hand[i] = DrawFromDeck(g);
    }
    g->p1_hand_size = g->p2_hand_size = HAND_SIZE;
    if (g->p1_account_index != -1)
    {
        g->p1_balance = (float)g->accounts[g->p1_account_index].balance;
    }
    else
    {
        g->p1_balance = DEFAULT_BALANCE;
    }
    if (g->p2_account_index != -1)
    {
        g->p2_balance = (float)g->accounts[g->p2_account_index].balance;
    }
    else if (g->mode == MODE_PVAI)
    {
        for (int i = 0; i < g->account_count; i++)
        {
            if (g->accounts[i].is_ai && g->accounts[i].ai_type == g->selected_opponent_ai)
            {
                g->p2_balance = (float)g->accounts[i].balance;
                g->p2_account_index = i;
                g->accounts[i].is_logged_in = true;
                break;
            }
        }
        if (g->p2_account_index == -1)
        {
            g->p2_balance = DEFAULT_BALANCE;
        }
    }
    else
    {
        g->p2_balance = DEFAULT_BALANCE;
    }
    g->delay_discard_p1 = BlankCard();
    g->delay_discard_p2 = BlankCard();
    g->discards_pending = false;
    g->state = STATE_ROUND_START;
    g->state = STATE_P1_SELECT_DISCARD;
    g->p1_completed_ranks = g->p2_completed_ranks = 0;
    g->total_rounds = 0;
    g->placement_phases_count = 0;
    g->p1_done_placing = g->p2_done_placing = false;
    g->game_over = false;
    g->p1_selected = g->p2_selected = false;
    g->p1_discard_ready = false;
    g->p2_discard_ready = false;
}
void RestartGameKeepingAccounts(GameState *g)
{
    InitGame(g);
    g->p1_balance = (g->p1_account_index != -1) ? (float)g->accounts[g->p1_account_index].balance : DEFAULT_BALANCE;
    g->p2_balance = (g->p2_account_index != -1) ? (float)g->accounts[g->p2_account_index].balance : DEFAULT_BALANCE;
}
void UpdateScale()
{
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    float scaleX = (float)screenW / GAME_WIDTH;
    float scaleY = (float)screenH / GAME_HEIGHT;
    scale = fminf(fmaxf(scaleX, scaleY), MAX_SCALE); // Fills screen, may crop
    offset.x = ((float)screenW - (GAME_WIDTH * scale)) / 2.0f;
    offset.y = ((float)screenH - (GAME_HEIGHT * scale)) / 2.0f;
}
int main(void)
{
    GameState g = {0}; // Initialize all members to zero/NULL
    g.ai_move_delay = 2.0f;
    srand((unsigned)time(NULL));
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_W, SCREEN_H, "JOKERS GAMBIT");
    SetTargetFPS(60);
    InitAudioDevice();
    LoadCardAtlas();
    g_discard_sound = LoadSound("sfx/discard.wav");
    g_filled_rank_sound = LoadSound("sfx/filledrank.wav");
    g_win_sound = LoadSound("sfx/win.wav");
    g_place_sound = LoadSound("sfx/place.wav");
    g_reveal_sound = LoadSound("sfx/reveal.wav");
    g_joker_sound = LoadSound("sfx/onejoker.wav");
    g_matching_jokers_sound = LoadSound("sfx/twojokers.wav");
    g_matching_cards_sound = LoadSound("sfx/matchingcards.wav");
    g_continue_sound = LoadSound("sfx/continue.wav");
    g_coin_sound = LoadSound("sfx/coin.wav");
    g_shuffle_sound = LoadSound("sfx/shuffle.wav");
    g_background_music = LoadMusicStream("sfx/track.mp3");
    SetMusicVolume(g_background_music, 0.3f);
    PlayMusicStream(g_background_music);
    SetSoundVolume(g_discard_sound, 0.5f);
    SetSoundVolume(g_filled_rank_sound, 0.7f);
    g_background_texture = LoadTexture("assets/background.png");
    g_ui_frame_texture = LoadTexture("assets/frame.png");
    g_button_texture = LoadTexture("assets/button.png");
    InitAccounts(&g);
    LoadAllAccounts(&g);
    LoadLeaderboard(&g);
    InitGame(&g);
    g.state = STATE_MAIN_MENU;
    g.account_status_message[0] = '\0';
    g.account_status_timer = 0.0;
    while (!WindowShouldClose())
    {
        UpdateMusicStream(g_background_music);
        Vector2 mouse = GetMousePosition();
        mouse.x = (mouse.x - offset.x) / scale;
        mouse.y = (mouse.y - offset.y) / scale;
        Rectangle menu_btn_rect = {40, 20, 300, 70};
        Rectangle restart_btn_rect = {SCREEN_W - 340, 20, 300, 70};
        bool game_active = (g.state >= STATE_P1_SELECT_DISCARD && g.state <= STATE_CHECK_WIN);
        // In Game Menu/Restart button handling
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (CheckCollisionPointRec(mouse, menu_btn_rect) &&
                (game_active))
            {
                if (game_active)
                {
                    if (g.p1_account_index != -1)
                        g.accounts[g.p1_account_index].balance = (double)g.p1_balance;
                    if (g.p2_account_index != -1)
                        g.accounts[g.p2_account_index].balance = (double)g.p2_balance;
                    SaveAllAccounts(&g);
                }
                g.state = STATE_MAIN_MENU;
            }
            // Restart button - accessible during game or game over
            else if (CheckCollisionPointRec(mouse, restart_btn_rect) && (game_active || g.state == STATE_GAME_OVER))
            {
                if (g.p1_account_index != -1)
                    g.accounts[g.p1_account_index].balance = (double)g.p1_balance;
                if (g.p2_account_index != -1)
                    g.accounts[g.p2_account_index].balance = (double)g.p2_balance;
                SaveAllAccounts(&g);
                // Trigger game restart
                RestartGameKeepingAccounts(&g);
                g.state = STATE_P1_SELECT_DISCARD;
            }
            // Leaderboard-specific handling
            else if (g.state == STATE_LEADERBOARD)
            {
                Rectangle sort_btn = {CENTER_X + 400, 130, 200, 50};
                if (CheckCollisionPointRec(mouse, sort_btn))
                {
                    // Toggle sort mode
                    g.leaderboard_sort_by_rounds = !g.leaderboard_sort_by_rounds;
                }
                else if (!CheckCollisionPointRec(mouse, menu_btn_rect))
                {
                    // Click anywhere else (except menu button) to return
                    g.state = STATE_MAIN_MENU;
                }
            }
            // Game Over screen buttons
            else if (g.state == STATE_GAME_OVER)
            {
                Rectangle restart_btn = {CENTER_X - 450, SCREEN_H - 150, 280, 80};
                Rectangle menu_btn = {CENTER_X - 140, SCREEN_H - 150, 280, 80};
                Rectangle quit_btn = {CENTER_X + 170, SCREEN_H - 150, 280, 80};
                if (CheckCollisionPointRec(mouse, restart_btn))
                {
                    RestartGameKeepingAccounts(&g);
                    g.state = STATE_P1_SELECT_DISCARD;
                }
                else if (CheckCollisionPointRec(mouse, menu_btn))
                {
                    g.state = STATE_MAIN_MENU;
                }
                else if (CheckCollisionPointRec(mouse, quit_btn))
                {
                    break;
                }
            }
        }
        // State updates (outside the mouse button press check)
        if (g.state == STATE_MAIN_MENU)
        {
            UpdateMainMenu(&g, mouse);
        }
        else if (g.state == STATE_SETTINGS)
        {
            UpdateSettings(&g, mouse);
        }
        else if (g.state == STATE_ACCOUNTS_MANAGER)
        {
            UpdateAccountsManager(&g, mouse);
        }
        else if (g.state == STATE_REVEAL_AND_RESOLVE)
        {
            g.revealed_p1 = g.player1_hand[g.p1_discard_idx];
            g.revealed_p2 = g.player2_hand[g.p2_discard_idx];
            g.p1_discard_ready = false; // CRITICAL: Reset ready flags
            g.p2_discard_ready = false;
            for (int i = g.p1_discard_idx; i < g.p1_hand_size - 1; i++)
                g.player1_hand[i] = g.player1_hand[i + 1];
            g.p1_hand_size--;
            for (int i = g.p2_discard_idx; i < g.p2_hand_size - 1; i++)
                g.player2_hand[i] = g.player2_hand[i + 1];
            g.p2_hand_size--;
            ResolveDiscards(&g);
            g.state = STATE_WAIT_FOR_TURN;
            g.ai_timer = 0;
            g.Reshuffle_cover_timer = 0; // RESET P2 AI TIMER HERE FOR SIMULTANEOUS MOVES
        }
        else if (g.state == STATE_HAND_RESHUFFLE)
        {
            g.placement_phases_count++;
            if (g.placement_phases_count % 5 == 0)
            {
                g.state = STATE_COVER_ANIMATION;
            }
            else
            {
                g.revealed_p1 = BlankCard();
                g.revealed_p2 = BlankCard();
                g.state = STATE_CHECK_WIN;
            }
        }
        else if (g.state == STATE_COVER_ANIMATION)
        {
            Rectangle animSource = GetDealingAnimationRect();
            PlaySound(g_shuffle_sound);

            // Draw cards flying or shimmering for both players
            for (int i = 0; i < 5; i++)
            {
                // Calculate a "fly-in" position based on the timer
                float progress = 1.0f - (g.Reshuffle_cover_timer / 2.0f);
                Rectangle dest = HandRect(1, i);
                dest.y -= (1.0f - progress) * 500; // Fly up from bottom
                PlaySound(g_coin_sound);

                DrawTexturePro(g_atlas_texture, animSource, dest, (Vector2){0, 0}, 0.0f, WHITE);

                g.Reshuffle_cover_timer -= GetFrameTime();
                if (g.Reshuffle_cover_timer <= 0)
                {
                    RefreshHands(&g);
                    g.revealed_p1 = BlankCard();
                    g.revealed_p2 = BlankCard();
                    g.state = STATE_CHECK_WIN;
                }
            }
        }
        else if (g.state == STATE_CHECK_WIN)
        {
            UpdateWinStats(&g);
            if (g.p1_completed_ranks >= 3 || g.p2_completed_ranks >= 3)
            {
                g.winner = (g.p1_completed_ranks >= 3) ? 1 : 2;
                g.game_over = true;
                g.state = STATE_GAME_OVER;
                g.win_timer_start = GetTime();
                g.final_score_p1 = CalculateFinalScore(g.p1_balance, g.total_rounds, g.winner == 1);
                g.final_score_p2 = CalculateFinalScore(g.p2_balance, g.total_rounds, g.winner == 2);
                g.p1_balance = g.final_score_p1;
                g.p2_balance = g.final_score_p2;
                UpdateAccountBalances(&g);
                AddLeaderboardEntry(&g, g.winner);
                PlaySound(g_win_sound);
            }
            else
            {
                g.revealed_p1 = BlankCard();
                g.revealed_p2 = BlankCard();
                g.state = STATE_P1_SELECT_DISCARD;
            }
        }
        else if (g.state == STATE_GAME_OVER)
        {
            if (IsKeyPressed(KEY_R))
            {
                RestartGameKeepingAccounts(&g);
                g.state = STATE_P1_SELECT_DISCARD;
            }
        }
        if (game_active)
        {
            if (g.state == STATE_P1_SELECT_DISCARD) // DISCARD PHASE - Both players select simultaneously
            {
                if (IsPlayerAI(&g, 1) && !g.p1_discard_ready)
                {
                    AI_SelectDiscard(&g, 1);
                    g.p1_discard_ready = true;
                }
                else if (!IsPlayerAI(&g, 1) && !g.p1_discard_ready) // Human P1 - click handled below
                {
                    for (int i = 0; i < g.p1_hand_size; i++)
                    {
                        Rectangle btn_rect = ButtonRect(1, i);
                        if (CheckCollisionPointRec(mouse, btn_rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                        {
                            g.p1_discard_idx = i;
                            g.p1_discard_ready = true;
                            PlaySound(g_discard_sound);
                            break;
                        }
                    }
                }
                if (IsPlayerAI(&g, 2) && !g.p2_discard_ready) // P2 Discard
                {
                    AI_SelectDiscard(&g, 2);
                    g.p2_discard_ready = true;
                }
                else if (!IsPlayerAI(&g, 2) && !g.p2_discard_ready)
                {
                    // Human P2
                    for (int i = 0; i < g.p2_hand_size; i++)
                    {
                        Rectangle btn_rect = ButtonRect(2, i);
                        if (CheckCollisionPointRec(mouse, btn_rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                        {
                            g.p2_discard_idx = i;
                            g.p2_discard_ready = true;
                            PlaySound(g_discard_sound);
                            break;
                        }
                    }
                }
                // When both ready, advance to reveal
                if (g.p1_discard_ready && g.p2_discard_ready)
                {
                    g.state = STATE_REVEAL_AND_RESOLVE;
                }
            }
            // ================================================================
            // PLACEMENT PHASE
            // ================================================================
            if (g.state == STATE_WAIT_FOR_TURN)
            {
                // AI Placement Updates
                if (IsPlayerAI(&g, 1) && !g.p1_done_placing)
                {
                    AI_UpdatePlacementPhase(&g, 1);
                }
                if (IsPlayerAI(&g, 2) && !g.p2_done_placing)
                {
                    // For AIVSAI, allow independent updating
                    AI_UpdatePlacementPhase(&g, 2);
                }
                // Check if both done
                if (g.p1_done_placing && g.p2_done_placing)
                {
                    g.state = STATE_HAND_RESHUFFLE;
                    g.p1_ai_done_placing_rounds = false;
                }
                // Human Placement & Continue Buttons
                float mult1 = GetRewardMultiplier(g.p1_completed_ranks);
                float mult2 = GetRewardMultiplier(g.p2_completed_ranks);
                float reward1 = REWARD_PLACEMENT * mult1;
                float reward2 = REWARD_PLACEMENT * mult2;
                // P1 Human Card Placement
                if (!IsPlayerAI(&g, 1) && !g.p1_done_placing)
                {
                    for (int i = 0; i < g.p1_hand_size; i++)
                    {
                        Rectangle card_rect = HandRect(1, i);
                        if (CheckCollisionPointRec(mouse, card_rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                        {
                            Card c = g.player1_hand[i];
                            if (c.suit != SUIT_HEARTS && c.rank != RANK_JOKER)
                            {
                                for (int k = 0; k < KEYCARDS; k++)
                                {
                                    if (c.rank == g.keycards[k].rank)
                                    {
                                        int cards_before = 0;
                                        for (int s_check = 0; s_check < 3; s_check++)
                                            if (g.p1_slots[k][s_check].is_valid)
                                                cards_before++;
                                        if (cards_before < 3)
                                        {
                                            for (int s = 0; s < 3; s++)
                                            {
                                                if (!g.p1_slots[k][s].is_valid)
                                                {
                                                    g.p1_slots[k][s] = c;
                                                    g.p1_balance += reward1;
                                                    CheckRankCompletionBonus(&g, 1, k, cards_before);
                                                    for (int j = i; j < g.p1_hand_size - 1; j++)
                                                        g.player1_hand[j] = g.player1_hand[j + 1];
                                                    g.p1_hand_size--;
                                                    g.player1_hand[g.p1_hand_size++] = DrawFromDeck(&g);
                                                    PlaySound(g_place_sound);
                                                    break;
                                                }
                                            }
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                if (!IsPlayerAI(&g, 2) && !g.p2_done_placing) // P2 Human Card Placement
                {
                    for (int i = 0; i < g.p2_hand_size; i++)
                    {
                        Rectangle card_rect = HandRect(2, i);
                        if (CheckCollisionPointRec(mouse, card_rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                        {
                            Card c = g.player2_hand[i];
                            if (c.suit != SUIT_HEARTS && c.rank != RANK_JOKER)
                            {
                                for (int k = 0; k < KEYCARDS; k++)
                                {
                                    if (c.rank == g.keycards[k].rank)
                                    {
                                        int cards_before = 0;
                                        for (int s_check = 0; s_check < 3; s_check++)
                                            if (g.p2_slots[k][s_check].is_valid)
                                                cards_before++;
                                        if (cards_before < 3)
                                        {
                                            for (int s = 0; s < 3; s++)
                                            {
                                                if (!g.p2_slots[k][s].is_valid)
                                                {
                                                    g.p2_slots[k][s] = c;
                                                    g.p2_balance += reward2;
                                                    CheckRankCompletionBonus(&g, 2, k, cards_before);
                                                    for (int j = i; j < g.p2_hand_size - 1; j++)
                                                        g.player2_hand[j] = g.player2_hand[j + 1];
                                                    g.p2_hand_size--;
                                                    g.player2_hand[g.p2_hand_size++] = DrawFromDeck(&g);
                                                    PlaySound(g_place_sound);
                                                    break;
                                                }
                                            }
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                bool p1_continue = IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1);
                bool p2_continue = IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2);
                // P1 Continue Button Click
                if (!IsPlayerAI(&g, 1) && !g.p1_done_placing)
                {
                    Rectangle btn = ContinueButtonRect(1);
                    if (CheckCollisionPointRec(mouse, btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                        p1_continue = true;
                }
                // P2 Continue Button Click
                if (!IsPlayerAI(&g, 2) && !g.p2_done_placing)
                {
                    Rectangle btn = ContinueButtonRect(2);
                    if (CheckCollisionPointRec(mouse, btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                        p2_continue = true;
                }
                // Apply Continue Actions
                if (p1_continue && !g.p1_done_placing && !IsPlayerAI(&g, 1))
                {
                    g.p1_done_placing = true;
                    g.p1_balance -= COST_PER_ROUND; // Removed magic number
                    PlaySound(g_coin_sound);
                }
                if (p2_continue && !g.p2_done_placing && !IsPlayerAI(&g, 2))
                {
                    g.p2_done_placing = true;
                    g.p2_balance -= COST_PER_ROUND;
                    PlaySound(g_coin_sound);
                }
            }
        }
        BeginDrawing();
        ClearBackground(BLACK); // Black letterbox
        BeginMode2D((Camera2D){
            .offset = offset,
            .target = {0, 0},
            .rotation = 0.0f,
            .zoom = scale});
        if (g_background_texture.id)
            DrawTexture(g_background_texture, 0, 0, RAYWHITE);
        else
            ClearBackground(RAYWHITE);
        if (g.state == STATE_MAIN_MENU)
        {
            DrawMainMenu(&g);
        }
        else if (g.state == STATE_SETTINGS)
        {
            DrawSettings(&g);
        }
        else if (g.state == STATE_ACCOUNTS_MANAGER)
        {
            DrawAccountsManager(&g);
        }
        else if (g.state == STATE_LEADERBOARD)
        {
            DrawLeaderboard(&g);
        }
        else if (g.state == STATE_GAME_OVER)
        {
            DrawGameOver(&g);
        }
        else
        {
            DrawGameLayout(&g);
            // Draw Main Menu Button (Top Left)
            bool menu_hover = CheckCollisionPointRec(mouse, menu_btn_rect);
            if (g_button_texture.id != 0)
            {
                DrawTexturePro(g_button_texture, (Rectangle){0, 0, (float)g_button_texture.width, (float)g_button_texture.height},
                               menu_btn_rect, (Vector2){0, 0}, 0, menu_hover ? GOLD : WHITE);
            }
            else
            {
                DrawRectangleRec(menu_btn_rect, menu_hover ? GOLD : GRAY);
            }
            DrawText("MAIN MENU", (int)(menu_btn_rect.x + 60), (int)(menu_btn_rect.y + 20), 30, BLACK);
            // Draw Restart Button (Top Right)
            bool restart_hover = CheckCollisionPointRec(mouse, restart_btn_rect);
            if (g_button_texture.id != 0)
            {
                DrawTexturePro(g_button_texture, (Rectangle){0, 0, (float)g_button_texture.width, (float)g_button_texture.height},
                               restart_btn_rect, (Vector2){0, 0}, 0, restart_hover ? GOLD : WHITE);
            }
            else
            {
                DrawRectangleRec(restart_btn_rect, restart_hover ? GOLD : GRAY);
            }
            DrawText("RESTART", (int)(restart_btn_rect.x + 85), (int)(restart_btn_rect.y + 20), 30, BLACK);
            // Check if game is currently in an active state or game over for shortcuts
            bool game_active_or_over = (g.state >= STATE_P1_SELECT_DISCARD && g.state <= STATE_CHECK_WIN) || (g.state == STATE_GAME_OVER);
            if (IsKeyPressed(KEY_COMMA))
            {
                if (g.state >= STATE_P1_SELECT_DISCARD && g.state <= STATE_CHECK_WIN)
                {
                    // Save balances before leaving active game
                    if (g.p1_account_index != -1)
                        g.accounts[g.p1_account_index].balance = (double)g.p1_balance;
                    if (g.p2_account_index != -1)
                        g.accounts[g.p2_account_index].balance = (double)g.p2_balance;
                    SaveAllAccounts(&g);
                }
                g.state = STATE_MAIN_MENU;
            }
            // Shortcut for Restart (Period key)
            if (IsKeyPressed(KEY_PERIOD) && game_active_or_over)
            {
                // Save current balances
                if (g.p1_account_index != -1)
                    g.accounts[g.p1_account_index].balance = (double)g.p1_balance;
                if (g.p2_account_index != -1)
                    g.accounts[g.p2_account_index].balance = (double)g.p2_balance;
                SaveAllAccounts(&g);
                // Trigger game restart
                RestartGameKeepingAccounts(&g);
                g.state = STATE_P1_SELECT_DISCARD;
            }
        }

        EndMode2D();
        EndDrawing();
        UpdateScale();
    }
    UnloadTexture(g_atlas_texture);
    UnloadTexture(g_background_texture);
    UnloadTexture(g_ui_frame_texture);
    UnloadTexture(g_button_texture);
    UnloadSound(g_discard_sound);
    UnloadSound(g_filled_rank_sound);
    UnloadSound(g_win_sound);
    UnloadSound(g_place_sound);
    UnloadSound(g_reveal_sound);
    UnloadSound(g_joker_sound);
    UnloadSound(g_matching_jokers_sound);
    UnloadSound(g_matching_cards_sound);
    UnloadSound(g_continue_sound);
    UnloadSound(g_coin_sound);
    UnloadSound(g_shuffle_sound);
    UnloadMusicStream(g_background_music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}