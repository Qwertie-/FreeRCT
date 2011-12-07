/* $Id$ */

/*
 * This file is part of FreeRCT.
 * FreeRCT is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * FreeRCT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with FreeRCT. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file widget.h Widget declarations. */

#ifndef WIDGET_H
#define WIDGET_H

#include "geometry.h"

struct BorderSpriteData;

static const int INVALID_WIDGET_INDEX = -1; ///< Widget number of invalid index.

/** Available widget types. */
enum WidgetType {
	WT_EMPTY,          ///< Empty widget (used for creating empty space and/or centering).
	WT_TITLEBAR,       ///< Title of the window.
	WT_CLOSEBOX,       ///< Close box.
	WT_RESIZEBOX,      ///< Resize box.
	WT_LEFT_TEXT,      ///< Text label with left-aligned text.
	WT_CENTERED_TEXT,  ///< Text label with centered text.
	WT_RIGHT_TEXT,     ///< Text label with right-aligned text.
	WT_PANEL,          ///< Panel.
	WT_TEXTBUTTON,     ///< Button with text.
	WT_IMAGEBUTTON,    ///< Button with a sprite.
	WT_RADIOBUTTON,    ///< Radio button widget.
	WT_HOR_SCROLLBAR,  ///< Scrollbar widget.
	WT_VERT_SCROLLBAR, ///< Scrollbar widget.
	WT_GRID,           ///< Intermediate widget.
};

/** Padding space around widgets. */
enum PaddingDirection {
	PAD_TOP,        ///< Padding at the top.
	PAD_LEFT,       ///< Padding at the left.
	PAD_RIGHT,      ///< Padding at the right.
	PAD_BOTTOM,     ///< Padding at the bottom.
	PAD_VERTICAL,   ///< Inter-child vertical padding space.
	PAD_HORIZONTAL, ///< Inter-child horizontal padding space.

	PAD_COUNT,      ///< Number of paddings.
};

/**
 * Base class for all widgets.
 * Also implements #WT_EMPTY, #WT_CLOSEBOX, #WT_RESIZEBOX.
 */
class BaseWidget {
public:
	BaseWidget(WidgetType wtype);
	virtual ~BaseWidget();

	virtual void SetupMinimalSize(BaseWidget **wid_array);
	virtual void SetSmallestSizePosition(const Rectangle16 &rect);
	virtual void Draw(const Point &base);

	WidgetType wtype; ///< Widget type.
	int16 number;     ///< Widget number.

	uint16 min_x;     ///< Minimal horizontal size.
	uint16 min_y;     ///< Minimal vertical size.
	Rectangle16 pos;  ///< Current position and size (relative to window top-left edge).
	uint16 fill_x;    ///< Horizontal fill step.
	uint16 fill_y;    ///< Vertical fill step.
	uint16 resize_x;  ///< Horizontal resize step.
	uint16 resize_y;  ///< Vertical resize step.
	uint8 paddings[PAD_COUNT]; ///< Padding.

protected:
	void SetWidget(BaseWidget **wid_array);

	void InitMinimalSize(uint16 content_width, uint16 content_height, uint16 border_hor, uint16 border_vert);
	void InitMinimalSize(const BorderSpriteData *bsd, uint16 content_width, uint16 content_height);
};

/** Flags of the #LeafWidget widget. */
enum LeafWidgetFlags {
	LWF_CHECKED = 1, ///< Widget is checked (on/off).
	LWF_PRESSED = 2, ///< Widget is pressed (button up/down).
	LWF_SHADED  = 4, ///< Widget is shaded (enabled/disabled).
};
DECLARE_ENUM_AS_BIT_SET(LeafWidgetFlags)

/**
 * Base class for a (visible) leaf widget.
 * Implements #WT_RADIOBUTTON
 * @todo use #LeafWidget::colour.
 */
class LeafWidget : public BaseWidget {
public:
	LeafWidget(WidgetType wtype);

	virtual void SetupMinimalSize(BaseWidget **wid_array);
	virtual void Draw(const Point &base);

	uint8 flags;    ///< Flags of the leaf widget. @see LeafWidgetFlags
	uint8 colour;   ///< Colour of the widget.
	uint16 tooltip; ///< Tool-tip of the widget.
};

/**
 * Data widget
 * Implements #WT_TITLEBAR, #WT_LEFT_TEXT, #WT_CENTERED_TEXT, #WT_RIGHT_TEXT, #WT_TEXTBUTTON and #WT_IMAGEBUTTON.
 */
class DataWidget : public LeafWidget {
public:
	DataWidget(WidgetType wtype);

	virtual void SetupMinimalSize(BaseWidget **wid_array);
	virtual void Draw(const Point &base);

	uint16 value; ///< String number or sprite id.
};

/**
 * Scrollbar widget.
 * Implements #WT_HOR_SCROLLBAR and #WT_VERT_SCROLLBAR.
 */
class ScrollbarWidget : public LeafWidget {
public:
	ScrollbarWidget(WidgetType wtype);

