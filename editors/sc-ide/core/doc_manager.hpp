/*
    SuperCollider Qt IDE
    Copyright (c) 2012 Jakob Leben & Tim Blechmann
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifndef SCIDE_DOC_MANAGER_HPP_INCLUDED
#define SCIDE_DOC_MANAGER_HPP_INCLUDED

#include "../widgets/code_editor/editor.hpp"
#include <QDateTime>
#include <QFileSystemWatcher>
#include <QHash>
#include <QList>
#include <QMetaType>
#include <QObject>
#include <QStringList>
#include <QTextDocument>
#include <QPlainTextDocumentLayout>
#include <QUuid>
#include <QTimer>
#include <QStandardItemModel>
#include <QApplication>
#include <QStyle>

#define RESTORE_COAL 100
#define RESTORE_COAL_MSECS 60000

namespace ScIDE {

namespace Settings { class Manager; }
class SyntaxHighlighter;

class Main;
class DocumentManager;

class Document : public QObject
{
    Q_OBJECT

    friend class DocumentManager;

public:
    Document( bool isPlainText, const QByteArray & id = QByteArray(),
              const QString & title = QString(),
             const QString & text = QString());

    QTextDocument *textDocument() { return mDoc; }
    const QByteArray & id() { return mId; }
    const QString & filePath() { return mFilePath; }
    const QString & title() { return mTitle; }
    void setTitle(QString & newTitle) { mTitle = newTitle; mModelItem->setText(mTitle); }

    QFont defaultFont() const { return mDoc->defaultFont(); }
    void setDefaultFont( const QFont & font );

    int indentWidth() const { return mIndentWidth; }
    void setIndentWidth( int numSpaces );

    void deleteTrailingSpaces();

    bool isPlainText() const { return mHighlighter == NULL; }
    bool isModified() const  { return mDoc->isModified(); }

    QStandardItem * modelItem() { return mModelItem; }
    
    QString textAsSCArrayOfCharCodes(int start, int range);
    QString titleAsSCArrayOfCharCodes();
    QString pathAsSCArrayOfCharCodes();
    QString bytesToSCArrayOfCharCodes(QByteArray stringBytes);

    void setTextInRange(const QString text, int start, int range);
    
    bool keyDownActionEnabled() { return mKeyDownActionEnabled; }
    bool keyUpActionEnabled() { return mKeyUpActionEnabled; }
    bool mouseDownActionEnabled() { return mMouseDownActionEnabled; }
    bool mouseUpActionEnabled() { return mMouseUpActionEnabled; }
    bool textChangedActionEnabled() { return mTextChangedActionEnabled; }
    GenericCodeEditor * lastActiveEditor() { return mLastActiveEditor; }
    int initialSelectionStart() { return mInitialSelectionStart; }
    int initialSelectionRange() { return mInitialSelectionRange; }
    bool editable() { return mEditable; }
    bool promptsToSave() { return mPromptsToSave; }
    
    void setKeyDownActionEnabled(bool enabled) { mKeyDownActionEnabled = enabled; }
    void setKeyUpActionEnabled(bool enabled) {  mKeyUpActionEnabled = enabled; }
    void setMouseDownActionEnabled(bool enabled) {  mMouseDownActionEnabled = enabled; }
    void setMouseUpActionEnabled(bool enabled) {  mMouseUpActionEnabled = enabled; }
    void setTextChangedActionEnabled(bool enabled) {  mTextChangedActionEnabled = enabled; }
    void setLastActiveEditor(GenericCodeEditor * lastActive) {  mLastActiveEditor = lastActive; }
    void setInitialSelection(int start, int range) { mInitialSelectionStart = start; mInitialSelectionRange = range; }
    void setEditable(bool editable) { mEditable = editable; }
    void setPromptsToSave(bool prompts) { mPromptsToSave = prompts; }

    void removeTmpFile();

public slots:
    void applySettings( Settings::Manager * );
    void resetDefaultFont();
    void storeTmpFile();
    void onTmpCoalUsecs();

    void onModificationChanged(bool changed) {
         if(changed){
             mModelItem->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton));
         } else {
             mModelItem->setIcon(QIcon());
         }
     }

signals:
    void defaultFontChanged();

private:
    void setPlainText(bool flag);

    QByteArray mId;
    QTextDocument *mDoc;
    QString mFilePath;
    QString mTitle;
    QString mTmpFilePath;
    int mTmpCoalCount;
    QTimer mTmpCoalTimer;
    QDateTime mSaveTime;
    int mIndentWidth;
    SyntaxHighlighter * mHighlighter;
    bool mKeyDownActionEnabled;
    bool mKeyUpActionEnabled;
    bool mMouseDownActionEnabled;
    bool mMouseUpActionEnabled;
    bool mTextChangedActionEnabled;
    GenericCodeEditor * mLastActiveEditor;
    int mInitialSelectionStart, mInitialSelectionRange;
    bool mEditable;
    bool mPromptsToSave;
    QStandardItem * mModelItem;
};

class DocumentManager : public QObject
{
    Q_OBJECT

public:
    typedef QList< Document * > DocumentList;

    DocumentManager( Main *, Settings::Manager * );
    QList<Document*> documents() {
        return mDocHash.values();
    }
    QList<QByteArray> documentIDs() {
        return mDocHash.uniqueKeys();
    }

    void create();
    void close( Document * );
    bool save( Document * );
    bool saveAs( Document *, const QString & path );
    bool reload( Document * );
    bool needRestore();
    void restore();
    void deleteRestore();
    const QStringList & recents() const { return mRecent; }
    Document * documentForId(const QByteArray id);
    bool textMirrorEnabled() { return mTextMirrorEnabled; }
    void setActiveDocument(class Document *);
    void sendActiveDocument();
    Document * activeDocument() { return mCurrentDocument; }
    bool globalKeyDownActionEnabled() { return mGlobalKeyDownEnabled; }
    bool globalKeyUpActionEnabled() { return mGlobalKeyUpEnabled; }
    QStandardItemModel * docModel() { return mDocumentModel; }

public slots:
    // initialCursorPosition -1 means "don't change position if already open"
    Document * open( const QString & path, int initialCursorPosition = -1, int selectionLength = 0, bool addToRecent = true, const QByteArray & id = QByteArray(), bool syncLang = true );
    void clearRecents();
    void storeSettings( Settings::Manager * );
    void handleScLangMessage( const QString &selector, const QString &data );

Q_SIGNALS:
    void opened( Document *, int cursorPosition, int selectionLength );
    void closed( Document * );
    void saved( Document * );
    void showRequest( Document *, int pos = -1, int selectionLength = 0 );
    void changedExternally( Document * );
    void recentsChanged();
    void titleChanged( Document * );

private slots:
    void onFileChanged( const QString & path );
    void updateCurrentDocContents ( int position, int charsRemoved, int charsAdded );

private:
    Document * createDocument( bool isPlainText = false,
                               const QByteArray & id = QByteArray(),
                               const QString & title = QString(),
                               const QString & text = QString()  );
    bool doSaveAs( Document *, const QString & path );
    void addToRecent( Document * );
    void loadRecentDocuments( Settings::Manager * );
    QStringList tmpFiles();
    void closeSingleUntitledIfUnmodified();
    QString decodeDocument(QByteArray const &);
    void handleDocListScRequest();
    void handleNewDocScRequest( const QString & data );
    void handleOpenFileScRequest( const QString & data );
    void handleGetDocTextScRequest( const QString & data );
    void handleSetDocTextScRequest( const QString & data );
    void handleSetDocSelectionScRequest( const QString & data );
    void handleSetDocEditableScRequest( const QString & data );
    void handleSetDocPromptsToSaveScRequest( const QString & data );
    void handleSetCurrentDocScRequest( const QString & data );
    void handleRemoveDocUndoScRequest( const QString & data );
    void handleCloseDocScRequest( const QString & data );
    void handleSetDocTitleScRequest( const QString & data );
    
    bool parseActionEnabledRequest( const QString & data, std::string *idString, bool *en);
    void handleEnableKeyDownScRequest( const QString & data );
    void handleEnableKeyUpScRequest( const QString & data );
    void handleEnableGlobalKeyDownScRequest( const QString & data );
    void handleEnableGlobalKeyUpScRequest( const QString & data );
    void handleEnableMouseDownScRequest( const QString & data );
    void handleEnableMouseUpScRequest( const QString & data );
    void handleEnableTextChangedScRequest( const QString & data );
    void handleEnableTextMirrorScRequest( const QString & data );
    void syncLangDocument( Document * );

    typedef QHash<QByteArray, Document*>::iterator DocIterator;

    QHash<QByteArray, Document*> mDocHash;
    QFileSystemWatcher mFsWatcher;

    QStringList mRecent;
    static const int mMaxRecent = 10;
    
    bool mTextMirrorEnabled;
    QString mCurrentDocumentPath;
    class Document * mCurrentDocument;
    bool mGlobalKeyDownEnabled, mGlobalKeyUpEnabled;
    QStandardItemModel * mDocumentModel;
};

} // namespace ScIDE

Q_DECLARE_METATYPE( ScIDE::Document* )

#endif // SCIDE_DOC_MANAGER_HPP_INCLUDED
