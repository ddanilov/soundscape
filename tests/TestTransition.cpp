#include "Transition.h"

#include <QTest>

class TestTransition : public QObject
{
  Q_OBJECT

private slots:
  void testConvertTransition();
};

void TestTransition::testConvertTransition()
{
  QCOMPARE(convertTransition(Qt::CheckState::Unchecked), Transition::FadeOutIn);
  QCOMPARE(convertTransition(Transition::FadeOutIn), Qt::CheckState::Unchecked);

  QCOMPARE(convertTransition(Qt::CheckState::PartiallyChecked), Transition::CrossFade);
  QCOMPARE(convertTransition(Transition::CrossFade), Qt::CheckState::PartiallyChecked);

  QCOMPARE(convertTransition(Qt::CheckState::Checked), Transition::FadeOutGapIn);
  QCOMPARE(convertTransition(Transition::FadeOutGapIn), Qt::CheckState::Checked);
}

QTEST_MAIN(TestTransition)
#include "TestTransition.moc"
