#include <QDir>
#include <QDebug>
#include <QRegularExpressionMatch>
#include <QFile>
#include <QStringList>
#include <QImage>
#include <math.h>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <functional>



/*Configuration*/
auto codeAni = QString("anixxx");
auto codeAtlas = QString("atlas");
auto extension = QString(".png");
auto reFrameImage = QString("(?<name>[a-zA-Z0-9_\+-]+?)(?<number>\\d+)" + extension);
QStringList reFrameImageConverted = {QString("*\[0-9\]*" + extension)};

/*Log*/
#ifdef Q_OS_LINUX
    const auto separator = QString("/");
    auto logDir = QString("/.local/CuuAmChanKinh/");
#elif defined(Q_OS_CYGWIN)
    const auto separator =  QString("\\");
   auto logDir = QString("\\AppData\\Local\\CuuAmChanKinh\\");
#else
#error "We don't support that version yet..."
#endif
QFile logFile;
QString logContent;

/*Params*/
auto isQuietMode = false;
auto isClean = false;
auto isCleanAll = false;
auto row = -1;
auto col = -1;

/*Summary*/
auto countSucces = 0;



/**********************************************************************************/
QString hash(QString s) {
    std::hash<std::string> hasher;
    auto hashed = hasher(s.toStdString());
    return QString::fromStdString(std::to_string(hashed));
}

QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    ts << QString("\n");
    ts.flush();
    return ts;
}

void formatLast(QString& s, const QString& last) {
    s += s.right(last.length()) != last ? last : "";
}

void openLog(QString dirPath) {
    if(dirPath.isEmpty()) return;
    formatLast(dirPath, separator);

    auto dir = QDir(dirPath);
    if( !dir.exists() ) {
        qStdOut() << QDir::cleanPath(dirPath) << " : no such directory";
        return;
    }

    if(!QDir().mkpath(QDir::homePath() + logDir)) qStdOut() << "Failed to open folder" << QDir::homePath() + logDir;
    auto logPath = QDir::homePath() + logDir + hash(dir.absolutePath()) + ".log";
    logFile.setFileName(logPath);

    QIODevice::OpenMode mode;
    if(isCleanAll) mode = QIODevice::ReadWrite | QIODevice::Text;
    else mode = QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text;
    if(!logFile.open(mode)) {
        qStdOut() << "Failed to open log" << logPath;
    }
    QFile tmp(logPath);
    if(tmp.open(QIODevice::ReadOnly | QIODevice::Text)) {
        logContent = QString(tmp.readAll());
//        qDebug() << logContent;
    }
}
void closeLog() {
    logFile.close();
    qStdOut().flush();
    if(isCleanAll) logFile.remove();
}
void writeLineLog(QString s) {
    logFile.write(s.toStdString().c_str());
    logFile.write("\n");
}
void clean(QString dirPath) {
    if(dirPath.isEmpty()) return;
    formatLast(dirPath, separator);

    auto dir = QDir(dirPath);
    if( !dir.exists() ) {
        qStdOut() << QDir::cleanPath(dirPath) << " : no such directory";
        return;
    }

    // if( !isQuietMode ) qDebug() << __FUNCTION__ << dirPath;

    if( dirPath.contains(codeAni) ) {
         if( dir.removeRecursively() ) if( !isQuietMode ) qDebug() << "\t--> [Removed]\t" << QDir::cleanPath(dirPath);
         return;
    }

    auto subDirs = dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot);
    for(const auto& subDir : subDirs ) {
        auto subDirPath = subDir.filePath();
        clean(subDirPath);
    }

}

void clearAll(QString dirPath) {
    if(dirPath.isEmpty()) return;
    formatLast(dirPath, separator);

    auto dir = QDir(dirPath);
    if( !dir.exists() ) {
        qStdOut() << QDir::cleanPath(dirPath) << " : no such directory";
        return;
    }

    // if( !isQuietMode ) qDebug() << __FUNCTION__ << dirPath;

    if( dirPath.contains(codeAni) ) {
         if( dir.removeRecursively() ) if( !isQuietMode ) qStdOut() << "\t--> [Removed]\t" << QDir::cleanPath(dirPath);
         return;
    }

    auto els = dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);
    for(const auto& el : els ) {
        if(el.isDir()) {
            clearAll(el.filePath());
        }
        if(el.isFile()) {
            if (el.fileName().contains(codeAtlas) && dir.remove(el.fileName())) if( !isQuietMode ) qStdOut() << "\t--> [Removed]\t" << QDir::cleanPath(el.filePath());
        }
    }
}

