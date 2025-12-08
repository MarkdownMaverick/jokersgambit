# Joker's Gambit  
**A fast-paced, high-stakes 2-player card game built with raylib**

![screenshot placeholder]

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

## Known Limitations / Planned Improvements  
- Single-screen PvP means both players can see each other’s hands (intentional for now – local hot-seat style).  
- AI is functional but fairly basic (prioritises Jokers → low cards → useful placements).  
- No sound / music yet.  
- No online multiplayer (obviously).  
- No settings screen beyond the basic menu (volume, card backs, etc.).

```
