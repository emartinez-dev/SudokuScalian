#include "scaliansudoku.h"
#include "ui_scaliansudoku.h"
#include <QPixmap>
#include <QDebug>
#include <unordered_set>
#include <stdint.h>

ScalianSudoku::ScalianSudoku(QWidget *parent)
    : QMainWindow(parent)
    , tablero {}
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
    for (int i = 0; i < TAMAÑO_TABLERO; i++)
        tablero[i] = 0;
}

void ScalianSudoku::resolverSudoku()
{
    resolverBT();
    for (uint_fast8_t i = 0; i < TAMAÑO_TABLERO; i++)
        escribirCelda(tablero[i], getFila(i), getCol(i), Qt::GlobalColor::blue);
}

bool ScalianSudoku::resolverBT(int index)
{
    if (index == TAMAÑO_TABLERO)
        return true;
    if (tablero[index] != 0)
        return resolverBT(index + 1);
    for (uint_fast8_t n = 1; n <= TAMAÑO_FILA; n++)
    {
        if (interInsertLegal(index, n))
        {
            tablero[index] = n;
            if (resolverBT(index + 1))
                return true;
        }
        tablero[index] = 0;
    }
    return false;
}

bool ScalianSudoku::interInsertLegal(int coord, unsigned char n)
{
    int col = getCol(coord);
    int fila = getFila(coord);
    for (int i = 0; i < TAMAÑO_FILA; i++)
    {
        if (tablero[getIndex(fila, i)] == n || tablero[getIndex(i, col)] == n)
            return false;
    }
    return regionInsertLegal(fila, col, n);
}

bool ScalianSudoku::regionInsertLegal(int filaId, int colId, unsigned char n)
{
    int inicioFila = filaId / 3 * 3;
    int inicioCol = colId / 3 * 3;

    for (uint y = 0; y < 3; y++)
    {
        for (uint x = 0; x < 3; x++)
        {
            if (tablero[getIndex(inicioFila + y, inicioCol + x)] == n)
                return false;
        }
    }
    return true;
}

bool ScalianSudoku::chequearLleno()
{
    for (uint i = 0; i < TAMAÑO_TABLERO; i++)
    {
        if (tablero[i] == 0)
            return false;
    }
    return true;
}

bool ScalianSudoku::chequearLegal()
{
    for (uint_fast8_t y = 0; y < TAMAÑO_FILA; y++)
    {
        std::unordered_set<unsigned char> col;
        std::unordered_set<unsigned char> fila;
        std::unordered_set<unsigned char> region;
        for (uint_fast8_t x = 0; x < TAMAÑO_FILA; x++)
        {
            if (fila.find(tablero[getIndex(y, x)]) != fila.end())
                return false;
            if (tablero[getIndex(y, x)] != 0)
                fila.insert(tablero[getIndex(y, x)]);
            if (col.find(tablero[getIndex(x, y)]) != col.end())
                return false;
            if (tablero[getIndex(x, y)] != 0)
                col.insert(tablero[getIndex(x, y)]);

            unsigned char ry = 3 * (y / 3) + x / 3;
            unsigned char rx = 3 * (y % 3) + x % 3;
            if (region.find(tablero[getIndex(ry, rx)]) != region.end())
                return false;
            if (tablero[getIndex(ry, rx) != 0])
                region.insert(tablero[getIndex(ry, rx)]);
        }
    }
    return true;
}

bool ScalianSudoku::chequearSudoku()
{
    for (uint i = 0; i < TAMAÑO_FILA; i++)
    {
        if (!interResuelta(i, i)) // chequear el tablero en diagonal
            return false;
        if (!(i % 3)) // chequear las regiones solo cuando entremos en una nueva
        {
            if (!regionResuelta(i, i))
                return false;
        }
    }
    return true;
}

/* Revisa que la fila y la columna sea válida */
bool ScalianSudoku::interResuelta(uint filaId, uint colId)
{
    unsigned char sum_fila = 0;
    unsigned char sum_col = 0;

    for (uint_fast8_t i = 0; i < TAMAÑO_FILA; i++)
    {
        sum_fila += tablero[getIndex(filaId, i)];
        sum_col += tablero[getIndex(i, colId)];
    }
    if (!filaResuelta(filaId) || !colResuelta(colId))
        return false;
    if (sum_fila != 45 || sum_col != 45)
        return false;
    return true;
}

/* Revisa que el subtablero de 3x3 sea válido */
bool ScalianSudoku::regionResuelta(uint filaId, uint colId)
{
    uint inicioFila = filaId / 3 * 3;
    uint inicioCol = colId % 3 * 3;
    unsigned char sum = 0;

    std::array<unsigned char, TAMAÑO_FILA> arr;
    for (uint_fast8_t y = 0; y < 3; y++)
    {
        for (uint_fast8_t x = 0; x < 3; x++)
        {
            arr[getIndex(y, x, 3)] = tablero[getIndex(inicioFila + y, inicioCol + x)];
            sum += tablero[getIndex(inicioFila + y, inicioCol + x)];
        }
    }
    if (sum != 45 || chequearDuplicados(arr))
        return false;
    return true;
}

bool ScalianSudoku::filaResuelta(uint filaId)
{
    std::array<unsigned char, TAMAÑO_FILA> arr;
    for (uint_fast8_t i = 0; i < TAMAÑO_FILA; i++)
        arr[i] = tablero[getIndex(filaId, i)];
    if (chequearDuplicados(arr))
        return false;
    return true;
}

bool ScalianSudoku::colResuelta(uint colId)
{
    std::array<unsigned char, TAMAÑO_FILA> arr;
    for (uint_fast8_t i = 0; i < TAMAÑO_FILA; i++)
        arr[i] = tablero[getIndex(i, colId)];
    if (chequearDuplicados(arr))
        return false;
    return true;
}


bool ScalianSudoku::chequearDuplicados(std::array<unsigned char, TAMAÑO_FILA> arr)
{
    auto it = std::unique(arr.begin(), arr.end());
    return (std::distance(arr.begin(), it) != arr.size());
}

void ScalianSudoku::setearCelda(uint filaId, uint colId, uint valor)
{
    tablero[getIndex(filaId, colId)] = valor;
    escribirCelda(valor, filaId, colId);
}

void ScalianSudoku::borrarCelda(uint filaId, uint colId)
{
    tablero[getIndex(filaId, colId)] = 0;
    limpiarCelda(filaId, colId);
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
    bool legal = chequearLegal();
    if (legal)
        resolverSudoku();
    bool resultado = chequearSudoku();

    if (resultado)
    {
        escribirResultado("Correcto", QColor(Qt::GlobalColor::green));
    }
    else if (!legal && !chequearLleno())
    {
        escribirResultado("Imposible", QColor(Qt::GlobalColor::red));
    }
    else
        escribirResultado("Incorrecto", QColor(Qt::GlobalColor::yellow));
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

int ScalianSudoku::getIndex(int filaId, int colId, int tamaño)
{
    return (filaId * tamaño + colId);
}

int ScalianSudoku::getCol(int coord)
{
    return (coord % TAMAÑO_FILA);
}

int ScalianSudoku::getFila(int coord)
{
    return (coord / TAMAÑO_FILA);
}
