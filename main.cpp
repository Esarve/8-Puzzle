#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <cmath>
#include <set>
#include <unordered_set>
#include <queue>
#include <chrono>
#include <iomanip>
#define n 3
const bool SUCCESS = true;
using namespace std;
using i64 = long long int;

class state {
public:
  int board[9], g, f;

  state* came_from;

  state () {
    g = 0;
    f = 0;
    came_from = NULL;
  }

  bool operator == (state otherState) const {
    for (int i = 0; i < 9; i++)
      if (this->board[i] != otherState.board[i])
        return false;
    return true;
  }

  bool operator < (const state& other) const {
    for (int i = 0; i < 9; i++) {
      if (this->board[i] < other.board[i]) return true;
      if (this->board[i] > other.board[i]) return false;
    }
    return false;
  }

  size_t hash() const {
    size_t h = 0;
    for (int i = 0; i < 9; i++) {
      h = h * 10 + board[i];
    }
    return h;
  }

  void print () {
    for (int i = 0; i < 9; i++) {
      cout << board[i] << " ";
      if ((i + 1) % 3 == 0)
        cout << endl;
    }
    cout << "g = " << g << " | f = " << f << endl;
  }
};

namespace std {
  template<>
  struct hash<state> {
    size_t operator()(const state& s) const {
      return s.hash();
    }
  };
}

vector <state> solutionPath;

struct CompareState {
  bool operator()(const state& a, const state& b) const {
    return a.f > b.f; // Min-heap based on f score
  }
};

void addNeighborState (state currentState, state goalState, int newPos, int emptyPos, 
                      priority_queue<state, vector<state>, CompareState>& openSet, 
                      unordered_set<state>& closedSet, function<int(state, state)> heuristic) {
  state neighborState = currentState;
  swap (neighborState.board[newPos], neighborState.board[emptyPos]);
  
  // Skip if already in closed set
  if (closedSet.find(neighborState) != closedSet.end()) {
    return;
  }
  
  neighborState.g = currentState.g + 1;
  neighborState.f = neighborState.g + heuristic(neighborState, goalState);
  state* parentState = new state();
  *parentState = currentState;
  neighborState.came_from = parentState;
  openSet.push(neighborState);
}

void generateNeighbors (state currentState, state goalState, 
                       priority_queue<state, vector<state>, CompareState>& openSet, 
                       unordered_set<state>& closedSet, function<int(state, state)> heuristic) {
  int emptyPos = -1;
  for (int i = 0; i < 9; i++)
    if (currentState.board[i] == 0) {
      emptyPos = i;
      break;
    }
  
  int row = emptyPos / 3;
  int col = emptyPos % 3;
  
  if (row - 1 >= 0)
    addNeighborState(currentState, goalState, emptyPos - 3, emptyPos, openSet, closedSet, heuristic);
  if (row + 1 < n)
    addNeighborState(currentState, goalState, emptyPos + 3, emptyPos, openSet, closedSet, heuristic);
  if (col + 1 < n)
    addNeighborState(currentState, goalState, emptyPos + 1, emptyPos, openSet, closedSet, heuristic);
  if (col - 1 >= 0)
    addNeighborState(currentState, goalState, emptyPos - 1, emptyPos, openSet, closedSet, heuristic);
}

bool reconstruct_path(state currentState, vector<state> &path) {
    state *statePtr = &currentState;
    while(statePtr != NULL) {
        path.push_back(*statePtr);
        statePtr = statePtr->came_from;
    }
    return SUCCESS;
}

bool astar (state startState, state goalState, function<int(state, state)> heuristic, int& statesExplored) {
  priority_queue<state, vector<state>, CompareState> openSet;
  unordered_set<state> closedSet;
  state currentState;
  startState.g = 0;
  startState.f = startState.g + heuristic(startState, goalState);
  openSet.push(startState);
  statesExplored = 0;
  while (!openSet.empty()) {
    currentState = openSet.top();
    openSet.pop();
    
    // Skip if already processed
    if (closedSet.find(currentState) != closedSet.end()) {
      continue;
    }
    
    statesExplored++;
    
    if (currentState == goalState)
      return reconstruct_path(currentState, solutionPath);
    
    closedSet.insert(currentState);
    generateNeighbors(currentState, goalState, openSet, closedSet, heuristic);
  }
  return !SUCCESS;
}

void generateRandomState(state& s) {
  vector<int> tiles = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  for (int i = 8; i > 0; i--) {
    int j = rand() % (i + 1);
    swap(tiles[i], tiles[j]);
  }
  for (int i = 0; i < 9; i++)
    s.board[i] = tiles[i];
}

