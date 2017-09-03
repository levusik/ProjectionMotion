#include "calculatordialog.h"
#include "QLayout"
#include "QPushButton"
#include "QLineEdit"
#include "QLabel"
#include "qdebug.h"
#include <qmath.h>

using namespace constants;

// TODO: z lekka kiepskie rozwiązanie, można wymyślec coś bardziej przejrzystego
#define TIME  (this->setTime->lineEdit->text())
#define POSX  (this->setPosX->lineEdit->text())
#define POSY  (this->setPosY->lineEdit->text())
#define VX    (this->setVx->lineEdit->text())
#define VY    (this->setVy->lineEdit->text())
#define V     (this->setV->lineEdit->text())
#define ANGLE (this->setAngle->lineEdit->text())

/*---------------------------------------------------------------------*/
calculatorDialog::calculatorDialog()
{
    this->setWindowTitle("Kalkulator");

    // inicjalizacja layoutów
    //
    //      ########################################
    //      # set Vx  |     set Vy      |   set V  #
    //      # set PosX|    set PosY     | set angle#
    //      #              set Time                #
    //      #                                      #
    //      #       POSX              POSY         #
    //      #                         POSY         #
    //      #                       Ok  | cancel   #
    //      ########################################
    //
    // ustawienie Vx (automatycznie wyliczone jeżeli podano V i angle)
    initializeSetters();
    setupLayouts();

    //              cause everyone loves magic numbers !
    this->setFixedSize(450,350);
}

/*---------------------------------------------------------------------*/
calculatorDialog::~calculatorDialog()
{
}

/*---------------------------------------------------------------------*/
void calculatorDialog::changedComponentVec()
{
    // jeżeli podano te 2 wartości to obliczamy
    if (VX != "" && VY != "")
     {
        double valX = VX.toDouble();
        double valY = VY.toDouble();
        this->setV->lineEdit->setText(QString::number( sqrt(pow(valX, 2) + pow(valY,2)) ));
        this->setAngle->lineEdit->setText(QString::number(qAtan2(valY, valX) * 180. / M_PI ));

        // ustawienie textów z sinusem i cosinusem
       this->sinText->setText(SIN_TEXT_STR + QString::number(VY.toDouble() / V.toDouble()));
       this->cosText->setText(COS_TEXT_STR + QString::number(VX.toDouble() / V.toDouble()));
    }
    else
    {
        this->setV->lineEdit->setText("");
        this->setAngle->lineEdit->setText("");
        this->sinText->setText(SIN_TEXT_STR);
        this->cosText->setText(COS_TEXT_STR);

    }

    solveEquations();
}

/*---------------------------------------------------------------------*/
void calculatorDialog::changedVOrAngle()
{
    if (this->setAngle->lineEdit->text().toDouble() > 360.)
        this->setAngle->lineEdit->setText("0");

    if (this->setV->lineEdit->text() != "" && this->setAngle->lineEdit->text() != "")
    {
        // pobranie wartości i zapisanie ich jako double
        double val     = this->setV->lineEdit->text().toDouble();
        double angle   = this->setAngle->lineEdit->text().toDouble();

        this->setVx->lineEdit->setText(QString::number(qCos(angle * M_PI/180.) * val));
        this->setVy->lineEdit->setText(QString::number(qSin(angle * M_PI/180.) * val));

        // ustawienie textów z sinusem i cosinusem
       this->sinText->setText(SIN_TEXT_STR + QString::number(VY.toDouble() / V.toDouble()));
       this->cosText->setText(COS_TEXT_STR + QString::number(VX.toDouble() / V.toDouble()));
    }
    else
    {
        this->setVx->lineEdit->setText("");
        this->setVy->lineEdit->setText("");
        this->sinText->setText(SIN_TEXT_STR);
        this->cosText->setText(COS_TEXT_STR);

    }
    solveEquations();
}

/*---------------------------------------------------------------------*/
void calculatorDialog::changedPosX()
{
    solveEquations();
}

/*---------------------------------------------------------------------*/
void calculatorDialog::changedPosY()
{
    solveEquations();
}

/*---------------------------------------------------------------------*/
void calculatorDialog::changedTime()
{
    solveEquations();
}

