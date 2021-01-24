/*
 * Patience Deck is a collection of patience games.
 * Copyright (C) 2020-2021  Tomi Leppänen
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

import QtQuick 2.6
import Sailfish.Silica 1.0
import Patience 1.0

Page {
    id: page

    allowedOrientations: Orientation.All
    property bool isPortrait: orientation & Orientation.PortraitMask

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                //% "About"
                text: qsTrId("patience-me-about")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }

            MenuItem {
                //% "Select game"
                text: qsTrId("patience-me-select_game")
                onClicked: pageStack.push(Qt.resolvedUrl("SelectGame.qml"))
            }

            MenuItem {
                //% "Game options"
                text: qsTrId("patience-me-game_options")
                onClicked: pageStack.push(Qt.resolvedUrl("GameOptions.qml"))
            }
        }

        contentHeight: parent.height

        Toolbar {
            id: toolbar
            vertical: page.isLandscape
            z: 10
        }

        Item {
            id: tableContainer

            clip: page.isPortrait && (toolbar.expanded || toolbar.animating)
            x: page.isLandscape ? toolbar.width : 0
            y: page.isLandscape ? 0 : toolbar.height
            height: page.height - message.height - (page.isLandcape ? 0 : toolbar.height)
            width: page.width - (page.isLandscape ? toolbar.width : 0)

            Table {
                id: table

                enabled: Patience.state < Patience.GameOverState
                height: page.height - (page.isLandscape ? 0 : Theme.itemSizeLarge) - message.height
                width: page.width - (page.isLandscape ? Theme.itemSizeLarge : 0)
                minimumSideMargin: Theme.horizontalPageMargin
                horizontalMargin: isPortrait ? Theme.paddingSmall : Theme.paddingLarge
                maximumHorizontalMargin: Theme.paddingLarge
                verticalMargin: isPortrait ? Theme.paddingLarge : Theme.paddingSmall
                maximumVerticalMargin: Theme.paddingLarge
                Component.onCompleted: Patience.restoreSavedOrLoad("klondike.scm")
            }
        }

        Label {
            id: message
            anchors {
                left: page.isLandscape ? toolbar.right : parent.left
                right: parent.right
                bottom: parent.bottom
            }
            text: Patience.message
        }

        Loader {
            id: overlayLoader

            active: Patience.state === Patience.WonState || Patience.state === Patience.GameOverState
            source: "GameOverOverlay.qml"
            x: tableContainer.x
            y: tableContainer.y
            height: table.height
            width: table.width
            z: 5

            onActiveChanged: if (active) toolbar.expanded = false
        }
    }

    Connections {
        target: Patience
        onStateChanged: {
            if (Patience.state === Patience.LoadedState) {
                Patience.startNewGame()
            }
        }
    }
}
