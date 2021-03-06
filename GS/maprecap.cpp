#include "maprecap.h"

MapRecap::MapRecap(QWidget *parent):QDialog(parent), ui(new Ui::MapRecap) {
    ui->setupUi(this);
    //buttonGroup = new QButtonGroup();

    connect(ui->webView->page()->mainFrame(),SIGNAL(javaScriptWindowObjectCleared()),this,SLOT(addClickListener()), Qt::UniqueConnection);
    ui->webView->load(QUrl("qrc:/res/html/mapsExecution.html"));
    model = new TableModel();
    ui->tableView->setModel(model);
    ui->tableView->setItemDelegate(new QComboBoxDelegate());
    ui->tableView->setColumnHidden(0, true);
    ui->tableView->setColumnHidden(5, true);
    ui->tableView->setColumnWidth(2, 42);
    ui->tableView->setColumnWidth(4, 42);
}

MapRecap::MapRecap(Mission *mission, QWidget *parent):QDialog(parent), ui(new Ui::MapRecap) {
    ui->setupUi(this);
    //buttonGroup = new QButtonGroup();

    myMission = Mission(*mission);

    connect(ui->webView->page()->mainFrame(),SIGNAL(javaScriptWindowObjectCleared()),this,SLOT(addClickListener()), Qt::UniqueConnection);
    ui->webView->load(QUrl("qrc:/res/html/mapsExecution.html"));
    model = new TableModel();
    ui->tableView->setModel(model);
    ui->tableView->setItemDelegate(new QComboBoxDelegate());
    ui->tableView->setColumnHidden(0, true);
    ui->tableView->setColumnHidden(5, true);
    ui->tableView->setColumnWidth(2, 42);
    ui->tableView->setColumnWidth(4, 42);
}

MapRecap::~MapRecap(){
    delete ui;
}

/* Since c++/JS bridges are broken when the JS page refreshes this slot
 is used to rebruild the bridge each time when triggered by a
 javaScriptWindowObjectCleared signal from the page frame. Function
 added by Jordan Dickson Feb 21st 2015. */
void MapRecap::addClickListener() {
    std::cout << "listenerAdded!" << std::endl;
    //Creates the bridge called cbridge between the java script object and this class.
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("cbridge",this);
}


void MapRecap::on_backButton_clicked() {
    this->done(0);
}


/* Sends a request for the map to clear itself, causing the JavaScript page
to reload itself. This function then cycles through each entry on the table
and enters the coordinates on the map one by one in order. Function added by
Jordan Dickson Feb 14th 2015. */
//Sends clearMap request.
void MapRecap::updateMap() {
    ui->webView->page()->mainFrame()->evaluateJavaScript("clearMap()");
    drawFlightPath(myMission.getFlightPath());
}


void MapRecap::closeWindow() {
    this->close();
}


/*  Function called by the JavaScript to add the map data from mission planning.
This is necessary because data cannot be added until the html file is completely
loaded. Jordan 2/21/2015 */
void MapRecap::addNewMap() {
    std::cout << "adding new map" << std::endl;
    updateMap();
}

void MapRecap::drawFlightPath(FlightPath *flightPath) {
    ui->webView->page()->mainFrame()->evaluateJavaScript("clearMap()");
    QList<Protocol::Waypoint> *points = flightPath->getOrderedWaypoints();
    for (Protocol::Waypoint wp : *points){
        sendCoordToJSMap(wp.lat,wp.lon,0);
    }
    delete points;
}

void MapRecap::sendCoordToJSMap(double lat, double lng, int mapID){
    ui->webView->page()->mainFrame()->evaluateJavaScript("plotPointOnMap("+QString::number(lat)+","+QString::number(lng)+","+QString::number(mapID)+")");
}

/* Passer function for adding entries to the table with coordinates recived
from the JavaScript file. See tablemodel.cpp file for more information.
Added by Jordan Dickson Feb 14th 2015.*/
void MapRecap::addPointToTable(double lat, double lng) {
    model->insertRow(lng,lat);
}


