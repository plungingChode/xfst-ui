#include "highlighter.hpp"
#include <QDebug>

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] = {
        QStringLiteral("\\bdefine\\b"),
        QStringLiteral("\\bregex\\b"),
    };

    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    singleLineCommentFormat.setForeground(Qt::darkCyan);
    rule.pattern = QRegularExpression(QStringLiteral("#[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    rule.format.setForeground(Qt::magenta);
    rule.format.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression(QStringLiteral("\\bapply down\\b"));
    highlightingRules.append(rule);

    identifierSearch = QRegularExpression(QStringLiteral("define\\s(.+?)\\s"));
}

void Highlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &r : qAsConst(highlightingRules)) {
        auto matchIterator = r.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            auto match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), r.format);
        }
    }

//    auto idMatches = identifierSearch.globalMatch(text);
//    QVector<QString> matched;
//    while (idMatches.hasNext()) {
//        auto match = idMatches.next().captured(1);
//        if (!identifiers.contains(match)) {
//            HighlightingRule r = {
//                QRegularExpression("\\b" + match + "\\b"),
//                identifierFormat
//            };
//            identifiers.insert(match, r);
//        }
//        matched.append(match);
//    }
}


