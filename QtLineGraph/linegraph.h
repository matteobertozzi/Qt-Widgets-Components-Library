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

#ifndef _THLINEGRAPH_H_
#define _THLINEGRAPH_H_

#include <QWidget>
class THLineGraphPrivate;

class THLineGraphData {
	public:
		THLineGraphData(const QString& xValue, const QString& yValue) {
			m_xValue = xValue;
			m_yValue = yValue;
		}

		~THLineGraphData() {
		}

		const QString& x (void) const { return(m_xValue); }
		const QString& y (void) const { return(m_yValue); }

		void setX (const QString& xValue) { m_xValue = xValue; }
		void setY (const QString& yValue) { m_yValue = yValue; }

	private:
		QString m_xValue;
		QString m_yValue;
};

class THLineGraph : public QWidget {
	Q_OBJECT

	public:
		THLineGraph (QWidget *parent = 0);
		~THLineGraph();

		void append (const THLineGraphData& data);
		void insert (int index, const THLineGraphData& data);

		bool save (const QString& fileName, const QSize& size);

		bool sortX (void) const;
		bool sortY (void) const;

		void setSortX (bool sortX);
		void setSortY (bool sortY);

		void setGraphBorder (const QColor& graphBorder);
		void setGraphBackground (const QColor& graphBg);
		void setTextColor (const QColor& textColor);
		void setGraphLine (const QColor& graphLine);
		void setBackground (const QColor& bgColor);
		void setBorder (const QColor& bgBorder);

	protected:
		void resizeEvent (QResizeEvent *event);
		void paintEvent (QPaintEvent *event);

	private:
		THLineGraphPrivate *d;
};

#endif /* !_THLINEGRAPH_H_ */

