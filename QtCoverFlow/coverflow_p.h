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

#ifndef _THCOVERFLOW_PRIVATE_H_
#define _THCOVERFLOW_PRIVATE_H_

#include <QRunnable>
#include <QObject>
#include <QImage>
#include <QDir>

class THCoverFlowImageLoader : public QObject, public QRunnable {
	Q_OBJECT

	public:
		THCoverFlowImageLoader (const QImage& image);
		THCoverFlowImageLoader (const QString& path);
		~THCoverFlowImageLoader();

		void run (void);

	Q_SIGNALS:
		void completed (const QImage& image);

	private:
		QString m_path;
		QImage m_image;
};

class THCoverFlowItem : public QObject {
	Q_OBJECT

	public:
		THCoverFlowItem (	const QString& title,
							const QString& path, 
							QObject *parent = 0);
		~THCoverFlowItem();

		const QImage *image (void) const;
		const QString& path (void) const;
		const QString& title (void) const;

	Q_SIGNALS:
		void imageChanged (void);

	public Q_SLOTS:
		void setImage (const QImage& image);

	private:
		QString m_title;
		QString m_path;
		QImage m_image;
};

class THCoverFlowItemsLoader : public QObject, public QRunnable {
	Q_OBJECT

	public:
		THCoverFlowItemsLoader (const QDir& directory);
		~THCoverFlowItemsLoader();

		void run (void);

	Q_SIGNALS:
		void completedItem (const QString& path);

	private:
		QDir m_dir;
};

#endif /* !_THCOVERFLOW_PRIVATE_H_ */

