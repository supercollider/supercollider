#include "../util/popup_widget.hpp"
#include "../../core/sc_introspection.hpp"

#include <QObject>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QListView>
#include <QHBoxLayout>
#include <QSortFilterProxyModel>
#include <QTextEdit>

namespace ScIDE {

namespace ScLanguage { struct Method; struct Class; }

class CompletionMenu : public PopUpWidget
{
    Q_OBJECT

public:
    enum DataRole {
        CompletionRole = Qt::UserRole,
        MethodRole
    };

    CompletionMenu(QWidget * parent = 0);
    void addItem(QStandardItem * item);
    void addInfo(QString info);
    void adapt();
    void setCompletionRole(int role);
    QString currentText();
    const ScLanguage::Method * currentMethod();
    QString exec(const QRect & rect);
    QSortFilterProxyModel *model();
    QListView *view();

protected:
    virtual bool eventFilter(QObject * obj, QEvent * ev);

private:
    QListView *mListView;
    QStandardItemModel *mModel;
    QSortFilterProxyModel *mFilterModel;
    QTextEdit *mTextEdit;
    QHBoxLayout *mLayout;
    int mCompletionRole;

signals:
    void itemChanged(int item);
};

} // namespace ScIDE
