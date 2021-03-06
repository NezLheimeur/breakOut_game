
#include "window.hpp"

#include "render_area.hpp"
#include "ui_mainwindow.h"

#include <iostream>

window::window(QWidget *parent)
    :QMainWindow(parent),ui(new Ui::MainWindow),render(new render_area)
{
    ui->setupUi(this);
    ui->layout_scene->addWidget(render);
    render->setup_points_number(ui->points_number);

    //Connection with signals
    connect(ui->quit,SIGNAL(clicked()),this,SLOT(action_quit()));

}


window::~window()
{}

void window::action_quit()
{
    close();
}
