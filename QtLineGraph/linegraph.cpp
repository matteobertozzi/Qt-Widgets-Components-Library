/* 
 * Copyright (C) 2009 Matteo Bertozzi.
 *
 * This file is part of Thasis.
 * 
 * Thasis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Thasis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Thasis.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cmath>

#include <QPaintEvent>
#include <QPainter>
#include <QDebug>

#include "linegraph.h"

/* ============================================================================
 *  PRIVATE Consts
 */
#define GRAPH_MARGIN_Y			(10)
#define GRAPH_MARGIN_X			(10)
#define GRAPH_MARGIN_AREA		(5)
#define GRAPH_VALUE_HEIGHT		(20)
#define GRAPH_VALUE_XPAD		(18)
#define GRAPH_VALUE_YPAD		(14)
#define GRAPH_POINT_SIZE		(10)

/* ============================================================================
 *  PRIVATE Methods
 */
static bool caseInsensitiveGreatThan (const QString& s1, const QString& s2) {
	bool ok1, ok2;
	double d1, d2;

	d1 = s1.toDouble(&ok1);
	d2 = s2.toDouble(&ok2);

	return((ok1 && ok2) ? d1 > d2 : s1.toLower() > s2.toLower()); 
}

static bool caseInsensitiveLessThan (const QString& s1, const QString& s2) {
	bool ok1, ok2;
	double d1, d2;

	d1 = s1.toDouble(&ok1);
	d2 = s2.toDouble(&ok2);

	return((ok1 && ok2) ? d1 < d2 : s1.toLower() < s2.toLower()); 
}

/* ============================================================================
 *  PRIVATE Class
 */
class THLineGraphPrivate {
	public:
		QList<THLineGraphData> data;
		QColor graphBorder;
		QColor textColor;
		QColor graphLine;
		QColor bgBorder;
		QColor graphBg;
		QColor bgColor;
		QImage buffer;
		bool sortX;
		bool sortY;

	public:
		QStringList yValues (int visibleVals) const;
		QStringList xValues (int visibleVals) const;
		QPoint graphPoint (	int index, 
							const QStringList& xVals,
							const QStringList& yVals,
							int xVisibleVals, int yVisibleVals,
							int xSpacing, int ySpacing);

		void valuesSizes (const QFontMetrics& fontMetrics, 
						  QSize *xSize, QSize *ySize) const;

		void clearBuffer (void);		

		void render (const QSize& size);
		void render (QPainter *p, const QSize& size);

		bool save (const QString& fileName, const QSize& size);
};

bool THLineGraphPrivate::save (const QString& fileName, const QSize& size) {
	QImage img(size, QImage::Format_ARGB32_Premultiplied);

	QPainter p(&img);
	p.setRenderHint(QPainter::Antialiasing);
	render(&p, size);
	p.end();

	return(img.save(fileName));
}

QStringList THLineGraphPrivate::yValues (int visibleVals) const {
	QStringList yVals;

	for (int i = 0; i < data.size(); ++i) {
		QString yItem = data.at(i).y();
		if (!yVals.contains(yItem)) yVals.append(yItem);
	}

	if (sortY) qSort(yVals.begin(), yVals.end(), caseInsensitiveGreatThan);

	int rmStep = (int)ceil(qreal(yVals.size()) / visibleVals);
	if (rmStep > 1) {
		QStringList yKeep;
		for (int i = 0; i < yVals.size(); i += rmStep)
			yKeep.append(yVals.at(i));

		if (yKeep.last() != yVals.last()) {
			if (yKeep.size() >= visibleVals)
				yKeep.removeLast();
			yKeep.append(yVals.last());
		}

		return(yKeep);
	}

	return(yVals);
}

QStringList THLineGraphPrivate::xValues (int visibleVals) const {
	QStringList xVals;

	for (int i = 0; i < data.size(); ++i) {
		QString xItem = data.at(i).x();
		if (!xVals.contains(xItem)) xVals.append(xItem);
	}

	if (sortX) qSort(xVals.begin(), xVals.end(), caseInsensitiveLessThan);

	int rmStep = (int)ceil(qreal(xVals.size()) / visibleVals);
	if (rmStep > 1) {
		QStringList xKeep;
		for (int i = 0; i < xVals.size(); i += rmStep)
			xKeep.append(xVals.at(i));

		if (xKeep.last() != xVals.last()) {
			if (xKeep.size() >= visibleVals)
				xKeep.removeLast();
			xKeep.append(xVals.last());
		}

		return(xKeep);
	}

	return(xVals);
}

