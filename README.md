```markdown
# Joker's Gambit  
**A fast-paced, high-stakes 2-player card game built with raylib**

![Screenshot 1: Game Interface](screenshot1.png)
![Screenshot 2: Game Board](screenshot2.png)
![Screenshot 3: Gameplay Rules](screenshot3.png)

## Quick Overview  
Single-screen competitive card game for 1 or 2 human players (or watch AI vs AI).  
Goal: be the first to complete **3 out of 5** target ranks (A♡ K♡ Q♡ J♡ 10♡) by placing matching-rank cards from your hand into your personal slots.

First to 3 completed ranks wins and gets their score immortalised on the leaderboard.

## Game Modes  
- **PvP** – Two humans on the same computer  
- **PvAI** – One human vs a simple AI opponent  
- **AI vs AI** – Sit back and watch the bots battle (great for testing or just vibes)

## Core Rules & Flow  

1. **The Board**  
   - Five fixed **Keycards** in the centre column (always Hearts: A, K, Q, J, 10).  
   - Each player has **3 slots** on their side of each keycard (left = Player 1, right = Player 2).  
   - Fill all 3 slots of a rank → that rank is **completed**.

2. **Deck**  
   - 47 normal cards (standard deck **minus** the 5 Heart high cards which are used as keycards)  
   - + 2 Jokers (JA.png & JB.png)  
   → 49 cards total.

3. **Round Structure**  
   a) Both players start with **5 cards**.  
   b) **Discard Phase** – Each player secretly chooses one card from their hand to discard (click the button under the card).  
   c) **Reveal & Resolve** – Discarded cards are shown and effects trigger:  
      - **Same rank** → +$1 reward each (multiplied by current rank bonus) + both players draw a new card + **all incomplete ranks on BOTH sides are swept** (cards returned to deck).  
      - **One Joker** → Opponent’s entire board is swept + Joker player pays $1 penalty + both draw.  
      - **Two Jokers** → “Broken Hearts” – everything is swept, both hands are fully shuffled back and redrawn, each pays $1 penalty but also gets +$5 reward (multiplied).  
      - **No special case** → normal draw only.  
   d) **Placement Phase** – Players take turns (or simultaneously in PvP) placing **non-Heart, non-Joker** cards that match a keycard rank into empty slots.  
      - Each placement gives +$0.10 (multiplied by current completed-rank bonus).  
      - Completing a rank instantly awards a big completion bonus.  
      - You may **Skip** placement (SPACE or button) when you have nothing useful.  
   e) After both players pass → next round.

4. **Every 3 placement phases** the hands are fully refreshed (returned + new 5-card hands).

5. **Scoring & Economy**  
   - Base rewards/penalties as listed above.  
   - Reward multiplier:  
     - 0 completed ranks → ×1  
     - 1 completed rank   → ×2  
     - 2+ completed ranks → ×4  
   - **Final Score** when someone wins =  
     `balance + speed bonus (more moves = lower bonus) - half your raw balance as risk penalty`  
     → Encourages fast, aggressive play while still rewarding strong economy.
     ECONOMY WILL BE TWEAKED CONSTANTLY SO EXPECT Inconsistencies WITH THIS DOC!

6. **Win Condition**  
   First to **3 completed ranks** ends the game immediately. Winner goes to the leaderboard with final score, bonus breakdown, matchup, and timestamp.

## Controls  
- Mouse only (except name entry & skip)  
- Click the button under a card in your hand to select it for discard or placement (button text changes contextually)  
- SPACE or big “SKIP Placement” button to pass during placement phase  
- Top-left “MAIN MENU” / Top-right “RESTART” buttons any time  
- R or auto-restart after 10 seconds on victory screen

## Features Already Implemented  
- Full PvP / PvAI / AIvAI  
- Player name entry (up to 10 chars)  
- Persistent top-10 leaderboard (survives restarts)  
- Clean victory  & score display  
- Monetary rewards, multipliers, speed bonus, risk penalty  
- Card texture loading with proper scaling  
- Responsive layout for 1900×1100 (easily changeable)  
- AI that understands Joker power and prioritises useful placements

## Known Limitations / Planned Improvements  / Bugs
- Single-screen PvP means both players can see each other’s hands (intentional for now – local hot-seat style).  
- AI is functional but fairly basic (prioritises Jokers → low cards → useful placements).  
- No sound / music yet.  
- No online multiplayer (obviously).  
- No full keybinds yet / No xbox controller support.

---

# Joker's Gambit - Update Summary UPDATE V4.01 08/12/25

### ✅ 1. Settings Screen Implementation
**Location:** `mainmenu.c`, `mainmenu.h`, `main.h`, `main.c`

- **Added new `STATE_SETTINGS`** to game state enum
- **Created Settings Screen** with AI opponent selection for PvAI mode
- Three AI opponents to choose from:
  - **BOB** (original): Prefers Jokers, then low cards
  - **THEA** (new): Random discard strategy, no Joker preference
  - **FLINT** (new): Never discards Jokers, prefers low cards
- Settings accessible from main menu with purple "SETTINGS" button
- Selected AI opponent is saved in `g->selected_opponent_ai`

---

### ✅ 2. Three Distinct AI Behaviors
**Location:** `aibots.c`, `useraccount.c`, `main.h`

#### **BOB** (AI_BOB)
- Original behavior maintained
- Prioritizes discarding Jokers first
- Then targets low cards (2-9)
- Falls back to first card

#### **THEA** (AI_THEA)
- Completely random discard selection
- No special Joker handling
- Uses `rand() % hand_size`

#### **FLINT** (AI_FLINT)
- **Never** discards Jokers
- Prefers low cards (2-9)
- Only discards non-Joker cards
- AI accounts are created with their specific `AIType` stored in the `Account` struct

---

### ✅ 3. AI vs AI Account Management Fixed
**Location:** `mainmenu.c`, `main.c`

**Previous Bug:** Starting AI vs AI from a running PvAI game would continue the previous game state

**Fix:**
- When "AI vs AI" button is pressed:
  1. **Logout current accounts** from both player slots
  2. **Find and login FLINT** to Player 1
  3. **Find and login THEA** to Player 2
  4. Set mode to `MODE_AIVSAI`
  5. Call `RestartGameKeepingAccounts()` for clean game state

This ensures AI vs AI always starts fresh with the correct AI accounts.

---

### ✅ 4. Balance Persistence Fixed
**Location:** `main.c`, `useraccount.c`

**Previous Bugs:**
- AI balances reset to $10.00 after each AI vs AI match
- Returning to main menu mid-game lost balance changes
- Restart button didn't save progress

**Fixes:**

#### **On Game Win:**
```c
// Already working correctly
UpdateAccountBalances(g); // Saves winner/loser balances
```

#### **On Main Menu Button:**
```c
// NEW: Save balances before returning to menu
if (game_active) {
    if (g.p1_account_index != -1)
        g.accounts[g.p1_account_index].balance = (double)g.p1_balance;
    if (g.p2_account_index != -1)
        g.accounts[g.p2_account_index].balance = (double)g.p2_balance;
    SaveAllAccounts(&g);
}
```

#### **On Restart Button:**
```c
// NEW: Save balances before restarting
if (g.p1_account_index != -1)
    g.accounts[g.p1_account_index].balance = (double)g.p1_balance;
