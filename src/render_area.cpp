
#include "render_area.hpp"

#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include <cmath>

#include <iostream>


render_area::render_area(QWidget *parent)
    :QWidget(parent),
      //circ({200,150},25),
      //barre({300,290},50,8),
      dt(1/5.0f),
      stored_motion(),
      stored_time(),
      click_previous(),
      is_sphere_selected(false),
      draw_circle(true),
      timer(),
      time(),
      pause(false),
      speed_pause(0.0f,0.0f),
      depart(false),
      points_number(nullptr)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    circ.radius=5;
    barre.position.x=300;
    barre.position.y=290;
    barre.height=8;
    barre.width=75;


    setFocusPolicy(Qt::StrongFocus);

    liste_briques=init_niveau1();
    //timer calling the function update_timer periodicaly
    connect(&timer, SIGNAL(timeout()), this, SLOT(update_timer()));
    timer.start(30); //every 30ms
}

render_area::~render_area()
{}




void render_area::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen;
    pen.setWidth(1.0);
    pen.setColor(Qt::blue);
    painter.setPen(pen);

    QBrush brush = painter.brush();
    brush.setColor(Qt::gray);
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);

    if(draw_circle)
    {
        //the actual drawing of the circle
        vec2 const& p=circ.center;
        float const r=circ.radius;
        painter.drawEllipse(p.x-r,p.y-r,2*r,2*r);
    }

    // on dessine la barre
    painter.drawRect(barre.position.x,barre.position.y,barre.width,barre.height);

    // On dessine les briques

    for(rectangle& tampon: liste_briques)
    {
            painter.drawRect(tampon.position.x,tampon.position.y,tampon.width,tampon.height);
    }
}




void render_area::mousePressEvent(QMouseEvent *event)
{
    vec2 const click=vec2(event->x(),event->y());

    //check if the click is inside the circle
    if( point_inside_circle(click,circ) )
    {
        //clear the previous data to store new ones
        is_sphere_selected=true;
        stored_motion.clear();
        stored_time.clear();
        click_previous=click;
    }

}





void render_area::mouseMoveEvent(QMouseEvent *event)
{
    if(is_sphere_selected)
    {
        //compute the current translation of the mouse
        vec2 const click=vec2(event->x(),event->y());
        vec2 const translate=click-click_previous;

        //translate the center of the circle
        circ.center+=translate;

        //store previous values
        click_previous=click;
        store_values(click);
    }

    repaint();
}

void render_area::store_values(vec2 const& click)
{
    //store at most the 5 last position and time of the circle
    stored_motion.push_back(click);
    if(stored_motion.size()>5)
        stored_motion.pop_front();

    stored_time.push_back(time.elapsed());
    if(stored_time.size()>5)
        stored_time.pop_front();
}

void render_area::mouseReleaseEvent(QMouseEvent *event)
{
    //if the sphere was selected
    //  a new speed if computed
    //  the sphere is marked as released

    if(is_sphere_selected)
    {
        vec2 const click=vec2(event->x(),event->y());
        float const t=time.elapsed();

        //compute average speed of the last mouse motion
        float const time_motion=(t-*stored_time.cbegin())/1000.0f;
        vec2 const displacement=click-*stored_motion.cbegin();

        speed=displacement/time_motion*0.05f; //the new speed


        is_sphere_selected=false;
    }

}


void render_area::update_timer()
{
    //called periodically
    if(is_sphere_selected==false) //only integrate when the circle is not selected
        numerical_integration();

    repaint();
}

void render_area::numerical_integration()
{
    //numerical integration using Forward Euler method

    vec2& p=circ.center;

    p = p+dt*speed;          //integrate position

    //collision handling (set new position, and return a new speed value)
    speed=collision_handling(p);
}

