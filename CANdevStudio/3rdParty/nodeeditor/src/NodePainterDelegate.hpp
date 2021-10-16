#pragma once

#include <QPainter>

#include "NodeGeometry.hpp"
#include "NodeDataModel.hpp"
#include "Export.hpp"

namespace QtNodes {

class NodeGraphicsObject;

/// Class to allow for custom painting
class NODE_EDITOR_PUBLIC NodePainterDelegate
{

public:

  virtual
  ~NodePainterDelegate() = default;

  virtual void
  paint(QPainter* painter,
        NodeGeometry const& geom,
        NodeDataModel const * model,
        NodeGraphicsObject const & graphicsObject) = 0;
};
}
