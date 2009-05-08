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

#include <QApplication>
#include "coverflow.h"

int main (int argc, char **argv) {
	QApplication app(argc, argv);

	QDir wallDirs("photos/");
	wallDirs.setFilter(QDir::Files | QDir::NoSymLinks | QDir::Readable);
	wallDirs.setNameFilters(QStringList() << "*.png" << "*.jpg" << "*.jpeg");

	THCoverFlow coverFlow;
	coverFlow.addItems(wallDirs);
	coverFlow.show();

	return(app.exec());
}

