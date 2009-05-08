/* 
 * Copyright (C) 2009 Matteo Bertozzi.
 *
 * This file is part of MokoTouch.
 * 
 * MokoTouch is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * MokoTouch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with MokoTouch.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QDirIterator>
#include <QImageReader>
#include <QThreadPool>
#include <QPaintEvent>
#include <QFileInfo>
#include <QPainter>
#include <QCache>

#include "coverflow_p.h"
#include "coverflow.h"

#define COVERFLOW_IMAGE_HEIGHT		(200)
#define COVERFLOW_IMAGE_WIDTH		(250)

/* ============================================================================
 *  PRIVATE Methods
 */
static QImage *mirrorImage (const QImage *image) {
	QImage *tmpImage = new QImage(image->mirrored(false, true));

	QPoint p1, p2;
	p2.setY(tmpImage->height());

	QLinearGradient gradient(p1, p2);
	gradient.setColorAt(0, QColor(0, 0, 0, 100));
	gradient.setColorAt(1, Qt::transparent);

	QPainter p(tmpImage);
	p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
	p.fillRect(0, 0, tmpImage->width(), tmpImage->height(), gradient);
	p.end();

	return(tmpImage);
}

/* ============================================================================
 *  PRIVATE CoverFlow Image Loader Class
 */
THCoverFlowImageLoader::THCoverFlowImageLoader (const QImage& image) 
	: QRunnable()
{
	m_image = image;
}

THCoverFlowImageLoader::THCoverFlowImageLoader (const QString& path) {
	m_path = path;
}

THCoverFlowImageLoader::~THCoverFlowImageLoader() {
}

