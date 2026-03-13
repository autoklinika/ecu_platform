pragma Singleton
import QtQuick
import QtQuick.Controls

QtObject {
    property StackView stack

    function push(page, props) {
        if (!stack)
            return

        if (props === undefined)
            stack.push(page)
        else
            stack.push(page, props)
    }

    function pop() {
        if (stack)
            stack.pop()
    }

    function popToRoot() {
        if (!stack)
            return

        while (stack.depth > 1)
            stack.pop()
    }

    function goHome() {
        popToRoot()
    }
}
