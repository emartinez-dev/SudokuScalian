#ifndef SCALIANSUDOKU_H
#define SCALIANSUDOKU_H

#include <QMainWindow>

#include <optional>
#include <tuple>
#include <QLabel>
#include <QColor>

QT_BEGIN_NAMESPACE
namespace Ui { class ScalianSudoku; }
QT_END_NAMESPACE

class ScalianSudoku : public QMainWindow
{
    Q_OBJECT

public:
    ScalianSudoku(QWidget *parent = nullptr);

    virtual void limpiarSudoku();
    virtual void resolverSudoku();
    virtual bool chequearSudoku();
    virtual void setearCelda(uint filaId, uint colId, uint valor);
    virtual void borrarCelda(uint filaId, uint colId);

    ~ScalianSudoku();

protected:

    virtual void onDobleClickEnCelda(uint filaId, uint colId);
    std::optional<QLabel*> obtenerCelda(uint filaId, uint colId);

    bool limpiarCelda(uint filaId, uint colId);
    bool escribirCelda(uint valor, uint filaId, uint colId, QColor color = QColor(Qt::GlobalColor::black));
    void escribirResultado(const std::string &resultado, QColor color = QColor(Qt::GlobalColor::black));

    void onLimpiarSudoku();
    void onResolverSudoku();
    void onAceptar();
    void onCancelar();
    void onBorrar();

private:    

    bool eventFilter(QObject *object, QEvent *event) override;
    std::optional<std::tuple<uint, uint>> obtenerCoordenadas(QObject *object);

    Ui::ScalianSudoku *ui;
    bool sudokuVacio;
};
#endif // SCALIANSUDOKU_H
