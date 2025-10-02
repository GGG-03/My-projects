#include "extradisplay.h"
#include "qdebug.h"
#include "qdrag.h"
#include "qevent.h"
#include "qmimedata.h"
#include "ui_extradisplay.h"

#include <QWidget>
#include <QMenuBar>

ExtraDisplay::ExtraDisplay(QWidget *parent) : QWidget(parent), ui(new Ui::ExtraDisplay)
{
    ui->setupUi(this);
    setAcceptDrops(true);

    // Adds the menu bar
    menuBar->addAction(" ");
    menuBar->setStyleSheet("background-color: rgb(134,199,218); font-family: \"Consolas\"; font-weight: bold");
    ui->gridLayout->setMenuBar(menuBar);
}

ExtraDisplay::~ExtraDisplay()
{
    delete ui;
}

void ExtraDisplay::loadTab(QWidget *dir, QString name)
{
    ui->tabWidget->addTab(dir, QIcon(QString("")), name);
}

void ExtraDisplay::clearTab()
{
    ui->tabWidget->clear();
}

void ExtraDisplay::closeEvent(QCloseEvent*)
{
    emit closing(false);
}

void ExtraDisplay::receivingDragged(QWidget *w, QString t, QPointF pos)
{
    draggedWidget = w;
    draggedText = t;
    draggedPos = pos;
}

void ExtraDisplay::on_tabWidget_tabBarClicked(int index)
{
    draggedIndex = index;
    draggedWidget = ui->tabWidget->widget(index);
    draggedText = ui->tabWidget->tabText(index);
    draggedPos = QCursor::pos();
    emit aboutToBeDragged(draggedWidget, draggedText, draggedPos);

    ui->tabWidget->setTabVisible(index, false);

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    mimeData->setText(ui->tabWidget->tabText(index));
    drag->setMimeData(mimeData);

    Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
}

void ExtraDisplay::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void ExtraDisplay::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->answerRect().intersects(ui->tabWidget->geometry()))
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void ExtraDisplay::dropEvent(QDropEvent*)
{
    if ((QCursor::pos() - draggedPos).manhattanLength() > QApplication::startDragDistance())
    {
        ui->tabWidget->addTab(draggedWidget, draggedText);
        ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
    }
    else
    {
        ui->tabWidget->setTabVisible(draggedIndex, true);
    }
}

void ExtraDisplay::changeStylesheet(float factor)
{
    tabStyleSheet.replace("tabWidth", QString::number(200*factor));
    tabStyleSheet.replace("tabHeight", QString::number(30*factor));
    ui->tabWidget->setStyleSheet(tabStyleSheet);

    menuStyleSheet.replace("menuHeight", QString::number(22*factor));
    menuBar->setStyleSheet(menuStyleSheet);

    tabStyleSheet.replace("transparent", "rgb(173,217,230)");
    ui->tabWidget->setStyleSheet(tabStyleSheet);
}
