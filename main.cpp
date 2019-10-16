//Uncomment to draw the game board
#define DRAW_BOARD

//Uncomment to allow terminal box-drawing characters
#define BOX_CHARS_SUPPORTED

//Uncomment to perform additional tests when solving Sudoku puzzles
//#define CHECK_WORK

#ifdef DRAW_BOARD
#include "TermPainter.h"
#endif

#include <vector>
#include <stdint.h>
#include <cstring>
#include <bitset>
#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <functional>
#include <exception>
#include <string>
using namespace std;

/**
 * Simulates a Sudoku game board, tracking all possible values that could be
 * held by each square.
 */
class BoardMap
{
public:
    /**
     * Initializes the board map.  This creates a list of unsolved board
     * square indices, and generates bitmaps for each square representing its 
     * possible values. Bitmaps are also generated for each row, column, and
     * 3x3 block, tracking all values already found within each grouping.
     * 
     * @param board  A vector of char vectors representing a 9x9 Sudoku board.
     *               This must contain nine row vectors, and each row vector
     *               must contain nine characters.  Characters may only be
     *               within '1'-'9', or '.' for empty squares.
     */
    BoardMap(vector<vector<char>>& board);

    /**
     *  Attempts to solve the Sudoku game, by finding valid values for each 
     * empty square in the game board.  The BoardMap tracks all possible values 
     * for each square, eliminating possible values by removing values already 
     * found within the square's row, column, or 3x3 block.
     * 
     *  When that approach can no longer reduce the number of available
     * possibilities, the BoardMap will guess the value of an unsolved square
     * with the smallest number of possible values, and recursively attempt
     * to solve the resulting game board. 
     * 
     * If a guess leads to an unsolvable board state, all progress made after 
     * that guess will be discarded, and the guessed value will be removed from 
     * the list of possibilities.
     * 
     * @return true if all values were found, false if the initial board state
     *         has no solution.
     * 
     * @post  If this returns true, the value of each square will be copied into
     *        the board vector reference provided when the BoardMap was created.
     */
    bool solve();
    
#ifdef DRAW_BOARD
    /**
     * Sets a title to print over the game board.
     * 
     * @param puzzleName  A name representing this Sudoku puzzle.
     */
    void setName(string puzzleName);
#endif
    
private:

    /**
     * Finds the index of a board square's row.
     * 
     * @param squareIndex  The index (within 0-80, inclusive) of a square on
     *                     the game board.
     * 
     * @return             The index of that square's row.
     */
    static inline uint8_t rowIndex(const uint8_t squareIndex)
    {
        return squareIndex / 9;
    }

    /**
     * Finds the index of a board square's column.
     * 
     * @param squareIndex  The index (within 0-80, inclusive) of a square on
     *                     the game board.
     * 
     * @return             The index of that square's column.
     */
    static inline uint8_t columnIndex(const uint8_t squareIndex)
    {
        return squareIndex % 9;
    }

    /**
     * Finds the index of a board square's 3x3 block.
     * 
     * @param squareIndex  The index (within 0-80, inclusive) of a square on
     *                     the game board.
     * 
     * @return             The index of that square's 3x3 block.
     */
    static inline uint8_t blockIndex(const uint8_t squareIndex)
    {
        return columnIndex(squareIndex) / 3
                + 3 * (rowIndex(squareIndex) / 3);
    }

    /**
     * For a specific square in the game board, remove all values marked within 
     * a bitmap value as possible values of that square.
     * 
     * @param index  The index of the square on the game board to update.
     * 
     * @param flags  Represents a combination of potential values to eliminate.
     *               For any valid value v, v will be removed as a possibility
     *               if ((1 << (v + 1)) & flags) > 0.  
     *               e.g. if flags = 0b100011010, 2, 4, 5, and 9 will no longer
     *               be considered possible values for the board square at
     *               the given index.
     */
    inline void removePossibilities(const int8_t index, const uint16_t flags)
    {
        squares[index] &= (~flags);
    }

    /**
     * Gets the number of possible values a square on the game board may hold.
     * 
     * @param index  The index of the square on the game board
     * 
     * @return   All potential values of the square at the given index that
     *           have not yet been eliminated as possibilities.    
     */
    uint8_t numPossibilities(const uint8_t index) const;

    /**
     * For a solved square on the game board, update that square's row, column,
     * and block to mark the square's value as solved.
     * 
     * @param index  The index of a square on the game board.  Providing the
     *               index of an unsolved square will likely result in an
     *               invalid board state.
     */
    void updateGroups(const uint8_t index);