/*---------------------------------------------------------------------*/
void calculatorDialog::solveEquations()
{
    // jeżeli mamy Vy to możemy obliczyć maksymalną wysokość, czas rzutu i czas do osiągnięcia wysokość początkowej
    // w każdym z tych warunków można dodać else if'y sprawdzające czy wyniki nie są puste w celach optymalizacyjnych
    // ale porównywalnie do przyrostu kodu a wydajności nie ma to sensu

    if (VY != "")
    {
        this->maxHeigth->setText(MAX_HEIGTH_STR + QString::number(throwMath::maxHeigth(VY.toDouble(), POSY.toDouble())));
        this->timeOfThrow->setText(TIME_OF_THROW_STR+ QString::number(throwMath::timeOfThrow(VY.toDouble(), POSY.toDouble())));
        this->timeToReachMaxHeigth->setText(TIME_TO_REACH_MAXHEIGTH_STR+ QString::number(throwMath::timeToReachMaxHeigth(VY.toDouble(), POSY.toDouble())));
    }
    else
    {
        // zerujemy teksty
        this->maxHeigth->setText(MAX_HEIGTH_STR);
        this->timeOfThrow->setText(TIME_OF_THROW_STR);
        this->timeToReachMaxHeigth->setText(TIME_TO_REACH_MAXHEIGTH_STR);
    }

    // jeżeli mamy V i kąt to możemy obliczyć zasięg
    if (V != "0" && V != "" && ANGLE != "")
        this->range->setText(RANGE_STR + QString::number(throwMath::range(V.toDouble(), ANGLE.toDouble(), POSY.toDouble() )));
    else
        this->range->setText(RANGE_STR);

    // jeżeli mamy czas, kąt i siłę to możemy obliczyć pozycję końcową
    if (TIME != "" && ANGLE != "" && V != "")
    {
        this->posXResult->setText(POS_X_STR + QString::number(throwMath::calculateXPos(TIME.toDouble(), VX.toDouble(), POSX.toDouble())));
        this->posYResult->setText(POS_Y_STR + QString::number(throwMath::calculateYPos(TIME.toDouble(), VY.toDouble(), POSY.toDouble())));
    }
    else
    {
        this->posXResult->setText(POS_X_STR);
        this->posYResult->setText(POS_Y_STR);
    }
}

/*---------------------------------------------------------------------*/
void calculatorDialog::setupEditables(int smallWidgetHeigth, int smallWidgetWidth)
{
    // ustiaweinie Vx ( automatycznie wyliczone jeżeli podany V i angle)
    setVx    = new lineEditSetter("Vx : ", "[0-9.]{" + HOW_MANY_VALS_IN_EDITABLE + "}", nullptr, QSize(smallWidgetWidth,smallWidgetHeigth));
    // callback dla widgetu
    connect(setVx->lineEdit, &QLineEdit::textEdited, this, &calculatorDialog::changedComponentVec);

    // ustawienie Vy (automatycznie wyliczone jeżeli podano V i angle)
    setVy    = new lineEditSetter("Vy : ", "[0-9.]{" + HOW_MANY_VALS_IN_EDITABLE +"}", nullptr, QSize(smallWidgetWidth,smallWidgetHeigth));
    // callback dla widgetu
    connect(setVy->lineEdit, &QLineEdit::textEdited, this, &calculatorDialog::changedComponentVec);

    // ustawienie V ( automatycznie wyliczone jeżeli podane Vx i Vy )
    setV     = new lineEditSetter("V : ", "[0-9.]{" + HOW_MANY_VALS_IN_EDITABLE +"}", nullptr,  QSize(smallWidgetWidth,smallWidgetHeigth));
    // callback dla widgetu
    connect(setV->lineEdit, &QLineEdit::textEdited, this, &calculatorDialog::changedVOrAngle);

    // ustawienie kąta ( automatycznie wyliczone jeżeli podane Vx i Vy )
    setAngle = new lineEditSetter("Kąt :", "[0-9.]{" + HOW_MANY_VALS_IN_EDITABLE +"}", nullptr,  QSize(smallWidgetWidth,smallWidgetHeigth));
    // callback dla widgetu
    connect(setAngle->lineEdit, &QLineEdit::textEdited, this, &calculatorDialog::changedVOrAngle);

    // ustawienie czasu
    setTime  = new lineEditSetter("Czas : ", "[0-9.]{"+ HOW_MANY_VALS_IN_EDITABLE+ "}",nullptr,  QSize(smallWidgetWidth,smallWidgetHeigth));
    connect(setTime->lineEdit, &QLineEdit::textEdited, this, &calculatorDialog::changedTime);

    // ustawienie pozycji X
    setPosX  = new lineEditSetter("Pozycja X :","[0-9.]{" + HOW_MANY_VALS_IN_EDITABLE + "}", nullptr,  QSize(bigWidgetWidth,bigWidgetHeigth));
    // callback dla widgetu
    connect(setPosX->lineEdit, &QLineEdit::textEdited, this, &calculatorDialog::changedPosX);
    setPosX->lineEdit->setText("0");

    // ustawienie pozycji Y
    setPosY  = new lineEditSetter("Pozycja Y :", "[0-9.]{"+ HOW_MANY_VALS_IN_EDITABLE +"}", nullptr, QSize(bigWidgetWidth,bigWidgetHeigth));
    // callback dla widgetu
    connect(setPosY->lineEdit, &QLineEdit::textEdited, this, &calculatorDialog::changedPosY);
    setPosY->lineEdit->setText("0");
}

