#include "ScriptEditor.h"

void ScriptEditor::editScriptLoop() {
    if (flag_edit_script) {
        editorWindow->show();
        while (isServer() && !flag_run_script)
            Fl::check(); // update only fltk while user edits the script
        editorWindow->hide();

        // we are going to run the script now, reset the
        // var for the next time we enter the editor
        flag_run_script = 0;
    }
}