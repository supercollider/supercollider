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

#include "doc_manager.hpp"
#include "main.hpp"
#include "main_window.hpp"
#include "settings/manager.hpp"
#include "../widgets/code_editor/highlighter.hpp"
#include "../../common/SC_TextUtils.hpp"
#include "util/standard_dirs.hpp"

#include <QPlainTextDocumentLayout>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTextBlock>
#include <QApplication>

#include <yaml-cpp/yaml.h>

using namespace ScIDE;

Document::Document(bool isPlainText, const QByteArray & id,
                    const QString & title, const QString & text ):
    mId(id),
    mDoc(new QTextDocument(text, this)),
    mTitle(title),
    mIndentWidth(4),
    mHighlighter(0),
    mKeyDownActionEnabled(false),
    mKeyUpActionEnabled(false),
    mMouseDownActionEnabled(false),
    mMouseUpActionEnabled(false),
    mTextChangedActionEnabled(false),
    mLastActiveEditor(0),
    mInitialSelectionStart(0),
    mInitialSelectionRange(0),
    mEditable(true),
    mPromptsToSave(true)
{
    mTmpCoalCount = 0;
    mTmpCoalTimer.setInterval(RESTORE_COAL_MSECS);
    mTmpCoalTimer.setSingleShot(true);
    connect(&mTmpCoalTimer, SIGNAL(timeout()), this, SLOT(onTmpCoalUsecs()));

    if (mId.isEmpty())
        mId = QUuid::createUuid().toString().toLatin1();
    if (mTitle.isEmpty())
        mTitle = tr("Untitled");

    mDoc->setDocumentLayout( new QPlainTextDocumentLayout(mDoc) );

    if (!isPlainText)
        mHighlighter = new SyntaxHighlighter(mDoc);

    connect( Main::instance(), SIGNAL(applySettingsRequest(Settings::Manager*)),
             this, SLOT(applySettings(Settings::Manager*)) );

    applySettings( Main::settings() );
}

void Document::setPlainText(bool set_plain_text)
{
    if (isPlainText() == set_plain_text)
        return;

    delete mHighlighter;
    mHighlighter = 0;

    if (!set_plain_text)
        mHighlighter = new SyntaxHighlighter(mDoc);
}

void Document::applySettings( Settings::Manager *settings )
{
    QFont font = settings->codeFont();
    int indentWidth = settings->value("IDE/editor/indentWidth").toInt();

    setDefaultFont(font);
    setIndentWidth(indentWidth);
}

void Document::deleteTrailingSpaces()
{
    QTextCursor cursor (textDocument());
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::EndOfBlock);
    QTextDocument * doc = textDocument();

    while( !cursor.atEnd() ) {
        while( (cursor.block().length() > 1) && doc->characterAt(cursor.position() - 1).isSpace())
            cursor.deletePreviousChar();

        cursor.movePosition(QTextCursor::NextBlock);
        cursor.movePosition(QTextCursor::EndOfBlock);
    }
    cursor.endEditBlock();
}

void Document::setDefaultFont( const QFont & font )
{
    mDoc->setDefaultFont( font );
    // update tab stop, since it depends on font:
    setIndentWidth( mIndentWidth );
    emit defaultFontChanged();
}

void Document::resetDefaultFont()
{
    Settings::Manager *settings = Main::settings();
    setDefaultFont( settings->codeFont() );
}

void Document::setIndentWidth( int numSpaces )
{
    mIndentWidth = numSpaces;

    QFontMetricsF fontMetrics( mDoc->defaultFont() );
    qreal tabStop = fontMetrics.width(' ') * numSpaces;

    QTextOption options = mDoc->defaultTextOption();
    options.setTabStop(tabStop);
    mDoc->setDefaultTextOption(options);
}

QString Document::textAsSCArrayOfCharCodes(int start = 0, int range = -1)
{
    QTextCursor cursor = QTextCursor(mDoc);
    cursor.setPosition(start, QTextCursor::MoveAnchor);
    if(range == -1){
        cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor, 1);
    } else {
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, range);
    }

    QByteArray stringBytes = cursor.selectedText().replace(QChar(0x2029), QChar('\n')).toUtf8();
    return bytesToSCArrayOfCharCodes(stringBytes);
}

QString Document::titleAsSCArrayOfCharCodes()
{
    QByteArray stringBytes = mTitle.toUtf8();
    return bytesToSCArrayOfCharCodes(stringBytes);
}

QString Document::pathAsSCArrayOfCharCodes()
{
    QString path;
    if(mFilePath.isEmpty()) {
        return QStringLiteral("nil");
    } else {
        path = mFilePath;
    }
    QByteArray stringBytes = path.toUtf8();
    return bytesToSCArrayOfCharCodes(stringBytes);
;
}