	virtual void SetupMinimalSize(BaseWidget **wid_array);
	virtual void Draw(const Point &base);

	int16 canvas_widget; ///< Widget number of the canvas.
};

/**
 * Base class for canvas-like widgets.
 * Implements #WT_PANEL
 */
class BackgroundWidget : public LeafWidget {
public:
	BackgroundWidget(WidgetType wtype);
	virtual ~BackgroundWidget();

	virtual void SetupMinimalSize(BaseWidget **wid_array);
	virtual void SetSmallestSizePosition(const Rectangle16 &rect);
	virtual void Draw(const Point &base);

	BaseWidget *child; ///< Child widget displayed on top of the background widget.
};

/** Equal size settings of child widgets. */
enum EqualSize {
	EQS_HORIZONTAL = 1, ///< Try to keep equal size for all widgets in horizontal direction.
	EQS_VERTICAL   = 2, ///< Try to keep equal size for all widgets in vertical direction.
};

/** Data about a row or a column. */
struct RowColData {
	uint16 min_size; ///< Minimal size.
	uint16 fill;     ///< Fill step.
	uint16 resize;   ///< Resize step.

	void InitRowColData();
	void Merge(uint16 min_size, uint16 fill, uint16 resize);
};

/** Base class for intermediate (that is, non-leaf) widget. */
class IntermediateWidget : public BaseWidget {
public:
	IntermediateWidget(uint8 num_rows, uint8 num_cols);
	~IntermediateWidget();

	virtual void SetupMinimalSize(BaseWidget **wid_array);
	virtual void SetSmallestSizePosition(const Rectangle16 &rect);
	virtual void Draw(const Point &base);

	void AddChild(uint8 col, uint8 row, BaseWidget *sub);
	void ClaimMemory();

	BaseWidget **childs; ///< Grid of child widget pointers.
	RowColData *rows;    ///< Row data.
	RowColData *columns; ///< Column data.
	uint8 num_rows;      ///< Number of rows.
	uint8 num_cols;      ///< Number of columns.
	uint8 flags;         ///< Equal size flags.

};


/** Available widget parts. */
enum WidgetPartType {
	WPT_NEW_WIDGET,       ///< Start a new widget.
	WPT_NEW_INTERMEDIATE, ///< Start a new widget.
	WPT_MIN_SIZE,         ///< Set minimal size.
	WPT_FILL,             ///< Set fill sizes.
	WPT_RESIZE,           ///< Set resize sizes.
	WPT_PADDING,          ///< Set padding.
	WPT_HOR_PIP,          ///< Set horizontal PIP.
	WPT_VERT_PIP,         ///< Set vertical PIP.
	WPT_DATA,             ///< Additional data values.
	WPT_EQUAL_SIZE,       ///< Define how sizing of child widgets behaves.
	WPT_END_CON,          ///< End of container or row.
};

/** Class for describing a widget-tree in linear source code. */
class WidgetPart {
public:
	WidgetPartType type; ///< Type of this widget part.
	union {
		struct {
			WidgetType wtype; ///< Widget type of the new widget.
			int16 number;     ///< Widget number of the new widget.
			uint8 colour;     ///< Colour of the new widget.
		} new_widget;             ///< Data for #WPT_NEW_WIDGET.
		struct {
			uint8 num_rows;   ///< Number of rows.
			uint8 num_cols;   ///< Number of columns.
		} new_intermediate;       ///< Data for #WPT_NEW_INTERMEDIATE.
		struct {
			uint16 value;     ///< Value of the widget (string or sprite id).
			uint16 tip;       ///< Tool tip string. Use #STR_NULL for no tip.
		} dat;                    ///< Additional data of the widget.
		Point16 size;             ///< Data for #WPT_MIN_SIZE, #WPT_FILL, #WPT_RESIZE.
		uint8 flags;              ///< Equal size flags for intermediate widgets.
		uint8 padding[PAD_COUNT]; ///< Data for #WPT_PADDING, #WPT_HOR_PIP, #WPT_VERT_PIP.
	} data; ///< Data of the widget part.
};

WidgetPart Widget(WidgetType wtype, int16 number, uint8 colour);
WidgetPart Intermediate(uint8 num_rows, uint8 num_cols = 0);
WidgetPart SetFill(uint8 x, uint8 y);
WidgetPart SetResize(uint8 x, uint8 y);
WidgetPart SetPadding(uint8 top, uint8 right, uint8 bottom, uint8 left);
WidgetPart SetHorPIP(uint8 pre, uint8 inter, uint8 post);
WidgetPart SetVertPIP(uint8 pre, uint8 inter, uint8 post);
WidgetPart SetData(uint16 data, uint16 tip);
WidgetPart SetEqualSize(bool hor_equal, bool vert_equal);
WidgetPart EndContainer();

void DrawPanel(const Rectangle &rect);
BaseWidget *MakeWidgetTree(const WidgetPart *parts, int length, int16 *biggest);

#endif