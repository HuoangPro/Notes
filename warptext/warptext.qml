import QtQuick 2.14
Rectangle {
	id: r
    width: t.width
    height: 600
    Text {
        id: t
        anchors.left: parent.left
        text: Qt.application.arguments[3]
        wrapMode: Text.WordWrap
        width: 432
        horizontalAlignment: Text.AlignHCenter
        // font configration, TODO
        font.pixelSize: 32
        font.family: "GenesisSansUI_JP_KR_Latin"
    	font.weight: Font.DemiBold
    }
    Text {
        id: tt
        opacity: 0
        // same font configuration with t, TODO
        font.pixelSize: 32
    }
    Component.onCompleted: {getLineOfWrappedText(); /*Qt.quit();*/}
    function getLineOfWrappedText() { let result = ""; let lines = t.text.split('\n'); for(const line of lines) { let words = line.split(' '); let newline = ""; for(const word of words) { tt.text = newline + (newline == "" ? "" : " ") + word; if (tt.contentWidth > t.width) { result = result + '\n' + newline; newline = word; } else { newline = newline + (newline == "" ? "" : " ") + word } } if(newline != "") { result = result + '\n' + newline; } } console.log(result); }
}


//잠시 후부터 직접 운전하십시오. 전방에 작동 불가 터널이 있습니다.
