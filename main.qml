import QtQuick 2.9
import QtQuick.Window 2.2

import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Model.CaseInfoModel 1.0
import Manager 1.0

import "qml"


Window {
    id:root
    visibility: Window.Maximized
    visible: true
    title: qsTr("Run TestCases")


    SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical

        Rectangle{
            color: "black"

            height: 6*root.height/10

            TextArea {
                id: monitorView

                anchors.fill: parent
                backgroundVisible: false
                textColor: "white"
                font.pointSize: 11

                readOnly: true

                function logMonitorText(s){
                    monitorView.insert(monitorView.length, s)
                    monitorView.cursorPosition = text.length
                }
            }
        }

        SplitView
        {
            id:subview
            orientation: Qt.Horizontal

            Rectangle{

                color: "darkcyan"
                width: subview.width/2

                TextArea {
                    id: caseView

                    anchors.fill: parent
                    backgroundVisible:false
                    textColor: "white"
                    font.pointSize: 11


                    signal textChangedSignal(string s)

                    onTextChanged: {
                        textChangedSignal(text)
                    }
                }
            }


            SplitView
            {
                id: rightView
                orientation: Qt.Vertical

                Rectangle{

                    color: "black"

                    Layout.fillHeight: true
                    Layout.maximumHeight: 4*rightView.height/5
                    Layout.minimumHeight: 4*rightView.height/5

                    TextArea {
                        id: logView

                        anchors.fill: parent
                        backgroundVisible: false
                        textColor: "white"
                        font.pointSize: 10

                        readOnly: true

                        function logText(s){
                            logView.insert(logView.length, s)
                            logView.cursorPosition = text.length
                        }
                    }
                }

                Rectangle {
                    id:rect1

                    width: rightView.width

                    ColumnLayout{
                        spacing: 2

                        anchors.fill:parent

                        TextField{
                            id: rootPath
                            placeholderText: "Root Directory"

                            Layout.preferredWidth: rightView.width

                            style: TextFieldStyle {
                                textColor: "white"
                                placeholderTextColor: "gray"
                                background: Rectangle {
                                    color:"black"
                                }
                            }
                        }

                        RowLayout{
                            Button{
                                id:runButton
                                Layout.preferredHeight: 20
                                Layout.leftMargin: 10
                                text: "run"

                                function enable(b){
                                    enabled = b
                                }

                                onClicked: {
                                     manager.setRootDir(rootPath.text)
                                     manager.start()
                                }
                            }

                            Button{
                                id:stopButton
                                Layout.preferredHeight: 20
                                Layout.leftMargin: 10
                                text: "stop"
                                enabled: false

                                function enable(b){
                                    enabled = b
                                }

                                onClicked: {
                                     manager.stop()
                                }
                            }

                            Rectangle{
                                width: 50
                            }

                            OrderView{
                                Layout.rightMargin: 10
                                id: orderView
                            }
                        }

                    }

                }

            }

        }

    }


    CaseInfoModel{
        id:caseInfoModel;
    }

    Manager{
        id:manager
    }

    Component.onCompleted: {
        manager.addLogText.connect(logView.logText)
        manager.addMonitorText.connect(monitorView.logMonitorText)
        manager.setEnabledRunButton.connect(runButton.enable)
        manager.setEnabledStopButton.connect(stopButton.enable)
        caseView.textChangedSignal.connect(manager.onCaseTextChanged)

        manager.setDataMgr(caseInfoModel)
        caseInfoModel.loadData()
        rootPath.text = manager.getRootDir()
        caseView.text = manager.getCases()
    }

    onClosing:{
        console.log("close")
        manager.setRootDir(rootPath.text)
        caseInfoModel.saveData()
    }


}
