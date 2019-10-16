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

        if (copy.solve())
        {
            board = copy.board;
            return true;
        }
        else
        {
            squares[guessIndex] &= ~(1 << (guess - 1));
        }
    }
    return unsolved.empty();
}

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
                board[rowIndex(index)][columnIndex(index)] = solution;
                solved.insert(index);
                updateGroups(index);
            }
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


class Solution
{
public:

    void solveSudoku(vector<vector<char>>&board, string name = "")
    {
        BoardMap boardMap(board);
        if (!boardMap.solve())
        {
            cerr << "No solution found!\n";
            exit(-1);
        }
    }
};