QPoint THLineGraphPrivate::graphPoint (	int index, 
										const QStringList& xVals,
										const QStringList& yVals,
										int xVisibleVals, int yVisibleVals,
										int xSpacing, int ySpacing)
{
	int xIndex = xVals.indexOf(data.at(index).x());
	int yIndex = yVals.indexOf(data.at(index).y());

	qreal xFactor = qreal(xVisibleVals) / xVals.size();
	qreal yFactor = qreal(yVisibleVals) / yVals.size();

	int x = xIndex * (xFactor * xSpacing);
	int y = yIndex * (yFactor * ySpacing);
	return(QPoint(x + xFactor * (xSpacing / 2), y + yFactor * (ySpacing / 2)));
}

void THLineGraphPrivate::valuesSizes (const QFontMetrics& fontMetrics, 
									  QSize *xSize, QSize *ySize) const
{
	int width;

	xSize->setWidth(0);
	ySize->setWidth(0);
	xSize->setHeight(fontMetrics.xHeight());
	ySize->setHeight(fontMetrics.xHeight());
	for (int i = 0; i < data.size(); ++i) {
		if ((width = fontMetrics.width(data.at(i).x())) > xSize->width())
			xSize->setWidth(width);

		if ((width = fontMetrics.width(data.at(i).y())) > ySize->width())
			ySize->setWidth(width);
	}

	// Setup Padding
	xSize->setHeight(xSize->height() + GRAPH_VALUE_YPAD);
	ySize->setHeight(ySize->height() + GRAPH_VALUE_YPAD);
	xSize->setWidth(xSize->width() + GRAPH_VALUE_XPAD);
	ySize->setWidth(ySize->width() + GRAPH_VALUE_XPAD);
}

void THLineGraphPrivate::clearBuffer (void) {
	if (!buffer.isNull())
		buffer = QImage();
}

void THLineGraphPrivate::render (const QSize& size) {
	buffer = QImage(size, QImage::Format_ARGB32_Premultiplied);

	QPainter p(&buffer);
	p.setRenderHint(QPainter::Antialiasing);
	render(&p, size);
	p.end();
}

void THLineGraphPrivate::render (QPainter *p, const QSize& size) {
	// Draw Background
	p->setBrush(bgColor);
	p->setPen(bgBorder);
	p->drawRect(QRect(QPoint(0, 0), size));

	// Setup Font
	p->setFont(QFont(p->font().family(), 12));

	// Calculate X and Y "Header" Size
	QSize xSize, ySize;
	valuesSizes(p->fontMetrics(), &xSize, &ySize);

	int height = size.height() - (GRAPH_MARGIN_Y * 2);
	int width = size.width() - (GRAPH_MARGIN_X * 2);
	int yVisibleItems = qMin(data.size(), (height - xSize.height())/ ySize.height());
	int xVisibleItems = qMin(data.size(), (width - ySize.width()) / xSize.width());

	QStringList yVals = yValues(yVisibleItems);
	QStringList xVals = xValues(xVisibleItems);
	yVisibleItems = yVals.size();
	xVisibleItems = xVals.size();

	int areaHeight = height - ySize.height();
	int areaWidth = width - ySize.width();
	int startX = ySize.width() + GRAPH_MARGIN_AREA;
	int startY = GRAPH_MARGIN_Y;
	int ySpacing = (yVisibleItems > 0) ? (areaHeight / yVisibleItems) : 0;
	int xSpacing = (xVisibleItems > 0) ? (areaWidth / xVisibleItems) : 0;

	// Draw Area
	p->setPen(Qt::NoPen);
	p->setBrush(graphBg);
	p->drawRect(startX, startY, areaWidth, areaHeight);

	// Draw Area Border
	p->setPen(graphBorder);
	p->setBrush(Qt::transparent);
	p->drawLine(startX, startY, startX, startY + areaHeight);

	// Draw Y "Header" and Line
	for (int i = 0; i < yVisibleItems; ++i) {
		int y = (i * ySpacing);

		p->setPen(textColor);
		p->drawText(0, y, ySize.width(), ySpacing, 
					Qt::AlignVCenter | Qt::AlignRight,
					yVals.at(i));

		y += (ySpacing / 2);
		p->setPen(graphBorder);
		p->drawLine(startX, y, startX + areaWidth, y);
	}

	// Draw X "Header"
	for (int i = 0; i < xVisibleItems; ++i) {
		p->setPen(textColor);
		p->drawText(startX + (i * xSpacing), 
					startY + areaHeight + GRAPH_MARGIN_AREA, 
					xSpacing, xSize.height(), 
					Qt::AlignCenter, xVals.at(i));
	}

	if (data.size() > 0) {
		QList<QPoint> pointList;
		QPoint point;

		xVals = xValues(data.size());
		yVals = yValues(data.size());

		// Draw Line Graph
		QPainterPath linePath;

		point = QPoint(startX, 0) + graphPoint( 0, xVals, yVals,
												xVisibleItems, yVisibleItems, 
												xSpacing, ySpacing);
		pointList.append(point);
		linePath.moveTo(point);

		for (int i = 1; i < data.size(); ++i) {
			point = QPoint(startX, 0) + graphPoint( i, xVals, yVals,
												xVisibleItems, yVisibleItems, 
												xSpacing, ySpacing);
			pointList.append(point);
			linePath.lineTo(point);
		}

		p->setPen(QPen(graphLine, 3));
		p->setBrush(Qt::transparent);
		p->setClipRegion(QRegion(startX, startY, areaWidth, areaHeight));
		p->drawPath(linePath);

		// Draw Item Points
		p->setPen(Qt::NoPen);
		p->setBrush(graphLine);
		for (int i = 0; i < pointList.size(); ++i) {
			point = pointList.at(i) - QPoint(GRAPH_POINT_SIZE / 2, GRAPH_POINT_SIZE / 2);
			p->drawEllipse(QRect(point, QSize(GRAPH_POINT_SIZE, GRAPH_POINT_SIZE)));
		}
	}
}

