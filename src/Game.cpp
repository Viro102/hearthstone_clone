#include "Game.h"

Game::Game() {
        players = new Player[2];
        panel = new Panel();
        selectedCard = nullptr;
        turnCounter = 0;
        new Frame(panel);
    }

void Game::startGame(string player1, string player2) {
        players[0] = new Player(PLAYER_MAX_HP, 0, player1);
        players[1] = new Player(PLAYER_MAX_HP, 1, player2);
        players[0]setTurn(true);
        players[0]setMana(1);
        for (int i = 0; i < 3; i++) {
            players[0]drawCard();
            players[1]drawCard();
        }
        panel.update();
        Logging->printStateAll(players);
    }

void Game::endTurn() {
        if (turnCounter >= 30) {
            JOptionPaneshowMessageDialog(panel, "Game over, no one won");
            System.exit(0);
        }
        auto currentPlayer = getOnTurnPlayer();
        auto offPlayer = getOffTurnPlayer();

        currentPlayer.setTurn(false);
        offPlayer.setTurn(true);
        offPlayer.setMana(offPlayer.getCurrentMaxMana() + 1);
        offPlayer.drawCard();

        for (auto card : currentPlayer.getBoard().getCards()) {
            if (card != nullptr) {
                card.setHasAttacked(false);
            }
        }
        if (selectedCard != nullptr) {
            selectedCard = nullptr;
        }
        panel.removeGlow();
        panel.update();
        turnCounter++;
        System.out.println("Turn ended");
        LoggingprintStateHand(players);
    }


void Game::playACard(Player player, int i) {
        auto card = player.playCard(i);
        if (card != nullptr) {
            specialCard(player, card);
            panel.update();
        }
    }

void Game::selectCardBoard(Player player, int i) {
        if (selectedCard == nullptr) {
            selectedCard = player.getBoard().getCard(i);
            panel.addGlow(i, "m_board");

        } else if (selectedCard == player.getBoard().getCard(i)) {
            selectedCard = nullptr;
            panel.removeGlow();
        } else {
            JOptionPaneshowMessageDialog(panel, "Can't select two m_cards at once");
            selectedCard = nullptr;
            panel.removeGlow();
        }
    }

void Game::selectCardHand(Player player, int i) {
        if (selectedCard == player.getHand().getCard(i)) {
            selectedCard = nullptr;
            panel.removeGlow();
        } else {
            selectedCard = player.getHand().getCard(i);
            panel.addGlow(i, "m_hand");
        }
    }

void Game::attack(int i) {
        if (selectedCard == nullptr) {
            System.out.println("No card selected");
            return;
        }
        panel.removeGlow();
        auto opponent = getOffTurnPlayer();
        auto currentPlayer = getOnTurnPlayer();
        auto targetCard = opponent.getBoard().getCard(i);

        if (!selectedCard.getHasAttacked()) {
            targetCardsetHp(targetCard.getHp() - selectedCard.getDamage());
            selectedCardsetHp(selectedCard.getHp() - targetCard.getDamage());
            selectedCard.setHasAttacked(true);
            if (targetCard.getHp() <= 0) {
                opponent.getBoard().removeCard(i);
            }
            if (selectedCard.getHp() <= 0) {
                currentPlayer.getBoard().removeCard(selectedCard);
            }
        } else {
            JOptionPaneshowMessageDialog(panel, "Card has already attacked!");
        }

        selectedCard = nullptr;
        panel.update();
    }

void Game::attackFace() {
        auto target = getOffTurnPlayer();
        auto attacker = getOnTurnPlayer();

        if (selectedCard == nullptr) {
            std::cout << "No card selected";
            return;
        }

        panel.removeGlow();

        for (auto c : target.getBoard().getCards()) {
            if (c == nullptr) {
                continue;
            }
            if (c.getType().equals("taunt")) {
                JOptionPaneshowMessageDialog(panel, "Taunt card in play, cannot attack hero");
                return;
            }
        }
        if (!selectedCard.getHasAttacked()) {
            target.setHp(target.getHp() - selectedCard.getDamage());
            selectedCard.setHasAttacked(true);
            if (selectedCard.getType()=="spell") {
                attacker.getBoard().removeCard(selectedCard);
            }
        } else {
            JOptionPaneshowMessageDialog(panel, "Card has already attacked!");
        }

        selectedCard = nullptr;
        panel.update();
        isGameOver();
    }

Player Game::getOnTurnPlayer() const{
        for (auto player : players) {
            if (player == nullptr) {
                continue;
            }
            if (player.isTurn()) {
                return player;
            }
        }
        System.out.println("Error");
        return nullptr;
    }

Player Game::getOffTurnPlayer() const{
        for (auto player : players) {
            if (player == nullptr) {
                continue;
            }
            if (!player.isTurn()) {
                return player;
            }
        }
        System.out.println("Error");
        return nullptr;
    }

array<Player, 2> Game::getPlayers() const{
        return players;
    }


bool Game::isSelected() const{
        return selectedCard != nullptr;
    }

void Game::isGameOver() const{
        if (players[0]getHp() <= 0) {
            JOptionPaneshowMessageDialog(panel, "Player 2 (" + players[1]getArchetype() + ") wins");
        } else if (players[1]getHp() <= 0) {
            JOptionPaneshowMessageDialog(panel, "Player 1 (" + players[0]getArchetype() + ") wins");
        }
    }

void Game::specialCard(Player player, Card card) {
        if (card.getType() == "buff") {
            int buffAmout = card.getBuffAmount();
            for (auto c : player.getBoard().getCards()) {
                if (c == nullptr) {
                    continue;
                }
                csetHp(c.getHp() + buffAmout);
                csetDamage(c.getDamage() + buffAmout);
            }
            return;
        }

        if (card.getType()=="spell") {
            getOnTurnPlayer().getHand().removeCard(card);
            selectedCard = card;
            return;
        }

        if (card.getType()=="aoe") {
            getOnTurnPlayer().getHand().removeCard(card);
            selectedCard = nullptr;
            for (auto target : getOffTurnPlayer().getBoard().getCards()) {
                if (target == nullptr) {
                    continue;
                }
                targetsetHp(target.getHp() - card.getDamage());
                if (target.getHp() <= 0) {
                    getOffTurnPlayer().getBoard().removeCard(target);
                }
            }
        }
    }