#pragma once

#include <QLabel>

class IconLabel : public QLabel
{
public:
  explicit IconLabel(const QString& icon_file, QWidget* parent = nullptr);
};
