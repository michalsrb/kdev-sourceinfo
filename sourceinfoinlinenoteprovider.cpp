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

#include <language/duchain/duchain.h>
#include <language/duchain/duchainlock.h>
#include <language/duchain/duchainutils.h>
#include <language/duchain/topducontext.h>
#include <language/duchain/ducontext.h>
#include <language/duchain/declaration.h>
#include <language/duchain/classmemberdeclaration.h>
#include <language/duchain/functiondeclaration.h>
#include <language/duchain/functiondefinition.h>
#include <language/duchain/use.h>
#include <language/duchain/types/enumeratortype.h>
#include <language/duchain/types/functiontype.h>

#include <kdevplatform/interfaces/idocument.h>

#include <KTextEditor/Range>

#include "sourceinfoinlinenoteprovider.h"

#include "notes/generictextnote.h"
#include "notes/membersizenote.h"


using namespace KDevelop;


SourceInfoInlineNoteProvider::SourceInfoInlineNoteProvider(QSharedPointer<SourceInfoConfig> config, IDocument* doc)
    : m_doc(doc)
    , m_config(config)
{
    connect(m_config.data(), &SourceInfoConfig::changed, this, &SourceInfoInlineNoteProvider::configChanged);

    rebuildNotes();

    // we want to rebuild notes whenever the current document has been reparsed
    connect(DUChain::self(), &DUChain::updateReady, this, &SourceInfoInlineNoteProvider::rebuildNotes);
}

SourceInfoInlineNoteProvider::~SourceInfoInlineNoteProvider()
{
    deleteNotes();
}

QVector<const KTextEditor::InlineNote *> SourceInfoInlineNoteProvider::inlineNotes(int line)
{
    if (m_notes.contains(line)) {
        return m_notes[line];
    } else {
        return {};
    }
}

void SourceInfoInlineNoteProvider::configChanged()
{
    rebuildNotes();
}

void SourceInfoInlineNoteProvider::deleteNotes()
{
    for (auto &notesOnLine : m_notes) {
        for (auto *note : notesOnLine) {
            delete note;
        }
    }
    m_notes.clear();
}

void SourceInfoInlineNoteProvider::rebuildNotes()
{
    deleteNotes();

    DUChainReadLocker lock;
    TopDUContext* topContext = DUChainUtils::standardContextForUrl(m_doc->url());
    if (topContext) {
        walkContext(topContext, topContext);
    }

    emit reset();
}