QString Document::bytesToSCArrayOfCharCodes(QByteArray stringBytes)
{
    QString returnString = QStringLiteral("[");
    for (int i = 0; i < stringBytes.size(); ++i) {
        returnString = returnString.append(QString::number(static_cast<int>(stringBytes.at(i)))).append(',');
    }
    returnString = returnString.append(QStringLiteral("]"));
    return returnString;
}

void Document::setTextInRange(const QString text, int start, int range)
{
    QTextCursor cursor = QTextCursor(mDoc);
    int size = mDoc->characterCount();
    if (start > (size - 1)) {
        start = size - 1;
        range = 0;
    }
    cursor.setPosition(start, QTextCursor::MoveAnchor);
    if(range == -1){
        cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor, 1);
    } else {
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, range);
    }
    cursor.insertText(text);
}

void Document::onTmpCoalUsecs()
{
    mTmpCoalCount = RESTORE_COAL;
    storeTmpFile();
}

void Document::storeTmpFile()
{
    QString path, name;
    QDir tmpFilesDir = standardDirectory(ScConfigUserDir);
    int i = 0;

    if (!textDocument()->isModified())
        return;

    if (++mTmpCoalCount < RESTORE_COAL) {
        mTmpCoalTimer.start();
        return;
    }

    mTmpCoalCount = 0;

    if (!mTmpFilePath.isEmpty()) {
        path = mTmpFilePath;
        goto store;
    }

    if (mFilePath.isEmpty())
        name = QStringLiteral("Untitled");
    else
        name = QFileInfo(mFilePath).baseName();

    if (!tmpFilesDir.exists("tmp"))
        tmpFilesDir.mkdir("tmp");
    tmpFilesDir.cd("tmp");

    path = QStringLiteral("%1/%2.bak").arg(tmpFilesDir.absolutePath())
                               .arg(name);
    while (QFile(path).exists())
        path = QStringLiteral("%1/%2-%3.bak")
                               .arg(tmpFilesDir.absolutePath())
                               .arg(name)
                               .arg(++i);
    mTmpFilePath = path;

store:
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly)) {
        qWarning() << "DocumentManager: the file" << path << "could not be opened for writing.";
        return;
    }

    QString str = textDocument()->toPlainText();
    file.write(str.toUtf8());
    file.close();
}

void Document::removeTmpFile()
{
    if (mTmpFilePath.isEmpty())
        return;

    if(!QFile(mTmpFilePath).remove())
        qWarning() << "DocumentManager: the file" << mTmpFilePath
                   << "could not be removed.'";
    mTmpFilePath = "";
}

DocumentManager::DocumentManager( Main *main, Settings::Manager * settings ):
QObject(main), mTextMirrorEnabled(true), mCurrentDocument(NULL), mGlobalKeyDownEnabled(false), mGlobalKeyUpEnabled(false)
{
    mDocumentModel = new QStandardItemModel(this);
    connect(&mFsWatcher, SIGNAL(fileChanged(QString)), this, SLOT(onFileChanged(QString)));

    connect(main, SIGNAL(storeSettingsRequest(Settings::Manager*)),
            this, SLOT(storeSettings(Settings::Manager*)));

    loadRecentDocuments( settings );
}

Document * DocumentManager::createDocument(bool isPlainText, const QByteArray & id,
                                           const QString & title, const QString & text  )
{
    Document *doc = new Document( isPlainText, id, title, text );
    mDocHash.insert( doc->id(), doc );

    QStandardItem * item = new QStandardItem(doc->title());
    doc->mModelItem = item;
    item->setData(QVariant::fromValue(doc));
    mDocumentModel->appendRow(item);
    QTextDocument *tdoc = doc->textDocument();
    connect(tdoc, SIGNAL(modificationChanged(bool)), doc, SLOT(onModificationChanged(bool)));
    return doc;
}

void DocumentManager::create()
{
    Document *doc = createDocument();

    connect(doc->textDocument(), SIGNAL(contentsChanged()), doc, SLOT(storeTmpFile()));
    syncLangDocument(doc);
    Q_EMIT( opened(doc, 0, 0) );
}

