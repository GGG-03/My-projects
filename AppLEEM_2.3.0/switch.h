#ifndef SWITCH_H
#define SWITCH_H

#include <QtWidgets>
#include "styleButton.h"

class Animator final : public QVariantAnimation
{
    Q_OBJECT
    Q_PROPERTY(QObject* targetObject READ targetObject WRITE setTargetObject)

    public:
        Animator(QObject* target, QObject* parent = nullptr);
        ~Animator() override;

        QObject* targetObject() const;
        void setTargetObject(QObject* target);

        inline bool isRunning() const {
            return state() == Running;
        }

      public slots:
        void setup(int duration, QEasingCurve easing = QEasingCurve::Linear);
        void interpolate(const QVariant& start, const QVariant& end);
        void setCurrentValue(const QVariant&);

      protected:
        void updateCurrentValue(const QVariant& value) override final;
        void updateState(QAbstractAnimation::State newState, QAbstractAnimation::State oldState) override final;

      private:
        QPointer<QObject> target;
};

class SelectionControl : public QAbstractButton {
    Q_OBJECT

  public:
    explicit SelectionControl(QWidget* parent = nullptr);
    ~SelectionControl() override;

    Qt::CheckState checkState() const;

  Q_SIGNALS:
    void stateChanged(int);

  protected:
    void enterEvent(QEvent*);
    void checkStateSet() override;
    void nextCheckState() override;
    virtual void toggle(Qt::CheckState state) = 0;
};

class Switch final : public SelectionControl
{
    Q_OBJECT

    double CORNER_RADIUS = 5.4;
    double THUMB_RADIUS = 10;
    double SHADOW_ELEVATION = 1.4;
    double SWITCH_HEIGHT = 26.0;
    double MARGINS = 20./3;
    QMargins INDICATOR_MARGIN = QMargins(20./3, 20./3, 20./3, 20./3);

  public:
    explicit Switch(QWidget* parent = nullptr);
    Switch(const QString& text, QWidget* parent = nullptr);
    Switch(const QString& text, const QBrush&, QWidget* parent = nullptr);
    ~Switch() override;

    void changeSize(float);
    QSize sizeHint() const override final;

  protected:
    void paintEvent(QPaintEvent*) override final;
    void resizeEvent(QResizeEvent*) override final;
    void toggle(Qt::CheckState) override final;

    void init();
    QRect indicatorRect();
    QRect textRect();

    static inline QColor colorFromOpacity(const QColor& c, qreal opacity) {
        return QColor(c.red(), c.green(), c.blue(), qRound(opacity * 255.0));
    }
    static inline bool ltr(QWidget* w) {
        if (nullptr != w)
            return w->layoutDirection() == Qt::LeftToRight;

        return false;
    }

  private:
    Style::Switch style;
    QPixmap shadowPixmap;
    QPointer<Animator> thumbBrushAnimation;
    QPointer<Animator> trackBrushAnimation;
    QPointer<Animator> thumbPosAniamtion;
};

#endif // SWITCH_H