void clearAll_v2(QString dirPath) {
    if(dirPath.isEmpty()) return;
    formatLast(dirPath, separator);

    auto dir = QDir::current();

    // if( !isQuietMode ) qDebug() << __FUNCTION__ << dirPath;

    auto paths = QString(logFile.readAll()).trimmed().split('\n');
    for(auto path : paths) {
        QFileInfo fi(path);
        if(fi.isFile()) if(dir.remove(path)) if( !isQuietMode ) qStdOut() << "\t--> [Removed]\t" << QDir::cleanPath(path);
        if(fi.isDir()) {
            auto subDir = QDir(path);
            if( subDir.removeRecursively() ) if( !isQuietMode ) qStdOut() << "\t--> [Removed]\t" << QDir::cleanPath(path);
        }
    }
}

bool createAtlasImage (QString dirPath, int col = -1, int row = -1) {
    if(dirPath.isEmpty()) return false;
    formatLast(dirPath, separator);

    auto dir = QDir(dirPath);
    if( !dir.exists() ) {
        qStdOut() << QDir::cleanPath(dirPath) << " : no such directory";
        return false;
    }

    // if( !isQuietMode ) qDebug() << __FUNCTION__ << dirPath;

    auto files = dir.entryInfoList(QStringList(reFrameImageConverted), QDir::Files, QDir::Name);
    if(col == -1 && row == -1) {
        col = std::sqrt(files.length());
        row = (files.length() - 1) / col  + 1;
    } else if (col == -1) {
        col = (files.length() - 1) / row  + 1;
    } else if (row == -1) {
        row = (files.length() - 1) / col  + 1;
    }

    auto createdAtlas = false;
    auto atlasImage = QImage();
    auto w = 0, h = 0;
    auto i = 1, j = 1;
    for(const auto& file : files ) {
        auto image = QImage(file.filePath());

        if( !createdAtlas ) {
            w = image.width();
            h = image.height();
            atlasImage = QImage(w * col, h * row ,QImage::Format::Format_ARGB32);
            for (int x = 0; x < w * col; x++) {
                for (int y = 0; y < h * row ; y++ ){
                    atlasImage.setPixelColor(x, y,  QColor::fromRgba(qRgba(0,0,0,0)));
                }
            }

            createdAtlas = true;
        }

        auto iw = image.width();
        auto ih = image.height();

        if(w != iw || h != ih) {
            qStdOut() << "============> Please check size image: " << QDir::cleanPath(file.filePath());
        }

        for (int x = 0; x < w; x++) {
            for (int y = 0; y < h ; y++ ){
                if(x < iw && y < ih) atlasImage.setPixel((j-1) * w + x,(i-1)* h + y, image.pixel(x,y) );
            }
        }

        j++;
        if ( j > col) {
            j = 1;
            i++;
        }
    }
    auto atlasPath = dirPath + "../" + dir.dirName().replace(codeAtlas.isEmpty() ? "_" + codeAni : codeAni, codeAtlas + extension);
    if( atlasImage.save(atlasPath) ) {
        writeLineLog(QDir::cleanPath(atlasPath));
        if( !isQuietMode ) qStdOut() << "\t--> [Saved]\t" << QDir::cleanPath(atlasPath);
        countSucces++;
        return true;
    }
    else {
        if( !isQuietMode ) qStdOut() << "\t--> [Cannot save]\t" << QDir::cleanPath(atlasPath);
        return false;
    }

    return true;
}

