import QtQuick            2.0
import QtQuick.Layouts    1.12
import QtQuick.Dialogs    1.3

Rectangle {
    id:                           _root
    width:                        (80*_columnCount)+80
    height:                       (50*_rowCount)+60+(50 * 2)
    color:                        "white"

    property int _rowCount:       10
    property int _columnCount:    10
    property int _gridSize:       _rowCount * _columnCount
    property int _mineCount:      Math.floor(_gridSize * 0.3)
    property int _winScore:       _gridSize - _mineCount
    property int _lastButton:     -1
    property int _score:          0
    property bool _firstClick:    false
    property var _mines:          []
    property int _maxPower1:      1
    property int _power1:         0
    property int _maxPower2:      3
    property int _power2:         0

    property alias _repeater:   gridContainer.repeater

    function resetGame() {
        _firstClick = false;
        _score = 0;
        _mines = [];
        _lastButton = -1;
        _power1 = 0;
        _power2 = 0;

        for(let i = 0; i < _gridSize; ++i) {
            _repeater.itemAt(i)._clicked = false;
            _repeater.itemAt(i)._text = "";
            _repeater.itemAt(i).color = "lightgray";
        }
    }

    // Given a cell, get all neighbouring cells
    function getSurroundingCells(lastBtn) {
        let x = Math.floor(lastBtn / _columnCount);
        let y = lastBtn % _columnCount;
        let cells = []

        for (let i = -1; i < 2; ++i) {
            for (let j = -1; j < 2; ++j) {
                if(x+i >= 0 && x+i < _rowCount && y+j >=0 && y+j < _columnCount) {
                    cells.push([x+i, y+j]);
                }
            }
        }
        return cells;
    }

    MessageDialog {
        id:      gameOverDialog
        title:   "Game Over!!"
        text:    "BOOM!!. Game Over. Your Score is %1/%2.".arg(_score).arg(_winScore)
        onAccepted: {
            resetGame();
        }
    }

    MessageDialog {
        id:     winDialog
        title:  "Congratulation!!"
        text:   "Congratulations. You won the game";
        onAccepted: {
            resetGame();
        }
    }

    Rectangle {
        id: header
        anchors.left: parent.left
        anchors.top: parent.top
        width: parent.width
        height: 50
        color: "lightgray"

        RowLayout {
            anchors.fill: parent
            Rectangle {
                id:                     p1Label
                Layout.fillWidth:       true
                Layout.minimumWidth:    _root.width * .3
                Layout.preferredWidth:  _root.width * .3
                Layout.maximumWidth:    _root.width * .3
                Layout.minimumHeight:   45
                color:                  "lightgray"
                border.color:           Qt.lighter(color)
                Text {
                    anchors.centerIn:   parent
                    font.bold:          true
                    font.pixelSize:     18
                    color:              "green"
                    text:               qsTr("Power-1: %1/%2".arg(_power1).arg(_maxPower1))
                }
            }

            Rectangle {
                Layout.minimumWidth:    _root.width * .4
                Layout.preferredWidth:  _root.width * .4
                Layout.maximumWidth:    _root.width * .4
                Layout.minimumHeight:   45
                color:                  "lightgray"
                border.color:           Qt.lighter(color)
                Text {
                    anchors.centerIn:  parent
                    font.bold:         true
                    font.pixelSize:    24
                    color:             "blue"
                    text:              qsTr("Score: %1/%2".arg(_score).arg(_winScore));
                }
            }

            Rectangle {
                Layout.minimumWidth:    _root.width * .3
                Layout.preferredWidth:  _root.width * .3
                Layout.maximumWidth:    _root.width * .3
                Layout.minimumHeight:   45
                color:                  "lightgray"
                border.color:           Qt.lighter(color)
                Text {
                    anchors.centerIn:   parent
                    font.bold:          true
                    font.pixelSize:     18
                    color:              "green"
                    text:               qsTr("Power-2: %1/%2".arg(_power2).arg(_maxPower2))
                }

            }

        }
    }

    Rectangle {
        id:            gridContainer
        anchors.left:  parent.left
        anchors.top:   header.bottom

        width:         parent.width
        height:        parent.height - 100

        property alias repeater: grid._repeater

        GridLayout {
            id:                grid
            anchors.fill:      parent
            columns:           _columnCount
            Layout.rowSpan:    1
            Layout.columnSpan: 1

            property alias _repeater: repeater

            Repeater {
                id:        repeater
                model:     _gridSize
                Rectangle {
                    Layout.fillWidth:    true
                    Layout.fillHeight:   true
                    color:               _clicked ? "blue" : "lightgray"
                    border.color:        "black"

                    property bool _clicked:   false
                    property int _index:      index
                    property string _color:   "blue"
                    property string _text:     ""

                    Text {
                        anchors.centerIn: parent
                        text:             _text
                        font.pointSize:   15
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if(!_clicked) {
                                _clicked = true
                                let x = Math.floor(_index / _columnCount)
                                let y = _index % _columnCount

                                if(!_firstClick) {
                                    _firstClick = true;
                                    // polpulate the _mines
                                    for(let i = 0; i < _gridSize; ++i){
                                        _mines.push(0);
                                    }

                                    // 30% of cells have _mines
                                    let totalMines = _mineCount;
                                    while(totalMines) {
                                        let rx = Math.floor(Math.random() * _gridSize);
                                        if(rx === index || _mines[rx] === -1)
                                            continue;
                                        _mines[rx] = -1;
                                        --totalMines;
                                    }
                                }

                                if(_mines[index] === -1) {
                                    repeater.itemAt(index).color = "red";
                                    console.log("BOOM!! Game Over.");
                                    gameOverDialog.visible = true;
                                    return;
                                }

                                if(_lastButton !== -1) {
                                    // clear previous neighbours
                                    let c = getSurroundingCells(_lastButton);
                                    for(let j = 0; j < c.length; ++j) {
                                        const [x, y] = c[j]
                                        repeater.itemAt(x*_columnCount+y).color = "lightgray";
                                    }
                                }

                                // highlight neighbours
                                let cells = getSurroundingCells(index);
                                for(let i = 0; i < cells.length; ++i) {
                                    const [x, y] = cells[i];
                                    repeater.itemAt(x*_columnCount+y).color = "cyan";
                                }
                                repeater.itemAt(index).color = "blue";

                                // count neighbouring mines
                                let nMineCells = getSurroundingCells(index);
                                let mineCount = 0;
                                for(let i = 0; i < nMineCells.length; ++i) {
                                    const [x, y] = nMineCells[i];
                                    if(_mines[x*_columnCount+y] === -1)
                                        mineCount++;
                                }

                                _score++;
                                _text = mineCount.toString();
                                _lastButton = index;
                                if(_score === _winScore) {
                                    winDialog.visible = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        anchors.left:          parent.left
        anchors.bottom:        parent.bottom
        anchors.leftMargin:    2
        anchors.bottomMargin:  2

        width:                 parent.width/2 - 4
        height:                45
        color:                (_power1 < _maxPower1) ? "lightgray" : "darkgray"
        Text {
            anchors.centerIn:  parent
            font.bold:         true
            font.pixelSize:    16
            text:              qsTr("All Mines")
        }
        MouseArea {
            anchors.fill:      parent
            onPressed: {
                if(!_firstClick || _power1 > _maxPower1) return;

                ++_power1;
                for(let i = 0; i < _gridSize; ++i) {
                    if(_mines[i] === -1)
                        _repeater.itemAt(i).color = "red";
                }
            }
            onReleased: {
                if(!_firstClick) return;

                for(let i = 0; i < _gridSize; ++i) {
                    if(_mines[i] === -1)
                        _repeater.itemAt(i).color = "lightgray";
                }
                // color neighbouring cell cyan
                let cells = getSurroundingCells(_lastButton);
                for(let j = 0; j < cells.length; ++j) {
                    let [x, y] = cells[j];
                    let idx = x*_columnCount+y;
                    if(idx !== _lastButton) {
                        _repeater.itemAt(idx).color = "cyan";
                    }
                }
            }
        }
    }

    Rectangle {
        anchors.right:         parent.right
        anchors.bottom:        parent.bottom
        anchors.rightMargin:   2
        anchors.bottomMargin:  2

        width:                 parent.width/2 - 4
        height:                45
        color:                (_power2 < _maxPower2) ? "lightgray" : "darkgray"
        Text {
            anchors.centerIn:  parent
            font.bold:         true
            font.pixelSize:    16
            text:              qsTr("Surrounding Mines")
        }
        MouseArea {
            anchors.fill:      parent
            onPressed: {
                if(!_firstClick || _power2 > _maxPower2) return;

                ++_power2;
                let cells = getSurroundingCells(_lastButton);
                for(let i = 0; i < cells.length; ++i) {
                    let [x, y] = cells[i];
                    if(_mines[x*_columnCount+y] === -1)
                        _repeater.itemAt(x*_columnCount+y).color = "red";
                }
                            }
            onReleased: {
                if(!_firstClick) return;
                let cells = getSurroundingCells(_lastButton);
                for(let i = 0; i < cells.length; ++i) {
                    const [x, y] = cells[i];
                    if(_mines[x*_columnCount+y] === -1)
                        _repeater.itemAt(x*_columnCount+y).color = "cyan";
                }
            }
        }
    }
}
