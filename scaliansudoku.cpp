#include "scaliansudoku.h"
#include "ui_scaliansudoku.h"
#include <QPixmap>
#include <QDebug>

ScalianSudoku::ScalianSudoku(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ScalianSudoku)
    , sudokuVacio{true}
{
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    ui->setupUi(this);

    ui->FrameCeldas->setVisible(false);

    ui->LogoScalian->setPixmap(QPixmap(":/logo/scalian"));
    ui->LogoCampus->setPixmap(QPixmap(":/logo/campus42"));
    ui->LogoCampus->setScaledContents(true);

    int itemIdx = 0;
    uint filas = ui->Tablero->count();
    for(uint filaId = 0; filaId < filas; filaId++)
    {
        if(auto widget = ui->Tablero->itemAt(filaId)->widget())
        {
            if(widget->objectName().contains("separator"))
            {
                continue;
            }
        }
        auto fila = ui->Tablero->itemAt(filaId)->layout();
        uint celdas = fila->count();
        for(uint celdaId = 0; celdaId < celdas; celdaId++)
        {
            auto celda = dynamic_cast<QLabel*>(fila->itemAt(celdaId)->widget());
            if(not celda)
            {
                continue;
            }

            celda->setText("");
            celda->setProperty("fila", itemIdx/9);
            celda->setProperty("col", itemIdx%9);
            celda->installEventFilter(this);
            itemIdx++;
        }
    }

    connect(ui->botonLimpiar, &QPushButton::clicked, this, &ScalianSudoku::onLimpiarSudoku);
    connect(ui->botonResolver, &QPushButton::clicked, this, &ScalianSudoku::onResolverSudoku);

    connect(ui->Aceptar, &QPushButton::clicked, this, &ScalianSudoku::onAceptar);
    connect(ui->Cancelar, &QPushButton::clicked, this, &ScalianSudoku::onCancelar);
    connect(ui->Borrar, &QPushButton::clicked, this, &ScalianSudoku::onBorrar);
}

void ScalianSudoku::limpiarSudoku()
{
    qDebug() << "Borrar Sudoku";
}

void ScalianSudoku::resolverSudoku()
{
    qDebug() << "Resolver Sudoku";
}

bool ScalianSudoku::chequearSudoku()
{
    qDebug() << "Chequear Sudoku";
    return false;
}

void ScalianSudoku::setearCelda(uint filaId, uint colId, uint valor)
{
    qDebug() << "Setear Celda ("<< filaId << "," << colId << "): " << valor;
}

void ScalianSudoku::borrarCelda(uint filaId, uint colId)
{
    qDebug() << "Borrar Celda ("<< filaId << "," << colId << ")";
}

ScalianSudoku::~ScalianSudoku()
{
    delete ui;
}

void ScalianSudoku::onDobleClickEnCelda(uint filaId, uint colId)
{
    ui->FrameCeldas->setProperty("fila", filaId);
    ui->FrameCeldas->setProperty("col", colId);

    ui->EtiquetaFila->setText(QString::number(filaId));
    ui->EtiquetaColumna->setText(QString::number(colId));
    ui->FrameCeldas->setVisible(true);
    ui->FrameControles->setVisible(false);
}

std::optional<QLabel*> ScalianSudoku::obtenerCelda(uint filaId, uint colId)
{
    if(filaId > 9 || colId > 9)
    {
        return std::nullopt;
    }

    if(colId > 5)
    {
        colId += 2;
    }
    else if(colId > 2)
    {
        colId += 1;
    }

    if(filaId > 5)
    {
        filaId += 2;
    }
    else if(filaId > 2)
    {
        filaId += 1;
    }

    auto fila = ui->Tablero->itemAt(filaId)->layout();
    auto celda = dynamic_cast<QLabel*>(fila->itemAt(colId)->widget());

    if(not celda)
    {
        return std::nullopt;
    }

    return celda;
}

bool ScalianSudoku::limpiarCelda(uint filaId, uint colId)
{
    auto celda = obtenerCelda(filaId, colId);
    if(celda.has_value())
    {
        QColor color(Qt::GlobalColor::black);
        celda.value()->setStyleSheet(QString("QLabel { color : rgb(%1,%2,%3); }").arg(color.red()).arg(color.green()).arg(color.blue()));
        celda.value()->setText("");
        return true;
    }

    return false;
}

bool ScalianSudoku::escribirCelda(uint valor, uint filaId, uint colId, QColor color)
{
    auto celda = obtenerCelda(filaId, colId);
    if(celda.has_value() && valor < 10)
    {
        celda.value()->setStyleSheet(QString("QLabel { color : rgb(%1,%2,%3); }").arg(color.red()).arg(color.green()).arg(color.blue()));
        celda.value()->setText(QString::number(valor));
        return true;
    }

    return false;
}

void ScalianSudoku::escribirResultado(const std::string &resultado, QColor color)
{
    ui->EtiquetaResultado->setStyleSheet(QString("QLabel { color : rgb(%1,%2,%3); }").arg(color.red()).arg(color.green()).arg(color.blue()));
    ui->EtiquetaResultado->setText(resultado.c_str());
}

void ScalianSudoku::onLimpiarSudoku()
{
    for(uint filaId = 0; filaId < 9; filaId++)
    {
        for(uint colId = 0; colId < 9; colId++)
        {
            limpiarCelda(filaId, colId);
        }
    }

    escribirResultado("");
    limpiarSudoku();
}

void ScalianSudoku::onResolverSudoku()
{
    resolverSudoku();
    bool resultado = chequearSudoku();

    if(resultado)
    {
        escribirResultado("Correcto", QColor(Qt::GlobalColor::green));
    }
    else
    {
        escribirResultado("Incorrecto", QColor(Qt::GlobalColor::red));
    }
}

void ScalianSudoku::onAceptar()
{
    uint fila = ui->FrameCeldas->property("fila").value<uint>();
    uint col = ui->FrameCeldas->property("col").value<uint>();
    uint valor = ui->ValorCelda->value();
    ui->FrameCeldas->setVisible(false);
    ui->FrameControles->setVisible(true);
    setearCelda(fila, col, valor);
}

void ScalianSudoku::onCancelar()
{
    ui->FrameCeldas->setVisible(false);
    ui->FrameControles->setVisible(true);
}

void ScalianSudoku::onBorrar()
{
    uint fila = ui->FrameCeldas->property("fila").value<uint>();
    uint col = ui->FrameCeldas->property("col").value<uint>();
    ui->FrameCeldas->setVisible(false);
    ui->FrameControles->setVisible(true);
    limpiarCelda(fila, col);
    borrarCelda(fila, col);
}

bool ScalianSudoku::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonDblClick)
    {
        if(sudokuVacio)
        {
            auto coordinates = obtenerCoordenadas(object);
            if(coordinates.has_value())
            {
                uint fila = std::get<0>(coordinates.value());
                uint col = std::get<1>(coordinates.value());
                onDobleClickEnCelda(fila, col);
            }
        }
        else
        {
            // Popup
        }
    }

    return QMainWindow::eventFilter(object, event);
}

std::optional<std::tuple<uint, uint>> ScalianSudoku::obtenerCoordenadas(QObject *object)
{
    if(object)
    {
        auto label = dynamic_cast<QLabel*>(object);
        if(label)
        {
            uint fila = object->property("fila").value<uint>();
            uint col = object->property("col").value<uint>();
            return std::tuple<int,int>{fila,col};
        }
    }

    return std::nullopt;
}