Document *DocumentManager::open( const QString & path, int initialCursorPosition, int selectionLength, bool toRecent, const QByteArray & id, bool syncLang )
{
    QFileInfo info(path);
    QString cpath = info.canonicalFilePath();
    info.setFile(cpath);

    if (cpath.isEmpty()) {
        MainWindow::instance()->showStatusMessage (
                    tr("Cannot open file: %1 (file does not exist)").arg(path) );
        return 0;
    }

    // Check if file already opened
    for( DocIterator it = mDocHash.begin(); it != mDocHash.end(); ++it ) {
        Document *doc = it.value();
        if(doc->mFilePath == cpath) {
            Q_EMIT( showRequest(doc, initialCursorPosition, selectionLength) );
            if (toRecent) addToRecent(doc);
            return doc;
        }
    }

    // Open the file
    QFile file(cpath);
    if(!file.open(QIODevice::ReadOnly)) {
        MainWindow::instance()->showStatusMessage(
                    tr("Cannot open file for reading: %1").arg(cpath));
        return 0;
    }
    QByteArray bytes( file.readAll() );
    file.close();

    // strip .rtf
    bool isRTF = false;
    QString filePath = cpath;
    if (info.suffix() == QStringLiteral("rtf")) {
        isRTF = true;

        filePath += QStringLiteral(".scd");
        int result = rtf2txt(bytes.data());
        bytes = bytes.left(result);
        QMessageBox::warning(NULL, QString(tr("Opening RTF File")),
                             QString(tr("Warning: RTF file will be converted to plain-text scd file.")));
    }

    closeSingleUntitledIfUnmodified();

    const bool fileIsPlainText = !(info.suffix() == QStringLiteral("sc") ||
                                  (info.suffix() == QStringLiteral("scd")) ||
                                  (info.suffix() == QStringLiteral("schelp")));

    Document *doc = createDocument( fileIsPlainText, id );
    doc->mDoc->setPlainText( decodeDocument(bytes) );
    doc->mDoc->setModified(false);
    doc->mFilePath = filePath;
    QString fileTitle = info.fileName();
    doc->setTitle(fileTitle);
    doc->mSaveTime = info.lastModified();
    doc->setInitialSelection(initialCursorPosition, selectionLength);
    connect(doc->textDocument(), SIGNAL(contentsChanged()), doc, SLOT(storeTmpFile()));

    if (!isRTF)
        mFsWatcher.addPath(cpath);

    // if this was opened from the lang we don't need to sync
    if(syncLang) {
        syncLangDocument(doc);
    }
    Q_EMIT( opened(doc, initialCursorPosition, selectionLength) );

    if (toRecent) this->addToRecent(doc);

    return doc;
}

bool DocumentManager::reload( Document *doc )
{
    Q_ASSERT(doc);

    if (doc->mFilePath.isEmpty())
        return false;

    QFile file(doc->mFilePath);
    if(!file.open(QIODevice::ReadOnly)) {
        MainWindow::instance()->showStatusMessage(
                    tr("Cannot open file for reading: %1").arg(doc->mFilePath));
        return false;
    }

    QByteArray bytes( file.readAll() );
    file.close();

    doc->mDoc->setPlainText( decodeDocument(bytes) );
    doc->mDoc->setModified(false);

    QFileInfo info(doc->mFilePath);
    doc->mSaveTime = info.lastModified();

    if (!mFsWatcher.files().contains(doc->mFilePath))
        mFsWatcher.addPath(doc->mFilePath);

    return true;
}

QStringList DocumentManager::tmpFiles()
{
    QDir tmpFilesDir = standardDirectory(ScConfigUserDir) + "/tmp";
    QStringList files = tmpFilesDir.entryList(QStringList("*.bak"), QDir::Files);
    int i;

    for (i = 0; i < files.size(); i++)
        files.replace(i, tmpFilesDir.absolutePath() + "/" + files[i]);

    return files;
}

bool DocumentManager::needRestore()
{
    return (!tmpFiles().isEmpty());
}

void DocumentManager::restore()
{
    foreach(QString path, tmpFiles()) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly))
            MainWindow::instance()->showStatusMessage(
                        tr("Cannot open file for reading: %1").arg(path));
        QByteArray bytes(file.readAll());
        file.close();
        Document *doc = createDocument(false, QByteArray(),
                                       QFileInfo(path).baseName(),
                                       decodeDocument(bytes));
        doc->mTmpFilePath = path;
        syncLangDocument(doc);
        Q_EMIT(opened(doc, 0, 0));
        connect(doc->textDocument(), SIGNAL(contentsChanged()), doc, SLOT(storeTmpFile()));
    }
}

void DocumentManager::deleteRestore()
{
    foreach(QString file, tmpFiles())
        QFile(file).remove();
}

Document * DocumentManager::documentForId(const QByteArray docID)
{
    Document * doc = mDocHash.value(docID);
    if(!doc) MainWindow::instance()->showStatusMessage(QStringLiteral("Lookup failed for Document %1").arg(docID.constData()));
    return doc;
}

QString DocumentManager::decodeDocument(const QByteArray & bytes)
{
    QTextStream stream(bytes);
    stream.setCodec("UTF-8");
    stream.setAutoDetectUnicode(true);
    return stream.readAll();
}

void DocumentManager::close( Document *doc )
{
    Q_ASSERT(doc);

    doc->removeTmpFile();

    if( mDocHash.remove(doc->id()) == 0 ) {
        qWarning("DocumentManager: trying to close an unmanaged document.");
        return;
    }

    mDocumentModel->removeRow(mDocumentModel->indexFromItem(doc->mModelItem).row());

    if (!doc->mFilePath.isEmpty())
        mFsWatcher.removePath(doc->mFilePath);

    Q_EMIT( closed(doc) );

    QString command =
            QStringLiteral("Document.findByQUuid(\'%1\').closed")
            .arg(doc->id().constData());
    Main::evaluateCodeIfCompiled( command, true );

    doc->deleteLater();
}

