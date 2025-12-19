# Joker's Gambit v4.55

<p align="center">
<img src="screenshot1.png" alt="Game Interface" width="30%" />
<img src="screenshot2.png" alt="Game Board" width="30%" />
<img src="screenshot3.png" alt="Gameplay Rules" width="30%" />
</p>

## Game Mechanics

**Objective:** First to complete **3 out of 5 ranks** (10, J, Q, K, A) wins by filling all 3 suit slots (♣♦♠) on your side of each Hearts keycard.

### Board & Deck
- **5 Hearts keycards** (A♥, K♥, Q♥, J♥, 10♥) fixed in center
- **3 slots per rank** per player (P1 left, P2 right)
- **49 cards total:** 47 standard (minus 5 Hearts keycards) + 2 Jokers
- **5-card hands**, reshuffled every 5 placement phases

### Round Structure

**1. Discard Phase:** Both players secretly select and discard 1 card simultaneously. Discarded cards are held until the next discard phase, then shuffled randomly back into the deck.

**2. Reveal & Resolve:**

| Discarded Cards | Effect | Rewards/Penalties |
|-----------------|--------|-------------------|
| **Same rank (pair)** | Sweep all incomplete ranks (both players) | +$ each (× multiplier), both draw 1 |
| **One Joker** | Sweep opponent's incomplete ranks | Joker user: -$ penalty, both draw 1 |
| **Two Jokers** | "JOKERS GAMBIT" - sweep both boards + refresh hands | +$ each (× multiplier), |
| **No match** | Normal play continues | -$ discard cost each, both draw 1 |

**3. Placement Phase:** Place cards from your hand onto matching rank slots. Only cards matching keycard ranks (A, K, Q, J, 10) can be placed—no Hearts or Jokers. Press "PASS" button when done (costs -$ to pass).

### Scoring & Economy
Rewards and costs changes daily trying to find balance. 
**Placement Rewards:**
- **+$ per rank card placed** (× multiplier)
- **+$ rank completion bonus** (× multiplier)
- **-$ for new round** to pass/continue each placement phase

**Multiplier System:**
- 0 completed ranks = ×1
- 1 completed rank = ×2
- 2+ completed ranks = ×4

**Game End Payouts:**
- **Winner:** Current balance + (rounds played × $10)
- **Loser:** Current balance - $100

**Important:** Filled ranks (all 3 slots) are **permanent** and immune to sweeps!

### Keybinds

**In-Game:**
- **Restart:** `.` (period)
- **Main Menu:** `,` (comma)
- **P1 Pass:** `1` (number key)
- **P2 Pass:** `2` (number key)

**Other Screens:** Mouse/click only

---

## Features

✅ **Game Modes:** PvP, PvAI, AIvAI  
✅ **Account System:** Persistent player profiles with balance tracking  
✅ **Leaderboard:** Top 100 scores with dual sort modes (by cash/by rounds)  
✅ **3 AI Opponents:** BOB (Hard), THEA (Medium), FLINT (Easy)  
✅ **Audio & Visuals:** Sound effects, card textures, animated UI  
✅ **Settings:**
- P2 hand privacy toggle (local PvP/ PvAi)
- AI move speed adjustment (Fast/Medium/Slow)
✅ **Display:** 1900×1080 max resolution with scaling & letterbox support
✅ AI move speed setting (Fast/Medium/Slow) in settings menu  
✅ Leaderboard sorting (by highest cash or lowest rounds)  

### AI Behaviors

- **BOB (Hard):** Prioritizes Jokers → low cards → any
- **THEA (Medium):** Fully random decisions
- **FLINT (Easy):** Never discards Jokers, prefers low cards

---

## Account & Login System

**First Launch:** No accounts logged in. Pressing "PLAY" redirects to account manager.

**Account Manager:**
- **Player Slots:** P1 and P2 seats (left and right)
- **Pre-created AI:** BOB, THEA, FLINT (cannot be deleted)
- **Human Accounts:** Create custom player profiles (first + last name)
- **Login/Logout:** Click account buttons to assign players to P1/P2 slots

**Matchup Rules:**
- ✅ Any combination allowed (PvP, PvAI, AIvAI)
- ❌ Cannot play against yourself (same account in both slots)
- ✅ AI can occupy either P1 or P2 seat

---

## Mechanics

### Discard Phase (PvP)
Both players select discards **simultaneously in any order**. Game waits for both selections before revealing. Cards are held until the **next** discard phase, then returned to a random position in the deck (prevents predictable draws).

### AI Placement
AI opponents in both P1 and P2 seats now place cards **simultaneously** during placement phase (matches human player behavior).

### Continue/Pass System
Press "PASS" button (or keybind) when done placing cards. Costs -$5. Game proceeds when **both players** pass.

---
## Changes
For better performance and lower memory usage, I have converted to use a Texture Atlas (a single large image containing all card faces) and drawing specific regions of it using DrawTextureRec DECK0.png = Texture Atlas
DECK0.png 2600px X 1500px the cards are arranged 13 cards wide 5 cards tall
in the order 
// The atlas layout:
//( ROW 1 2H, 3H, 4H, 5H, 6H, 7H, 8H, 9H, 10H, JH, QH, KH, AH
// ROW 2 2C, 3C, 4C, 5C, 6C, 7C, 8C, 9C, 10C, JC, QC, KC, AC
// ROW 3 2D, 3D, 4D, 5D, 6D, 7D, 8D, 9D, 10D, JD, QD, KD, AD
// ROW 4 2S, 3S, 4S, 5S, 6S, 7S, 8S, 9S, 10S, JS, QS, KS, AS
// ROW 5 JA, JB, B0, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10 )  (The first two cards in this row are useable cards(the two jokers) , the B0-B10 are variations of the back of the cards )

- current state: compiling with no errors ,- 