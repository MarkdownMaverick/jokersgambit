
# Joker's Gambit v4.51
<p align="center">
<img src="screenshot1.png" alt="Game Interface" width="30%" />
<img src="screenshot2.png" alt="Game Board" width="30%" />
<img src="screenshot3.png" alt="Gameplay Rules" width="30%" />
</p>

```markdown
## Game Mechanics

**Objective:** First to complete **3 out of 5 ranks** (10, J, Q, K, A) wins by filling all 3 suit slots (♣♦♠) on your side of each Hearts keycard.

### Board & Deck
- **5 Hearts keycards** (A♥, K♥, Q♥, J♥, 10♥) fixed center
- **3 slots per rank** per player (P1 left, P2 right)
- **49 cards total:** 47 standard (minus 5 Hearts) + 2 Jokers
- **5-card hands**, reshuffled every 5 placement phases (5 ROUNDS COMPLETED) (frequently changed to find balance)

### Round Structure

**1. Discard Phase:** Both players/Ais secretly discard 1 card in any order,( card held until next discard phase is started, then returned to a random place in the deck )

**2. Reveal & Resolve:**

| Discarded Cards | Effect | Rewards/Penalties(changed daily to find balance) |
|-----------------|--------|-------------------|
| **Same rank (pair)** | Sweep all incomplete ranks | +$1 each (× multiplier), both draw 1 |
| **One Joker** | Sweep opponent's board | Joker player: costs the placer of the joker -$5, both draw 1 |
| **Two Jokers** | "JOKERS GAMBIT" - sweep both boards + hands | + $50 each (× multiplier) |
| **No match** | Normal play | Both draw 1 |

- **+$1.00 per placement** (× multiplier)
- **Big bonus** for rank completion
- **Filled ranks are permanent** (immune to sweeps)
- **Discards**: Discarded cards randomly positioned back in deck after Delay

### Scoring
**Multiplier:** 0 ranks = ×1 | 1 rank = ×2 | 2+ ranks = ×4
**Win Score:** `balance + speed bonus - (balance/2 risk penalty) subject to frequent change to find balance`

---

## Features
✅ PvP / PvAI / AIvAI modes
✅ Player accounts & persistent data
✅ Top-100 leaderboard
✅ Economy system with multipliers
✅ 3 AI opponents (Bob, Thea, Flint)
✅ Sound effects & card textures
✅ P2 hand cover/uncover setting
✅ Window & ui scaling: centering at 1900×1080 max with black letterbox

### AI Behaviors
- **BOB:** Jokers first → low cards → any  (hard)
- **THEA:** Fully random  (medium)
- **FLINT:** Never Jokers, prefers low cards (easy)

## ✅ Resolved issues
- **App Start:** No accounts logged in. "Play New Game" redirects to accounts screen unless two accounts are already logged in.
- **Accounts Screen:** Two player slots for created accounts. Players can create/delete accounts. The 3 AI opponents (BOB, THEA, FLINT) are pre-created and non-modifiable.
- **Account Selection:** Players choose between BOB, THEA, FLINT, PLAYER1, PLAYER2. Login handled by pressing account button to set seat (P1 or P2).
- **Matchup Rules:** No Player or AI can play against themselves (prevents account conflicts).
- **AI Positioning:** AI can sit in either P1 or P2 seat with full logic support.
- **Discard Phase (PvP):** Both players discard 1 card in any order. Game proceeds to REVEAL phase when both complete. Cards held until next discard phase, then returned to random deck position.
- **Continue Buttons:** Keyboard mapping P1 = `1` key, P2 = `2` key. Press once when done → wait for other player/AI.
- **Return to menu & restart buttons** Added keybinds for restart = `.`   return to menu = `,`

---

# Issues I'm Working On


### High Priority

#### AI vs AI Placement Phase
P2 AI waits for P1 AI to place first instead of placing simultaneously. Both AIs should:
- Place cards at the same time during placement phase
- Press "Continue" once when done (no rank cards left to place)
- Wait for or join the other AI before proceeding


---

### Low Priority - Keyboard Navigation System

#### Main Menu Navigation
- **Arrow keys:** Navigate up/down through buttons (PLAY, ACCOUNTS, SETTINGS, LEADERBOARD)
- **Visual feedback:** Glowing edge around active/highlighted button
- **Selection:** Press `0` to activate highlighted button
- **Mouse support:** Existing click navigation continues working

#### Accounts Screen Navigation
- **Horizontal navigation:** 
  - Arrow right → LOGIN P2 → Arrow right → DELETE
- **Vertical navigation:** 
  - Arrow down from P1 → P2 slot in account below
- **Seamless 2D navigation:** 
  - Example: P1 highlighted → Right + Down = P2 slot in account below
- **Selection:** Press `0` to select highlighted account
- **Bottom buttons:** Arrow navigation highlights CREATE NEW or BACK buttons, press `0` to use
- **Mouse support:** Existing click navigation continues working

#### Leaderboard Navigation
- **Vertical scroll:** Up/Down arrows to scroll leaderboard
- **Sorting:** Left/Right arrows to toggle between:
  - Lowest rounds first
  - Highest cash total first
- **Return to menu:** New button with `,` keybind to return to main menu
- **Mouse support:** Existing click navigation continues working

#### Local PvP Navigation
- **Shared keyboard:** Highlighted buttons must navigate both players' hands and buttons (single keyboard setup)
- **Selection:** Press `0` to select highlighted discard or place buttons
- **Mouse support:** Existing click navigation continues working

---

### Implementation Notes

#### Phase Flow
1. **Reveal Phase:** Wait for both players to discard → Resolve (check for match/non-match/Joker) → Apply rewards/costs
2. **Placement Phase:** Both players/AIs place matching rank suits (A, K, Q, J, 10) in any order. Press "Continue" or keybind (P1 = `1`, P2 = `2`) when done.

#### Design Decisions
- First discard in PvP handles blank by design (no AI equivalent needed)


## Testing Status
- Fixes applied to most issues .v4.51 Testing in progress.

```