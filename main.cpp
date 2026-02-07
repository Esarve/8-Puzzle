#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <cmath>
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

  bool operator == (state otherState) {
    for (int i = 0; i < 9; i++)
      if (this->board[i] != otherState.board[i])
        return false;
    return true;
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

vector <state> solutionPath;
bool compareFScore (state stateA, state stateB) {
  return stateA.f < stateB.f;
}

bool containsState (state targetState, vector <state> stateSet) {
  for (int i = 0; i < stateSet.size(); i++)
    if (targetState == stateSet[i])
      return true;
  return false;
}

void addNeighborState (state currentState, state goalState, int newPos, int emptyPos, vector <state>& openSet, vector <state> closedSet, function<int(state, state)> heuristic) {
  state neighborState = currentState;
  swap (neighborState.board[newPos], neighborState.board[emptyPos]);
  if (!containsState(neighborState, closedSet) && !containsState(neighborState, openSet)) {
      neighborState.g = currentState.g + 1;
      neighborState.f = neighborState.g + heuristic(neighborState, goalState);
      state* parentState = new state();
      *parentState = currentState;
      neighborState.came_from = parentState;
      openSet.push_back(neighborState);
  }
}

void generateNeighbors (state currentState, state goalState, vector <state>& openSet, vector <state>& closedSet, function<int(state, state)> heuristic) {
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

bool astar (state startState, state goalState, function<int(state, state)> heuristic) {
  vector <state> openSet;
  vector <state> closedSet;
  state currentState;
  startState.g = 0;
  startState.f = startState.g + heuristic(startState, goalState);
  openSet.push_back(startState);
  while (!openSet.empty()) {
    sort(openSet.begin(), openSet.end(), compareFScore);
    currentState = openSet[0];
    if (currentState == goalState)
      return reconstruct_path(currentState, solutionPath);
    openSet.erase(openSet.begin());
    closedSet.push_back(currentState);
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
    cout << "Enter 9 values for start state (0-8, where 0 is empty): ";
    for (int i = 0; i < 9; i++) cin >> startState.board[i];
    
    cout << "Enter 9 values for goal state (0-8, where 0 is empty): ";
    for (int i = 0; i < 9; i++) cin >> goalState.board[i];
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
    
    if (astar(startState, goalState, heuristic) == SUCCESS) {
      cout << "Path length: " << solutionPath.size() << " steps\n" << endl;
      for (int i = solutionPath.size() - 1; i >= 0; i--)
        solutionPath[i].print();
      cout << "Success\n" << endl;
    }
    else {
      cout << "FAIL\n" << endl;
    }
  }
  
  return 0;
}