/* ============================================================================
 *  PUBLIC Constructors/Destructors
 */
THLineGraph::THLineGraph (QWidget *parent)
	: QWidget(parent), d(new THLineGraphPrivate)
{
	d->sortX = d->sortY = false;
	d->graphBorder = QColor(0x86, 0x86, 0x86);
	d->textColor = QColor(0xFF, 0xFF, 0xFF);
	d->graphLine = QColor(0xf6, 0x92, 0x40);
	d->graphBg = QColor(0x3f, 0x3f, 0x3f);
	d->bgBorder = QColor(0xFF, 0xFF, 0xFF);
	d->bgColor = QColor(0x00, 0x00, 0x00);
}

THLineGraph::~THLineGraph() {
	delete d;
}

/* ============================================================================
 *  PUBLIC Methods
 */
void THLineGraph::append (const THLineGraphData& data) {
	d->data.append(data);
	d->clearBuffer();
}

void THLineGraph::insert (int index, const THLineGraphData& data) {
	d->data.insert(index, data);
	d->clearBuffer();
}

bool THLineGraph::save (const QString& fileName, const QSize& size) {
	return(d->save(fileName, size));
}

bool THLineGraph::sortX (void) const {
	return(d->sortX);
}

bool THLineGraph::sortY (void) const {
	return(d->sortY);
}

void THLineGraph::setSortX (bool sortX) {
	d->sortX = sortX;
	d->clearBuffer();
}

void THLineGraph::setSortY (bool sortY) {
	d->sortY = sortY;
	d->clearBuffer();
}

void THLineGraph::setGraphBorder (const QColor& graphBorder) {
	d->graphBorder = graphBorder;
	d->clearBuffer();
}

void THLineGraph::setGraphBackground (const QColor& graphBg) {
	d->graphBg = graphBg;
	d->clearBuffer();
}

void THLineGraph::setTextColor (const QColor& textColor) {
	d->textColor = textColor;
	d->clearBuffer();
}

void THLineGraph::setGraphLine (const QColor& graphLine) {
	d->graphLine = graphLine;
	d->clearBuffer();
}

void THLineGraph::setBackground (const QColor& bgColor) {
	d->bgColor = bgColor;
	d->clearBuffer();
}

void THLineGraph::setBorder (const QColor& bgBorder) {
	d->bgBorder = bgBorder;
	d->clearBuffer();
}

/* ============================================================================
 *  PROTECTED Methods
 */
void THLineGraph::resizeEvent (QResizeEvent *event) {
	d->clearBuffer();

	QWidget::resizeEvent(event);
}

void THLineGraph::paintEvent (QPaintEvent *event) {
	if (d->buffer.isNull())
		d->render(event->rect().size());

	QPainter p(this);
	p.setRenderHint(QPainter::SmoothPixmapTransform);
	p.drawImage(event->rect(), d->buffer, event->rect());
	p.end();

	QWidget::paintEvent(event);
}

