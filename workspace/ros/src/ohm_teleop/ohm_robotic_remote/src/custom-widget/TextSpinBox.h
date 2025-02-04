/*******************************************************************************************/
/* Class TextSpinBox is a subclass of AbstractSpinBox with a text model.                   */
/*                                                                                         */
/* Author: Christian Merkl                                                                 */
/* Date  : 2. June 2013                                                                    */
/*******************************************************************************************/
#ifndef __TEXT_SPIN_BOX__
#define __TEXT_SPIN_BOX__

#include "AbstractSpinBox.h"

#include <QString>
#include <QList>
#include <QRect>
#include <QPointF>
#include <QVector>
#include <QVariant>

//! Text Spin Box
/*!
 * is a subclass of AbstractSpinBox with a text model.
 */
class TextSpinBox : public AbstractSpinBox
{
    Q_OBJECT

public:
    //! default constructor.
    explicit TextSpinBox(QWidget* parent = 0, const QColor& color = Qt::red);

    //! returns item count.
    unsigned int count(void) const { return _items.count(); }
    //! returns item data by index.
    const QVariant& itemData(const unsigned int index) const { return _datas.at(index); }

public slots:
    //! adds a new item to the end of the spin box with given data.
    void addItem(const QString& text, const QVariant& data = QVariant());
    //! set the current index to index.
    void setCurrentIndex(const unsigned int index);

    //! increments current index by 1.
    /*!
     * if current index >= count() then index will be set to 0.
     */
    virtual void increment(void);
    //! decrements current index by 1.
    /*!
     * if current index < 0 then index will be set to count() - 1.
     */
    virtual void decrement(void);

signals:
    //! signal will be emitted when current index was changed.
    void indexChanged(const QString& text);
    //! signal will be emitted when current index was changed.
    void indexChanged(const int index);

private:
    //! items
    QList<QString> _items;
    //! datas
    QList<QVariant> _datas;
    //! current index
    int _index;
};

#endif
