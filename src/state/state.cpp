#include <iostream>
#include <sstream>
#include <cstdint>
#include <math.h>

#include "./state.hpp"
#include "../config.hpp"


int State::evaluatePiece(int piece) {
  const int pieceValues[7] = {0, 2, 6, 7, 8, 20, 0};  // Piece values: 0-empty, 1-pawn, 2-rook, 3-knight, 4-bishop, 5-queen, 6-king
  return pieceValues[piece];
}

/**
 * @brief evaluate the state
 * 
 * @return int 
 */
int State::evaluate() {
  int score = 0;
  auto self_board = this->board.board[this->player];
  auto oppn_board = this->board.board[1 - this->player];
  int self_kingsPosition, oppn_kingsPosition,self_pawnPos,oppen_pawnPos;
  // Evaluate self pieces
  for (int i = 0; i < BOARD_H; i++) {
    for (int j = 0; j < BOARD_W; j++) {
      int piece = self_board[i][j];
      if (piece == 6)
        self_kingsPosition = j;
      if (piece == 1)
        self_pawnPos == j;
      score += evaluatePiece(piece);
    }
  }
  // Evaluate opponent pieces
  for (int i = 0; i < BOARD_H; i++) {
    for (int j = 0; j < BOARD_W; j++) {
      int piece = oppn_board[i][j];
      if (piece == 6)
        oppn_kingsPosition = j;
      if (piece == 1)
        oppen_pawnPos = j;
      score -= evaluatePiece(piece);
    }
  }
  
  if (this->player == 0) {
    if (self_kingsPosition == 4 && oppn_kingsPosition != 0)
      score += 6;
    if (self_pawnPos >2 && oppen_pawnPos<=3)
      score += 10;
  } else if (this->player == 1) {
    if (self_kingsPosition == 0 && oppn_kingsPosition != 4)
      score += 6;
    if (self_pawnPos <2 && oppen_pawnPos>=3)
      score+=10;
  }

  return score;
}

int State::minimax(State* state, int depth, bool maximizingPlayer) {
  if (depth == 0) {
    return evaluate();
  }
  if (maximizingPlayer) {
    int maxEval = std::numeric_limits<int>::min();
    std::vector<Move> possibleMoves = state->legal_actions;
    for (const auto& move : possibleMoves) {
      State* next_state = state->next_state(move);
      int eval = minimax(next_state, depth - 1, false);
      maxEval = std::max(maxEval, eval);
      delete next_state;
    }
    return maxEval;
  } else {
    int minEval = std::numeric_limits<int>::max();
    std::vector<Move> possibleMoves = state->legal_actions;
    for (const auto& move : possibleMoves) {
      State* next_state = state->next_state(move);
      int eval = minimax(next_state, depth - 1, true);
      minEval = std::min(minEval, eval);
      delete next_state;
    }
    return minEval;
  }
}

int State::sortMoves(State* state, std::vector<Move>& moves) {
  // Sort moves based on capturing moves first
  std::sort(moves.begin(), moves.end(), [state](const Move& a, const Move& b) {
    bool isCaptureA = state->isCaptureMove(a);
    bool isCaptureB = state->isCaptureMove(b);

    if (isCaptureA && !isCaptureB) {
      return true;
    } else if (!isCaptureA && isCaptureB) {
      return false;
    }

    // If both moves are capturing moves or non-capturing moves, you can use other criteria
    // to further sort the moves, such as evaluation scores, piece mobility, or history scores

    // Add your additional sorting criteria here

    // If no specific sorting criteria, maintain the original order
    return false;
  });
  return moves.size();
}

bool State::isCaptureMove(const Move& move) {
  Point to = move.second;  // Extract the 'to' position from the move
  auto oppn_board = this->board.board[1 - this->player];  // Get the opponent's board
  return oppn_board[to.first][to.second] != 0;  // Check if the opponent has a piece at the 'to' position
}


