# Joker's Gambit  
**A fast-paced, high-stakes 2-player card game built with raylib**

![Screenshot 1: Game Interface](screenshot1.png)
![Screenshot 2: Game Board](screenshot2.png)
![Screenshot 3: Gameplay Rules](screenshot3.png)

## Game Mechanics Overview

**Game Info:**  
PC card game built in C with raylib. Supports PvP, Player vs AI, AI vs AI, player accounts, and leaderboards.

**Objective:**  
First to complete **3 out of 5 ranks** (10s, Jacks, Queens, Kings, Aces) wins.  
Complete a rank by filling **all 3 slots** (clubs, diamonds, spades) on your side of the **Hearts keycard**.

**Core Ranks:** 10, J, Q, K, A  

**Key Rule: Filled Ranks**  
- Once a rank is **filled** (all 3 suits placed), it is **permanent** and immune to sweeps/disruptions.

## Core Rules & Flow  

### 1. The Board  
- **Five fixed Hearts keycards** in the center: A♥, K♥, Q♥, J♥, 10♥ (top to bottom).  
- Each player has **3 slots per rank** on their side (Player 1 left, Player 2 right).  
- Place matching rank (non-Hearts, non-Joker) cards in empty slots to build.

### 2. Deck & Hands  
- **47 standard cards** (full deck minus 5 Hearts keycards).  
- **+2 Jokers** (JA.png & JB.png).  
- **Total: 49 cards.**  
- Both players start with **5-card hands**.  
- **Every 3 placement phases**: Hands fully refreshed (returned to deck + new 5-card draw).

### 3. Round Structure  
**a) Discard Phase**  
- Each player **secretly discards 1 card** (click button under card).  

**b) Reveal & Resolve**  
| Discarded Cards | Effect | Rewards/Penalties | Other |
|-----------------|--------|-------------------|-------|
| **Same rank (pair)** | **All incomplete ranks swept** (both sides; cards back to deck). | +$1 each (× rank bonus multiplier) | Both draw 1 card. |
| **One Joker** | **Opponent’s entire board swept**. | Joker player: **-$1 penalty** | Both draw 1 card. |
| **Two Jokers** | **“JOKERS GAMBIT”** – Both boards + hands swept & redrawn (5 cards each). | Each: **-$1 penalty + $5 reward** (× multiplier) | - |
| **No match** | Normal play. | None | Both draw 1 card. |

**c) Placement Phase**  
- Players take turns (or simultaneous in PvP) placing valid cards into empty slots.  
  - **+$0.10 per placement** (× multiplier).  
  - **Big bonus** for completing a rank.  
- **Skip** if no useful cards (SPACE/button).  
- Both pass → End round.

### 4. Scoring & Economy  
*(Economy subject to tweaks!)*  
- **Multiplier**:  
  | Completed Ranks | Multiplier |  
  |-----------------|------------|  
  | 0               | ×1        |  
  | 1               | ×2        |  
  | 2+              | ×4        |  
- **Win Score**: `balance + speed bonus (fewer rounds = higher) - (half raw balance as risk penalty)`  
  → Rewards aggressive, efficient play.

### 5. Win Condition  
- First to **3 completed ranks** wins immediately.  
- Leaderboard entry: Score, bonuses, matchup, timestamp.

## Features Implemented  
- Full PvP / PvAI / AIvAI  
- Player name entry (≤10 chars)  
- Persistent top-10 leaderboard (survives restarts)  
- Victory & score screens  
- Economy system (rewards, multipliers, speed/risk)  
- Card textures & scaling  
- Responsive layout (optimized for 1900×1070; future updates planned)  
- AI: Understands Jokers, prioritizes placements  

## AI Behaviors  
**Defined in `aibots.c`, `useraccount.c`, `main.h`**  
AI accounts store `AIType` in `Account` struct.

| AI     | Discard Strategy                  |
|--------|-----------------------------------|
| **BOB** | Jokers first → low cards (2-9) → any |
| **THEA** | Fully random (`rand() % hand_size`) |
| **FLINT** | Never Jokers; prefers low (2-9)   |

## Known Limitations / Planned  
- Hot-seat PvP: Visible hands (intentional for local play).  
- Basic AI (improvements coming).  
- No sound/music.  
- No online multiplayer.  
- Limited keybinds/controller support.