import QtQuick 2.0
import QtQml.Models 2.1
import QtQuick.Controls 1.4


Rectangle{
    id: root
    //color: "grey"

    width: 300; height:20

    GridView {
        id: grid

        cellWidth: parent.width/4; cellHeight: parent.height

        anchors.fill: parent

        model: DelegateModel {

            id: visualModel

            model:caseInfoModel

            delegate: MouseArea {
                id: delegateRoot

                property int visualIndex: DelegateModel.itemsIndex

                property int sourceIndex : 0

                width: grid.cellWidth; height: grid.cellHeight
                drag.target: icon

                Rectangle {
                    id: icon
                    width: grid.cellWidth-3; height: grid.cellHeight-3

                    border.color: "white"

                    anchors {
                        horizontalCenter: parent.horizontalCenter;
                        verticalCenter: parent.verticalCenter
                    }

                    CheckBox {
                        text: model.caseType
                        checked: model.caseRunning
                        onClicked: {
                            console.log(delegateRoot.visualIndex)
                            model.caseRunning = checked
                        }
                    }

                    Drag.active: delegateRoot.drag.active
                    Drag.source: delegateRoot
                    Drag.hotSpot.x: 25
                    Drag.hotSpot.y: 10

                    Drag.dragType: Drag.Automatic


                    Drag.onDragStarted: {
                      //console.log("Drag started..")

                      delegateRoot.sourceIndex = delegateRoot.visualIndex;

                      //console.log("source: "+delegateRoot.visualIndex)
                    }

                    Drag.onDragFinished: {
                      console.log("Drag finished!")

                      if(delegateRoot.sourceIndex != delegateRoot.visualIndex){
                            //console.log("destination: "+delegateRoot.visualIndex)

                            caseInfoModel.changed(delegateRoot.sourceIndex, delegateRoot.visualIndex)
                      }
                    }

                    states: [
                        State {
                            when: icon.Drag.active
                            ParentChange {
                                target: icon
                                parent: grid
                            }

                            AnchorChanges {
                                target: icon;
                                anchors.horizontalCenter: undefined;
                                anchors.verticalCenter: undefined
                            }
                        }
                    ]
                }

                DropArea {
                    anchors { fill: parent; margins: 5 }

                    onEntered: {
                        visualModel.items.move(drag.source.visualIndex, delegateRoot.visualIndex)
                    }

                }
            }
        }
    }
}


