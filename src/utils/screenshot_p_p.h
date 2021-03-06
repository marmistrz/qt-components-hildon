/*
 * Copyright (C) 2014 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef SCREENSHOT_P_P_H
#define SCREENSHOT_P_P_H

#include "screenshot_p.h"
#include <QPixmap>

class ScreenShotPrivate
{

public:
    ScreenShotPrivate(ScreenShot *parent);
    virtual ~ScreenShotPrivate();

    QString getFileName();

    ScreenShot *q_ptr;

    QWidget *target;

    QString fileName;

    bool overwrite;

    bool smooth;

    int width;
    int height;

    int targetX;
    int targetY;
    int targetWidth;
    int targetHeight;

    QPixmap pixmap;

    Q_DECLARE_PUBLIC(ScreenShot)
};

#endif // SCREENSHOT_P_P_H
