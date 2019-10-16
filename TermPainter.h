#pragma once
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
using namespace std;

/**
 * @file TermPainter.h
 * 
 * @brief Simplifies the process of drawing within a Linux terminal. 
 */

/**
 * Simplifies the process of drawing within a Linux terminal.
 */
class TermPainter
{
public:

    /**
     * Initialize the painter, filling the terminal window with empty space.
     */
    TermPainter();

    /**
     * All text colors, text formats, and the cursor position will be reset. 
     */
    ~TermPainter();

    /**
     * Gets the width of the terminal window.
     * 
     * @return  The window's width, measured in characters.
     */
    int getWidth();

    

    /**
     * Gets the height of the terminal window.
     * 
     * @return  The window's height, measured in lines.
     */
    int getHeight();

    /**
     * Prints a value at a specific coordinate.
     * 
     * @tparam T       Any type that can be printed by cout.
     * 
     * @param x        The terminal column where the cursor will be placed 
     *                 before printing.
     * 
     * @param y        The terminal row where the cursor will be placed before
     *                 printing.
     * 
     * @param toPrint  The value that will be printed to the terminal window.
     */
    template<typename T>
    void print(int x, int y, T toPrint)
    {
        moveCursor(x, y);
        std::cout << toPrint;
        moveCursor(0, getHeight());
    }

    /**
     * Resets text color and style to the default values.
     */
    inline void resetText()
    {
        cout << "\33[0m";
    }

    /**
     * All ANSI color codes that can be used to set text color.
     */
    enum Color
    {
        fgBlack = 30,
        fgRed = 31,
        fgGreen = 32,
        fgYellow = 33,
        fgBlue = 34,
        fgMagenta = 35,
        fgCyan = 36,
        fgWhite = 37,
        bgBlack = 40,
        bgRed = 41,
        bgGreen = 42,
        bgYellow = 43,
        bgBlue = 44,
        bgMagenta = 45,
        bgCyan = 46,
        bgWhite = 47
    };

    /**
     * Sets a color to use when printing new text.  Until resetText() is called
     * or a conflicting color value is set, all terminal output will print in
     * this color.
     * 
     * @param color  The color value to set. 
     */
    inline void setColor(Color color)
    {
        std::cout << "\33[" << to_string((int) color) << "m";
    }

    /**
     * Sets if newly printed text will be printed in bold.
     * 
     * @param bold  True to start printing in bold, false to disable printing in
     *              bold.
     */
    inline void setBold(bool bold)
    {
        std::cout << "\33[" << (bold ? "1m" : "22m");
    }


    /**
     * Fills a rectangular area within the terminal window with empty space.
     * If a background color has been set with setColor, the rectangle will
     * be in that color.
     * 
     * @param column  The character column index to use for the rectangle's 
     *                leftmost side.
     * 
     * @param row     The terminal row index to use for the rectangle's topmost
     *                edge.
     * 
     * @param width   The width of the rectangle, measured in characters.
     * 
     * @param height  The height of the rectangle, measured in lines.
     */
    void fillRect(int column, int row, int width, int height);

private:
    /**
     * Moves the cursor to specific coordinates within the terminal window.
     * 
     * @param x        The terminal column where the cursor will be placed.
     * 
     * @param y        The terminal row where the cursor will be placed.
     */
    inline void moveCursor(int x, int y)
    {
        cout << "\33[" << y << ";" << x << "f";
    }
    
    /**
     * Holds the Linux terminal size.
     */
    struct winsize w;
};