    /**
     * For each unsolved board square, check that square's row, column, and
     * block for values to eliminate as possible solutions.  If a square is
     * reduced to a single possible value, copy that value to the board vector
     * and remove the square from the list of unsolved squares.
     * 
     * @return  The number of unsolved squares where at least one potential
     *          value was eliminated as a possibility. 
     */
    uint8_t reduceOptions();

    /**
     * For a given square's bitmap value, get the character representing that
     * square's solution.
     * 
     * @param bitmap  A map of all possible values for some square on the game
     *                board.
     * 
     * @return  If the bitmap holds exactly one possible value, return that
     *          value's numeric character.  Otherwise, return '.' to indicate 
     *          that the square is unsolved.
     */
    static char toChar(const uint16_t bitmap);
    
#ifdef DRAW_BOARD
    /**
     * Draws the current board state within the terminal window.  This will
     * only work correctly within Linux terminals.
     * 
     * @param highlight  If non-negative, marks the index of a square on the
     *                   game board to highlight.
     */
    void printBoard(int highlight = -1);
#endif

#ifdef CHECK_WORK
    /**
     * For a given index, print the bitmap representing all possible values
     * being considered for the board square at that index.  Similar bitmaps
     * will be printed for that square's row, column, and block, indicating
     * what values have already been found within each of those groups.
     * 
     * @param index  The index of a square on the game board.
     */
    void printInfo(const uint8_t index);
#endif
    //The game board reference provided on creation
    vector<vector<char>>&board;
    
    //Indices of all squares on the game board with unknown values.
    set<uint8_t> unsolved;

    //flags represent the possible values of each square on the game board
    uint16_t squares [81];

    //flags represent all values found in each group
    uint16_t rows [9];
    uint16_t columns[9];
    uint16_t blocks[9];

#ifdef DRAW_BOARD
    //Used to track recursion depth.
    int depth = 0;
    //a title to print above the game board
    string name;
#endif
};


BoardMap::BoardMap(vector<vector<char>>& board) : board(board)
{
    memset(rows, 0, 18);
    memset(columns, 0, 18);
    memset(blocks, 0, 18);
    uint8_t i = 0;
    for (const vector<char>& row : board)
    {
        for (const char& square : row)
        {
            if (square == '.')
            {
                unsolved.insert(i);
                squares[i] = 0b111111111;
            }
            else
            {
                squares[i] = (1 << (square - '1'));
                updateGroups(i);
            }
            i++;
        }
    }
}

bool BoardMap::solve()
{
    uint8_t updated = 0;
    do
    {
        updated = reduceOptions();
    }
    while (updated > 0);

    while (!unsolved.empty())
    {
        uint8_t fewestPossibilities = 9;
        uint8_t guessIndex = 0;
        for (const uint8_t& index : unsolved)
        {
            uint8_t options = numPossibilities(index);
            if (options == 0)
            {
                return false;
            }
            if (options < fewestPossibilities)
            {
                fewestPossibilities = options;
                guessIndex = index;
            }
        }
        uint8_t guess = 1;
        while (guess < 9)
        {
            if ((squares[guessIndex] & (1 << (guess - 1))) > 0)
            {
                break;
            }
            guess++;
        }
        vector<vector<char>> boardCopy = board;
        int x = columnIndex(guessIndex);
        int y = rowIndex(guessIndex);
        boardCopy[y][x] = ('0' + guess);
        BoardMap copy(boardCopy);

#ifdef DRAW_BOARD
        copy.depth = depth + 1;
        copy.setName("Guessing (" + to_string(x) + ","
                + to_string(y) + ") = " + to_string((int) guess));
#endif

        if (copy.solve())
        {
            board = copy.board;
#ifdef DRAW_BOARD
            unsolved.clear();
            printBoard();
            system("sleep 1");
#endif
            return true;
        }
        else
        {
            squares[guessIndex] &= ~(1 << (guess - 1));
        }
    }
#ifdef DRAW_BOARD
    printBoard();
    system("sleep 1");
#endif
    return unsolved.empty();
}
    
#ifdef DRAW_BOARD
void BoardMap::setName(string puzzleName)
{
    name = puzzleName;
}
#endif

