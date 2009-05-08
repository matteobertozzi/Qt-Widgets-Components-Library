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

#ifndef _THCOVERFLOW_H_
#define _THCOVERFLOW_H_

#include <QWidget>
#include <QDir>

class THCoverFlowPrivate;

class THCoverFlow : public QWidget {
	Q_OBJECT

	public:
		THCoverFlow (QWidget *parent = 0);
		~THCoverFlow();

	public Q_SLOTS:
		// Add Items
		void addItem (const QString& path);
		void addItem (const QString& title, const QString& path);

		void addItems (const QString& path);
		void addItems (const QDir& directory);

		// Show Item
		void showNext (void);
		void showPrevious (void);
		void showAtIndex (int index);

	protected:
		void paintEvent (QPaintEvent *event);

		void mouseReleaseEvent (QMouseEvent *event);
		void keyPressEvent (QKeyEvent *event);

	private:
		void drawSelectedItem (QPainter *painter);
		void drawItemsAfterSelected (QPainter *painter);
		void drawItemsBeforeSelected (QPainter *painter);

		void drawItemAt (	QPainter *painter, int x, int y,
							const QImage *img, int w, int h, int angle);

	private:
		THCoverFlowPrivate *d;
};

#endif /* !_THCOVERFLOW_H_ */

