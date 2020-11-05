#include "highlighter.hpp"
#include <QDebug>

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(QBrush(QColor(86, 155, 213)));
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

    // operators
    operatorFormat.setForeground(QBrush(QColor(155, 219, 253)));
    const QString operatorPatterns[] = {
        QStringLiteral("\\s?\\|\\|\\s?"),
        QStringLiteral("\\s?->\\s?"),
        QStringLiteral("\\s?\\.o\\.\\s?"),
        QStringLiteral("\\s?_\\s?"),
        QStringLiteral("\\s?@\\s?"),
    };

    for (const QString &pattern : operatorPatterns) {
        rule.format = operatorFormat;
        rule.pattern = QRegularExpression(pattern);
        highlightingRules.append(rule);
    }

    // apply down
    rule.format.setForeground(QBrush(QColor(224, 146, 207)));
    rule.format.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression(QStringLiteral("\\bapply down\\b"));
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(QBrush(QColor(106, 152, 85)));
    rule.pattern = QRegularExpression(QStringLiteral("#[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);
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
}


