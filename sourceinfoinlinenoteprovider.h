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

#ifndef SOURCEINFOINLINENOTEPROVIDER_H
#define SOURCEINFOINLINENOTEPROVIDER_H

#include <QMap>
#include <QVector>

#include <KTextEditor/Cursor>
#include <KTextEditor/InlineNoteInterface>
#include <KTextEditor/InlineNoteProvider>

#include "notes/inlinenotebase.h"


namespace KDevelop {
class IDocument;
class DUContext;
class TopDUContext;
}


class SourceInfoConfig : public QObject
{
    Q_OBJECT

public:
    bool showFunctionArgumentNames = true;
    bool showFunctionArgumentDefaultValues = true;
    bool showStructFieldSize = true;
    bool showAutoType = true;
    bool showEnumConstValues = true;

Q_SIGNALS:
    void changed();
};


class SourceInfoInlineNoteProvider : public KTextEditor::InlineNoteProvider
{
    Q_OBJECT

public:
    SourceInfoInlineNoteProvider(QSharedPointer<SourceInfoConfig> config, KTextEditor::Document* document);
    ~SourceInfoInlineNoteProvider();

    QVector<int> inlineNotes(int line) const override;
    QSize inlineNoteSize(const KTextEditor::InlineNote& note) const override;
    void paintInlineNote(const KTextEditor::InlineNote& note, QPainter& painter) const override;

private Q_SLOT:
    void configChanged();

private:
    void registerToView(KTextEditor::Document* /*document*/, KTextEditor::View* view);

    void deleteNotes();
    void rebuildNotes();

    void walkContext(KDevelop::DUContext* ctx, KDevelop::TopDUContext* top);

private:
    KTextEditor::Document* m_document;

    QMap<KTextEditor::Cursor, const InlineNoteBase *> m_notes; // TODO: Differently?

    QSharedPointer<SourceInfoConfig> m_config;
};

#endif // SOURCEINFOINLINENOTEPROVIDER_H
