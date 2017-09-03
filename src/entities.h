#ifndef ENTITIES_H
#define ENTITIES_H

#include <qmath.h>
#include <QString>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <qdebug.h>


namespace constants {

	// labele w Qt obsługują formatowanie html'a 
	// także definiujemy kilka makr które pozwolą nam ładnie wyformatować
	// tekst [ żeby potem nie trzeba było pamiętać jaki znacznik 
	//		   odpowiadał jakiemu formatowaniu] 
	#define BOLDFACE QString("<b>")
	#define END_BOLDFACE QString("</b>")
	#define CURSIVE	QString("<i>")
	#define END_CURSIVE QString("</i>")


    enum class simulationState
    {
        INTIALIZE = 0,
        SIMULATE  = 1,
    };
	// zbiór stałych stringów [ przez co nie powtarzamy kodu i łatwo 
	//							zmieniamy napisy w całym programie ]

	//										 wartość którą wstawimy do wyrażenia regularnego
    const QString HOW_MANY_VALS_IN_EDITABLE   = "8";
	const QString VECX_STR					  = "Wekt. skład. X : ";
	const QString VECY_STR					  = "Wekt. skład. Y : ";
	const QString SPACING_STR				  = "         ";
    const QString POS_X_STR					  = "Poz. X: ";
    const QString POS_Y_STR					  = "Poz. Y: ";
	const QString ANGLE_STR					  = "Ustaw kąt :";
    const QString RANGE_STR					  = "Zasięg : ";
    const QString MAX_HEIGTH_STR			  = "Maks. wys. :";
    const QString SIN_TEXT_STR				  = "Sin :";
    const QString COS_TEXT_STR				  = "Cos : ";
    const QString TIME_OF_THROW_STR			  = "Czas rzutu :";
    const QString TIME_TO_REACH_MAXHEIGTH_STR = "Czas do osiągnięcia maks. wys. :";
	const QString TIME_LABEL_STR			  = "Czas : ";
	const QString VELOCITY_X_STR			  = "Vx : ";
	const QString VELOCITY_Y_STR			  = "Vy : ";
	const QString SHOW_ALL_POINTS_STR		  = "pokaż wszystkie punkty";
	const QString HIDE_ALL_POINTS_STR		  = "schowaj wszystkie punkty";
	const QString SHOW_ROAD_VECTORS_STR		  = "pokaż wektory";
	const QString HIDE_ROAD_VECTORS_STR		  = "schowaj wektory";
	const QString SHOW_SINGLE_VECTOR_STR	  = "pokaż pojedyncze wektory";
	const QString SHOW_ALL_VECTORS_STR		  = "pokaż wszystkie wektory";


    const int SINGLEBUTTONWIDTH  = 100;
    const int SINGLEBUTTONHEIGHT = 50;
    const int DEFAULTSPACING     = 100;
    const int LINEEDITWIDTH      = 200;
    const int LINEEDITHEIGHT     = 50;

    const double ZOOMIN          = 1.10;
    const double ZOOMOUT         = 0.90;
    const double G_CONST         = 10.;
    const double FPS             = 60.;
	const double DEFAULT_SCALE	 = 10;
	const double MINIMAL_VALUE	 = 0.0001;

	// rozmiar kuli którą rzucamy 
	const double BALL_DIAMETER	   = 20.;
	
	// minimalny zasięg [ w pikselach] dla których będziemy 
	// tworzyć linie pionowe i poziome [ dla wartości mniejszych linijki praktycznie na siebie nachodzą
	//									 także bardziej utrudniają niż pomagają ]
	const int MIN_WIDTH_PXLS     = 50;
	const int MIN_HEIGTH_PXLS	 = 30;

	// maksymalna ilość linii pionowych/poziomych
	const int MAXLINES			 = 30;

	// mnożniki które definiujemy jako te ładne [ potrzebne do rysowania   
	//										      linii poziomych i pionowych ]
	const int NICE_LOOKING_MULTIPLERS[]	= { 1, 2, 5 };
    
	
	// rozmiar "kroku"
	const int	 STEP_DIAMETER	   = 5;

	//rozmiar  kółka kursora [ delikatny cheat ; tworzymy kółko w miejscu kursora
	//						   po czym wywołujemy collides.. który zwraca wektor kroków]
	const int	CURSOR_CIRC_DIAMETER   = 1;

	// co jaki czas możemy zmieniać zaznaczenie kółka
	const int	SELECTING_ITEMS_DELAY  = 0.75;

	// aproksymacja [ wartość przez którą pomnożymy wyliczoną wartość Y 
	//			    i sprawdzimy czy ta wartość jest większa od maksymalnej wysokości 
	//				( będzie to szacowana maksymalna wartość ) ]  [ stała wzięta 'na oko' ] 
	const double APPROX			   = 0.9999;

	// definicje stałych kolorków
	const QColor DEF_STEP_COLOR	      = QColor(0xff, 0xff, 0);
	const QColor STEP_SELECTED_COLOR  = QColor(0xaf,0xaf,0xaf);
	const QColor STEP_INSPECTED_COLOR = QColor(0x33,0xff,0xff);
	const QColor MAX_HEIGTH_COLOR     = QColor(0, 0x4f, 0xff);
	const QColor GRAPHICS_TEXT_COLOR  = QColor(0xcc,0x66,0x0);
	
	const QColor VX_LINE_VEC_COLOR = QColor(0xff, 0x80, 0);
	const QColor VY_LINE_VEC_COLOR = QColor(0x33, 0xff, 0x99);
	const QColor  V_LINE_VEC_COLOR = QColor(0x66,0x66,0xff);	
		
	// zmienna którą jeżeli dt przekroczy, to dodamy "krok" do sceny
	const double ADD_STEP_TO_ROAD_TIME = 1. / FPS;



	// nie chcemy wyświetlać brzydkich wyników wynikających z IEE 754 
	double correctParams(double param);


	// singleton z wzorami
	class throwMath
	{
	public:
	static double calculateXPos(double time, double Vx, double startingXPos)
	{
	     return correctParams(Vx * time + startingXPos);
	}

	static double calculateYPos(double time, double Vy, double startingYPos)
	{
		 if (!startingYPos)
			 return correctParams(Vy * time - ((G_CONST / 2.) * time * time));

	 // jeżeli mamy podaną y-kową pozycję startową mamy doczynienia 
	 // z rzutem poziomym
		 else
		{
			 return correctParams(startingYPos - (G_CONST / 2.) * (time * time));
		}
	 }

	static double timeOfThrow(double Vy, double startingYPos)
 {
	 if (!startingYPos)
		 return correctParams((2. * Vy) / G_CONST);
	 else
		 return correctParams(sqrt(2. * startingYPos / G_CONST));
 }

	static double timeToReachMaxHeigth(double Vy, double startingYPos)
 {
	 if (!startingYPos)
		 return correctParams(Vy / G_CONST);
	 else
		 return 0;
 }

	static double range(double v, double angle, double startingYPosInM)
 {
	 if (!startingYPosInM)
		 return correctParams(((v * v) / G_CONST) * qSin(2 * angle * M_PI / 180.));
	 else
		 return correctParams(v * sqrt(2. *startingYPosInM / G_CONST));
 }

	static double maxHeigth(double Vy, double startingYPosInM)
 {
	 if (!startingYPosInM)
		 return correctParams((Vy * Vy) / (2 * G_CONST));
	 else
		 return correctParams(startingYPosInM);
 }
};



/*-----------------------------------------------------------------------*/

}
#endif // ENTITIES_H


