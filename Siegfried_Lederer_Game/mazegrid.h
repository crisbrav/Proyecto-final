#ifndef MAZEGRID_H
#define MAZEGRID_H

#include <QObject>
#include <QVector>
#include <QPoint>
#include <QPointF>

class MazeGrid : public QObject
{
    Q_OBJECT

public:
    enum CellType {
        CellEmpty       = 0,
        CellWall        = 1,
        CellExit        = 2,
        CellPlayerStart = 3,
        CellGuardStart  = 4
    };

    explicit MazeGrid(int cellSize, QObject *parent = 0);

    int rows() const;
    int cols() const;
    int cellSize() const;

    int cellAt(int row, int col) const;
    bool isInside(int row, int col) const;
    bool isWalkable(const QPoint &cell) const;

    // conversión entre celda y escena
    QPointF cellCenter(const QPoint &cell) const;
    QPoint  sceneToCell(const QPointF &p) const;

    QPoint playerStartCell() const;
    QPoint exitCell() const;
    QVector<QPoint> guardStartCells() const;

    // BFS para camino más corto en celdas
    QVector<QPoint> bfs(const QPoint &start, const QPoint &goal) const;

private:
    int m_rows;
    int m_cols;
    int m_cellSize;
    QVector< QVector<int> > m_grid;

    QPoint m_playerStart;
    QPoint m_exitCell;
    QVector<QPoint> m_guardStarts;

    void buildDefaultMaze();
};

#endif // MAZEGRID_H
