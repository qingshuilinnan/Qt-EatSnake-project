#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPoint>
#include <QVector>
#include <QPaintEvent>
#include <QPixmap>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QMediaPlayer>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}

/*方向（枚举代替）*/
enum Direction
{
    UP = 0,
    RIGHT,
    DOWN,
    LEFT
};
/*游戏状态*/
enum GameState
{
    PAUSED = 0,
    RUNNING,
    OVER
};

QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:
    //处理蛇的更新
    void handleUpdateSnake();

protected:
    //绘图事件
    void paintEvent(QPaintEvent *event) override;
    //控制方向
    void keyPressEvent(QKeyEvent *event) override;

private:
    //绘制蛇
    void drawSnake(QPainter &painter);
    //贪吃蛇移动
    void snakeMove();
    //生成食物
    void generateFood();
    //绘制食物
    void drawFood(QPainter & painter);
    //绘制地图
    void drawBorder(QPainter &painter);
    //失败检测
    void checkCollision();
    //游戏结束
    void gameover();
    //游戏暂停
    void gamePause();
    //退出游戏提示
    void QuitGame();
    //重新游戏
    void resetGame();

private:
    Ui::Widget *ui;

    //贪吃蛇对象,食物对象
    QVector<QPoint> m_snake;
    QPoint m_food;
    int m_score = 0;    //分数


    //存储蛇头图片
    QPixmap m_snakeHeadUp;
    QPixmap m_snakeHeadDown;
    QPixmap m_snakeHeadLeft;
    QPixmap m_snakeHeadRight;
    //存储食物图片
    QPixmap m_foodPictuer;

    //定时器
    QTimer * m_timer;
    //定时器方向
    Direction m_direction;
    //游戏状态
    GameState m_state;

    //音效储存
    QMediaPlayer *bgm_plaver;
    QMediaPlayer *eat_player;
    QMediaPlayer *death_player;
};
#endif // WIDGET_H