bool DocumentManager::save( Document *doc )
{
    Q_ASSERT(doc);

    return doSaveAs( doc, doc->mFilePath );
}

bool DocumentManager::saveAs( Document *doc, const QString & path )
{
    Q_ASSERT(doc);

    if (path.isEmpty()) {
        qWarning() << "DocumentManager: the saving path is empty.";
        return false;
    }

    bool ok = doSaveAs( doc, path );
    if (ok)
        addToRecent(doc);
    return ok;
}

bool DocumentManager::doSaveAs( Document *doc, const QString & path )
{
    Q_ASSERT(doc);

    doc->deleteTrailingSpaces();


    QFile file(path);
    if(!file.open(QIODevice::WriteOnly)) {
        qWarning() << "DocumentManager: the file" << path << "could not be opened for writing.";
        return false;
    }

    QFileInfo info(path);
    QString cpath = info.canonicalFilePath();

    const bool pathChanged = ( !(doc->filePath().isEmpty()) && (cpath != doc->filePath()) );
    if (pathChanged)
        mFsWatcher.removePath(doc->filePath());

    QString str = doc->textDocument()->toPlainText();
    file.write(str.toUtf8());
    file.flush();
    file.close();

    info.refresh();

    const bool fileIsPlainText = !(info.suffix() == QStringLiteral("sc") ||
                                  (info.suffix() == QStringLiteral("scd")) ||
                                  (info.suffix() == QStringLiteral("schelp")));

    // It's possible the mod time has not been updated - if it looks like that is the case,
    // just set it one second in the future, so we don't trip the external modification alarm.
    if (doc->mSaveTime == info.lastModified()) {
        doc->mSaveTime = QDateTime::currentDateTime().addMSecs(1000);
    } else {
        doc->mSaveTime = info.lastModified();
    }

    doc->mFilePath = cpath;
    QString fileTitle = info.fileName();
    doc->setTitle(fileTitle);
    doc->mDoc->setModified(false);
    doc->setPlainText(fileIsPlainText);
    doc->removeTmpFile();

    // Always try to start watching, because the file could have been removed:
    if (!mFsWatcher.files().contains(cpath))
        mFsWatcher.addPath(cpath);

    Q_EMIT(saved(doc));
    syncLangDocument(doc);

    return true;
}

void DocumentManager::onFileChanged( const QString & path )
{
    DocIterator it;
    for( it = mDocHash.begin(); it != mDocHash.end(); ++it )
    {
        Document *doc = it.value();
        if (doc->mFilePath == path) {
            QFileInfo info(doc->mFilePath);
            if (doc->mSaveTime < info.lastModified()) {
                doc->mDoc->setModified(true);
                doc->mSaveTime = info.lastModified();
                emit changedExternally(doc);
            }
        }
    }
}

void DocumentManager::addToRecent( Document *doc )
{
    const QString &path = doc->mFilePath;
    int i = mRecent.indexOf(path);
    if (i != -1)
        mRecent.move( i, 0 );
    else {
        mRecent.prepend(path);
        if (mRecent.count() > mMaxRecent)
            mRecent.removeLast();
    }

    emit recentsChanged();
}

void DocumentManager::clearRecents()
{
    mRecent.clear();
    emit recentsChanged();
}

void DocumentManager::loadRecentDocuments( Settings::Manager *settings )
{
    QVariantList list = settings->value("IDE/recentDocuments").value<QVariantList>();
    mRecent.clear();
    foreach (const QVariant & var, list) {
        QString filePath = var.toString();
        if (QFile::exists(filePath))
            mRecent << filePath;
    }
}

void DocumentManager::storeSettings( Settings::Manager *settings )
{
    QVariantList list;
    foreach (const QString & path, mRecent)
        list << QVariant(path);

    settings->setValue("IDE/recentDocuments", QVariant::fromValue<QVariantList>(list));
}

void DocumentManager::closeSingleUntitledIfUnmodified()
{
    QList<Document*> openDocuments = documents();

    if (openDocuments.size() == 1) {
        Document * document = openDocuments.front();
        if (document->filePath().isEmpty() && !document->isModified())
            close(document);
    }
}

