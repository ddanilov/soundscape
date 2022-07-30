#include "PositionLabel.h"

#include <QTest>

class TestPositionLabel : public QObject
{
  Q_OBJECT

private slots:
  void testSetMax();
  void testSetValue();
};

void TestPositionLabel::testSetMax()
{
  PositionLabel label;
  QCOMPARE(label.m_precision, 1);
  QCOMPARE(label.m_field_width, 3);

  label.setMax(0);
  QCOMPARE(label.m_field_width, 3);

  label.setMax(10);
  QCOMPARE(label.m_field_width, 4);
}

void TestPositionLabel::testSetValue()
{
  PositionLabel label;
  label.setMax(100);

  label.setValue(0.0);
  QCOMPARE(label.text(), "000.0 s");

  label.setValue(0.05);
  QCOMPARE(label.text(), "000.1 s");

  label.setValue(0.5);
  QCOMPARE(label.text(), "000.5 s");

  label.setValue(10.5);
  QCOMPARE(label.text(), "010.5 s");

  label.setValue(100.5);
  QCOMPARE(label.text(), "100.5 s");

  label.setValue(1000.5);
  QCOMPARE(label.text(), "1000.5 s");

  label.setValue(1.5);
  QCOMPARE(label.text(), "001.5 s");
}

QTEST_MAIN(TestPositionLabel)
#include "TestPositionLabel.moc"