void THCoverFlowImageLoader::run () {
	if (!m_path.isEmpty()) {
		QImageReader reader(m_path);
		reader.setScaledSize(QSize(COVERFLOW_IMAGE_WIDTH, COVERFLOW_IMAGE_HEIGHT));
		m_image = reader.read();
	} else if (!m_image.isNull()) {
		m_image = m_image.scaled(COVERFLOW_IMAGE_WIDTH, COVERFLOW_IMAGE_HEIGHT,
							Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	}

	emit completed(m_image);
}

/* ============================================================================
 *  PRIVATE CoverFlow Items Loader Class
 */
THCoverFlowItemsLoader::THCoverFlowItemsLoader (const QDir& directory) {
	m_dir = directory;
}

THCoverFlowItemsLoader::~THCoverFlowItemsLoader() {
}

void THCoverFlowItemsLoader::run (void) {
	QDirIterator dirIt(m_dir);
	while (dirIt.hasNext())
		emit completedItem(dirIt.next());
}

/* ============================================================================
 *  PRIVATE CoverFlowItem Class
 */
THCoverFlowItem::THCoverFlowItem (	const QString& title, 
									const QString& path,
									QObject *parent)
	: QObject(parent)
{
	m_path = path;
	m_title = title;
}

THCoverFlowItem::~THCoverFlowItem() {
}

const QImage *THCoverFlowItem::image (void) const {
	return(m_image.isNull() ? NULL : &m_image);
}

const QString& THCoverFlowItem::path (void) const {
	return(m_path);
}

const QString& THCoverFlowItem::title (void) const {
	return(m_title);
}

void THCoverFlowItem::setImage (const QImage& image) {
	m_image = image;
	emit imageChanged();
}

/* ============================================================================
 *  PRIVATE Class
 */
class THCoverFlowPrivate {
	public:
		QCache<qint64, QImage> mirrorCache;
		QList<THCoverFlowItem *> items;
		QImage emptyItem;
		int selected;

	public:
		THCoverFlowPrivate();

		const QImage *itemImage (int index);

	private:
		void initializeEmptyImage (void);
};

const QImage *THCoverFlowPrivate::itemImage (int index) {
	const QImage *img = items.at(index)->image();
	return(img != NULL ? img : &emptyItem);
}

THCoverFlowPrivate::THCoverFlowPrivate() {
	initializeEmptyImage();
	selected = 0;
}

void THCoverFlowPrivate::initializeEmptyImage (void) {
	emptyItem = QImage(	COVERFLOW_IMAGE_WIDTH, COVERFLOW_IMAGE_HEIGHT,
						QImage::Format_ARGB32_Premultiplied);

	QLinearGradient gradient(0, 0, 0, COVERFLOW_IMAGE_HEIGHT);
	gradient.setColorAt(0, Qt::black);
	gradient.setColorAt(1, QColor(0x55, 0x55, 0x55));

	QPainter p(&emptyItem);
	p.setBrush(gradient);
	p.setPen(QPen(QColor(0x40, 0x40, 0x40), 4));
	p.drawRect(emptyItem.rect());
	p.end();
}

/* ============================================================================
 *  PUBLIC Constructor/Destructor
 */
THCoverFlow::THCoverFlow (QWidget *parent)
	: QWidget(parent), d(new THCoverFlowPrivate)
{
	setAttribute(Qt::WA_OpaquePaintEvent);
}

THCoverFlow::~THCoverFlow() {
	delete d;
}

/* ============================================================================
 *  PUBLIC Slots (Add Item)
 */
void THCoverFlow::addItem (const QString& path) {
	QFileInfo fileInfo(path);
	addItem(fileInfo.baseName(), path);
}

void THCoverFlow::addItem (const QString& title, const QString& path) {
	// Create CoverFlow Image Loader
	THCoverFlowImageLoader *imageLoader = new THCoverFlowImageLoader(path);

	// Create CoverFlow Item
	THCoverFlowItem *item = new THCoverFlowItem(title, path, this);
	connect(imageLoader, SIGNAL(completed(const QImage&)),
			item, SLOT(setImage(const QImage&)));
	connect(item, SIGNAL(imageChanged()), this, SLOT(update()));

	// Add Item to List
	d->items.append(item);

	// Run CoverFlow Image Loader
	QThreadPool::globalInstance()->start(imageLoader);
}

void THCoverFlow::addItems (const QString& path) {
	addItems(QDir(path));
}

void THCoverFlow::addItems (const QDir& directory) {
	// Create CoverFlow Items Loader
	THCoverFlowItemsLoader *itemsLoader = new THCoverFlowItemsLoader(directory);
	connect(itemsLoader, SIGNAL(completedItem(const QString&)),
			this, SLOT(addItem(const QString&)));

	// Run CoverFlow Items Loader
	QThreadPool::globalInstance()->start(itemsLoader);
}


/* ============================================================================
 *  PUBLIC Slots (Show Items)
 */
void THCoverFlow::showNext (void) {
	if ((d->selected + 1) < d->items.size()) {
		d->selected++;
		update();
	}
}

void THCoverFlow::showPrevious (void) {
	if (d->selected > 0) {
		d->selected--;
		update();
	}
}

void THCoverFlow::showAtIndex (int index) {
	if (index >= d->items.size())
		index = d->items.size() - 1;
	
	if (index < 0)
		index = 0;

	d->selected = index;
	update();
}

/* ============================================================================
 *  PROTECTED Methods
 */
void THCoverFlow::paintEvent (QPaintEvent *event) {
	QPainter p(this);

	// Draw Background
	p.setClipRect(event->rect());
	p.fillRect(rect(), Qt::black);

	// Set Painter Features
	p.setRenderHint(QPainter::SmoothPixmapTransform);
	p.setRenderHint(QPainter::Antialiasing);

	// Draw Items
	if (d->items.size() > 0) {
		drawItemsBeforeSelected(&p);
		drawItemsAfterSelected(&p);
		drawSelectedItem(&p);
	}

	p.end();
}

void THCoverFlow::mouseReleaseEvent (QMouseEvent *event) {
	const QImage *img = d->itemImage(d->selected);

	int x = (width() / 2) - img->width() / 2;
	if (event->x() >= x && event->x() <= (x + img->width())) {
		// You've Clicked on the Central Item
		qDebug("Central Item Clicked");
	} else if (event->x() < x) {
		showPrevious();
	} else {
		showNext();
	}
}

void THCoverFlow::keyPressEvent (QKeyEvent *event) {
	if (event->key() == Qt::Key_Left) {
		if (event->modifiers() == Qt::ControlModifier)
			showAtIndex(d->selected - 5);
		else
			showPrevious();
	} else if (event->key() == Qt::Key_Right) {
		if (event->modifiers() == Qt::ControlModifier)
			showAtIndex(d->selected + 5);
		else
			showNext();
	}
}

/* ============================================================================
 *  PRIVATE Methods
 */
void THCoverFlow::drawSelectedItem (QPainter *painter) {
	const THCoverFlowItem *item = d->items.at(d->selected);
	const QImage *img = d->itemImage(d->selected);

	int cw = width() / 2;
	int wh = height();
	int h = img->height();
	int w = img->width();

	// Draw Image
	drawItemAt(painter, cw, ((wh / 2) - (h / 4)), img, w, h, 0);

	painter->save();
	painter->setPen(Qt::white);

	// Draw Title
	painter->setFont(QFont(painter->font().family(), 12, QFont::Bold));
	int titleWidth = painter->fontMetrics().width(item->title());
	painter->drawText(cw - titleWidth / 2, wh - 30, item->title());

	// Draw Notes
	painter->setFont(QFont(painter->font().family(), 10, QFont::Normal));
	int notesWidth = painter->fontMetrics().width(item->path());
	painter->drawText(cw - notesWidth / 2, wh - 15, item->path());

	painter->restore();
}

void THCoverFlow::drawItemsAfterSelected (QPainter *painter) {
	const QImage *imgSelected = d->itemImage(d->selected);

	int winWidth = width();
	QPoint ptSelected(	(winWidth / 2) + (imgSelected->width() / 2),
						(height() / 2) - (imgSelected->height() / 4));

	int widthAvail = winWidth - ptSelected.x();
	int endIdx = (widthAvail / 40);
	int availItems = d->items.size() - (d->selected + 1);
	if (endIdx > availItems) endIdx = availItems;

	for (int i = 0, idx = d->selected + endIdx; i < endIdx; ++i, --idx) {
		const QImage *img = d->itemImage(idx);

		int x = ptSelected.x() + ((endIdx - i) * 40);
		drawItemAt(	painter, x, ptSelected.y(), img, 
					img->width() - 32, img->height() - 32, 55);
	}
}

void THCoverFlow::drawItemsBeforeSelected (QPainter *painter) {
	const QImage *imgSelected = d->itemImage(d->selected);

	QPoint ptSelected(	(width() / 2) - (imgSelected->width() / 2),
						(height() / 2) - (imgSelected->height() / 4));

	int widthAvail = ptSelected.x();
	int startIdx = d->selected - (widthAvail / 40);
	if (startIdx < 0) startIdx = 0;

	for (int i = 0, idx = startIdx; idx < d->selected; ++i, ++idx) {
		const QImage *img = d->itemImage(idx);

		int x = widthAvail - ((d->selected - idx) * 40);
		drawItemAt(	painter, x, ptSelected.y(), img, 
					img->width() - 32, img->height() - 32, -55);
	}
}

void THCoverFlow::drawItemAt (	QPainter *painter, int x, int y,
								const QImage *img, int w, int h, int angle)
{
	painter->save();

	// Get Image Reflection
	qint64 imageKey = img->cacheKey();
	QImage *mirrorImg;

	if (d->mirrorCache.contains(imageKey)) {
		mirrorImg = d->mirrorCache[imageKey];
	} else {
		mirrorImg = mirrorImage(img);
		d->mirrorCache.insert(imageKey, mirrorImg);
	}

	// Setup Image Transform
	QTransform transform;
	transform.scale(qreal(w) / img->width(), qreal(h) / img->height());
	transform.rotate(angle, Qt::YAxis);
	painter->setTransform(transform * QTransform().translate(x, y), true);

	QPointF pt(-img->width() / 2, -img->height() / 2);
	QPointF pt2(pt.x(), img->height() / 2);

	// Draw Image Reflection
	painter->save();
	painter->setCompositionMode(QPainter::CompositionMode_Source);
	painter->drawImage(pt2, *mirrorImg);
	painter->restore();

	// Draw Image
	painter->drawImage(pt, *img);

	painter->restore();
}

