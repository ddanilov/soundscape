#pragma once

#include <QLabel>

class IconLabel : public QLabel
{
public:
  explicit IconLabel(QString icon_file, QWidget* parent = nullptr);
};