int State::alphabeta(State* state, int depth, int alpha, int beta, bool maximizingPlayer) {
  if (depth == 0) {
    return evaluate();
  }
  if (maximizingPlayer) {
    int maxEval = std::numeric_limits<int>::min();
    std::vector<Move> possibleMoves = state->legal_actions;
    for (const auto& move : possibleMoves) {
      State* next_state = state->next_state(move);
      maxEval = std::max(maxEval,alphabeta(next_state,depth-1,alpha,beta,false));
      alpha = std::max(alpha,maxEval);
      delete next_state;
      if (alpha >= beta)
        break;
    }
    return maxEval;
  } else {
    int minEval = std::numeric_limits<int>::max();
    std::vector<Move> possibleMoves = state->legal_actions;
    for (const auto& move : possibleMoves) {
      State* next_state = state->next_state(move);
      minEval = std::min(minEval,alphabeta(next_state,depth-1,alpha,beta,true));
      beta = std::min(beta, minEval);
      delete next_state;
      if (beta<= alpha)
        break;
    }
    return minEval;
  }
}

// void State::storeTransposition(int depth, int score) {
//   uint64_t hash = std::hash<std::string>{}(encode_state());
//   transpositionTable[hash] = std::make_pair(depth, score);
// }

// bool State::retrieveTransposition(int depth, int& score) {
//   uint64_t hash = std::hash<std::string>{}(encode_state());
//   auto it = transpositionTable.find(hash);
//   if (it != transpositionTable.end() && it->second.first >= depth) {
//     score = it->second.second;
//     return true;
//   }
//   return false;
// }



/**
 * @brief return next state after the move
 * 
 * @param move 
 * @return State* 
 */
State* State::next_state(Move move){
  Board next = this->board;
  Point from = move.first, to = move.second;
  
  int8_t moved = next.board[this->player][from.first][from.second];
  //promotion for pawn
  if(moved == 1 && (to.first==BOARD_H-1 || to.first==0)){
    moved = 5;
  }
  if(next.board[1-this->player][to.first][to.second]){
    next.board[1-this->player][to.first][to.second] = 0;
  }
  
  next.board[this->player][from.first][from.second] = 0;
  next.board[this->player][to.first][to.second] = moved;
  
  State* next_state = new State(next, 1-this->player);
  
  if(this->game_state != WIN)
    next_state->get_legal_actions();
  return next_state;
}


static const int move_table_rook_bishop[8][7][2] = {
  {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}},
  {{0, -1}, {0, -2}, {0, -3}, {0, -4}, {0, -5}, {0, -6}, {0, -7}},
  {{1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}},
  {{-1, 0}, {-2, 0}, {-3, 0}, {-4, 0}, {-5, 0}, {-6, 0}, {-7, 0}},
  {{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}},
  {{1, -1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}, {7, -7}},
  {{-1, 1}, {-2, 2}, {-3, 3}, {-4, 4}, {-5, 5}, {-6, 6}, {-7, 7}},
  {{-1, -1}, {-2, -2}, {-3, -3}, {-4, -4}, {-5, -5}, {-6, -6}, {-7, -7}},
};
static const int move_table_knight[8][2] = {
  {1, 2}, {1, -2},
  {-1, 2}, {-1, -2},
  {2, 1}, {2, -1},
  {-2, 1}, {-2, -1},
};
static const int move_table_king[8][2] = {
  {1, 0}, {0, 1}, {-1, 0}, {0, -1}, 
  {1, 1}, {1, -1}, {-1, 1}, {-1, -1},
};


/**
 * @brief get all legal actions of now state
 * 
 */
