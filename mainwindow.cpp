#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    scene->installEventFilter(this);
    string objectname;

//    connect(ui->btnOpen, SIGNAL(clicked()), this, SLOT(ChooseVideoImage()));
    connect(ui->btnYolov4, SIGNAL(clicked()), this, SLOT(Yolov4()));
//    connect(ui->btnVideoRecord, SIGNAL(clicked()), this, SLOT(RecordVideo()));
    connect(ui->btn_select, SIGNAL(clicked()),this, SLOT(Select_Object_btn_clicked()));
    connect(ui->comboBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(onComboboxCurrentTextChanged(const QString &)));

    ui->list_object->addItem("object");
    ui->list_object->addItem("object1");


}



void MainWindow::Select_Object_btn_clicked(){
    QListWidgetItem *item = ui->list_object->currentItem();
    item->setBackgroundColor(Qt::green);

}
bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (target == scene)
    {
        if (event->type() == QEvent::GraphicsSceneMousePress)
        {
            const QGraphicsSceneMouseEvent* const me = static_cast<const QGraphicsSceneMouseEvent*>(event);
            const QPointF position = me->scenePos();
//            cout << position.x() << "," << position.y() << endl;
            selectPos.x = int(position.x());
            selectPos.y = int(position.y());
        }
    }
    return QMainWindow::eventFilter(target, event);
}


void MainWindow::Yolov4()
{
//    VideoWriter *writer;
    YOLO yolo_model(0.5,0.4,416,416, "/home/mami/darknet/cfg/coco.names" ,"/home/mami/darknet/cfg/yolov4.cfg",
                    "/home/mami/darknet/yolov4.weights", "yolov4");
//    capture.open(videoPath);
    capture.open(0);
//    int frame_w = capture.get(CAP_PROP_FRAME_WIDTH);
//    int frame_h = capture.get(CAP_PROP_FRAME_HEIGHT);
//    int fps = capture.get(CAP_PROP_FRAME_COUNT);
//    writer = new VideoWriter("/home/mami/tracked.avi", VideoWriter::fourcc('X', 'V', 'I', 'D'), fps, Size(frame_w, frame_h));
    if(capture.isOpened())
    {
        while(true){
            capture >> readFrame;
            yolo_model.predbox.clear();
            flip(readFrame, readFrame, 1);
            yolo_model.yolo_objects = objectname;
            yolo_model.detect(readFrame);

    //            writer->write(readFrame);
            cout << "object list : ";
            for (int i = 0; i <= yolo_model.predbox.size(); i++)
            {
                cout << yolo_model.predbox[i].x << " "
                     << yolo_model.predbox[i].y << " "
                     << yolo_model.predbox[i].width << " "
                     << yolo_model.predbox[i].height << " ";
                int x0 = yolo_model.predbox[i].x;
                int y0 = yolo_model.predbox[i].y;
                int x1 = yolo_model.predbox[i].x + yolo_model.predbox[i].width;
                int y1 = yolo_model.predbox[i].y + yolo_model.predbox[i].height;

                if(selectPos.x >= x0 && selectPos.x <= x1 && selectPos.y >= y0 && selectPos.y <= y1)
                {
                    qDebug() << "selected object";
                    rectangle(readFrame, Point(x0,y0), Point(x1,y1), Scalar(255,0,0),3);
                }
            }
            cout << " . " << endl;
            QPixmap image = QPixmap::fromImage(drawing.MatToQImage(readFrame));
            ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            ui->graphicsView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
            ui->graphicsView->scene()->addPixmap(image);

            QRectF testRect = scene->itemsBoundingRect();
            testRect.setWidth(testRect.width());
            testRect.setHeight(testRect.height());
            ui->graphicsView->fitInView(testRect, Qt::IgnoreAspectRatio);
            ui->graphicsView->centerOn(0,0);

            qApp->processEvents();
            waitKey(1);

        }




    }

}


void MainWindow::RecordVideo()
{

}

void MainWindow::onComboboxCurrentTextChanged(const QString &){
    int choice = ui->comboBox->currentIndex();
    objectname = ui->comboBox->currentText().toStdString();

    switch(choice){
    case 0:
        cout<<objectname<< endl;
        break;
    case 1:
       cout<<objectname<< endl;
        break;
    case 2:
         cout<<objectname<< endl;
        break;
    case 3:
        cout<<objectname<< endl;
        break;
    case 4:
         cout<<objectname<< endl;
        break;
    case 5:
         cout<<objectname<< endl;
        break;
    }

}


MainWindow::~MainWindow()
{
    delete ui;}



