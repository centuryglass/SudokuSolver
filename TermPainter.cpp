#include "TermPainter.h"
#include <string>
#include <iostream>
using namespace std;

TermPainter::TermPainter()
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    for (int y = 0; y < getHeight(); y++)
    {
        string line;
        for (int x = 0; x < getWidth(); x++)
        {
            cout << " ";
        }
        cout << endl;
    }
    cout.flush();
}

TermPainter::~TermPainter()
{
    resetText();
}

int TermPainter::getWidth()
{
    return w.ws_col;
}

int TermPainter::getHeight()
{
    return w.ws_row - 1;
}


void TermPainter::fillRect(int column, int row, int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        moveCursor(column, row + y);
        for (int x = 0; x < width; x++)
        {
            cout << " ";
        }
        cout.flush();
    }
    moveCursor(0, getHeight());
}
