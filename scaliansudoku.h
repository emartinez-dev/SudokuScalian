#ifndef SCALIANSUDOKU_H
#define SCALIANSUDOKU_H

#include <QMainWindow>

#include <optional>
#include <tuple>
#include <QLabel>
#include <QColor>
#include <unordered_set>

#define TAMAÑO_TABLERO 81
#define TAMAÑO_FILA 9

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
    unsigned char tablero[TAMAÑO_TABLERO];
    bool eventFilter(QObject *object, QEvent *event) override;
    std::optional<std::tuple<uint, uint>> obtenerCoordenadas(QObject *object);
    bool interResuelta(uint filaId, uint colId);
    bool regionResuelta(uint filaId, uint colId);
    bool colResuelta(uint colId);
    bool filaResuelta(uint filaId);

    bool chequearDuplicados(std::array<unsigned char, TAMAÑO_FILA> arr);

    bool interInsertLegal(int coord, unsigned char n);
    bool regionInsertLegal(int filaId, int colId, unsigned char n);

    bool chequearFilaLegal(int filaId);
    bool chequearColLegal(int colId);
    bool chequearRegionLegal(int coord);

    bool chequearCompleto();
    bool resolverBT(int index = 0);

    bool chequearLegal();
    bool chequearLleno();

    Ui::ScalianSudoku *ui;
    bool sudokuVacio;
    int getIndex(int filaId, int colId, int tamaño = TAMAÑO_FILA);
    int getCol(int coord);
    int getFila(int coord);
    void printMyBoard(); // TODO: remove later, DEBUG FUNCTION
    void fillMyBoard(); // TODO: remove later, DEBUG FUNCTION
};
#endif // SCALIANSUDOKU_H