void DocumentManager::handleScLangMessage( const QString &selector, const QString &data )
{
    static QString requestDocListSelector("requestDocumentList");
    static QString newDocSelector("newDocument");
    static QString openFileSelector("openFile");
    static QString getDocTextSelector("getDocumentText");
    static QString setDocTextSelector("setDocumentText");
    static QString setDocSelectionSelector("setDocumentSelection");
    static QString setDocEditableSelector("setDocumentEditable");
    static QString setDocPromptsToSaveSelector("setDocumentPromptsToSave");
    static QString setCurrentDocSelector("setCurrentDocument");
    static QString removeDocUndoSelector("removeDocUndo");
    static QString closeDocSelector("closeDocument");
    static QString setDocTitleSelector("setDocumentTitle");
    static QString enableGlobalKeyDownSelector("enableDocumentGlobalKeyDownAction");
    static QString enableGlobalKeyUpSelector("enableDocumentGlobalKeyUpAction");
    static QString enableKeyDownSelector("enableDocumentKeyDownAction");
    static QString enableKeyUpSelector("enableDocumentKeyUpAction");
    static QString enableMouseDownSelector("enableDocumentMouseDownAction");
    static QString enableMouseUpSelector("enableDocumentMouseUpAction");
    static QString enableTextChangedSelector("enableDocumentTextChangedAction");
    static QString enableTextMirrorSelector("enableDocumentTextMirror");

    if (selector == requestDocListSelector)
        handleDocListScRequest();

    if (selector == newDocSelector)
        handleNewDocScRequest(data);

    if (selector == openFileSelector)
        handleOpenFileScRequest(data);

    if (selector == getDocTextSelector)
        handleGetDocTextScRequest(data);

    if (selector == setDocTextSelector)
        handleSetDocTextScRequest(data);

    if (selector == setDocSelectionSelector)
        handleSetDocSelectionScRequest(data);

    if (selector == setDocEditableSelector)
        handleSetDocEditableScRequest(data);

    if (selector == setDocPromptsToSaveSelector)
        handleSetDocPromptsToSaveScRequest(data);

    if (selector == setCurrentDocSelector)
        handleSetCurrentDocScRequest(data);

    if (selector == removeDocUndoSelector)
        handleRemoveDocUndoScRequest(data);

    if (selector == closeDocSelector)
        handleCloseDocScRequest(data);

    if (selector == setDocTitleSelector)
        handleSetDocTitleScRequest(data);

    if (selector == enableKeyDownSelector)
        handleEnableKeyDownScRequest( data );

    if (selector == enableKeyUpSelector)
        handleEnableKeyUpScRequest( data );

    if (selector == enableGlobalKeyDownSelector)
        handleEnableGlobalKeyDownScRequest( data );

    if (selector == enableGlobalKeyUpSelector)
        handleEnableGlobalKeyUpScRequest( data );

    if (selector == enableMouseDownSelector)
        handleEnableMouseDownScRequest( data );

    if (selector == enableMouseUpSelector)
        handleEnableMouseUpScRequest( data );

    if (selector == enableTextChangedSelector)
        handleEnableTextChangedScRequest( data );

    if (selector == enableTextMirrorSelector)
        handleEnableTextMirrorScRequest( data );
}

void DocumentManager::handleDocListScRequest()
{
    QList<Document*> docs = documents();
    QList<Document*>::Iterator it;
    QString command = QStringLiteral("Document.syncDocs([");
    for (it = docs.begin(); it != docs.end(); ++it) {
        Document * doc = *it;
        int start, range;
        if(doc->lastActiveEditor()){ // we might have changed selection before sync happened
            QTextCursor cursor = doc->lastActiveEditor()->textCursor();
            start = cursor.selectionStart();
            range = cursor.selectionEnd() - start;
        } else {
            start = doc->initialSelectionStart();
            range = doc->initialSelectionRange();
        }
        QString docData = QStringLiteral("[\'%1\', %2, %3, %4, %5, %6, %7],")
            .arg(doc->id().constData())
            .arg(doc->titleAsSCArrayOfCharCodes())
            .arg(doc->textAsSCArrayOfCharCodes(0, -1))
            .arg(doc->isModified())
            .arg(doc->pathAsSCArrayOfCharCodes())
            .arg(start)
            .arg(range);
        command = command.append(docData);
    }
    command = command.append("]);");
    Main::evaluateCode ( command, true );
}

void DocumentManager::handleNewDocScRequest( const QString & data )
{
    try {
        YAML::Node doc = YAML::Load( data.toStdString() );
        if ( doc ) {
            if ( !doc.IsSequence() )
                return;

            std::string title = doc[0].as<std::string>();
            std::string text  = doc[1].as<std::string>();
            std::string id    = doc[2].as<std::string>();

            Document *document = createDocument( false,
                                                 id.c_str(),
                                                 QString::fromUtf8(title.c_str()),
                                                 QString::fromUtf8(text.c_str()) );
            syncLangDocument(document);
            Q_EMIT( opened(document, 0, 0) );
        }
    } catch (std::exception const& e) {
        qWarning() << "DocumentManager::" << __FUNCTION__ << ": could not handle request:" << e.what();
        return;
    }
}

void DocumentManager::handleOpenFileScRequest( const QString & data )
{
    try {
        YAML::Node doc = YAML::Load( data.toStdString() );
        if ( doc ) {
            if ( !doc.IsSequence() )
                return;

            std::string path    = doc[0].as<std::string>();
            int position        = doc[1].as<int>();
            int selectionLength = doc[2].as<int>();
            std::string id      = doc[3].as<std::string>();

            // we don't need to sync with lang in this case
            open(QString(path.c_str()), position, selectionLength, true, id.c_str(), false);
        }
    } catch (std::exception const& e) {
        qWarning() << "DocumentManager::" << __FUNCTION__ << ": could not handle request:" << e.what() << endl;
        return;
    }
}

