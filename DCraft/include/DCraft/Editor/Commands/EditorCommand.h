//
// Created by denzel on 09/04/2025.
//

class EditorCommand {
public:
    virtual  ~EditorCommand() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};