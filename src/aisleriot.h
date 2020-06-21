#ifndef AISLERIOT_H
#define AISLERIOT_H

#include <QObject>
#include <QSharedPointer>
#include "aisleriot_scm.h"

class QQmlEngine;
class QJSEngine;
class Slot;
class Aisleriot : public QObject, public AisleriotSCM
{
    Q_OBJECT
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
    Q_PROPERTY(bool canDeal READ canDeal NOTIFY canDealChanged)
    Q_PROPERTY(QString gameFile READ gameFile NOTIFY gameFileChanged)
    Q_PROPERTY(int score READ score NOTIFY scoreChanged)
    Q_PROPERTY(int state READ state NOTIFY stateChanged);
    Q_PROPERTY(QString message READ message NOTIFY messageChanged);

public:
    static Aisleriot* instance();
    static QObject* instance(QQmlEngine *engine, QJSEngine *scriptEngine);
    ~Aisleriot();

    // Values
    enum GameState {
        UninitializedState,
        LoadedState,
        BeginState,
        RunningState,
        GameOverState,
        WonState,
        LastGameState,
    };
    Q_ENUM(GameState)

    // QML API
    Q_INVOKABLE void startNewGame();
    Q_INVOKABLE void restartGame();
    Q_INVOKABLE bool loadGame(QString gameFile);
    Q_INVOKABLE void undoMove();
    Q_INVOKABLE void redoMove();

    // Properties
    bool canUndo() const;
    bool canRedo() const;
    bool canDeal() const;
    QString gameFile() const;
    int score() const;
    GameState state() const;
    QString message() const;

signals:
    void canUndoChanged();
    void canRedoChanged();
    void canDealChanged();
    void gameFileChanged();
    void scoreChanged();
    void stateChanged();
    void messageChanged();

private:
    friend AisleriotSCM;  // TODO: Can I get rid of this?

    explicit Aisleriot(QObject *parent = nullptr);
    static void interfaceInit(void *data);

    virtual void setCanUndo(bool canUndo);
    virtual void setCanRedo(bool canRedo);
    virtual void setCanDeal(bool canDeal);
    void setGameFile(QString file);
    virtual void setScore(int score);
    void setState(GameState state);
    virtual void setMessage(QString message);

    void endMove();
    void updateDealable();
    bool winningGame();

    // Methods required by SCM
    virtual void addSlot(QSharedPointer<Slot> slot);
    virtual QSharedPointer<Slot> getSlot(int slot);
    virtual void testGameOver();
    virtual void clearGame();

    bool m_canUndo;
    bool m_canRedo;
    bool m_canDeal;
    QString m_gameFile;
    GameState m_state;
    int m_score;
    QString m_message;
    QVector<QSharedPointer<Slot>> m_cardSlots;

    static Aisleriot *s_game;
};

#endif // AISLERIOT_H
