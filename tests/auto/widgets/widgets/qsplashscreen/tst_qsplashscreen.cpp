/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QTest>
#include <QSplashScreen>

class tst_QSplashScreen : public QObject
{
    Q_OBJECT

private slots:
    void checkCloseTime();
    void checkScreenConstructor();
};

class CloseEventSplash : public QSplashScreen
{
public:
    CloseEventSplash(const QPixmap &pix) : QSplashScreen(pix), receivedCloseEvent(false) {}
    bool receivedCloseEvent;
protected:
    void closeEvent(QCloseEvent *event)
    {
        receivedCloseEvent = true;
        QSplashScreen::closeEvent(event);
    }
};

void tst_QSplashScreen::checkCloseTime()
{
    QPixmap pix(100, 100);
    pix.fill(Qt::red);
    CloseEventSplash splash(pix);
    QVERIFY(!splash.receivedCloseEvent);
    QWidget w;
    splash.show();
    QTimer::singleShot(500, &w, SLOT(show()));
    QVERIFY(!splash.receivedCloseEvent);
    splash.finish(&w);
    QVERIFY(splash.receivedCloseEvent);
    // We check the window handle because if this is not valid, then
    // it can't have been exposed
    QVERIFY(w.windowHandle());
    QVERIFY(w.windowHandle()->isExposed());
}

void tst_QSplashScreen::checkScreenConstructor()
{
    for (const auto screen : QGuiApplication::screens()) {
        QSplashScreen splash(screen);
        splash.show();
        QCOMPARE(splash.screen(), screen);
        QVERIFY(splash.windowHandle());
        QCOMPARE(splash.windowHandle()->screen(), screen);
    }
}

QTEST_MAIN(tst_QSplashScreen)
#include "tst_qsplashscreen.moc"