void State::get_legal_actions(){
  // [Optional]
  // This method is not very efficient
  // You can redesign it
  this->game_state = NONE;
  std::vector<Move> all_actions;
  auto self_board = this->board.board[this->player];
  auto oppn_board = this->board.board[1 - this->player];
  
  int now_piece, oppn_piece;
  for(int i=0; i<BOARD_H; i+=1){
    for(int j=0; j<BOARD_W; j+=1){
      if((now_piece=self_board[i][j])){
        // std::cout << this->player << "," << now_piece << ' ';
        switch (now_piece){
          case 1: //pawn
            if(this->player && i<BOARD_H-1){
              //black
              if(!oppn_board[i+1][j] && !self_board[i+1][j])
                all_actions.push_back(Move(Point(i, j), Point(i+1, j)));
              if(j<BOARD_W-1 && (oppn_piece=oppn_board[i+1][j+1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i+1, j+1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
              if(j>0 && (oppn_piece=oppn_board[i+1][j-1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i+1, j-1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
            }else if(!this->player && i>0){
              //white
              if(!oppn_board[i-1][j] && !self_board[i-1][j])
                all_actions.push_back(Move(Point(i, j), Point(i-1, j)));
              if(j<BOARD_W-1 && (oppn_piece=oppn_board[i-1][j+1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i-1, j+1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
              if(j>0 && (oppn_piece=oppn_board[i-1][j-1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i-1, j-1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
            }
            break;
          
          case 2: //rook
          case 4: //bishop
          case 5: //queen
            int st, end;
            switch (now_piece){
              case 2: st=0; end=4; break; //rook
              case 4: st=4; end=8; break; //bishop
              case 5: st=0; end=8; break; //queen
              default: st=0; end=-1;
            }
            for(int part=st; part<end; part+=1){
              auto move_list = move_table_rook_bishop[part];
              for(int k=0; k<std::max(BOARD_H, BOARD_W); k+=1){
                int p[2] = {move_list[k][0] + i, move_list[k][1] + j};
                
                if(p[0]>=BOARD_H || p[0]<0 || p[1]>=BOARD_W || p[1]<0) break;
                now_piece = self_board[p[0]][p[1]];
                if(now_piece) break;
                
                all_actions.push_back(Move(Point(i, j), Point(p[0], p[1])));
                
                oppn_piece = oppn_board[p[0]][p[1]];
                if(oppn_piece){
                  if(oppn_piece==6){
                    this->game_state = WIN;
                    this->legal_actions = all_actions;
                    return;
                  }else
                    break;
                };
              }
            }
            break;
          
          case 3: //knight
            for(auto move: move_table_knight){
              int x = move[0] + i;
              int y = move[1] + j;
              
              if(x>=BOARD_H || x<0 || y>=BOARD_W || y<0) continue;
              now_piece = self_board[x][y];
              if(now_piece) continue;
              all_actions.push_back(Move(Point(i, j), Point(x, y)));
              
              oppn_piece = oppn_board[x][y];
              if(oppn_piece==6){
                this->game_state = WIN;
                this->legal_actions = all_actions;
                return;
              }
            }
            break;
          
          case 6: //king
            for(auto move: move_table_king){
              int p[2] = {move[0] + i, move[1] + j};
              
              if(p[0]>=BOARD_H || p[0]<0 || p[1]>=BOARD_W || p[1]<0) continue;
              now_piece = self_board[p[0]][p[1]];
              if(now_piece) continue;
              
              all_actions.push_back(Move(Point(i, j), Point(p[0], p[1])));
              
              oppn_piece = oppn_board[p[0]][p[1]];
              if(oppn_piece==6){
                this->game_state = WIN;
                this->legal_actions = all_actions;
                return;
              }
            }
            break;
        }
      }
    }
  }
  std::cout << "\n";
  this->legal_actions = all_actions;
}


const char piece_table[2][7][5] = {
  {" ", "♙", "♖", "♘", "♗", "♕", "♔"},
  {" ", "♟", "♜", "♞", "♝", "♛", "♚"}
};
/**
 * @brief encode the output for command line output
 * 
 * @return std::string 
 */
std::string State::encode_output(){
  std::stringstream ss;
  int now_piece;
  for(int i=0; i<BOARD_H; i+=1){
    for(int j=0; j<BOARD_W; j+=1){
      if((now_piece = this->board.board[0][i][j])){
        ss << std::string(piece_table[0][now_piece]);
      }else if((now_piece = this->board.board[1][i][j])){
        ss << std::string(piece_table[1][now_piece]);
      }else{
        ss << " ";
      }
      ss << " ";
    }
    ss << "\n";
  }
  return ss.str();
}


/**
 * @brief encode the state to the format for player
 * 
 * @return std::string 
 */
std::string State::encode_state(){
  std::stringstream ss;
  ss << this->player;
  ss << "\n";
  for(int pl=0; pl<2; pl+=1){
    for(int i=0; i<BOARD_H; i+=1){
      for(int j=0; j<BOARD_W; j+=1){
        ss << int(this->board.board[pl][i][j]);
        ss << " ";
      }
      ss << "\n";
    }
    ss << "\n";
  }
  return ss.str();
}