bool standardizedImageName (QString dirPath) {
    if(dirPath.isEmpty()) return false;
    formatLast(dirPath, separator);

    auto dir = QDir(dirPath);
    if( !dir.exists() ) {
        qStdOut() << QDir::cleanPath(dirPath) << " : no such directory";
        return false;
    }

    // if( !isQuietMode ) qDebug() << __FUNCTION__ << dirPath;

    auto files = dir.entryInfoList(QStringList({"*" + extension}), QDir::Files);

    auto re = QRegularExpression(reFrameImage);

    auto listAniStatus = QStringList();
    auto listAni = QStringList();
    for(const auto& file : files ) {
        auto filename = file.fileName();
        auto match = re.match(filename);
        if (match.hasMatch()) {
            auto name = match.captured("name");
            formatLast(name, QString("_"));
            auto newFileName = name + match.captured("number").rightJustified(5,'0',false) + extension;
            auto subDirName = name + codeAni;
            auto subDirPath = dirPath + subDirName;
            formatLast(subDirPath, separator);
//            if( !listAni.contains(subDirPath) ) listAni.push_back(subDirPath);

            if(dir.mkdir(subDirName)) {
                writeLineLog(QDir::cleanPath(subDirPath));
                if( !isQuietMode ) qStdOut() << "\t--> [Created]\t" << QDir::cleanPath(subDirPath);
                listAniStatus.push_back(subDirPath);
                listAni.push_back(subDirPath);
            }
            if(QFile::copy(file.filePath(), subDirPath + newFileName)) {
                if( !listAniStatus.contains(subDirPath) ) {
                    if( !isQuietMode ) qStdOut() << "\t--> [Updated]\t" << QDir::cleanPath(subDirPath);
                    listAniStatus.push_back(subDirPath);
                    listAni.push_back(subDirPath);
                }
            }

        } else {
            if( !isQuietMode ) {
                auto filePath = QDir::cleanPath(dirPath + filename);
                // ignore generated by tool. such as xxx_atlas.png
                if(!logContent.contains(filePath)) qStdOut() << "\t--> [Ignored]\t" << filePath;
            }
        }
    }

    for(const auto& aniPath: listAni) {
        createAtlasImage(aniPath, col, row);
    }

    auto subDirs = dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot);
    for(const auto& subDir : subDirs ) {
        auto subDirPath = subDir.filePath();
        if(! subDirPath.endsWith(codeAni)) {
            standardizedImageName(subDirPath);
        }
    }

    return true;
}

void run(QString dirPath) {
    standardizedImageName(dirPath);
    qStdOut() << "\n\nTotal atlas are generated: " << countSucces;
    return;
}

QCommandLineOption setQCommandLineOptionHidden(QCommandLineOption&& o) {
    o.setFlags(QCommandLineOption::Flag::HiddenFromHelp);
    return o;
}

bool setConfig(QString filePath) {
    if(filePath.isEmpty()) return false;

    QFile fileConfig(filePath);


    if (!fileConfig.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qStdOut() << QDir::cleanPath(filePath) << " : no such directory";
        return false;
    }

    auto lines = QString(fileConfig.readAll()).trimmed().split('\n');
    if(lines.length() < 4) return false;

    codeAni = QString(lines[0]);
    codeAtlas = QString(lines[1]);
    extension = QString(lines[2]);
    reFrameImage = QString(lines[3]);
    reFrameImageConverted = QStringList{QString("*\[0-9\]*" + extension)};

    return true;

}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOptions({
        {"clean", "remove all generated standardized animation folder"},
        {"cleanall", "remove all generated files, folders"},
        {{"d", "directory"}, "set directory", "dir", "."},
        {{"r", "row"}, "set number of rows for atlas image", "number", "-1"},
        {{"c", "col"}, "set number of coloumns for atlas image ", "number", "-1"},
        {{"q", "quiet"}, "run in quiet mode"},
        setQCommandLineOptionHidden(QCommandLineOption("config", "show secret config")),
        setQCommandLineOptionHidden(QCommandLineOption({"f", "file-config"}, "set config", "file")),
    });

    parser.process(app);

    if(parser.isSet("file-config")) {
        auto fileConfigPath = parser.value("f");
        if( !setConfig(fileConfigPath) ) {
            return -1;
        }

    }

    if(parser.isSet("config")) {
        qDebug() << codeAni;
        qDebug() << codeAtlas;
        qDebug() << extension;
        qDebug() << reFrameImage;

        return 0;
    }

    isQuietMode = parser.isSet("q");
    isClean = parser.isSet("clean");
    isCleanAll = parser.isSet("cleanall");
    auto dirPath = parser.value("d");

    openLog(dirPath);

    if(isCleanAll) {
//        clearAll(dirPath);
        clearAll_v2(dirPath);
    } else if( isClean ) {
        clean(dirPath);
    } else {
        row = parser.value("r").toInt();
        col = parser.value("c").toInt();
        run(dirPath);
    }

    closeLog();

    return 0;
}
