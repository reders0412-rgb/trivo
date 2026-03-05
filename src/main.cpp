#include <QApplication>
#include <QSurfaceFormat>
#include <QDir>
#include <QIcon>
#include "core/MainWindow.h"
#include "i18n/I18n.h"

int main(int argc, char *argv[])
{
    // ── OpenGL surface format ─────────────────────────────────────────────────
    QSurfaceFormat fmt;
    fmt.setVersion(4, 1);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setSamples(8); // 8x MSAA
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication app(argc, argv);
    app.setApplicationName("Trivo");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Trivo");
    app.setWindowIcon(QIcon(":/icons/trivo_app.svg"));

    // ── i18n ──────────────────────────────────────────────────────────────────
    I18n::instance().init();

    // ── Main window ───────────────────────────────────────────────────────────
    MainWindow w;
    w.show();

    // ── CLI: open file if passed ──────────────────────────────────────────────
    const QStringList args = app.arguments();
    for (int i = 1; i < args.size(); ++i) {
        if (QFile::exists(args[i])) {
            w.openFile(args[i]);
        }
    }

    return app.exec();
}
