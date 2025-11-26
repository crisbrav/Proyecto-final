#include "mazegrid.h"

#include <QQueue>

MazeGrid::MazeGrid(int cellSize, QObject *parent)
    : QObject(parent),
    m_rows(15),
    m_cols(20),
    m_cellSize(cellSize)
{
    buildDefaultMaze();
}

int MazeGrid::rows() const
{
    return m_rows;
}

int MazeGrid::cols() const
{
    return m_cols;
}

int MazeGrid::cellSize() const
{
    return m_cellSize;
}

int MazeGrid::cellAt(int row, int col) const
{
    if (!isInside(row, col))
        return CellWall;
    return m_grid[row][col];
}

bool MazeGrid::isInside(int row, int col) const
{
    return row >= 0 && row < m_rows && col >= 0 && col < m_cols;
}

bool MazeGrid::isWalkable(const QPoint &cell) const
{
    int r = cell.y();
    int c = cell.x();
    if (!isInside(r, c))
        return false;

    int v = m_grid[r][c];
    return v == CellEmpty || v == CellExit
           || v == CellPlayerStart || v == CellGuardStart;
}

QPointF MazeGrid::cellCenter(const QPoint &cell) const
{
    double x = cell.x() * m_cellSize + m_cellSize / 2.0;
    double y = cell.y() * m_cellSize + m_cellSize / 2.0;
    return QPointF(x, y);
}

QPoint MazeGrid::sceneToCell(const QPointF &p) const
{
    int col = static_cast<int>(p.x()) / m_cellSize;
    int row = static_cast<int>(p.y()) / m_cellSize;
    if (!isInside(row, col))
        return QPoint(-1, -1);
    return QPoint(col, row);
}

QPoint MazeGrid::playerStartCell() const
{
    return m_playerStart;
}

QPoint MazeGrid::exitCell() const
{
    return m_exitCell;
}

QVector<QPoint> MazeGrid::guardStartCells() const
{
    return m_guardStarts;
}

QVector<QPoint> MazeGrid::bfs(const QPoint &start, const QPoint &goal) const
{
    QVector<QPoint> emptyPath;

    if (!isWalkable(start) || !isWalkable(goal))
        return emptyPath;

    QVector< QVector<bool> > visited(m_rows, QVector<bool>(m_cols, false));
    QVector< QVector<QPoint> > parent(m_rows, QVector<QPoint>(m_cols, QPoint(-1, -1)));

    QQueue<QPoint> q;
    q.enqueue(start);
    visited[start.y()][start.x()] = true;

    const QPoint dirs[4] = {
        QPoint(1, 0),
        QPoint(-1, 0),
        QPoint(0, 1),
        QPoint(0, -1)
    };

    bool found = false;

    while (!q.isEmpty()) {
        QPoint current = q.dequeue();
        if (current == goal) {
            found = true;
            break;
        }

        for (int i = 0; i < 4; ++i) {
            QPoint next = current + dirs[i];
            int r = next.y();
            int c = next.x();

            if (!isInside(r, c))
                continue;
            if (visited[r][c])
                continue;
            if (!isWalkable(next))
                continue;

            visited[r][c] = true;
            parent[r][c] = current;
            q.enqueue(next);
        }
    }

    if (!found)
        return emptyPath;

    // reconstruir camino desde goal a start
    QVector<QPoint> path;
    QPoint curr = goal;
    while (curr != start) {
        path.prepend(curr);
        QPoint p = parent[curr.y()][curr.x()];
        if (p == QPoint(-1, -1))
            break;
        curr = p;
    }
    path.prepend(start);
    return path;
}

void MazeGrid::buildDefaultMaze()
{
    // 0 camino, 1 pared, 2 salida, 3 inicio jugador, 4 inicio guardia
    static const int data[15][20] = {
        // 0         5         10        15
        //01234567890123456789
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // fila 0
        {1,3,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,2,1}, // fila 1
        {1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,1}, // fila 2
        {1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,1}, // fila 3
        {1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,0,1}, // fila 4
        {1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1}, // fila 5
        {1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1}, // fila 6
        {1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,4,1}, // fila 7 (guardia)
        {1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1}, // fila 8
        {1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1}, // fila 9
        {1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,0,1}, // fila10
        {1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,1}, // fila11
        {1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,0,1}, // fila12
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, // fila13
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}  // fila14
    };

    m_grid.clear();
    m_grid.resize(m_rows);
    for (int r = 0; r < m_rows; ++r) {
        m_grid[r].resize(m_cols);
        for (int c = 0; c < m_cols; ++c) {
            int v = data[r][c];
            m_grid[r][c] = v;

            if (v == CellPlayerStart)
                m_playerStart = QPoint(c, r);
            else if (v == CellExit)
                m_exitCell = QPoint(c, r);
            else if (v == CellGuardStart)
                m_guardStarts.append(QPoint(c, r));
        }
    }
}
