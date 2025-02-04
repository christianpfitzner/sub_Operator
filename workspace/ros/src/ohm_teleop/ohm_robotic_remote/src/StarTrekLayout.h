/*******************************************************************************************/
/* Class StarTrekLayout is a first try of make a cool layout with a Star Trek design. It   */
/* gives two variants of this layout. From up to down and reverse, both with a main widget.*/
/*                                                                                         */
/* Author: Christian Merkl                                                                 */
/* Date  : 2. June 2013                                                                    */
/*******************************************************************************************/
#ifndef __STAR_TREK_LAYOUT__
#define __STAR_TREK_LAYOUT__

#include <QWidget>
#include <QColor>
#include <QVector>
#include <QPainterPath>
#include <QRect>
#include <QString>

//! layout with cool Star Trek look.
/*!
 * this layout class provides two alignments. From up to down and reverse, both with a main
 * widget. Also you can set a window titel (no common qt widget titel).
 */
class StarTrekLayout : public QWidget
{
public:

    //! directions the widget can align.
    enum Direction {
        TopToBottom,
        BottomToTop
    };

    //! default constructor
    explicit StarTrekLayout(const Direction direction = TopToBottom, QWidget* parent = 0);
    //! destructor
    virtual ~StarTrekLayout(void) { }
    //! set the color of this widget.
    void setColor(const QColor& color);
    //! returns the widget color.
    const QColor& color(void) const { return _color; }
    //! set the non qt titel of this widget.
    void setText(const QString& text);
    //! returns the non qt widget titel.
    const QString& text(void) const { return _text; }

    //! set the main widget.
    /*!
     * the main widget will be a child of this widget. That means it is not necessary to
     * delete the main widget by hand.
     */
    void setMainWidget(QWidget* widget);
    //! returns the main widget.
    QWidget* mainWidget(void) { return _mainWidget; }
    //! adds widget to end of the side widget.
    /*!
     * the added side widget will be a child of this widget. So remember it is not necessary
     * to delete the widget by hand.
     */
    void addSideWidget(QWidget* widget);
    //! adds a seperator to the end of side widgets.
    void addSeparator(void);

protected:
    //! overloaded method
    virtual void paintEvent(QPaintEvent* event);
    //! overloaded method
    virtual void resizeEvent(QResizeEvent* event);

private:
    //! holds the alignment.
    Direction _direction;
    //! holds the widget color.
    QColor _color;
    //! holds all side widgets and seperators.
    QVector<QWidget*> _sideWidgets;
    //! for internal.
    QVector<QRect> _sideRects;
    //! for internal.
    QRect _posText;
    //! holds the main widget.
    QWidget* _mainWidget;
    //! for internal.
    QVector<QPainterPath> _paths;
    //! holds the non qt widget titel.
    QString _text;
};

#endif
