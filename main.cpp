#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
/*
 * If you use members that are declared inside the standard header string then yes,
 * you have to include that header either directly or indirectly (via other headers).
 * Some compilers on some platforms may on some time of the month compile even though
 * you failed to include the header. This behaviour is unfortunate, unreliable and does
 * not mean that you shouldn’t include the header.
 * The reason is simply that you have included other standard headers which also happen
 * to include string. But as I said, this can in general not be relied on, and it may
 * also change very suddenly (when a new version of the compiler is installed, for instance).
 * Always include all necessary headers. Unfortunately, there does not appear to be a
 * reliable online documentation on which headers need to be included.
 * Consult a book, or the official C++ standard.
*/

using std::ifstream;
using std::istringstream;
using std::vector;
using std::string;
using std::sort;
using std::cout;
using std::endl;

enum class State {empty, obstacle, closed, path, start, finish};
// directional deltas
const int deltas[4][2] {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

string cell_string(State cell) {
    switch (cell) {
        case State::obstacle: return "⛰️   ";
        case State::path: return "🚗   ";
        case State::start: return "🚦   ";
        case State::finish: return "🏁   ";
        default: return "0   ";
    }
}

vector<State> parse_line(string line) {
    vector<State> parsed = {};
    istringstream stream(line);

    int n;
    char c;
    while (stream >> n >> c && c == ',') {
        if (n == 0) {parsed.push_back(State::empty);}
        else {parsed.push_back(State::obstacle);}
    }

    return parsed;
}

vector<vector<State>> read_board(string path) {
    vector<vector<State>> board = {};

    ifstream file(path);  // file.open(path);
    if (file) {
        string line;
        while (getline(file, line)) {
            board.push_back(parse_line(line));
        }
    }

    return board;
}

void print_board(const vector<vector<State>> board) {
    int i, j;
    for (i = 0; i < board.size(); i++) {
        for (j = 0; j < board[i].size(); j++) {
            cout << cell_string(board[i][j]);
        }
        cout << endl;
    }
}

bool compare(const vector<int> node1, const vector<int> node2) {
    /* Return true if the f-value of the first argument is greater than
     * the f-value of the second, and it should return false otherwise.
     * Recall that the f-value if the sum of the cost and the heuristic.
     * */
    int f1 = node1[2] + node1[3];
    int f2 = node2[2] + node2[3];
    return f1 > f2;
}

void cell_sort(vector<vector<int>> *v) {
    /* Sort the given collection of cells using the built-in sort()
     * function.
     * */
    sort(v->begin(), v->end(), compare);
}

int heuristic(int x1, int y1, int x2, int y2) {
    /* Return the Manhattan distance from (x1, y1) to (x2, y2).*/
    return abs(x1 - x2) + abs(y1 - y2);
}

void add_to_open(int x, int y, int g, int h, vector<vector<int>> &open,
                 vector<vector<State>> &grid) {
    /* Create a vector<int> node with the form {x, y, g, h} and push the node
     *  to the back of the open vector. Set the grid value for the (x, y)
     *  coordinates to the enum value closed.
     *
     *  grid is not empty and x and y are valid coordinates in grid.
     *  */
    open.push_back(vector<int>{x, y, g, h});
    grid[x][y] = State::closed;
}

bool check_valid_cell(int x, int y, vector<vector<State>> &grid) {
    /* Return whether the cell at coordinates (x, y) in the given grid is a
     * valid cell. By valid, we mean that (x, y) are valid coordinates on the
     * grid and grid[x][y] is empty.
     * */
    bool on_grid = (x >= 0 && x < grid.size() && y >= 0 && y < grid[0].size());

    // Warning: when x and y are not valid coordinates on the grid, checking
    // grid[x][y] == State::empty directly may result in a segmentation fault.
    // So a short-circuit mechanism should be introduced here.
    return (on_grid && grid[x][y] == State::empty);
}

void expand_neighbors(vector<int> &curr_node, int goal[2],
                      vector<vector<int>> &open, vector<vector<State>> &grid) {
    int x = curr_node[0], y = curr_node[1], g = curr_node[2];
    for (auto delta: deltas) {  // const int*
        int dx = delta[0], dy = delta[1];
        int next_x = x + dx, next_y = y + dy;
        int next_g = g + 1, next_h = heuristic(next_x, next_y, goal[0], goal[1]);

        if (check_valid_cell(next_x, next_y, grid)) {
            add_to_open(next_x, next_y, next_g, next_h, open, grid);
        }
    }
}

vector<vector<State>> search(vector<vector<State>> &grid, int init[2], int goal[2]) {
    /* Search for an optimal path in the given grid using A* search
     * algorithm.
     * */
    vector<vector<int>> open = {};
    int x = init[0], y = init[1];
    int g = 0, h = heuristic(init[0], init[1], goal[0], goal[1]);
    add_to_open(x, y, g, h, open, grid);

    while (!open.empty()) {
        cell_sort(&open);
        vector<int> curr = open.back();
        open.pop_back();
        x = curr[0], y = curr[1];
        grid[x][y] = State::path;

        if (x == goal[0] && y == goal[1]) {
            grid[init[0]][init[1]] = State::start;
            grid[goal[0]][goal[1]] = State::finish;
            return grid;
        }
        expand_neighbors(curr, goal, open, grid);
    }

    cout << "No path found" << endl;
    return vector<vector<State>> {};
}

int main() {
    string path = "/Users/kaiwenzheng/Desktop/maze_solver/board.txt";

    int init[2] = {0, 0}, goal[2] = {4, 5};
    auto board = read_board(path);
//    print_board(board);
    auto solution = search(board, init, goal);
    print_board(solution);
}