uint8_t BoardMap::numPossibilities(const uint8_t index) const
{
    uint16_t val = squares[index];
    uint8_t found = 0;
    while (val > 0)
    {
        if (val % 2 == 1)
        {
            found++;
            val--;
        }
        val >>= 1;
    }
    return found;
}

void BoardMap::updateGroups(const uint8_t index)
{
#ifdef CHECK_WORK
    if (numPossibilities(index) != 1)
    {
        cerr << "Tried to update groups with unsolved bitset "
                << bitset<9>(squares[index]) << ", index "
                << (int) index << endl;
        printInfo(index);
        exit(-1);
    }
#endif

    rows[rowIndex(index)] |= squares[index];
    columns[columnIndex(index)] |= squares[index];
    blocks[blockIndex(index)] |= squares[index];
}

uint8_t BoardMap::reduceOptions()
{
    static set<uint8_t> solved;
    uint8_t reduced = 0;
    for (auto iter = unsolved.begin(); iter != unsolved.end(); iter++)
    {
        uint8_t index = *iter;
        uint16_t options = squares[index];

        uint8_t y = rowIndex(index);
        uint8_t x = columnIndex(index);
        removePossibilities(index, rows[y]);
        removePossibilities(index, columns[x]);
        removePossibilities(index, blocks[blockIndex(index)]);

        if (options != squares[index])
        {
            reduced++;
            char solution = toChar(squares[index]);
            if (solution != '.')
            {
#ifdef CHECK_WORK
                uint8_t row = rowIndex(index);
                uint8_t col = columnIndex(index);
                uint8_t block = blockIndex(index);
                for (int i = 0; i < 9; i++)
                {
                    if (board[row][i] == value || board[i][col] == value
                        || board[(row - (row % 3)) + (i / 3)]
                        [(col - (col % 3) + (i % 3))] == value)
                    {
                        cerr << "Tried to set " << index << " to " 
                             << value << endl;
                        printInfo(index);
                        exit(-1);
                    }
                }
                board[row][col] = value;
#else
                board[rowIndex(index)][columnIndex(index)] = solution;
#endif
                solved.insert(index);
                updateGroups(index);
            }
#ifdef DRAW_BOARD
            printBoard(index);
#endif
        }
    }
    for (const uint8_t& index : solved)
    {
        unsolved.erase(index);
    }
    solved.clear();
    return reduced;
}

char BoardMap::toChar(const uint16_t bitmap)
{
    for (uint8_t i = 0; i < 9; i++)
    {
        if (bitmap == (1 << i))
        {
            return '1' + i;
        }
    }
    return '.';
}
    