vec2 render_area::collision_handling(vec2& p)
{
    vec2 new_speed=speed;

    //size of the window
    float const h=height();
    float const w=width();

    //radius of the sphere
    float const r=circ.radius;

    //special condition in cases of collision
    bool collision=false;
    bool collision_brique=false;
    
    //collision avec les briques:
   
    std::list<rectangle>::iterator it_hited;
    rectangle tampon;
   
   // for(rectangle tampon: liste_briques)
    for(auto it1=liste_briques.begin(); it1!=liste_briques.end(); it1++) 
    {
	tampon=*it1;
        // par la gauche
        if(p.x-r<tampon.position.x+tampon.width && p.x-r>tampon.position.x && p.y+r<tampon.position.y+tampon.height+2 && p.y-r>tampon.position.y-2 && collision==false)
        {

            p.x=tampon.position.x+tampon.width+r;
            new_speed.x *= -1;
            collision=true;
            collision_brique=true;
	    it_hited=it1;

        }

      // par la droite
        else if(p.x+r>tampon.position.x && p.x+r<tampon.position.x+tampon.width && p.y+r<tampon.position.y+tampon.height+2 && p.y-r>tampon.position.y-2 && collision==false)
        {

            p.x=tampon.position.x-r;
            new_speed.x *= -1;
            collision=true;
            collision_brique=true;
	    it_hited=it1;

        }

        // par au dessus
        else if(p.y+r>tampon.position.y && p.y+r<tampon.position.y+tampon.height && p.x+r>tampon.position.x+2 && p.x-r<tampon.position.x+tampon.width-2 && collision==false )
        {
            p.y=tampon.position.y-r;
            new_speed.y *= -1;
            collision=true;
            collision_brique=true;
	    it_hited=it1;
        }

        // par en dessous
        else if(p.y-r<tampon.position.y+tampon.height && p.y-r>tampon.position.y && p.x+r>tampon.position.x+2 && p.x-r<tampon.position.x+tampon.width-2 && collision==false )
        {

            p.y=tampon.position.y+tampon.height+r+2;
            new_speed.y *= -1;
            collision=true;
            collision_brique=true;
	    it_hited=it1;
	    

        }


   // ++it1;
    }
    if(collision==true){
   	 liste_briques.erase(it_hited);
    }
    //collision with the barre
    if(p.y+r>barre.position.y-barre.height && p.x+r>barre.position.x && p.x-r<barre.position.x+barre.width && depart==true )
    {
        float d=p.x-barre.position.x; // position relative de la balle/barre
        float L=barre.width; // Longueur de la barre
        float v_max=35.0f; // vitesse maximum

        if(d>(L/2)-r && d<(L/2)+r)
            new_speed.x=0;

        else if(d<(L/2)-r)
            new_speed.x=(2*v_max)/L*d+v_max*((2*r)/L-1);


        else if(d>L/2+r)
            new_speed.x=(2*v_max)/L*d-v_max*(1+(2*r)/L);


        new_speed.y=sqrt(1300-pow(new_speed.x,2));
        new_speed.y *= -1;
        p.y=barre.position.y-barre.height;
        collision=true;
    }

    //collision with the ground
    if(p.y+r>h)
    {
        QMessageBox msgBox;
        msgBox.setText("vous avez perdu");
        msgBox.exec();
        depart=false;
        new_speed.x=0.0f;
        new_speed.y=0.0f;
        p.x=barre.position.x+20;
        p.y=barre.position.y-r;


    }
    //collision with the left wall
    if(p.x-r<0)
    {
        p.x=r;
        new_speed.x *= -1;
        collision=true;
    }
    //collision with the right wall
    if(p.x+r>w)
    {
        p.x=w-r;
        new_speed.x *= -1;
        collision=true;        
    }
    //collision with the top wall
    if(p.y-r<0)
    {
        p.y=r;
        new_speed.y *= -1;
        collision=true; 
    }

    if(collision_brique && points_number!=nullptr)
    {
        //increase the information of the number of bounces
        int points_nbr=points_number->text().toInt();
        points_nbr++;
        points_number->setText(QString::number(points_nbr));
    }

    if(liste_briques.empty())
    {
        QMessageBox msgBox;
        msgBox.setText("vous avez gagne");
        msgBox.exec();
        liste_briques=init_niveau1();
        new_speed.x=0;
        new_speed.y=0;
    }
    return new_speed;

}


void render_area::change_draw_circle_state()
{
   // draw_circle=!draw_circle;
    circ.center.x=375;
    circ.center.y=150;
    speed.x=0;
    speed.y=0;

    std::cout<<"height: "<<height()<<" width: "<<width()<<std::endl;
}


void render_area::keyPressEvent(QKeyEvent *event)
{


    if(event->key() == Qt::Key_Left && barre.position.x>0 && pause==false) // si c'est echape on quitte
    {

        barre.position.x-=20;
        if(depart==false)
            circ.center.x-=20;
    }
    if(event->key() == Qt::Key_Right && barre.position.x+barre.width<width() && pause==false) // si c'est echape on quitte
    {

        barre.position.x+=20;
        if(depart==false

                )
            circ.center.x+=20;

    }

    if(event->key() == Qt::Key_Space)
    {

        if(pause==  true)
        {
            speed=speed_pause;
        }
        else
        {
           speed_pause=speed;
           speed.x=0;
           speed.y=0;
        }
        pause=!pause;
    }

    if(event->key() == Qt::Key_Return && pause == false)
    {
        if(depart==false)
        {
            speed.x=10.0f;
            speed.y=30.0f;
        }
        depart=true;


    }
}


std::list<rectangle> render_area::init_niveau1()
{
    depart=false;
    speed.x=0.0f;
    speed.y=0.0f;

    circ.center.x=barre.position.x+20;
    circ.center.y=barre.position.y-circ.radius;
    int nb_lignes=3;
    int nb_colonnes=9;


    for(int j=0;j<nb_lignes;j++)
    {
        for (int i=0; i<nb_colonnes; i++)
        {
            rectangle brique;
            brique.height=20;
            brique.width=50;
            brique.position.y=(j+3)*20;
            if(i<3)
                brique.position.x=(i+1)*50;
            if(i>=3 && i<6)
                brique.position.x=(i+3)*50;
            if(i>=6 && i<9)
                brique.position.x=(i+5)*50;

            liste_briques.push_back(brique);
        }
    }

    return liste_briques;

}

void render_area::setup_points_number(QLabel* points_number_value)
{
    points_number=points_number_value;
}