void DocumentManager::handleGetDocTextScRequest( const QString & data )
{
    try {
        YAML::Node doc = YAML::Load( data.toStdString() );
        if ( doc ) {
            if ( !doc.IsSequence() )
                return;

            std::string id     = doc[0].as<std::string>();
            std::string funcID = doc[1].as<std::string>();
            int start          = doc[2].as<int>();
            int range          = doc[3].as<int>();

            Document *document = documentForId(id.c_str());
            if(document) {
                QString docText = document->textAsSCArrayOfCharCodes(start, range);

                QString command = QStringLiteral("Document.executeAsyncResponse(\'%1\', %2.asAscii)").arg(funcID.c_str(), docText);
                Main::evaluateCode ( command, true );
            }
        }
    } catch (std::exception const& e) {
        qWarning() << "DocumentManager::" << __FUNCTION__ << ": could not handle request:" << e.what() << endl;
        return;
    }
}

void DocumentManager::handleSetDocTextScRequest( const QString & data )
{
    try {
        YAML::Node doc = YAML::Load( data.toStdString() );
        if ( doc ) {
            if ( !doc.IsSequence() )
                return;

            // Parse funcID (doc[1]) later, if it was not null.
            std::string id   = doc[0].as<std::string>();
            std::string text = doc[2].as<std::string>();
            int start        = doc[3].as<int>();
            int range        = doc[4].as<int>();

            Document *document = documentForId(id.c_str());
            if(document) {
                // avoid a loop
                if(document == mCurrentDocument) {
                    disconnect(document->textDocument(), SIGNAL(contentsChange(int, int, int)), this, SLOT(updateCurrentDocContents(int, int, int)));
                }

                document->setTextInRange(QString::fromUtf8(text.c_str()), start, range);

                if(document == mCurrentDocument) {
                    connect(document->textDocument(), SIGNAL(contentsChange(int, int, int)), this, SLOT(updateCurrentDocContents(int, int, int)));
                }

                // Only execute a call if a function name was passed.
                if (!doc[1].IsNull()) {
                    std::string funcID = doc[1].as<std::string>("");
                    QString command = QStringLiteral("Document.executeAsyncResponse(\'%1\')").arg(funcID.c_str());
                    Main::evaluateCode ( command, true );
                }
            }
        }
    } catch (std::exception const& e) {
        qWarning() << "DocumentManager::" << __FUNCTION__ << ": could not handle request:" << e.what();
        return;
    }
}

void DocumentManager::handleSetDocSelectionScRequest( const QString & data )
{
    try {
        YAML::Node doc = YAML::Load( data.toStdString() );
        if ( doc ) {
            if ( !doc.IsSequence() )
                return;

            std::string id     = doc[0].as<std::string>();
            int start          = doc[1].as<int>();
            int range          = doc[2].as<int>();

            Document *document = documentForId(id.c_str());
            if(document) {
                if(document->lastActiveEditor()) {
                    document->lastActiveEditor()->showPosition(start, range);
                }
            }
        }
    } catch (std::exception const& e) {
        qWarning() << "DocumentManager::" << __FUNCTION__ << ": could not handle request:" << e.what();
        return;
    }
}

void DocumentManager::handleSetDocEditableScRequest( const QString & data )
{
    try {
        YAML::Node doc = YAML::Load( data.toStdString() );
        if ( doc ) {
            if ( !doc.IsSequence() )
                return;

            std::string id     = doc[0].as<std::string>();
            bool editable      = doc[1].as<bool>();

            Document *document = documentForId(id.c_str());
            if(document){
                document->setEditable(editable);
                if(document->lastActiveEditor()) {
                    document->lastActiveEditor()->setReadOnly(!editable);
                }
            }
        }
    } catch (std::exception const& e) {
        qWarning() << "DocumentManager::" << __FUNCTION__ << ": could not handle request:" << e.what();
        return;
    }
}

void DocumentManager::handleSetDocPromptsToSaveScRequest( const QString & data )
{
    try {
        YAML::Node doc = YAML::Load( data.toStdString() );
        if ( doc ) {
            if ( !doc.IsSequence() )
                return;

            std::string id     = doc[0].as<std::string>();
            bool promptsToSave = doc[1].as<bool>();

            Document *document = documentForId(id.c_str());
            if(document) {
                document->setPromptsToSave(promptsToSave);
            }
        }
    } catch (std::exception const& e) {
        qWarning() << "DocumentManager::" << __FUNCTION__ << ": could not handle request:" << e.what();
        return;
    }
}