#ifdef DRAW_BOARD
void BoardMap::printBoard(int highlight)
{
    static bool boardDrawn = false;
    static TermPainter tPainter;
    static const int width = 37;
    static const int height = 19;
    static const int xPos = (tPainter.getWidth() - width) / 2;
    static const int yPos = (tPainter.getHeight() - height) / 2;
    if (!boardDrawn)
    {
        tPainter.setColor(TermPainter::bgBlue);
        tPainter.fillRect(0, 0, tPainter.getWidth(), tPainter.getHeight());
        tPainter.setColor(TermPainter::bgBlack);

#ifdef BOX_CHARS_SUPPORTED
        string topLeft = "┌";
        string bottomLeft = "└";
        string upperRight = "┐";
        string lowerRight = "┘";
        string horizontal = "─";
        string vertical = "│";
        string leftT = "├";
        string bottomT = "┴";
        string rightT = "┤";
        string topT = "┬";
        string fork = "┼";
#else
        string topLeft = "=";
        string bottomLeft = "=";
        string upperRight = "=";
        string lowerRight = "=";
        string horizontal = "-";
        string vertical = "|";
        string leftT = "|";
        string bottomT = "-";
        string rightT = "|";
        string topT = "=";
        string fork = "+";
#endif

        struct CharSet
        {
            string left;
            string line;
            string fork;
            string right;
        };

        vector<CharSet> rows;
        rows.push_back({topLeft, horizontal, topT, upperRight});
        CharSet line = {leftT, horizontal, fork, rightT};
        CharSet squares = {vertical, " ", vertical, vertical};
        for (int row = 0; row < 8; row++)
        {
            rows.push_back(squares);
            rows.push_back(line);
        }
        rows.push_back(squares);
        rows.push_back({bottomLeft, horizontal, bottomT, lowerRight});
        tPainter.setColor(TermPainter::fgCyan);
        tPainter.setBold(true);
        for (int y = 0; y < rows.size(); y++)
        {
            //bold rows : 0, 6, 12, 18
            bool boldRow = (y % 6) == 0;
            const CharSet& charSet = rows[y];
            for (int x = 0; x < width; x++)
            {
                //bold columns: 0, 12, 24, 36
                if (!boldRow)
                {
                    tPainter.setBold((x % 12) == 0);
                }
                else tPainter.setBold(boldRow);

                string gridChar = charSet.line;
                if (x == 0) gridChar = charSet.left;
                else if (x == (width - 1)) gridChar = charSet.right;
                else if ((x % 4) == 0) gridChar = charSet.fork;
                tPainter.print(x + xPos, y + yPos, gridChar);
            }
        }
        boardDrawn = true;
    }
    tPainter.setColor(TermPainter::bgBlack);
    tPainter.setColor(TermPainter::fgWhite);
    tPainter.setBold(false);
    uint8_t squareIndex = 0;
    for (int y = 1; y < height; y += 2)
    {
        for (int x = 2; x < width; x += 4)
        {
            if (squareIndex == highlight)
            {
                tPainter.setColor(TermPainter::bgWhite);
                tPainter.setColor(TermPainter::fgBlack);
                tPainter.setBold(true);
            }
            tPainter.print(x + xPos, y + yPos,
                    board[rowIndex(squareIndex)][columnIndex(squareIndex)]);
            if (squareIndex == highlight)
            {
                tPainter.setColor(TermPainter::bgBlack);
                tPainter.setColor(TermPainter::fgWhite);
                tPainter.setBold(false);
            }
            squareIndex++;
        }
    }
    tPainter.setColor(TermPainter::bgBlue);
    tPainter.fillRect(0, yPos - 2, tPainter.getWidth(), 1);
    tPainter.fillRect(0, yPos + height + 1, tPainter.getWidth(), 1);
    tPainter.setColor(TermPainter::bgWhite);
    tPainter.setColor(TermPainter::fgBlack);
    tPainter.setBold(true);

    if (!name.empty())
    {
        tPainter.print(xPos + (width - name.length()) / 2,
                yPos - 2, name);
    }
    string solved = "Solved ";
    solved += to_string(81 - unsolved.size()) + "/81";
    if (depth > 0)
    {
        solved += " depth " + to_string(depth);
    }
    tPainter.print(xPos + (width - solved.length()) / 2,
            yPos + height + 1, solved);
    tPainter.resetText();
    cout.flush();
}
#endif

#ifdef CHECK_WORK
void BoardMap::printInfo(const uint8_t index)
{
    int rowNum = rowIndex(index);
    int colNum = columnIndex(index);
    int blkNum = blockIndex(index);

    string iStr = to_string(index);
    while (iStr.length() < 2)
    {
        iStr += " ";
    }
    cerr << "square  " << iStr << ": "
            << bitset<9>(squares[index]) << endl;
    cerr << "   row  " << rowNum << " : "
            << bitset<9>(rows[rowNum]) << endl;
    cerr << " column " << colNum << " : "
            << bitset<9>(columns[colNum]) << endl;
    cerr << "  block " << blkNum << " : "
            << bitset<9>(blocks[blkNum]);
}
#endif

class Solution
{
public:

    void solveSudoku(vector<vector<char>>&board, string name = "")
    {
        BoardMap boardMap(board);
#ifdef DRAW_BOARD
        if (!name.empty())
        {
            boardMap.setName(name);
        }
#endif
        if (!boardMap.solve())
        {
            cerr << "No solution found!\n";
            exit(-1);
        }
    }
};

int main(int argc, char** argv)
{
    Solution s;
    ifstream gameFile;
    gameFile.open("puzzles.txt");
    int gameNum = 1;
    while (gameFile.is_open() && !gameFile.eof())
    {
        vector<vector<char>> game;
        for (int x = 0; x < 9; x++)
        {
            game.push_back(vector<char>());
            for (int y = 0; y < 9; y++)
            {
                char c = '.';
                gameFile >> c;
                game[x].push_back(c);
            }
        }
        if (gameNum % 100 == 0)
        {
            cout << " Game " << gameNum << " :\n";
        }
        s.solveSudoku(game);

        gameNum++;
        cout << "Continue?: ";
        char shouldContinue;
        cin >> shouldContinue;
        if(shouldContinue == 'n' || shouldContinue == 'N')
        {
            exit(0);
        }
    }
}