/*---------------------------------------------------------------------*/
void calculatorDialog::initializeSetters()
{
    // ustawienie widgetów do których użytkownik będzie mógł wpisać wartości
    setupEditables(smallWidgetHeigth, smallWidgetWidth);

    // ustawienie labelów do których wrzucimy wyniki
    this->posXResult           = new QLabel(POS_X_STR);
    this->posYResult           = new QLabel(POS_Y_STR);
    this->range                = new QLabel(RANGE_STR);
    this->maxHeigth            = new QLabel(MAX_HEIGTH_STR);
    this->sinText              = new QLabel(SIN_TEXT_STR);
    this->cosText              = new QLabel(COS_TEXT_STR);
    this->timeOfThrow          = new QLabel(TIME_OF_THROW_STR);
    this->timeToReachMaxHeigth = new QLabel(TIME_TO_REACH_MAXHEIGTH_STR);
}

/*---------------------------------------------------------------------*/
void calculatorDialog::setupLayouts()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // górny layout
    QHBoxLayout *upperLayout = new QHBoxLayout();
    upperLayout->addWidget(setVx);
    upperLayout->addWidget(setVy);
    upperLayout->addWidget(setV);

    // środkowy layout
    QHBoxLayout *middleLayout = new QHBoxLayout();
    middleLayout->addWidget(setPosX);
    middleLayout->addWidget(setPosY);
    middleLayout->addWidget(setAngle);

    // dolny layout
    QHBoxLayout *downLayout = new QHBoxLayout();
    downLayout->addWidget(setTime);


    // wyniki [sin, cos]
    QHBoxLayout* trigResults = new QHBoxLayout();
    trigResults->addWidget(this->sinText);
    trigResults->addWidget(this->cosText);

    // wyniki [ zasięg, maksymalna wysokość]
    QHBoxLayout *rangeAndMaxHeigthLayout = new QHBoxLayout();
    rangeAndMaxHeigthLayout->addWidget(this->range);
    rangeAndMaxHeigthLayout->addWidget(this->maxHeigth);

    // wyniki [czas rzutu i czas do osiągniecia maksymalnej wysokości]
    QHBoxLayout *timesResults = new QHBoxLayout();
    timesResults->addWidget(this->timeOfThrow);
    timesResults->addWidget(this->timeToReachMaxHeigth);

    // wyniki [ pozycja x, pozycja y]
    QHBoxLayout *positionsResults = new QHBoxLayout();
    positionsResults->addWidget(this->posXResult);
    positionsResults->addWidget(this->posYResult);

    //powrót do głównej aplikacji
    QHBoxLayout *close = new QHBoxLayout();

    // guzik powrotu
    QPushButton *button = new QPushButton("Powrót");
    button->setFixedSize(QSize(SINGLEBUTTONWIDTH, SINGLEBUTTONHEIGHT));
    connect(button, SIGNAL(clicked(bool)), this, SLOT(accept()));

    // spacing
    close->addSpacing(20);

    // dodanie guzika
    close->addWidget(button);


    // posklejanie wszystkiego w jedno
    mainLayout->addLayout((upperLayout));
    mainLayout->addLayout(middleLayout);
    mainLayout->addLayout(downLayout);
    mainLayout->addSpacing(this->height() / 16.);
    mainLayout->addLayout(trigResults);
    mainLayout->addLayout(rangeAndMaxHeigthLayout);
    mainLayout->addLayout(timesResults);
    mainLayout->addLayout(positionsResults);
    mainLayout->addSpacing(this->height() / 16.);
    mainLayout->addLayout(close);

    // ustawienie layout'u
    this->setLayout(mainLayout);
}

/*---------------------------------------------------------------------*/
