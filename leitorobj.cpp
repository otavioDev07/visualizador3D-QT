#include "leitorobj.h"
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <cmath>
#include <unordered_set>

namespace {

quint64 chaveAresta(int a, int b) {
    if (a > b) std::swap(a, b);
    return (static_cast<quint64>(a) << 32) | static_cast<quint32>(b);
}

}

bool LeitorOBJ::ler(const QString &caminho, Malha &malha, QString &erro)
{
    QFile arquivo(caminho);
    if (!arquivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        erro = QString("Não foi possível abrir o arquivo:\n%1").arg(caminho);
        return false;
    }

    malha.vertices.clear();
    malha.arestas.clear();

    std::unordered_set<quint64> arestasVistas;
    QTextStream entrada(&arquivo);
    int numeroLinha = 0;

    auto falha = [&](const QString &motivo) {
        erro = QString("Erro na linha %1 do arquivo OBJ: %2").arg(numeroLinha).arg(motivo);
        return false;
    };

    while (!entrada.atEnd()) {
        ++numeroLinha;
        QString linha = entrada.readLine();
        const qsizetype comentario = linha.indexOf(QLatin1Char('#'));
        if (comentario >= 0)
            linha.truncate(comentario);
        linha = linha.simplified();
        if (linha.isEmpty())
            continue;

        const QStringList campos = linha.split(QLatin1Char(' '), Qt::SkipEmptyParts);
        const QString &comando = campos[0];

        if (comando == QLatin1String("v")) {
            if (campos.size() < 4)
                return falha("vértice com menos de 3 coordenadas.");
            bool okX = false, okY = false, okZ = false;
            const double x = campos[1].toDouble(&okX);
            const double y = campos[2].toDouble(&okY);
            const double z = campos[3].toDouble(&okZ);
            if (!(okX && okY && okZ && std::isfinite(x) && std::isfinite(y) && std::isfinite(z)))
                return falha("coordenada de vértice inválida.");
            malha.vertices.emplace_back(x, y, z);
        }
        else if (comando == QLatin1String("f")) {
            if (campos.size() < 4)
                continue;   // face degenerada

            const int totalVertices = static_cast<int>(malha.vertices.size());
            std::vector<int> indices;
            indices.reserve(static_cast<size_t>(campos.size() - 1));

            for (int k = 1; k < campos.size(); ++k) {
                bool ok = false;
                const int i = campos[k].section(QLatin1Char('/'), 0, 0).toInt(&ok);
                if (!ok || i == 0)
                    return falha("índice de vértice inválido na face.");
                const int indice = (i < 0) ? totalVertices + i : i - 1;
                if (indice < 0 || indice >= totalVertices)
                    return falha("a face usa um vértice que não existe.");
                indices.push_back(indice);
            }

            for (size_t k = 0; k < indices.size(); ++k) {
                const int a = indices[k];
                const int b = indices[(k + 1) % indices.size()];
                if (a == b)
                    continue;
                if (arestasVistas.insert(chaveAresta(a, b)).second)
                    malha.arestas.emplace_back(a, b);
            }
        }
    }

    if (malha.vertices.empty() || malha.arestas.empty()) {
        erro = "O arquivo OBJ não contém vértices e faces utilizáveis.";
        return false;
    }
    return true;
}