int main () {
  srand(time(0));
  state startState, goalState;

  // Heuristic functions
  
  // Admissible Heuristic: Diagonal distance counts the number of diagonal moves needed to reach the goal.
  auto diagonalDistance = [](state fromState, state toState) {
    int distance = 0;
    for (int i = 0; i < 9; i++) {
      if (fromState.board[i] != 0) {
        int targetPos = -1;
        for (int j = 0; j < 9; j++) {
          if (toState.board[j] == fromState.board[i]) {
            targetPos = j;
            break;
          }
        }
        int currentRow = i / 3, currentCol = i % 3;
        int targetRow = targetPos / 3, targetCol = targetPos % 3;
        distance += max(abs(currentRow - targetRow), abs(currentCol - targetCol));
      }
    }
    return distance;
  };
  
  // Admissible Heuristic: Manhattan distance counts the total number of horizontal and vertical moves needed to reach the goal.
  auto manhattanDistance = [](state fromState, state toState) {
    int distance = 0;
    for (int i = 0; i < 9; i++) {
      if (fromState.board[i] != 0) {
        int targetPos = -1;
        for (int j = 0; j < 9; j++) {
          if (toState.board[j] == fromState.board[i]) {
            targetPos = j;
            break;
          }
        }
        int currentRow = i / 3, currentCol = i % 3;
        int targetRow = targetPos / 3, targetCol = targetPos % 3;
        distance += abs(currentRow - targetRow) + abs(currentCol - targetCol);
      }
    }
    return distance;
  };
  
  // Admissible Heuristic: Misplaced tiles counts the number of tiles that are not in their goal position.
  auto misplacedTiles = [](state fromState, state toState) {
    int mismatchCount = 0;
    for (int i = 0; i < 9; i++)
      if (fromState.board[i] != toState.board[i])
        mismatchCount++;
    return mismatchCount;
  };
  
  // Inadmissible Heuristic: Combined distance sums Manhattan and Diagonal distances.
  auto combinedDistance = [](state fromState, state toState) {
    int manhattanTotal = 0;
    int diagonalTotal = 0;
    
    for (int i = 0; i < 9; i++) {
      if (fromState.board[i] != 0) {
        int targetPos = -1;
        for (int j = 0; j < 9; j++) {
          if (toState.board[j] == fromState.board[i]) {
            targetPos = j;
            break;
          }
        }
        int currentRow = i / 3, currentCol = i % 3;
        int targetRow = targetPos / 3, targetCol = targetPos % 3;
        manhattanTotal += abs(currentRow - targetRow) + abs(currentCol - targetCol);
        diagonalTotal += max(abs(currentRow - targetRow), abs(currentCol - targetCol));
      }
    }
    
    return manhattanTotal + diagonalTotal;
  };

  
  cout << "Choose input method:" << endl;
  cout << "1. Manual input" << endl;
  cout << "2. Random generation" << endl;
  cout << "3. Use default state" << endl;
  cout << "Enter choice (1, 2, or 3): ";
  
  int choice;
  cin >> choice;
  
  if (choice == 1) {
    string startInput, goalInput;
    cout << "Enter 9 values for start state (e.g., 123045678): ";
    cin >> startInput;
    for (int i = 0; i < 9; i++) startState.board[i] = startInput[i] - '0';
    
    cout << "Enter 9 values for goal state (e.g., 123456780): ";
    cin >> goalInput;
    for (int i = 0; i < 9; i++) goalState.board[i] = goalInput[i] - '0';
  } else if (choice == 2) {
    generateRandomState(startState);
    generateRandomState(goalState);
    
    cout << "\nGenerated Start State:" << endl;
    startState.print();
    cout << "\nGenerated Goal State:" << endl;
    goalState.print();
    cout << endl;
  } else {
    int start[9] = {1, 2, 3, 4, 0, 5, 6, 7, 8};
    for (int i = 0; i < 9; i++) startState.board[i] = start[i];
    
    int goal[9] = {1, 2, 3, 4, 5, 6, 7, 8, 0};
    for (int i = 0; i < 9; i++) goalState.board[i] = goal[i];
    
    cout << "\nDefault Start State:" << endl;
    startState.print();
    cout << "\nDefault Goal State:" << endl;
    goalState.print();
    cout << endl;
  }
  
  // Run with all heuristics
  vector<pair<string, function<int(state, state)>>> heuristics = {
    {"Diagonal Distance", diagonalDistance},
    {"Manhattan Distance", manhattanDistance},
    {"Misplaced Tiles", misplacedTiles},
    {"Combined Distance (Manhattan + Diagonal)", combinedDistance}
  };
  
  for (auto& [name, heuristic] : heuristics) {
    solutionPath.clear();
    cout << "========================================" << endl;
    cout << "Running with " << name << " heuristic" << endl;
    cout << "========================================" << endl;
    
    int statesExplored = 0;
    auto startTime = chrono::high_resolution_clock::now();
    
    if (astar(startState, goalState, heuristic, statesExplored) == SUCCESS) {
      auto endTime = chrono::high_resolution_clock::now();
      auto duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime);
      
      int steps = solutionPath.size() - 1;
      cout << "Total steps to reach goal: " << steps << endl;
      // cout << "States explored: " << statesExplored << endl;
      cout << "Time taken: " << fixed << setprecision(3) << duration.count() / 1000.0 << " ms\n" << endl;
      
      // for (int i = solutionPath.size() - 1; i >= 0; i--)
      //   solutionPath[i].print();
      // cout << "Success\n" << endl;
    }
    else {
      auto endTime = chrono::high_resolution_clock::now();
      auto duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime);
      cout << "Time taken: " << duration.count() / 1000.0 << " ms" << endl;
      cout << "FAIL\n" << endl;
    }
  }
  
  return 0;
}