void SourceInfoInlineNoteProvider::walkContext(KDevelop::DUContext* ctx, KDevelop::TopDUContext* top)
{
    if (m_config->showEnumConstValues) {
        // Add " = 123" notes after enums that do not have explicit value.
        if (ctx->type() == DUContext::ContextType::Enum) {
            foreach (const Declaration* declaration, ctx->localDeclarations(top)) {
                if(EnumeratorType::Ptr enumerator = declaration->type<EnumeratorType>()) {
                    const CursorInRevision &pos = declaration->range().end;

                    // XXX: Ugly and slow hack to figure out whether the enum value is set explicitly or not.
                    QString followingText = m_doc->text(KTextEditor::Range(pos.line, pos.column, pos.line, pos.column + 100 /*xxx*/ ));
                    if (followingText.trimmed().startsWith('=')) continue;

                    KTextEditor::InlineNote *note = new GenericTextNote(pos.column, QString::fromUtf8(" = ") + enumerator->valueAsString(), Qt::gray, QBrush(), false, 0.0);
                    m_notes[pos.line].push_back(note);
                }
            }
        }
    }

    if (m_config->showAutoType) {
        // Add notes with the derived type of auto declarations
        foreach (const Declaration* declaration, ctx->localDeclarations(top)) {
            if (declaration->kind() != Declaration::Instance) continue;

            const CursorInRevision &pos = declaration->range().start;

            // XXX: Ugly, slow and unreliable way to determine if the declaration is C++ "auto"
            if (pos.column < 5) continue;
            const auto range = KTextEditor::Range(pos.line, pos.column - 5, pos.line, pos.column);
            if (m_doc->text(range) != "auto ") continue;

            const AbstractType::Ptr abstractType = declaration->abstractType();
            if (!abstractType) continue;

            QString text = "= " + abstractType->toString();

            KTextEditor::InlineNote *note = new GenericTextNote(range.end().column() - 1, text, QColor(0x9090b0), QBrush(QColor(0xf5f5f5)), true, 4.0, 6.0);
            m_notes[range.end().line()].push_back(note);
        }
    }

    if (m_config->showStructFieldSize) {
        // Add member size and offset notes behind struct fields
        if (ctx->type() == DUContext::ContextType::Class) {
            QVector<MemberSizeNote*> memberNotes;
            MemberSizeNote *previousNote = nullptr;
            uint64_t previousBytesOffsetOf = 0;
            int maxColumn = 0;
            foreach (const Declaration* declaration, ctx->localDeclarations(top)) {
                if (declaration->kind() != Declaration::Instance) continue;
                if (declaration->isFunctionDeclaration()) continue;

                const ClassMemberDeclaration* classMemberDeclaration = dynamic_cast<const ClassMemberDeclaration*>(declaration);
                if (!classMemberDeclaration) continue;
                if (classMemberDeclaration->isStatic()) continue;

                const CursorInRevision &pos = declaration->range().end;

                uint64_t bytesOffsetOf = classMemberDeclaration->bitOffsetOf() / 8; // TODO: Display somehow bit offets?

                MemberSizeNote *note = new MemberSizeNote(
                    0,
                    classMemberDeclaration->sizeOf(),
                    0,
                    bytesOffsetOf,
                    4 /* TODO: Configurable */
                );
                if (previousNote) {
                    previousNote->setPadding(bytesOffsetOf - previousBytesOffsetOf - previousNote->size());
                }
                previousNote = note;
                previousBytesOffsetOf = bytesOffsetOf;
                m_notes[pos.line].push_back(note);

                memberNotes.push_back(note);

                // XXX: Ugly, slow and unreliable way to find the end of line
                const auto range = KTextEditor::Range(pos.line, pos.column, pos.line, pos.column + 500 /* xxx */);
                int endColumn = pos.column + m_doc->text(range).length() + 1;
                if (endColumn > maxColumn) {
                    maxColumn = endColumn;
                }
            }
            foreach (MemberSizeNote* memberNote, memberNotes) {
                memberNote->setColumn(memberNote->column() + maxColumn);
            }
        }
    }

    if (m_config->showFunctionArgumentNames || m_config->showFunctionArgumentDefaultValues) {
        // Display function parameter names on call sites
        // and values of default parameters.
        for (int i = 0; i < ctx->usesCount(); i++) {
            const auto &use = ctx->uses()[i];
            Declaration* declaration = top->usedDeclarationForIndex(use.m_declarationIndex);
            if (!declaration) continue;

            if(FunctionType::Ptr function = declaration->type<FunctionType>()) {
                // Do not show if the function has no or one argument (TODO: the later configurable?)
                if (function->indexedArgumentsSize() <= 1) {
                    continue;
                }

                if (DUContext* argumentContext = DUChainUtils::getArgumentContext(declaration)) {
                    CursorInRevision pos = use.m_range.end;

                    // XXX: Ugly, slow and incorrect hack to figure out where the parameters are
                    QString followingText = m_doc->text(KTextEditor::Range(pos.line, pos.column, pos.line + 10 /* xxx */, pos.column + 500 /* xxx */ ));
                    int stackDepth = -1;
                    int argumentIndex = 0;
                    bool argumentPending = false;

                    auto decls = argumentContext->localDeclarations(top);

                    int char_i = 0;

                    // First skip any whitespaces // XXX: Comments here will break stuff
                    while (char_i < followingText.length() && followingText.at(char_i).isSpace()) {
                        char_i++;
                    }

                    if (followingText.at(char_i) != '(') break; // So the use was not a function call (e.g. taking address of the function, nevermind)

                    // Go thru the arguments and every time we find beginning of expression in place of argument, place a note with the argument name
                    for(;
                        char_i < followingText.length() &&
                        argumentIndex < function->indexedArgumentsSize() &&
                        argumentIndex < decls.size();
                        char_i++)
                    {
                        QChar c = followingText.at(char_i);
                        // XXX: Very primitive parser, does not understand strings and many other things!
                        if (c == '(' || c == '{' || c == '[') stackDepth++;
                        if (c == ')' || c == '}' || c == ']') stackDepth--;

                        if (c == ')' && stackDepth == -1) {
                            if (m_config->showFunctionArgumentDefaultValues) {
                                // If we reach the end and still have arguments left, we expect they have default values. Put out note with them.
                                if (argumentIndex < function->indexedArgumentsSize()) {
                                    if (FunctionDeclaration* functionDeclaration = dynamic_cast<FunctionDeclaration*>(declaration)) {
                                        QString text;

                                        for (; argumentIndex < function->indexedArgumentsSize() && argumentIndex < decls.size(); argumentIndex++) {
                                            text += ", ";
                                            if (m_config->showFunctionArgumentNames) {
                                                auto indentifier = decls[argumentIndex]->identifier();
                                                if (!indentifier.isEmpty()) text += indentifier.toString() + ": ";
                                            }
                                            text += functionDeclaration->defaultParameterForArgument(argumentIndex).str();
                                        }

                                        GenericTextNote *note = new GenericTextNote(pos.column, text, QColor(0x9090b0), QBrush(QColor(0xf5f5f5)), true, 4.0);
                                        m_notes[pos.line].push_back(note);
                                    }
                                }
                            }
                            break;
                        }

                        if (argumentPending && !c.isSpace()) {
                            if (m_config->showFunctionArgumentNames) {
                                auto identifier = decls[argumentIndex]->identifier();
                                if (!identifier.isEmpty()) {
                                    QString text = identifier.toString() + ":";
                                    GenericTextNote *note = new GenericTextNote(pos.column, text, QColor(0x9090b0), QBrush(QColor(0xf5f5f5)), true, 4.0);
                                    note->setSpaceRight(true);
                                    m_notes[pos.line].push_back(note);
                                }
                            }
                            argumentIndex++;
                            argumentPending = false;
                        }

                        if (stackDepth == 0 && (c == '(' || c == ',')) {
                            argumentPending = true;
                        }

                        if (c == '\n') {
                            pos.column = 0;
                            pos.line++;
                        } else {
                            pos.column++;
                        }
                    }
                }
            }
        }
    }

    if (m_config->showFunctionArgumentDefaultValues) {
        // Display default argument values at function definition
        foreach (const Declaration* declaration, ctx->localDeclarations(top)) {
            if (declaration->kind() != Declaration::Instance) continue;

            if (const FunctionDefinition* functionDefinition = dynamic_cast<const FunctionDefinition*>(declaration)) {
                if (!functionDefinition->isDefinition()) continue; // Only definitions, declarations already have the default parameters

                const FunctionDeclaration* functionDeclaration = dynamic_cast<const FunctionDeclaration*>(functionDefinition->declaration());
                if (!functionDeclaration) continue;
                if (functionDeclaration->defaultParametersSize() == 0) continue;

                auto *argumentContext = functionDefinition->internalContext();
                if (!argumentContext) continue;

                int argumentIndex = 0;
                foreach (const Declaration* argumentDeclaration, argumentContext->localDeclarations(top)) {
                    if (argumentDeclaration->kind() != Declaration::Instance) continue;

                    const auto identifier = functionDeclaration->defaultParameterForArgument(argumentIndex);
                    if (!identifier.isEmpty()) {
                        const CursorInRevision &pos = argumentDeclaration->range().end;
                        QString text = " = " + identifier.str();
                        GenericTextNote *note = new GenericTextNote(pos.column, text, QColor(0x9090b0), QBrush(QColor(0xf5f5f5)), true, 4.0);
                        m_notes[pos.line].push_back(note);
                    }

                    argumentIndex++;
                }
            }
        }
    }

#if 0
    // Make every use fully qualified
    {
        CursorInRevision lastPosEnd;
        for (int i = 0; i < ctx->usesCount(); i++) {
            const auto &use = ctx->uses()[i];
            Declaration* declaration = top->usedDeclarationForIndex(use.m_declarationIndex);
            if (!declaration) continue;

            const CursorInRevision &pos = use.m_range.start;

            bool separatedByDoubleColon = false;
            if (lastPosEnd.column != 0 && lastPosEnd.line != 0) {
                // XXX: Ugly, slow and unreliable way to determine if the previous and this use of something are separate only by "::" and whitespace
                QString tmp = m_doc->text(KTextEditor::Range(lastPosEnd.castToSimpleCursor(), pos.castToSimpleCursor()));
                separatedByDoubleColon = (tmp.trimmed() == "::");
            }

            lastPosEnd = use.m_range.end;
            if (separatedByDoubleColon) {
                continue;
            }

            QStringList parts = declaration->qualifiedIdentifier().toStringList(RemoveTemplateInformation);
            if (parts.size() <= 1) {
                continue;
            }
            parts.removeLast();
            QString text = parts.join("::") + "::";

            GenericTextNote *note = new GenericTextNote(pos.column, text, QColor(0x9090b0), QBrush(QColor(0xf5f5f5)), true, 4.0);
            m_notes[pos.line].push_back(note);
        }
    }
#endif

#if 0
    // Put note to all declarations
    foreach (const Declaration* declaration, ctx->localDeclarations(top)) {
        const CursorInRevision &pos = declaration->range().end;

        QString text = QString(" declaration(") +
            "auto: " + (declaration->isAutoDeclaration() ? "true" : "false") + ", " +
            "kind: " + QString::number(declaration->kind()) + ", " +
            "type modifiers: " + QString::number(declaration->abstractType()->modifiers()) + ", " +
            "type as string: " + declaration->abstractType()->toString() + ") ";

        KTextEditor::InlineNote *note = new GenericTextNote(pos.column, text, Qt::white, QBrush(QColor(0x3a496c)), true, 4.0);
        m_notes[pos.line].push_back(note);
    }
#endif

#if 0
    // Put note to all uses
    for (int i = 0; i < ctx->usesCount(); i++) {
        const auto &use = ctx->uses()[i];
        const CursorInRevision &pos = use.m_range.start;

        KTextEditor::InlineNote *note = new GenericTextNote(pos.column, QString::fromUtf8("Use: "), Qt::white, QBrush(QColor(0x3a496c)), true, 4.0);

        m_notes[pos.line].push_back(note);
    }
#endif

#if 0
    // Put note to all context
    {
        const auto &pos = ctx->range().end;
        QString text = QString::fromUtf8(" <- End context (") + QString::number(ctx->type()) + ") ";
        KTextEditor::InlineNote *note = new GenericTextNote(pos.column, text, Qt::white, QBrush(QColor(0x3a496c)), true, 4.0);
        m_notes[pos.line].push_back(note);
    }
    {
        const auto &pos = ctx->range().start;
        QString text = QString::fromUtf8(" Start context (") + QString::number(ctx->type()) + ") -> ";
        KTextEditor::InlineNote *note = new GenericTextNote(pos.column, text, Qt::white, QBrush(QColor(0x3a496c)), true, 4.0);
        m_notes[pos.line].push_back(note);
    }
#endif

    foreach (DUContext* childContext, ctx->childContexts()) {
        walkContext(childContext, top);
    }
}
