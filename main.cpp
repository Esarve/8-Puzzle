#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
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

  static int heuristic (state fromState, state toState) {
    int mismatchCount = 0;
    for (int i = 0; i < 9; i++)
      if (fromState.board[i] != toState.board[i])
        mismatchCount++;
    return mismatchCount;
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

void addNeighborState (state currentState, state goalState, int newPos, int emptyPos, vector <state>& openSet, vector <state> closedSet) {
  state neighborState = currentState;
  swap (neighborState.board[newPos], neighborState.board[emptyPos]);
  if (!containsState(neighborState, closedSet) && !containsState(neighborState, openSet)) {
      neighborState.g = currentState.g + 1;
      neighborState.f = neighborState.g + state :: heuristic(neighborState, goalState);
      state* parentState = new state();
      *parentState = currentState;
      neighborState.came_from = parentState;
      openSet.push_back(neighborState);
  }
}

void generateNeighbors (state currentState, state goalState, vector <state>& openSet, vector <state>& closedSet) {
  int emptyPos = -1;
  for (int i = 0; i < 9; i++)
    if (currentState.board[i] == 0) {
      emptyPos = i;
      break;
    }
  
  int row = emptyPos / 3;
  int col = emptyPos % 3;
  
  if (row - 1 >= 0)
    addNeighborState(currentState, goalState, emptyPos - 3, emptyPos, openSet, closedSet);
  if (row + 1 < n)
    addNeighborState(currentState, goalState, emptyPos + 3, emptyPos, openSet, closedSet);
  if (col + 1 < n)
    addNeighborState(currentState, goalState, emptyPos + 1, emptyPos, openSet, closedSet);
  if (col - 1 >= 0)
    addNeighborState(currentState, goalState, emptyPos - 1, emptyPos, openSet, closedSet);
}

bool reconstruct_path(state currentState, vector<state> &path) {
    state *statePtr = &currentState;
    while(statePtr != NULL) {
        path.push_back(*statePtr);
        statePtr = statePtr->came_from;
    }
    return SUCCESS;
}

bool astar (state startState, state goalState) {
  vector <state> openSet;
  vector <state> closedSet;
  state currentState;
  startState.g = 0;
  startState.f = startState.g + state :: heuristic(startState, goalState);
  openSet.push_back(startState);
  while (!openSet.empty()) {
    sort(openSet.begin(), openSet.end(), compareFScore);
    currentState = openSet[0];
    if (currentState == goalState)
      return reconstruct_path(currentState, solutionPath);
    openSet.erase(openSet.begin());
    closedSet.push_back(currentState);
    generateNeighbors(currentState, goalState, openSet, closedSet);
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
  
  if (astar(startState, goalState) == SUCCESS) {
    for (int i = solutionPath.size() - 1; i >= 0; i--)
      solutionPath[i].print();
    cout << "Success" << endl;
  }
  else cout << "FAIL" << endl;
  return 0;
}
