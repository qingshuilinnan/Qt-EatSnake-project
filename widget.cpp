#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <QMessageBox>
#include <cstdlib>
#include <QTime>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QDebug>
#include <QSettings>

Widget::Widget(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::Widget),
    m_direction(RIGHT)
{
    ui->setupUi(this);

    //初始化贪吃蛇
    setWindowTitle("贪吃蛇");
    //固定窗口大小
    setFixedSize(820,680);
    //初始化蛇的位置
    m_snake.append(QPoint(100,100));
    m_snake.append(QPoint(80,100));
    m_snake.append(QPoint(60,100));

    //蛇头图片
    m_snakeHeadUp.load(":/snakeicon/up.png");
    m_snakeHeadDown.load(":/snakeicon/down.png");
    m_snakeHeadLeft.load(":/snakeicon/left.png");
    m_snakeHeadRight.load(":/snakeicon/right.png");

    //音效

    bgm_plaver = new QMediaPlayer(this);
    QAudioOutput *audioBgm = new QAudioOutput(this);
    bgm_plaver->setAudioOutput(audioBgm);
    bgm_plaver->setSource(QUrl("qrc:///music/game_bgm.mp3"));
    connect(bgm_plaver, &QMediaPlayer::playbackStateChanged, this,
            [=](QMediaPlayer::PlaybackState newState) {
                if (newState == QMediaPlayer::StoppedState) {
                    bgm_plaver->play();
                }
            });
    bgm_plaver->play();
    eat_player = new QMediaPlayer(this);
    QAudioOutput *audioEat = new QAudioOutput(this);
    eat_player->setAudioOutput(audioEat);
    eat_player->setSource(QUrl("qrc:///music/eat_sound.mp3"));

    death_player = new QMediaPlayer(this);
    QAudioOutput *audioDeath = new QAudioOutput(this);
    death_player->setAudioOutput(audioDeath);
    death_player->setSource(QUrl("qrc:///music/death_sound.mp3"));

    //食物图片
    m_foodPictuer.load(":/snakeicon/food.jpg");
    m_timer = new QTimer(this);


    connect(m_timer,&QTimer::timeout,this,&Widget::handleUpdateSnake);

    m_timer->start(500);
    m_state = RUNNING;
}
//绘制蛇
void Widget::drawSnake(QPainter & painter)
{
    QPixmap headImg;
    switch (m_direction) {
    case UP: headImg = m_snakeHeadUp; break;
    case RIGHT: headImg = m_snakeHeadRight; break;
    case DOWN: headImg = m_snakeHeadDown; break;
    case LEFT: headImg = m_snakeHeadLeft; break;
    }
    for(int idx = 0; idx < m_snake.size(); idx++)
    {
        if(idx == 0)
        {
            //绘制蛇头
            painter.drawPixmap(m_snake[idx].x(),m_snake[idx].y(),20,20,headImg);
        }
        else{
            //绘制蛇身
            painter.setBrush(Qt::red);
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(m_snake[idx].x(),m_snake[idx].y(),18,18);
        }
    }
}
//绘制食物
void Widget::drawFood(QPainter & painter)
{
    painter.drawPixmap(m_food.x(),m_food.y(),20,20,m_foodPictuer);
}
//绘制地图
void Widget::drawBorder(QPainter & painter)
{
    QPixmap m_background(":/snakeicon/background.jpg");
    painter.drawPixmap(0,0,820,680,m_background);
    painter.setPen(Qt::gray);
    // 绘制网格（步长20，与蛇移动步长一致）
    for (int x = 0; x < 820; x += 20) {
        painter.drawLine(x, 0, x, 680);
    }
    for (int y = 0; y < 680; y += 20) {
        painter.drawLine(0, y, 820, y);
    }
    // 绘制外边框
    painter.setPen(Qt::black);
    painter.drawRect(0, 0, 819, 679);
}
//绘图事件
void Widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);    //画家，在此界面进行绘画

    drawBorder(painter);
    painter.drawText(750, 670, QString("分数：%1").arg(m_score)); //绘制分数
    drawSnake(painter);    //绘制蛇
    drawFood(painter);      //绘制食物
}
//贪吃蛇状态更新
void Widget::handleUpdateSnake()
{
    snakeMove();    //移动函数
    update();   //更新状态
}
//贪吃蛇移动
void Widget::snakeMove()
{
    QPoint newHead = m_snake.first();
    switch (m_direction) {
    case UP:
        newHead.ry() -= 20;
        break;
    case RIGHT:
        newHead.rx() += 20;
        break;
    case DOWN:
        newHead.ry() += 20;
        break;
    case LEFT:
        newHead.rx() -= 20;
        break;
    default:
        break;
    }
    //判断蛇是否吃到食物
    if(newHead == m_food)
    {
        eat_player->play();
        /*将贪吃蛇的头添加到向量的开头*/
        m_snake.prepend(newHead);
        /*生成新的食物*/
        generateFood();
        /*不删除蛇尾位置，蛇身总长增加，并且增加分数*/
        m_score += 10;
        // 吃食物后检查（加速，难度递增）
        if (m_score % 50 == 0 && m_timer->interval() > 100) {  // 最低间隔100ms
            m_timer->setInterval(m_timer->interval() - 50);  // 每得50分加速
        }
    }
    else{
        /*将贪吃蛇的头添加到向量的开头*/
        m_snake.prepend(newHead);
        //移除向量中最后一个元素的位置，即蛇整体移动
        m_snake.removeLast();
    }
    checkCollision();   //检查碰撞
}
//控制方向
void Widget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_W:
        if (m_direction != DOWN) m_direction = UP;
        break;
    case Qt::Key_D:
        if(m_direction != LEFT) m_direction = RIGHT;
        break;
    case Qt::Key_S:
        if(m_direction != UP) m_direction = DOWN;
        break;
    case Qt::Key_A:
        if(m_direction != RIGHT) m_direction = LEFT;
        break;
    case Qt::Key_Space:
        gamePause();
        break;
    case Qt::Key_Q:
        QuitGame();
        break;
    case Qt::Key_R:
        resetGame();
        break;
    default:
        QWidget::keyPressEvent(event);  //重复调用函数已达持续控制
    }
}
//生成食物
void Widget::generateFood()
{
    int x = rand() % 40 * 20;  // 820窗口宽度，减去20边距
    int y = rand() % 33 * 20;  // 680窗口高度，减去20边距
    m_food = QPoint(x,y);
    //检查食物位置是否与蛇身重合，若重合则重新生成
    while (m_snake.contains(m_food)) {
        int x = rand() % 40 * 20;
        int y = rand() % 33 * 20;
        m_food =QPoint(x,y);
    }
}
//失败检测
void Widget::checkCollision()
{
    QPoint head = m_snake.first();
    /*墙体碰撞检测*/
    if(head.x() < 0 || head.x() > 820 || head.y() < 0 || head.y()> 680)
    {
        gameover();
    }
    /*身体碰撞检测*/
    for(int idx = 1;idx < m_snake.size();idx++)
    {
        if(m_snake[idx] == head)
            gameover();
    }
}
//游戏暂停
void Widget::gamePause()
{
    if(m_state == PAUSED)
    {
        m_timer->start();
        m_state = RUNNING;
    }
    else
    {
        m_timer->stop();
        m_state = PAUSED;
        QMessageBox::information(this,"游戏提示","游戏暂停！再次按下space键继续游戏");
    }
}
//游戏结束
void Widget::gameover()
{
    death_player->play();
    m_timer->stop();
    // 游戏结束时
    QSettings settings("SnakeGame", "HighScore");  // 存储在注册表或配置文件
    int highScore = settings.value("high", 0).toInt();
    if (m_score > highScore) {
        settings.setValue("high", m_score);
        highScore = m_score;
    }
    QMessageBox::information(this, "游戏结束",
                             QString("得分：%1\n最高分：%2\n按下R重新开始").arg(m_score).arg(highScore));
}
//退出提示
void Widget::QuitGame()
{
    m_timer->stop();
    QMessageBox::StandardButton ret = QMessageBox::question(
        this,  // 父窗口指针，通常用当前 Widget 的 this
        "退出提示",  // 标题
        "确定要退出游戏吗？",
        QMessageBox::Ok | QMessageBox::Cancel  // 确定和取消按钮
        );
    if (ret == QMessageBox::Ok) {
        this->close();        // 点击确定，执行退出操作
    } else {
        m_timer->start();   // 点了取消，不执行
    }
}
//重新游戏
void Widget::resetGame()
{
    m_snake.clear();
    m_snake.append(QPoint(100,100));
    m_snake.append(QPoint(80,100));
    m_snake.append(QPoint(60,100));
    m_direction = RIGHT;
    m_score = 0;
    generateFood();
    m_state = RUNNING;
    m_timer->start(500);
    update();
}
Widget::~Widget()
{
    delete ui;
}
