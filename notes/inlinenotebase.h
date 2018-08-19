#ifndef INLINENOTEBASE_H
#define INLINENOTEBASE_H

class InlineNoteBase
{
public:
    /**
     * Virtual destructor to allow inheritance.
     */
    virtual ~InlineNoteBase() = default;

    /**
     * Column on which the note is located.
     *
     * 0 means the note is located before the first character of the line.
     * 1 means the note is located after the first character, etc. If the
     * returned number is bigger than the length of the line, the note will be
     * placed behind the text as if there were additional spaces.
     */
    virtual int column() const = 0;

    /**
     * Width to be reserved for the note in the text.
     *
     * The method is given the height of the line and the metrics of current
     * font which it may use for calculating the width.
     *
     * \param height the height of the line in pixels
     * \param fontMetrics the QFontMetricsF of the font used by the editor
     *
     * \return the width of the note in pixels
     */
    virtual qreal width(qreal height, const QFontMetricsF &fontMetrics) const = 0;

    /**
     * Paint the note into the line.
     *
     * The method should use the given painter to render the note into the
     * line. The painter is translated such that coordinates 0x0 mark the top
     * left corner of the note. The method should not paint outside rectangle
     * given by the height parameter and the width previously returned by the
     * width method.
     *
     * The method is given the height of the line, the metrics of current font
     * and the font which it may use during painting.
     *
     * \param height the height of the line in pixels
     * \param fontMetrics the QFontMetricsF of the font used by the editor
     * \param font the QFont used in the editor
     * \param painter painter prepared for rendering the note
     */
    virtual void paint(qreal height, const QFontMetricsF &fontMetrics, const QFont &font, QPainter &painter) const = 0;
};

#endif
