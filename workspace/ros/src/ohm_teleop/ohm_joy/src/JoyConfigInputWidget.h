/*
 * JoyConfigInputWidget.h
 *
 *  Created on: 11.02.2015
 *      Author: chris
 */

#ifndef OHM_JOY_SRC_UI_JOYCONFIGINPUTWIDGET_H_
#define OHM_JOY_SRC_UI_JOYCONFIGINPUTWIDGET_H_

// ros includes

// qt includes
#include <QWidget>

/**
 * @namespace Ui
 */
namespace Ui {
class JoyConfigInputWidget;
}

class JoyConfigInputWidget : public QWidget
{
   Q_OBJECT
public:
   enum Type{
      Button,
      Axis
   };
   /**
    * Default constructor
    * @param parent
    */
   JoyConfigInputWidget(QWidget* parent = 0);
   /**
    * Default destructor
    */
   virtual ~JoyConfigInputWidget(void);


   // SETTERS
   /**
    * Function to set Text to Widget
    * @param text
    */
   void setText(QString text);

   /**
    * Function to set type
    * @param t
    */
   void setType(Type t) { _type = t; }
   /**
    * Function to set index
    * @param idx
    */
   void setIndex(unsigned int idx) { _idx = idx; }
   /**
    * Function to set widget as active
    */
   void setActive(void);
   /**
    * Function to set window for inactive
    */
   void setInactive(void);
   /**
    * Function to set axis or button id
    * @param id
    */
   void setAxisOrButton(unsigned int id);

   unsigned int getAxisOrButton(void) const { return _axis_button_idx; }

   // GETTERS
   /**
    * Function to get index of widget
    * @return
    */
   unsigned int getIndex(void) const { return _idx; }

   QString getName(void);
   /**
    * Function to return type of widget input
    * @return
    */
   Type getType(void) const { return _type; }

public slots:
   void slot_clicked(void);

signals:
   void clicked(unsigned int idx);


private:
   Ui::JoyConfigInputWidget* _ui;

   unsigned int _idx;
   unsigned int _axis_button_idx;
   Type         _type;
};

#endif /* OHM_JOY_SRC_UI_JOYCONFIGINPUTWIDGET_H_ */
