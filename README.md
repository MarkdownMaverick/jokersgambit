
# Joker's Gambit v4.50
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
| **One Joker** | Sweep opponent's board | Joker player: -$1, both draw 1 |
| **Two Jokers** | "JOKERS GAMBIT" - sweep both boards + hands | Each: -$1 + $5 (× multiplier) |
| **No match** | Normal play | Both draw 1 |

- **+$0.10 per placement** (× multiplier)
- **Big bonus** for rank completion
- **Filled ranks are permanent** (immune to sweeps)
- **Discards**: Discarded cards randomly positioned back in deck after Delay

### Scoring
**Multiplier:** 0 ranks = ×1 | 1 rank = ×2 | 2+ ranks = ×4
**Win Score:** `balance + speed bonus - (balance/2 risk penalty)`

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
- **BOB:** Jokers first → low cards → any
- **THEA:** Fully random
- **FLINT:** Never Jokers, prefers low cards


---

# Issues im working on 

- **App Start:** No accounts logged in. "Play New Game" redirects to accounts screen unless two accounts are already logged in.

- **Accounts Screen:** There are two player slots for created player accounts. Players can create new accounts, and player accounts are deletable. The 3 AI opponents (BOB, THEA, FLINT) are already created and cannot be modified by the player.

- **Account Selection:** Players can choose between BOB, THEA, FLINT, PLAYER1, and PLAYER2. Login is handled by pressing the button on the corresponding account to set which "Seat" the account sits at (P1 or P2).

- **Matchup Rules:** No Player or AI should be able to play against themselves (this would break accounts).

- **AI Positioning:** If any AI (BOB, THEA, FLINT) is logged into the P1 seat and a player chooses to sit at P2, the AI logic should allow any position. All AI should be able to sit at seat P1 or P2.

- **Discard Phase:** Both players/AIs discard 1 card in any order. When both players or AI have discarded their cards, the game continues to the REVEAL phase. (Discarded cards are held until the next discard phase starts, then returned to a random place in the deck.)

- **Reveal Phase:** Wait for both players to discard, then Resolve (check for match, non-match, or one Joker; apply rewards/costs).

- **Placement Phase:** Both players/AIs can place ANY suits of matching ranks (A, K, Q, J, 10) from their hands onto their keycard ranks. Placement is done in any order. Players or AI can press a new UI button underneath their current discard buttons.

- **Continue Buttons:** New buttons for P1 & P2 seats labeled "Continue" (h60p w100). When players or AI are done in the placement phase, this button only needs to be pressed once by each player or AI, then wait for the other player or AI to continue. mapping of p1 & p2 continue buttons for p1 continue the "1" key for p2 continue the "2" key

- **Animation/ ui** the temp.png cards are displaying in the wrong position. The gamescreen has functional menu & restart buttons just not visible  
---

## Flow Summary

1. Both players or AI must discard in any order and press their own button once when ready.
2. Players or AI regain control at the placement phase. Players can choose some, all, or no rank cards from their hand to add to the board. Both press "Continue" once when ready (or when out of rank cards for AI).
3. This brings us back to the discard phase.

---

**Note:** There is currently logic to handle the first discard in PvP with a blank, but not for AI. this is by design.

--- **endof Joker's Gambit v4.50**
// fixes applied to most issues testing in progress.




