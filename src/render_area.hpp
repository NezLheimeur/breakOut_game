#pragma once

#ifndef RENDER_AREA_HPP
#define RENDER_AREA_HPP

#include "circle.hpp"
#include "rectangle.hpp"

#include <QWidget>
#include <QTimer>
#include <QTime>
#include <list>

//forward declaration of QLabel
class QLabel;



class render_area : public QWidget
{
    Q_OBJECT
public:

    render_area(QWidget *parent = 0);
    ~render_area();

    /** Draw or not the circle when called */
    void change_draw_circle_state();
    void setup_points_number(QLabel* points_number_param);


protected:
    /** Actual drawing function */
    void paintEvent(QPaintEvent *event);
    /** Function called when the mouse is pressed */

    void mousePressEvent(QMouseEvent *event);
    /** Function called when the mouse is moved */
    void mouseMoveEvent(QMouseEvent *event);
    /** Function called when the button of the mouse is released */
    void mouseReleaseEvent(QMouseEvent *event);
    /** Function called when a button is pushed*/
    void keyPressEvent(QKeyEvent *event);



private slots:

    /** Function called periodically at constant time interval by a timer */
    void update_timer();

private: //functions

    /** Insert current position and time inside the recording structures */
    void store_values(vec2 const& click);

    /** Deal with collision */
    vec2 collision_handling(vec2& p);

    /** Move forward in time */
    void numerical_integration();

    std::list<rectangle> init_niveau1();

private: //attributes


    QLabel *points_number;

    /** la barre qu'on va bouger */
    rectangle barre;

    /** les briques que nous allons casser */
    std::list<rectangle> liste_briques;

    /** The circle */
    circle circ;
    /** The speed of the circle */
    vec2 speed;
    /** The time integration step */
    float dt;

    /** A vector of previous position when the circle is grabbed */
    std::list<vec2> stored_motion;
    /** A vector of previous time when the circle is grabbed */
    std::list<int> stored_time;
    /** The position of the previous click */
    vec2 click_previous;
    /** Indicates is the circle is currently selected */
    bool is_sphere_selected;

    /** Indicates if the circle should be drawn */
    bool draw_circle;

    bool pause;
    vec2 speed_pause;
    bool depart;


    /** Timer for periodic time iteration */
    QTimer timer;
    /** Time accessor */
    QTime time;

};

#endif
