#ifndef PICTUREPREVIEWPANEL_H
#define PICTUREPREVIEWPANEL_H

/********************************************************************************
 * QT实现的类似QQ的头像选择框，使用事件和双缓冲来尽量减少重新绘制的几率
 * 主要由4个类实现
 * 1、PicturePreviewPanel：图标展示框，控件基类
 * 2、BackgroundWidget：阴影窗口，是PicturePreviewPanel子类
 * 3、CutShape：交互图形基类，实现了拖拽和放大缩小功能
 * 4、CutRound：圆形剪贴，父类为CutShape，实现父类的paintInit接口重绘
 * 5、CutRectangle：矩形剪贴，父类为CutShape，实现父类的paintInit接口重绘
 * 开发过程中只需要声明PicturePreviewPanel类，并调用期LoadPicture接口就可以加载图片，并进行图片剪切
 * ******************************************************************************/

#include <QObject>
#include <QWidget>
#include <QtWidgets>

class QLabel;

enum ShapeType
{
    Rect,//矩形
    Round,//圆形
};
//剪贴图基类 实现了基本的交互功能，并绘制了部分图案，主要的团绘制在子类实现，通过实现paintInit接口
class CutShape : public QWidget
{
public:
    CutShape(QWidget * parent = nullptr);
    ~CutShape();

public:
    QPainterPath CutRegion();
    QRect GetGeometry();

protected:
    //QWidget
    virtual void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

    virtual bool paintInit(QPaintEvent *, QPaintDevice *) = 0;
    virtual QPainterPath Region(){ return QPainterPath(); };

protected:
    ShapeType m_Type;
    bool m_MouseDown = false;
    bool m_IsMoving = false;
    bool m_IsFirst = true;
    int border = 5;

private:
    QRect getResizeGem(QRect oldgeo, QPoint mousePoint, bool & ignore);

private:
    bool m_Left = false;
    bool m_Right = false;
    bool m_Bottom = false;
    bool m_Top = false;
    QPoint m_startPoint;
    QPoint m_old_pos;
    QLabel * m_Label;
    QPixmap m_BufferPix;
};

class CutRectangle : public CutShape
{
public:
    CutRectangle(QWidget * parent = nullptr);
    ~CutRectangle();

protected:
    //CutShape
    virtual bool paintInit(QPaintEvent *, QPaintDevice *) Q_DECL_OVERRIDE;
    virtual QPainterPath Region() Q_DECL_OVERRIDE;
};

class CutRound : public CutShape
{
public:
    CutRound(QWidget * parent = nullptr);
    ~CutRound();

protected:
    //CutShape
    virtual bool paintInit(QPaintEvent *, QPaintDevice *) Q_DECL_OVERRIDE;
    virtual QPainterPath Region() Q_DECL_OVERRIDE;

private:
};

class BackgroundWidget : public QWidget
{
public:
    BackgroundWidget(QWidget * parent = nullptr, ShapeType type = Round);
    ~BackgroundWidget();

public:
    void PictureLoadFinished();
    QRect PictureSaveFinished();

protected:
    virtual void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

private:
    ShapeType m_Type;
    CutShape * m_CutShape = nullptr;
};

class PicturePreviewPanel : public QWidget
{
    Q_OBJECT
public:
    explicit PicturePreviewPanel(QWidget *parent = nullptr);
    ~PicturePreviewPanel();

    void LoadPicture(const QString & filepath);//加载图片
    void SavePicture(const QString & filepath);

protected:
    virtual bool eventFilter(QObject *, QEvent *) Q_DECL_OVERRIDE;

private:
    void InitializeUI();
    void LoadPicture_p();
    QPixmap picture1;
    QString m_PicturePath;
    QLabel * m_PictureContainer = nullptr;
    BackgroundWidget * m_BgWidget = nullptr;
signals:

public slots:
};

#endif // PICTUREPREVIEWPANEL_H
