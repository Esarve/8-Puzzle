#include <iostream>
#include <vector>
#include <algorithm>
#define n 3
const bool SUCCESS = true;
using namespace std;
using i64 = long long int;

class state {
public:
  int board[n][n], g, f;
  state* came_from;
  state () {
    g = 0;
    f = 0;
    came_from = NULL;
  }
  static int heuristic (state fromState, state toState) {
    int mismatchCount = 0;
    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
        if (fromState.board[i][j] != toState.board[i][j])
          mismatchCount++;
    return mismatchCount;
  }
  bool operator == (state otherState) {
    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
        if (this->board[i][j] != otherState.board[i][j])
          return false;
    return true;
  }
  void print () {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++)
        cout << board[i][j] << " ";
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

void addNeighborState (state currentState, state goalState, int newRow, int newCol, int emptyRow, int emptyCol, vector <state>& openSet, vector <state> closedSet) {
  state neighborState = currentState;
  swap (neighborState.board[newRow][newCol], neighborState.board[emptyRow][emptyCol]);
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
  int i, j, emptyRow ,emptyCol;
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      if (currentState.board[i][j] == 0) {
        emptyRow = i;
        emptyCol = j;
        break;
      }
  i = emptyRow, j = emptyCol;
  if (i - 1 >= 0)
    addNeighborState(currentState, goalState, i - 1, j, emptyRow, emptyCol, openSet, closedSet);
  if (i + 1 < n)
    addNeighborState(currentState, goalState, i + 1, j, emptyRow, emptyCol, openSet, closedSet);
  if (j + 1 < n)
    addNeighborState(currentState, goalState, i, j + 1, emptyRow, emptyCol, openSet, closedSet);
  if (j - 1 >= 0)
    addNeighborState(currentState, goalState, i, j - 1, emptyRow, emptyCol, openSet, closedSet);
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

int main () {
  state startState, goalState;
  freopen("in.txt", "r", stdin);
  for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) cin >> startState.board[i][j];
  for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) cin >> goalState.board[i][j];
  if (astar(startState, goalState) == SUCCESS) {
    for (int i = solutionPath.size() - 1; i >= 0; i--)
      solutionPath[i].print();
    cout << "SUCKsess" << endl;
  }
  else cout << "FAIL" << endl;
  return 0;
}