if (g.p2_account_index != -1)
    g.accounts[g.p2_account_index].balance = (double)g.p2_balance;
SaveAllAccounts(&g);
```

**Result:** All balance changes are now persisted correctly for both human and AI accounts.

---

### ✅ 5. Improved Account Manager UI
**Location:** `mainmenu.c`

**Changes:**
- **Separated Human and AI accounts** in the display
- Human accounts shown first with DELETE button
- AI accounts shown below with "AI ACCOUNTS:" header
- AI accounts display in **purple** instead of blue
- Shows wins/losses: `$100.00 | W:5 L:2`
- AI accounts **cannot be deleted** (no DELETE button)
- Click account to cycle: `None → P1 → P2 → None`

---

## 🔧 Technical Changes Summary

### New Enums & Types
```c
typedef enum {
    AI_BOB = 0,
    AI_THEA = 1,
    AI_FLINT = 2
} AIType;
```

### Updated Structs
```c
typedef struct Account {
    // ... existing fields ...
    AIType ai_type;  // NEW: Which AI behavior to use
} Account;

typedef struct GameState {
    // ... existing fields ...
    AIType selected_opponent_ai;  // NEW: Selected AI for PvAI
} GameState;
```

### New Functions
- `DrawSettings()` - Renders settings screen
- `UpdateSettings()` - Handles settings input
- `AI_Bob_SelectDiscard()` - BOB AI logic
- `AI_Thea_SelectDiscard()` - THEA AI logic
- `AI_Flint_SelectDiscard()` - FLINT AI logic

### Modified Functions
- `AI_SelectDiscard()` - Now routes to appropriate AI function
- `UpdateMainMenu()` - Added settings button handler
- `LoadAllAccounts()` - Reads AI type from accounts file
- `SaveAllAccounts()` - Preserves AI type in accounts file
- Main game loop - Added balance saving before menu/restart

---

## 🎮 User Experience Improvements

1. **Settings Menu**: Easy AI opponent selection with clear descriptions
2. **Visual Feedback**: Selected AI highlighted in lime green
3. **Persistent Balances**: All progress saved automatically
4. **Clean AI vs AI**: Always starts fresh with FLINT vs THEA
5. **Organized Accounts**: Human and AI accounts clearly separated
6. **Win/Loss Tracking**: Full statistics for all accounts

---

## 📝 Files Modified

1. `main.h` - Added AIType enum, updated structs
2. `main.c` - Fixed balance saving, integrated settings
3. `mainmenu.h` - Added settings function declarations
4. `mainmenu.c` - Implemented settings screen, fixed AI vs AI login
5. `useraccount.h` - Updated Account struct (in doc)
6. `useraccount.c` - Added AI type handling in load/save
7. `aibots.h` - No changes needed
8. `aibots.c` - Implemented three AI behaviors

---

## ✨ All Requested Features Completed

- ✅ Settings screen with AI selection
- ✅ Three distinct AI personalities (BOB, THEA, FLINT)
- ✅ AI vs AI mode fixed (FLINT vs THEA)
- ✅ Balance persistence for all scenarios
- ✅ Improved account manager UI
- ✅ Automatic AI account login for AI vs AI

# Joker's Gambit - End Of Update Summary UPDATE V4.01

---

## VERSION V4.01Bug Fix: Account Login/Logout System 09/12/2025

### Current Problem
- Players must click accounts multiple times to log out
- If two accounts exist, clicking Player 1's account logs them out BUT automatically logs Player 2 in
- Player must click again to fully log out of all accounts
- This creates an awkward, annoying user experience

### Solution: Separate Button Controls

**For Player-Created Accounts:**
Each account should display **6 buttons** in this pattern:
```
[LOGIN P1] [LOGOUT P1] [DELETE]
[LOGIN P2] [LOGOUT P2] [DELETE]
```

Continue this pattern for any additional player accounts created.

**For AI Accounts:**
Each AI account should display **2 buttons**:
- **Bob:** `[BOB P2 SLOT]` `[LOG BOB OUT]`
- **Flint:** `[FLINT P2 SLOT]` `[LOG FLINT OUT]`
- **Thea:** `[THEA P2 SLOT]` `[LOG THEA OUT]`

### Default State
**Bob should be logged into P2 slot by default**



Based on the provided files and your description of the desired UX overhaul, here is a review of the proposed "V4.01 Bug Fix" for the Account Manager system.

### 1. Diagnosis of Current Issue
**Location:** `mainmenu.c` (Functions: `DrawAccountsManager`, `UpdateAccountsManager`)

* **Current Behavior:** The code currently uses a single large collision rectangle (`box`) for the entire account row. Clicking this triggers a "cycle" logic: `None → P1 → P2 → None`.
* **The Friction:** As you noted, this is clumsy. If a user wants to swap P1 from "Account A" to "Account B", they currently have to click "Account A" multiple times to clear it, then click "Account B". If they misclick, they accidentally assign P2.
* **Conclusion:** The proposed explicit button system is the correct solution for a desktop/mouse-driven interface.

### 2. UI/UX Analysis & Implementation Strategy

You have plenty of screen width (`SCREEN_W 1900`) to accommodate the new button layout without cramping the text.

#### **A. Human Accounts Layout**
You requested a pattern of Login/Logout buttons. To keep the UI clean and prevent logical errors (like logging one person into both slots), I recommend **Context-Aware Buttons** rather than static buttons.

**Proposed Logic for `DrawAccountsManager`:**
Instead of always showing 6 buttons (which looks cluttered), change the buttons based on the account's state:

1.  **State: Account is Offline**
    * **Buttons:** `[P1 LOGIN]` `[P2 LOGIN]` `[DELETE]`
    * **Action:** Clicking `[P1 LOGIN]` calls `LoginAccount(..., 1)`.

2.  **State: Account is Logged in as P1**
    * **Buttons:** `[LOGOUT P1]` `[DELETE]` (P2 Login hidden or disabled)
    * **Action:** Clicking `[LOGOUT P1]` calls `LogoutAccount(..., 1)`.

3.  **State: Account is Logged in as P2**
    * **Buttons:** `[LOGOUT P2]` `[DELETE]` (P1 Login hidden or disabled)
    * **Action:** Clicking `[LOGOUT P2]` calls `LogoutAccount(..., 2)`.

*This satisfies your requirement for explicit control while preventing the user from trying to log an active P1 account into the P2 slot.*

#### **B. AI Accounts Layout**
**Location:** `mainmenu.c` (AI Loop section)

Your request: `[BOB P2 SLOT] [LOG BOB OUT]`
* **Constraint Check:** You mentioned "AI vs AI" mode uses P1 and P2 slots.
* **Refinement:** If the user manually selects an AI for the **P1** slot (to set up a custom AI vs AI match without using the main menu shortcut), the buttons should support that.
* **Recommendation:**
    * Standard View: `[ASSIGN P2]` (Since PvAI is the most common mode).
    * If `MODE_AIVSAI` is intended manually: Allow `[ASSIGN P1]` for AI rows as well.

### 3. Default State Implementation ("Bob as P2")
**Location:** `main.c` (`InitGame` or `LoadAllAccounts`)

To ensure Bob is logged into P2 by default:
1.  In `LoadAllAccounts` (in `useraccount.c`), after loading the file, check if `g->p2_account_index == -1`.
2.  If true, iterate through `g->accounts` to find the one where `first_name == "BOB"`.
3.  Set `p2_account_index` to Bob's index and set `is_logged_in = true`.

### 4. Technical Feasibility Review

* **Structs:** The `Account` struct and `GameState` struct in `main.h` already support everything needed. No data structure changes are required.
* **Functions:** `LoginAccount` and `LogoutAccount` in `useraccount.c` are robust enough to handle these explicit calls.
* **Drawing:** `mainmenu.c` will need the most work. You will replace the single `DrawRectangleRec` (the hitbox) with individual `DrawButton` calls (using the helper from `gui.c` or a local rectangle logic) positioned at specific X offsets relative to the account row `y`.

### 5. Potential Pitfalls to Watch For

1.  **Z-Fighting / Overlap:** Ensure the "Delete" button `CheckCollisionPointRec` is checked *before* the general row background if you keep a background hover effect.
2.  **Text Width:** Ensure long player names (12 chars) don't overlap with the new button array on the right side.
3.  **Default Override:** If the user explicitly logs Bob out, ensure `Update` doesn't auto-log him back in on the next frame. The "Default Bob" logic should only run on **Application Start** or **Game Reset**, not every frame.

**Verdict:** The plan fixes have been applied but its still in testing currently with a few issues
```

