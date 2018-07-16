/*
 * Copyright 2018 Michal Srb <michalsrb@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "sourceinfoplugin.h"
#include "sourceinfoinlinenoteprovider.h"
#include "sourceinfotoolview.h"

#include <QUrl>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KPluginFactory>

#include <interfaces/icore.h>
#include <interfaces/idocument.h>
#include <interfaces/idocumentcontroller.h>
#include <interfaces/iuicontroller.h>

#include <debug.h>


using namespace KDevelop;

class SourceInfoToolViewFactory: public KDevelop::IToolViewFactory
{
public:
    explicit SourceInfoToolViewFactory(QSharedPointer<SourceInfoConfig> config): m_config(config) {}

    QWidget* create(QWidget *parent = nullptr) override
    {
        SourceInfoToolView *view = new SourceInfoToolView(m_config, parent);
        return view;
    }

    Qt::DockWidgetArea defaultPosition() override
    {
        return Qt::RightDockWidgetArea;
    }

    QString id() const override
    {
        return QStringLiteral("org.kdevelop.SourceInfo");
    }

private:
    QSharedPointer<SourceInfoConfig> m_config;
};

//KPluginFactory stuff to load the plugin dynamically at runtime
K_PLUGIN_FACTORY_WITH_JSON(KDevExecuteFactory, "kdevsourceinfo.json", registerPlugin<SourceInfoPlugin>();)

SourceInfoPlugin::SourceInfoPlugin(QObject *parent, const QVariantList&)
    : KDevelop::IPlugin("kdevsourceinfo", parent)
    , m_config(QSharedPointer<SourceInfoConfig>::create())
    , m_viewFactory(new SourceInfoToolViewFactory(m_config))
{
    core()->uiController()->addToolView(i18n("Source Info"), m_viewFactory);

    auto docController = ICore::self()->documentController();

    for (auto *document : docController->openDocuments()) {
        documentOpened(document);
    }

    connect(docController, &IDocumentController::textDocumentCreated, this, &SourceInfoPlugin::documentOpened);
    connect(docController, &IDocumentController::documentClosed, this, &SourceInfoPlugin::documentClosed);
}

SourceInfoPlugin::~SourceInfoPlugin()
{
}

void SourceInfoPlugin::unload()
{
    core()->uiController()->removeToolView(m_viewFactory);

    auto docController = ICore::self()->documentController();
    for (auto *document : docController->openDocuments()) {
        documentClosed(document);
    }
}

void SourceInfoPlugin::documentOpened(KDevelop::IDocument* document)
{
    if (document->isTextDocument()) {
        auto textDocument = document->textDocument();

        KTextEditor::InlineNoteInterface* iface = qobject_cast<KTextEditor::InlineNoteInterface*>((QObject* /*xxx?*/)textDocument);

        if (!iface) {
            return;
        }

        auto *provider = new SourceInfoInlineNoteProvider(m_config, document);

        m_documentToProviderMap.insert(textDocument, provider);
        iface->registerInlineNoteProvider(provider);
    }
}

void SourceInfoPlugin::documentClosed(KDevelop::IDocument* document)
{
    if (document->isTextDocument()) {
        auto textDocument = document->textDocument();

        KTextEditor::InlineNoteInterface* iface = qobject_cast<KTextEditor::InlineNoteInterface*>((QObject* /*xxx?*/)textDocument);

        if (!iface) {
            return;
        }

        auto provider = m_documentToProviderMap.find(textDocument);
        if (provider != m_documentToProviderMap.end()) {
            iface->unregisterInlineNoteProvider(*provider);
            delete *provider;
            m_documentToProviderMap.erase(provider);
        }
    }
}


#if 0
QUrl SourceInfoPlugin::url( KDevelop::ILaunchConfiguration* cfg, QString& err_ ) const
{
    QUrl url;

    if( !cfg )
    {
        return url;
    }
    KConfigGroup grp = cfg->config();

    QString host = grp.readEntry( SourceInfoPlugin::serverEntry, "" );
    if( host.isEmpty() )
    {
        err_ = i18n("Empty server in launch configuration");
        qCWarning(KDEV_SOURCEINFO) << "Launch Configuration:" << cfg->name() << err_;
        return url;
    }

    QString path(grp.readEntry( SourceInfoPlugin::pathEntry, "" ));
    if( !host.endsWith("/") && !path.isEmpty() && !path.startsWith("/") ) {
        path.prepend("/");
    }
    if( !host.contains(QStringLiteral("://")) )
    {
        host.prepend(QStringLiteral("http://"));
    }

    url.setUrl(host + path);
    url.setPort(grp.readEntry( SourceInfoPlugin::portEntry, 80 ));
    {
        QString q = grp.readEntry( SourceInfoPlugin::argumentsEntry, "" );
        if (!q.isEmpty()) {
            url.setQuery(q);
        }
    }

    if( url.toString().isEmpty() ) {
        err_ = i18n("Invalid launch configuration");
        qCWarning(KDEV_SOURCEINFO) << "Launch Configuration:" << cfg->name() << err_;
        return url;
    }
    qCDebug(KDEV_SOURCEINFO) << "Url:" << url.toString();
    return url;
}

QString SourceInfoPlugin::browser( ILaunchConfiguration* cfg ) const
{
    return cfg->config().readEntry( SourceInfoPlugin::browserEntry, "" );
}

QString SourceInfoPlugin::browserAppConfigTypeId() const
{
    return _browserAppConfigTypeId;
}
#endif


#include "sourceinfoplugin.moc"
