#pragma once

#include <QMainWindow>
#include <QPointer>
#include <QSystemTrayIcon>

class MainWindow : public QMainWindow
{
public:
  explicit MainWindow(QWidget* parent = nullptr);

public slots:
  void trayIconAction(QSystemTrayIcon::ActivationReason reason);

protected:
  void closeEvent(QCloseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;

private:
  void setupTrayIcon();
  void addItemsToMenu(QMenu* menu) const;
  void windowShowOrHide();

  QPointer<QSystemTrayIcon> m_tray_icon;
  QPointer<QMenu> m_tray_menu;
  QPointer<QMenu> m_mouse_menu;
  QByteArray m_old_geometry;
};
