#include "JsonRW.h"
#include "Player.h"
#include "Track.h"

#include <QTest>

class TestTrack : public QObject
{
  Q_OBJECT

private slots:
  void testPlayer();
};

void TestTrack::testPlayer()
{
  Track track;
  QVERIFY(track.isPlaying());

  QString file_name("sound_01.mp3");
  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name;
  const auto& base_dir = QDir::temp();
  track.fromJsonObject(json, base_dir);
  QCOMPARE(track.m_player->source(), QUrl::fromLocalFile(base_dir.absoluteFilePath(file_name)));

  track.play();
  QCOMPARE(track.isPlaying(), true);
  QCOMPARE(track.m_player->playbackState(), QMediaPlayer::PlayingState);

  track.pause();
  QCOMPARE(track.isPlaying(), false);
  QCOMPARE(track.m_player->playbackState(), QMediaPlayer::PausedState);
}

QTEST_MAIN(TestTrack)
#include "TestTrack.moc"
