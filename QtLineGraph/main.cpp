#include <QApplication>

#include "linegraph.h"

int main (int argc, char **argv) {
	QApplication app(argc, argv);

	THLineGraph w;
	w.append(THLineGraphData("1 May", "4"));
	w.append(THLineGraphData("2 May", "1"));
	w.append(THLineGraphData("3 May", "3"));
	w.append(THLineGraphData("4 May", "5"));
	w.append(THLineGraphData("5 May", "2"));
	w.append(THLineGraphData("6 May", "6"));
	w.append(THLineGraphData("7 May", "2"));
	w.append(THLineGraphData("8 May", "2"));
	w.append(THLineGraphData("9 May", "2"));
	w.append(THLineGraphData("10 May", "7"));
	w.setSortY(true);

	w.save("QtLineGraph.png", QSize(700, 420));

	w.show();

	return(app.exec());
}

