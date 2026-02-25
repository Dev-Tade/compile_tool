#include <string.h>
#include "compiler_window.h"

#define BUFFER_SIZE 256

void CompilerWindowInit(CompilerWindow *compiler, const char *compilerWindowLayoutFile)
{
    if (compiler == NULL) return;

    compiler->layout = LoadLayoutFile(compilerWindowLayoutFile);

    // Expose window and anchor
    RGLControl *window = GetControlByName(&compiler->layout, "CompilerWindow");
    compiler->anchor = GetAnchorById(&compiler->layout, window->anchorID);
    compiler->window = window;

    // Expose window drag handle (status bar)
    compiler->dragHandle = GetControlRect(&compiler->layout, window);
    compiler->dragHandle.height = 24; // raygui window statusbar height 

    // Default state
    compiler->windowActive = true;
    compiler->compile = false;

    compiler->includeTextEdit = false;
    compiler->libraryTextEdit = false;
    compiler->linkTextEdit = false;
    compiler->outputTextEdit = false;

    compiler->includeText = RL_CALLOC(BUFFER_SIZE, sizeof(char));
    compiler->libraryText = RL_CALLOC(BUFFER_SIZE, sizeof(char));
    compiler->linkText = RL_CALLOC(BUFFER_SIZE, sizeof(char));
    compiler->outputText = RL_CALLOC(BUFFER_SIZE, sizeof(char));

}

void GuiCompilerWindow(CompilerWindow *compiler)
{
    // Update drag handle
    RGLControl *window = GetControlByName(&compiler->layout, "CompilerWindow");

    Rectangle windowRect = GetControlRect(&compiler->layout, window);
    compiler->dragHandle = windowRect;
    compiler->dragHandle.height = 24; // raygui window status bar

    if (compiler->windowActive)
    {
        compiler->windowActive = !GuiWindowBox(windowRect, window->text);

        // Information/decorative controls
        // Render labels first
        RGLControl *includeLabel = GetControlByName(&compiler->layout, "IncludeLabel");
        RGLControl *libraryLabel = GetControlByName(&compiler->layout, "LibraryLabel");
        RGLControl *linkLabel = GetControlByName(&compiler->layout, "LinkLabel");
        RGLControl *outputLabel = GetControlByName(&compiler->layout, "OutputLabel");

        GuiLabel(GetControlRect(&compiler->layout, includeLabel), includeLabel->text);
        GuiLabel(GetControlRect(&compiler->layout, libraryLabel), libraryLabel->text);
        GuiLabel(GetControlRect(&compiler->layout, linkLabel), linkLabel->text);
        GuiLabel(GetControlRect(&compiler->layout, outputLabel), outputLabel->text);

        RGLControl *separatorLine = GetControlByName(&compiler->layout, "SeparatorLine");
        GuiLine(GetControlRect(&compiler->layout, separatorLine), separatorLine->text);
        
        // Interactive controls
        RGLControl *includeTextbox = GetControlByName(&compiler->layout, "AdditionalIncludesText");
        RGLControl *libraryTextbox = GetControlByName(&compiler->layout, "AdditionalLibrariesText");
        RGLControl *linkTextbox = GetControlByName(&compiler->layout, "LinkLibrariesText");
        RGLControl *outputTextbox = GetControlByName(&compiler->layout, "OutputPathText");

        if (GuiTextBox(GetControlRect(&compiler->layout, includeTextbox), compiler->includeText, BUFFER_SIZE, compiler->includeTextEdit))
        {
            compiler->includeTextEdit = !compiler->includeTextEdit;
        }

        if (GuiTextBox(GetControlRect(&compiler->layout, libraryTextbox), compiler->libraryText, BUFFER_SIZE, compiler->libraryTextEdit))
        {
            compiler->libraryTextEdit = !compiler->libraryTextEdit;
        }

        if (GuiTextBox(GetControlRect(&compiler->layout, linkTextbox), compiler->linkText, BUFFER_SIZE, compiler->linkTextEdit))
        {
            compiler->linkTextEdit = !compiler->linkTextEdit;
        }

        if (GuiTextBox(GetControlRect(&compiler->layout, outputTextbox), compiler->outputText, BUFFER_SIZE, compiler->outputTextEdit))
        {
            compiler->outputTextEdit = !compiler->outputTextEdit;
        }

        RGLControl *compileButton = GetControlByName(&compiler->layout, "CompileButton");
        compiler->compile = GuiButton(GetControlRect(&compiler->layout, compileButton), compileButton->text);
    }
}