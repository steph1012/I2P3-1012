#ifndef __STATE_H__
#define __STATE_H__

#include <string>
#include <cstdlib>
#include <vector>
#include <utility>
#include <unordered_map>

#include "../config.hpp"


typedef std::pair<size_t, size_t> Point;
typedef std::pair<Point, Point> Move;
class Board{
  public:
    char board[2][BOARD_H][BOARD_W] = {{
      //white
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {1, 1, 1, 1, 1},
      {2, 3, 4, 5, 6},
    }, {
      //black
      {6, 5, 4, 3, 2},
      {1, 1, 1, 1, 1},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
    }};
};

enum GameState {
  UNKNOWN = 0,
  WIN,
  DRAW,
  NONE
};


class State{
  public:
    //You may want to add more property for a state
    GameState game_state = UNKNOWN;
    Board board;
    int player = 0;
    std::vector<Move> legal_actions;
    
    State(){};
    State(int player): player(player){};
    State(Board board): board(board){};
    State(Board board, int player): board(board), player(player){};
    
    int evaluate();
    // int calculateMaterial(int player);
    // int getPieceValue(int piece);
    int minimax(State* state, int depth, bool maxmizingPlayer);
    int alphabeta(State* state, int depth, int alpha, int beta, bool maximizingPlayer);
    int sortMoves(State* state, std::vector<Move>& moves);
    int evaluatePiece(int piece);
    bool isCaptureMove(const Move& move);
    void storeTransposition(int depth, int score);
    bool retrieveTransposition(int depth, int& score);
    State* next_state(Move move);
    void get_legal_actions();
    std::string encode_output();
    std::string encode_state();
    std::unordered_map<uint64_t, std::pair<int, int>> transpositionTable;
};

#endif