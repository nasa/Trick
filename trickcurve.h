#ifndef TRICKCURVE_H
#define TRICKCURVE_H

#include <QVector2D>
#include "qplot/qcustomplot.h"
#include "trickmodel.h"
#include "timeit_linux.h"

class QCP_LIB_DECL TrickCurve : public QCPAbstractPlottable
{
    Q_OBJECT
    Q_PROPERTY(LineStyle lineStyle READ lineStyle WRITE setLineStyle)
    enum LineStyle { lsNone, lsLine };

public:
    explicit TrickCurve(QCPAxis *keyAxis, QCPAxis *valueAxis) ;
    virtual ~TrickCurve();

    LineStyle lineStyle() const { return mLineStyle; }
    void setLineStyle(LineStyle style);
    QCPRange xRange(bool &validRange, SignDomain inSignDomain=sdBoth);
    QCPRange yRange(bool &validRange, SignDomain inSignDomain=sdBoth);

    virtual double selectTest(const QPointF &pt,
                              bool onlySelectable, QVariant *details=0) const;

    void setData(TrickModel* model, int tcol, int xcol, int ycol);
    void setValueScaleFactor(double sf) { _valueScaleFactor = sf; }
    double valueScaleFactor() { return _valueScaleFactor; }

    virtual void clearData()
    {
        _model = 0 ;
        _tcol = 0;
        _xcol = 0;
        _ycol = 0;
        _isPainterPathCreated = false;
    }

    static double _distSquaredLineSegmentToPoint(const QPointF &l0,
                                               const QPointF &l1,
                                               const QPointF &pt);

protected:
    // property members:

    LineStyle mLineStyle;

    // reimplemented virtual methods:
    virtual void draw(QCPPainter *painter);
    virtual void drawLegendIcon(QCPPainter *painter, const QRectF &rect) const;
    virtual QCPRange getKeyRange(bool &validRange,
                                 SignDomain inSignDomain=sdBoth) const;
    virtual QCPRange getValueRange(bool &validRange,
                                   SignDomain inSignDomain=sdBoth) const;

    // non-virtual methods:
    void getCurveData(QVector<QPointF> *lineData) const;
    double pointDistance(const QPointF &pixelPoint) const;
    QPointF outsideCoordsToPixels(double key, double value,
                                  int region, QRect axisRect) const;


private:
    TrickCurve();
    bool _isPainterPathCreated;
    QPainterPath _painterPath;
    void _createPainterPath();
    inline QTransform _coordToPixelTransform() const
    {
        //
        // Create transform from coord to pixel for painter
        //
        double xl = keyAxis()->range().lower;
        double xu = keyAxis()->range().upper;
        double yl = valueAxis()->range().lower;
        double yu = valueAxis()->range().upper;
        double wx = (xu-xl);
        double wy = (yu-yl);
        QRect r = keyAxis()->axisRect()->rect();
        double mx = (r.width())/wx;
        double my = (r.height())/wy;

        return  QTransform (  mx,               0.0,
                              0.0,               -my,
                              -mx*xl+r.left(),   my*yl+r.bottom());
    }

    TrickModel* _model;
    int _tcol;
    int _xcol;
    int _ycol;
    double _valueScaleFactor;
    QCPRange _xrange;
    QCPRange _yrange;

private slots:
    void _slotSelectionChanged(bool sel);

    friend class AxisRect;
    friend class QCustomPlot;
    friend class QCPLegend;
};


#endif // TRICKCURVE_H
