#include "NodeTreeItem.h"
#include "AIDebugger.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QFont>

namespace ai {
namespace debug {

namespace {
const qreal padding = 1;
const qreal fontSize = 10;
const QColor backgroundColor = QColor::fromRgb(32, 32, 32, 64);
const QColor activeBackgroundColor = QColor::fromRgb(255, 0, 0, 128);
const QFont font("Times", fontSize);
const QFontMetrics fontMetrics(font);
}

NodeTreeItem::NodeTreeItem (const AIStateNode& node, NodeTreeItem* parent, int height, int horizontalSpacing, int verticalSpacing) :
		QGraphicsItem(), _node(node), _parent(parent), _height(height), _horizontalSpacing(
				horizontalSpacing), _verticalSpacing(verticalSpacing) {
	_condition = QString::fromStdString(_node.getCondition());
	_name = QString::fromStdString(_node.getName());
	_width = std::max(130, std::max(fontMetrics.width(_name), fontMetrics.width(_condition)));
	_lineHeight = fontMetrics.lineSpacing();
}

NodeTreeItem::~NodeTreeItem () {
}

void NodeTreeItem::init() {
	setOffset(QPointF(100.0f, fullSize().height() / 2.0f - _height));
}

void NodeTreeItem::setOffset (const QPointF& offset) {
	_offset = offset;

	if (_parent == nullptr){
		setPos(_offset);
	} else {
		setPos(_parent->pos() + _offset);
	}

	float yOffset = 0.0f;
	foreach (NodeTreeItem* node, _children) {
		const float halfHeight = node->fullSize().height() / 2.0f;
		yOffset += halfHeight;
		float heightOffset = -_size.height() / 2.0f + yOffset;
		yOffset += halfHeight + _verticalSpacing;
		const QPointF offsetF(_width + _horizontalSpacing, heightOffset);
		node->setOffset(offsetF);
	}
}

QRectF NodeTreeItem::boundingRect () const {
	return QRectF(0.0f, 0.0f, _width, _height);
}

QRectF NodeTreeItem::fullSize() {
	if (!_size.isEmpty())
		return _size;

	_size = QRectF(0.0f, 0.0f, _width + _horizontalSpacing, _height + _verticalSpacing);

	if (_children.empty()) {
		return _size;
	}

	QRectF total;
	foreach (NodeTreeItem* node, _children) {
		QRectF childDimension = node->fullSize();
		childDimension.translate(node->_width + _horizontalSpacing, total.height() + childDimension.height() / 2.0f);
		total |= childDimension;
	}

	total.moveTo(total.x(), _size.center().y() - total.height() / 2.0f);

	_size |= total;

	return _size;
}

QPointF NodeTreeItem::getChildPos (NodeTreeItem* child) const {
	const QPointF& childPos = child->pos() - pos();
	const float childRelX = childPos.x();
	const float childRelY = childPos.y() + child->boundingRect().center().y();
	return QPointF(childRelX, childRelY);
}

void NodeTreeItem::paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
	Q_UNUSED(option);
	Q_UNUSED(widget);
	const qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
	const bool active = _node.isActive();
	QBrush b = painter->brush();
	if (active)
		painter->setBrush(activeBackgroundColor);
	else
		painter->setBrush(backgroundColor);
	painter->drawRect(0.0f, 0.0f, _width, _height);

	const std::vector<AIStateNode>& c = _node.getChildren();
	if (!c.empty()) {
		// location of the vertical line
		const float seperatorX = _width + _horizontalSpacing / 2.0f;
		// draw the (right) vertical line for connecting the children
		painter->drawLine(_width, _height / 2.0f, seperatorX, _height / 2.0f);
		foreach (NodeTreeItem* child, _children) {
			const QPointF& childPos = getChildPos(child);
			// draw the (left) vertical line for connecting to the parent
			painter->drawLine(seperatorX, childPos.y(), childPos.x(), childPos.y());
		}
		// draw the vertical connection line
		if (_children.size() >= 2) {
			const QPointF& firstChildPos = getChildPos(_children.first());
			const QPointF& lastChildPos = getChildPos(_children.last());
			painter->drawLine(seperatorX, firstChildPos.y(), seperatorX, lastChildPos.y());
		}
	}

	painter->setBrush(b);

	if (lod < 0.4)
		return;

	painter->setFont(font);
	painter->save();
	QRect rect(padding, padding, _width - 2 * padding, _height - 2 * padding);
	painter->drawText(rect, _name);
	rect.setY(rect.y() + _lineHeight);
	const TreeNodeStatus status = _node.getStatus();
	QString stateString;
	switch (status) {
	case UNKNOWN:
		stateString = "UNKNOWN";
		break;
	case CANNOTEXECUTE:
		stateString = "CANNOTEXECUTE";
		break;
	case RUNNING:
		stateString = "RUNNING";
		break;
	case FAILED:
		stateString = "FAILED";
		break;
	case FINISHED:
		stateString = "FINISHED";
		break;
	case EXCEPTION:
		stateString = "EXCEPTION";
		break;
	}
	painter->drawText(rect, stateString);
	rect.setY(rect.y() + _lineHeight);
	painter->drawText(rect, _condition);
	painter->restore();
}

}
}
