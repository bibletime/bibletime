/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTM_CONFIG_H
#define BTM_CONFIG_H

#include <QObject>
#include <QColor>

namespace btm {

class BtmConfig : public QObject {
    Q_OBJECT

    Q_PROPERTY(QStringList availableBibles
               READ  getAvailableBibles
               NOTIFY availableBiblesChanged)

    Q_PROPERTY(QStringList availableGreekStrongsLexicons
               READ  getAvailableGreekStrongsLexicons
               NOTIFY availableGreekStrongsLexiconsChanged)

    Q_PROPERTY(QStringList availableHebrewStrongsLexicons
               READ getAvailableHebrewStrongsLexicons
               NOTIFY availableHebrewStrongsLexiconsChanged)

    Q_PROPERTY(QStringList availableGreekMorphLexicons
               READ  getAvailableGreekMorphLexicons
               NOTIFY availableGreekMorphLexiconsChanged)

    Q_PROPERTY(QStringList availableHebrewMorphLexicons
               READ getAvailableHebrewMorphLexicons
               NOTIFY availableHebrewMorphLexiconsChanged)

public:

    BtmConfig();

    Q_INVOKABLE QString getDefaultSwordModuleByType(const QString& type);
    Q_INVOKABLE void setDefaultSwordModuleByType(const QString& type, const QString& moduleName);
    Q_INVOKABLE qreal getReal(const QString& settingName, qreal defaultValue);
    Q_INVOKABLE void setReal(const QString& settingName, qreal value);
    Q_INVOKABLE void initializeAvailableModules();

    QStringList getAvailableBibles();
    QStringList getAvailableGreekStrongsLexicons();
    QStringList getAvailableHebrewStrongsLexicons();
    QStringList getAvailableGreekMorphLexicons();
    QStringList getAvailableHebrewMorphLexicons();

signals:

    void availableBiblesChanged();
    void availableGreekStrongsLexiconsChanged();
    void availableHebrewStrongsLexiconsChanged();
    void availableGreekMorphLexiconsChanged();
    void availableHebrewMorphLexiconsChanged();

private:

    void configModuleByType(const QString& type, const QStringList& availableModuleNames);
    void lookupAvailableModules();
    void updateDefaultModules();

    bool m_availableModulesReady;

    QStringList m_availableBibles;
    QStringList m_availableGreekStrongsLexicons;
    QStringList m_availableHebrewStrongsLexicons;
    QStringList m_availableGreekMorphLexicons;
    QStringList m_availableHebrewMorphLexicons;
};

} // end namespace

#endif