void DocumentManager::handleSetCurrentDocScRequest( const QString & data )
{
    try {
        YAML::Node doc = YAML::Load( data.toStdString() );
        if ( doc ) {
            if ( !doc.IsSequence() )
                return;

            std::string id     = doc[0].as<std::string>();

            Document *document = documentForId(id.c_str());
            if(document)
                Q_EMIT( showRequest(document) );
        }
    } catch (std::exception const& e) {
        qWarning() << "DocumentManager::" << __FUNCTION__ << ": could not handle request:" << e.what();
        return;
    }
}

void DocumentManager::handleRemoveDocUndoScRequest( const QString & data )
{
    try {
        YAML::Node doc = YAML::Load( data.toStdString() );
        if ( doc ) {
            if ( !doc.IsSequence() )
                return;

            std::string id     = doc[0].as<std::string>();

            Document *document = documentForId(id.c_str());
            if(document){
                QTextDocument *textDoc = document->textDocument();
                textDoc->clearUndoRedoStacks();
                textDoc->setModified(false);
            }
        }
    } catch (std::exception const& e) {
        qWarning() << "DocumentManager::" << __FUNCTION__ << ": could not handle request:" << e.what();
        return;
    }
}

void DocumentManager::handleCloseDocScRequest( const QString & data )
{
    try {
        YAML::Node doc = YAML::Load( data.toStdString() );
        if ( doc ) {
            if ( !doc.IsSequence() )
                return;

            std::string id     = doc[0].as<std::string>();
            Document *document = documentForId(id.c_str());
            if(document){
                close(document);
            }
        }
    } catch (std::exception const& e) {
        qWarning() << "DocumentManager::" << __FUNCTION__ << ": could not handle request:" << e.what();
        return;
    }
}

void DocumentManager::handleSetDocTitleScRequest( const QString & data )
{
    try {
        YAML::Node doc = YAML::Load( data.toStdString() );
        if ( doc ) {
            if ( !doc.IsSequence() )
                return;

            std::string id     = doc[0].as<std::string>();
            std::string title  = doc[1].as<std::string>();
            Document *document = documentForId(id.c_str());
            if(document) {
                document->mTitle = QString::fromUtf8(title.c_str());
                Q_EMIT(titleChanged(document));
            }
        }
    } catch (std::exception const& e) {
        qWarning() << "DocumentManager::" << __FUNCTION__ << ": could not handle request:" << e.what();
        return;
    }
}

bool DocumentManager::parseActionEnabledRequest( const QString & data, std::string *idString, bool *en)
{
    try {
        YAML::Node doc = YAML::Load( data.toStdString() );
        if ( doc ) {
            if ( !doc.IsSequence() )
                return false;

            std::string id     = doc[0].as<std::string>();
            bool enabled       = doc[1].as<bool>();

            *idString = id;
            *en = enabled;

            return true;
        }
    } catch (std::exception const& e) {
        qWarning() << "DocumentManager::" << __FUNCTION__ << ": could not handle request:" << e.what();
    }
    return false;
}

void DocumentManager::handleEnableKeyDownScRequest( const QString & data )
{
    std::string id;
    bool enabled;
    if (parseActionEnabledRequest(data, &id, &enabled)) {
        Document *document = documentForId(id.c_str());
        if(document) {
            document->setKeyDownActionEnabled(enabled);
        }
    }
}

void DocumentManager::handleEnableKeyUpScRequest( const QString & data )
{
    std::string id;
    bool enabled;
    if (parseActionEnabledRequest(data, &id, &enabled)) {
        Document *document = documentForId(id.c_str());
        if(document)
        {
            document->setKeyUpActionEnabled(enabled);
        }

    }

}

void DocumentManager::handleEnableGlobalKeyDownScRequest( const QString & data )
{
    try {
        YAML::Node doc = YAML::Load( data.toStdString() );
        if ( doc ) {
            if ( !doc.IsSequence() )
                return;

            bool enabled = doc[0].as<bool>(enabled);
            mGlobalKeyDownEnabled = enabled;
        }
    } catch (std::exception const& e) {
        qWarning() << "DocumentManager::" << __FUNCTION__ << ": could not handle request:" << e.what();
    }
}

void DocumentManager::handleEnableGlobalKeyUpScRequest( const QString & data )
{
    try {
        YAML::Node doc = YAML::Load( data.toStdString() );
        if ( doc ) {
            if ( !doc.IsSequence() )
                return;

            bool enabled = doc[0].as<bool>();

            mGlobalKeyUpEnabled = enabled;
        }
    } catch (std::exception const& e) {
        qWarning() << "DocumentManager::" << __FUNCTION__ << ": could not handle request:" << e.what();
    }
}

void DocumentManager::handleEnableMouseDownScRequest( const QString & data )
{
    std::string id;
    bool enabled;
    if (parseActionEnabledRequest(data, &id, &enabled)) {
        Document *document = documentForId(id.c_str());
        if(document)
        {
            document->setMouseDownActionEnabled(enabled);
        }

    }

}

