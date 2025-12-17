
***

### Future Plans
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

#### Settings enhancements
- Adjustable AI decision speed (slow, medium, fast), unlocked via the shop or tied to account type (Basic: slow, Premium: medium, HighRoller: fast).  
- Music toggle with volume control (for `track.mp3` or future tracks).  
- JSON-based persistence for all settings so preferences are saved across sessions.

#### Profile pictures / avatars
- Each player account starts with one unique default avatar.  
- 5–10 additional avatars available for purchase.  
- Support for circular 80px avatars in the UI (profile, leaderboard, game screen).

#### New currency (Tokens)
- Premium currency separate from in-game dollars.  
- Sold in the shop at a rate of: 1 Token = $1,000 in-game dollars.  
- Players must have a positive dollar balance to make any shop purchase (including Tokens).  
- Tokens are used for premium shop items (exclusive decks, avatars, etc.).  
- Players earn 1 Token for each game win against the AI; there is no Token cost on a loss.  
- The AI earns 1 Token every time a full game is played (win or lose).  
- When the AI reaches 10 or more Tokens while playing against a human player, the accumulated Tokens are awarded to the player if they trigger a “Joker’s Gambit” (matching discard jokers) during gameplay.  
- AI vs. AI games are not affected by Tokens.

#### Shop system
- Dedicated shop screen accessible from the main menu.  
- Categories: Card Decks, Avatars, Insurance, Tokens, and future items.  
- Unlockables include:
  - Alternative card back designs and full deck themes.  
  - Additional 80px circular profile avatars.  
  - One-time-use Insurance item.  
  - Token purchases.  
  - Special items that require both a minimum dollar balance and a Token cost.  
- Example:  
  - Account: PLAYER ONE, W/L 23/3, Member status: Basic Account, Balance: $1,000,384.00, Tokens: 50.  
  - Because the player’s balance exceeds one million in-game dollars, they can upgrade their account at the cost of 50 Tokens and no additional dollars.

#### Deck switching
- New “Deck Selection” option in the Settings menu.  
- Players can equip any deck they own (default plus purchased).  
- In PvP, each player uses their own selected deck (P1 and P2 can have different visuals).  
- In PvAI or AIvAI, the human player’s selected deck is used on their side.

#### AI opponent unlocks via shop
- Bob is unlocked by default and has the lowest reward multiplier when beaten.  
- Thea and Flint are locked initially and can be purchased in the shop with in-game dollars or Tokens.  
- Thea: Moderate cost, medium reward multiplier.  
- Flint: Higher cost, highest reward multiplier.  
- Reward multipliers apply only to PvAI wins and have no effect in AI vs. AI or betting modes.

#### Insurance mechanic
- One-time-use item purchasable in the shop.  
- When used (via a new button on the account/profile screen), it instantly resets a negative dollar balance to $0.  
- No effect on zero or positive balances.  
- A visual indicator shows whether the logged-in player currently owns an Insurance item.  
- The item is consumed immediately upon use.



#### Betting feature on main menu

- New “Betting” button on the main menu.  
- Opens a betting lobby where the player selects a match type and places a wager using Tokens from their balance.  
- Initial mode: Bob vs. Bob exhibition match.  flint vs flint & thea vs thea (unlock in shop"Higher rewards")
- No effect on AI accounts or the main leaderboard.  
- Simple payout: 2x stake on win; lose the entire Token stake on loss.  
- The player chooses which side (P1 or P2) to bet on before the match starts.

- **Player vs. AI self-betting (shop unlock):**  
  - Unlockable betting mode where the player can bet on themselves in a match against the AI, using Tokens only.  
  - This is the most expensive betting unlock in the shop, costing more than any Bob vs. Bob, Thea vs. Thea, or Flint vs. Flint betting unlock.  
  - AI accounts and AI Token balances are never affected by betting outcomes.
#### Advanced betting modes (shop unlocks)
- Additional match types unlocked via the shop:
  - Thea vs. Thea matches (higher stakes/rewards).  
  - Flint vs. Flint matches (highest stakes/rewards, most expensive unlock).  
- Unlocked match types appear as extra options in the betting lobby.  
- Higher-tier matches offer increased potential payouts (for example, 2.5x–4x based on difficulty) while keeping the same win/lose stake risk.

#### Future considerations (after core shop/betting)
- Daily and weekly challenges with Token or dollar rewards.  
- Customizable UI themes.  
- Expanded leaderboard section including betting records.  
- Achievement system tied to unlocks, rewards, and total game wins.

#### Loopback PvP mode (planned)
- Purpose: Simulate true network PvP on a single PC for rapid testing of latency and sync issues without multiple devices, and to debug the full client–server flow during development.  
- Setup (after networking implementation, for example ENet + raylib):
  - Launch two instances of the game (run the executable twice).  
  - Player 1: Select “Host Game” → shows localhost IP (127.0.0.1) and a port (for example, 7777).  
  - Player 2: Select “Join Game” → enters “127.0.0.1:7777” to connect via loopback.  
  - Zero-latency local network simulation using the TCP/UDP loopback interface.  
- Game flow:
  - Identical to LAN PvP with fully synchronized turns, discards, placements, reveals, and sweeps.  
  - Each window shows one player’s view (own hand visible, opponent’s hidden).  
- Account and progression:
  - Each instance uses its own local account login.  
  - Full economy, decks, avatars, multipliers, and leaderboard updates apply independently.  
- Testing perks:
  - Debug tools: toggle artificial latency/packet loss, log network packets, verify sync state.  
  - Performance: runs smoothly on one machine, as loopback is the fastest possible network.  
  - Falls back to hotseat if the connection fails.  
- Implementation note:
  - Builds directly on LAN PvP code and reuses ENet/raylib networking patterns.  
  - One-time setup: integrate a networking library like ENet for reliable client–server communication, keeping it in separate files to avoid raylib/Windows.h conflicts.

**Pros over hotseat:** Exercises real networking code (desyncs, reconnects, error handling) without additional hardware.  
**When:** After LAN PvP is stable, as a bridge to full online multiplayer.