#include <cstdlib>

#include "../state/state.hpp"
#include "./alphabeta.hpp"
#include <climits>
#include <limits>


/**
 * @brief Randomly get a legal action
 * 
 * @param state Now state
 * @param depth You may need this for other policy
 * @return Move 
 */
Move Alphabeta::get_move(State *state, int depth){
  if(!state->legal_actions.size())
    state->get_legal_actions();
  
  auto actions = state->legal_actions;
  // Initialize variables for storing the best move and its evaluation score
  Move bestMove = actions[0];  // Start with the first move as the best move
  int bestScore = INT_MIN;     // Start with a very low score
  // Evaluate each move and find the move with the highest evaluation score
    int alpha = INT_MIN;
    int beta = INT_MAX;
  for (const auto& move : actions) {
    // Create a new state by applying the current move
    State* nextState = state->next_state(move);
    //how to make you strong
    // Call the minimax function to evaluate the new state
    depth = 5;
    int score = state->alphabeta(nextState, depth,alpha, beta,true);;  // Assuming alphbeta is implemented

    // Check if the current move has a higher score than the best move
    if (score > bestScore) {
      bestMove = move;
      bestScore = score;
    }
    // Clean up the dynamically allocated state
    delete nextState;
  }
  return bestMove;
}