void DocumentManager::handleEnableMouseUpScRequest( const QString & data )
{
    std::string id;
    bool enabled;
    if (parseActionEnabledRequest(data, &id, &enabled)) {
        Document *document = documentForId(id.c_str());
        if(document)
        {
            document->setMouseUpActionEnabled(enabled);
        }

    }

}

void DocumentManager::handleEnableTextChangedScRequest( const QString & data )
{
    std::string id;
    bool enabled;
    if (parseActionEnabledRequest(data, &id, &enabled)) {
        Document *document = documentForId(id.c_str());
        if(document)
        {
            document->setTextChangedActionEnabled(enabled);
        }

    }

}

void DocumentManager::handleEnableTextMirrorScRequest( const QString & data )
{
    try {
        YAML::Node doc = YAML::Load( data.toStdString() );
        if ( doc ) {
            if ( !doc.IsSequence() )
                return;

            bool enabled = doc[0].as<bool>();

            mTextMirrorEnabled = enabled;

            QList<Document*> docs = documents();
            QList<Document*>::Iterator it;
            if(enabled) {
                for (it = docs.begin(); it != docs.end(); ++it) {
                    Document * doc = *it;
                    Main::scProcess()->updateTextMirrorForDocument(doc, 0, -1, doc->textDocument()->characterCount());
                    doc->lastActiveEditor()->updateDocLastSelection();
                }
            } else {
                // this sets the mirror to empty strings
                for (it = docs.begin(); it != docs.end(); ++it) {
                    Document * doc = *it;
                    Main::scProcess()->updateTextMirrorForDocument(doc, 0, -1, 0);
                }
                QString warning = QStringLiteral("Document Text Mirror Disabled\n");
                Main::scProcess()->post(warning);
            }
        }
    } catch (std::exception const& e) {
        qWarning() << "DocumentManager::" << __FUNCTION__ << ": could not handle request:" << e.what();
    }
}

void DocumentManager::syncLangDocument(Document *doc)
{
    int start, range;
    if(doc->lastActiveEditor()){ // we might have changed selection before sync happened
        QTextCursor cursor = doc->lastActiveEditor()->textCursor();
        start = cursor.selectionStart();
        range = cursor.selectionEnd() - start;
    } else {
        start = doc->initialSelectionStart();
        range = doc->initialSelectionRange();
    }
    QString command =
            QStringLiteral("Document.syncFromIDE(\'%1\', %2, %3, %4, %5, %6, %7)")
            .arg(doc->id().constData())
            .arg(doc->titleAsSCArrayOfCharCodes())
            .arg(doc->textAsSCArrayOfCharCodes(0, -1))
            .arg(doc->isModified())
            .arg(doc->pathAsSCArrayOfCharCodes())
            .arg(start)
            .arg(range);
    Main::evaluateCodeIfCompiled ( command, true );
}

void DocumentManager::setActiveDocument(Document * document)
{
    if(mCurrentDocument)
        disconnect(mCurrentDocument->textDocument(), SIGNAL(contentsChange(int, int, int)), this, SLOT(updateCurrentDocContents(int, int, int)));
    if (document){
        mCurrentDocumentPath = document->filePath();
        connect(document->textDocument(), SIGNAL(contentsChange(int, int, int)), this, SLOT(updateCurrentDocContents(int, int, int)));
        mCurrentDocument = document;
    } else {
        mCurrentDocumentPath.clear();
        mCurrentDocument = NULL;
    }

    sendActiveDocument();
}

void DocumentManager::sendActiveDocument()
{
    if (Main::scProcess()->state() != QProcess::Running)
        return;
    if(mCurrentDocument){
        QString command = QStringLiteral("Document.setActiveDocByQUuid(\'%1\');").arg(mCurrentDocument->id().constData());
        if (mCurrentDocumentPath.isEmpty()) {
            command = command.append(QStringLiteral("ScIDE.currentPath_(nil);"));
        } else {
            command = command.append(QStringLiteral("ScIDE.currentPath_(\"%1\");").arg(mCurrentDocumentPath));
        }
        Main::evaluateCodeIfCompiled(command, true);
    } else
        Main::evaluateCodeIfCompiled(QStringLiteral("ScIDE.currentPath_(nil); Document.current = nil;"), true);
}

void DocumentManager::updateCurrentDocContents ( int position, int charsRemoved, int charsAdded )
{
    if (mTextMirrorEnabled) {
        Main::scProcess()->updateTextMirrorForDocument(mCurrentDocument, position, charsRemoved, charsAdded);
    }

    if (mCurrentDocument->textChangedActionEnabled()) {
        QString addedChars = mCurrentDocument->textAsSCArrayOfCharCodes(position, charsAdded);
        Main::evaluateCode(QStringLiteral("Document.findByQUuid(\'%1\').textChanged(%2, %3, %4);").arg(mCurrentDocument->id().constData()).arg(position).arg(charsRemoved).arg(addedChars), true);
    }
}
