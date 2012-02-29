#include <QMainWindow>
#include <QVector>

namespace ScIDE
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    enum ActionRole {
        DocNew = 0,
        DocOpen,
        DocSave,
        Quit,

        ActionCount
    };

    MainWindow();

    QAction *action( ActionRole );

private:

    QVector<QAction*> _actions;
};

} // namespace ScIDE
