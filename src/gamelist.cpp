/*
 * Patience Deck is a collection of patience games.
 * Copyright (C) 2020  Tomi Leppänen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <MGConfItem>
#include <QDir>
#include <QSet>
#include "gamelist.h"
#include "constants.h"

const QString ShowAllConf = QStringLiteral("/showAllGames");

/*
 * List of supported patience games.
 * These are tested and any bugs found must be addressed.
 */
QSet<QString> GameList::s_allowlist = {
    QStringLiteral("klondike"),
    QStringLiteral("freecell"),
    QStringLiteral("spider"),
    QStringLiteral("clock")
};

QHash<int, QByteArray> GameList::s_roleNames = {
    { Qt::DisplayRole, "display" },
    { FileNameRole, "filename" },
    { NameRole, "name" },
    { SupportedRole, "supported" }
};

GameList::GameList(QObject *parent)
    : QAbstractListModel(parent)
{
    bool showAll = GameList::showAll();
    QDir gameDirectory(Constants::GameDirectory);
    for (auto &entry : gameDirectory.entryList(QStringList() << QStringLiteral("*.scm"),
                                               QDir::Files | QDir::Readable, QDir::Name)) {
        if (showAll || isSupported(entry))
            m_games.append(entry);
    }
}

int GameList::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_games.count();
}

QVariant GameList::data(const QModelIndex &index, int role) const
{
    if (index.row() > m_games.count())
        return QVariant();

    switch (role) {
    case DisplayRole:
        return displayable(m_games[index.row()]);
    case FileNameRole:
        return m_games[index.row()];
    case NameRole:
        return name(m_games[index.row()]);
    case SupportedRole:
        return isSupported(m_games[index.row()]);
    default:
        return QVariant();
    }
}

QString GameList::displayable(const QString &fileName)
{
    QString displayName(name(fileName));
    bool startOfWord = true;
    for (int i = 0; i < displayName.length(); i++) {
        if (startOfWord) {
            displayName[i] = QChar(displayName[i]).toUpper();
            startOfWord = false;
        } else if (displayName[i] == QChar('-')) {
            startOfWord = true;
            displayName[i] = QChar(' ');
        }
    }
    return displayName;
}

QString GameList::name(const QString &fileName)
{
    return fileName.left(fileName.length()-4);
}

QHash<int, QByteArray> GameList::roleNames() const
{
    return s_roleNames;
}

bool GameList::isSupported(const QString &fileName)
{
    return s_allowlist.contains(name(fileName));
}

bool GameList::showAll()
{
    MGConfItem showAllConf(Constants::ConfPath + ShowAllConf);
    return showAllConf.value().toBool();
}

void GameList::setShowAll(bool show)
{
    MGConfItem showAllConf(Constants::ConfPath + ShowAllConf);
    showAllConf.set(show);
    showAllConf.sync();
}
